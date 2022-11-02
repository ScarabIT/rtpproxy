/*
 * Copyright (c) 2022 Sippy Software, Inc., http://www.sippysoft.com
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <srtp2/srtp.h>

#include "config_pp.h"

#include "rtpp_types.h"
#include "rtpp_debug.h"
#include "rtpp_module.h"
#include "rtpp_module_wthr.h"
#include "rtpp_module_cplane.h"
#include "rtpp_log.h"
#include "rtpp_log_obj.h"
#include "rtpp_refcnt.h"
#include "rtpp_cfg.h"
#include "rtpp_wi.h"
#include "rtpp_wi_sgnl.h"
#include "rtpp_wi_data.h"
#include "rtpp_queue.h"
#include "rtpp_stream.h"
#include "rtp.h"
#include "rtpp_time.h"
#include "rtp_packet.h"
#include "rtpp_command.h"
#include "rtpp_command_args.h"
#include "rtpp_command_sub.h"
#include "rtpp_util.h"
#include "rtpp_socket.h"
#include "rtpp_sessinfo.h"
#include "rtpp_session.h"
#include "rtpp_stats.h"
#include "rtpp_stream.h"
#include "rtpp_pipe.h"
#include "rtpp_proc_async.h"
#include "rtpp_ttl.h"
#include "rtpp_util.h"
#include "advanced/packet_processor.h"
#include "advanced/pproc_manager.h"

#include "rtpp_dtls.h"
#include "rtpp_dtls_conn.h"

struct rtpp_module_priv {
    struct rtpp_dtls *dtls_ctx;
    const struct rtpp_cfg *cfsp;
};

struct dtls_gw_stream_cfg {
    struct rtpp_refcnt *rcnt;
    struct rtpp_dtls_conn *dtls_conn;
};

enum rtpp_dtls_dir {
    DTLS_IN, SRTP_IN, RTP_OUT
};

struct rtpp_dtls_gw_aux {
    enum rtpp_dtls_dir direction;
    struct rtpp_dtls_conn *dtls_conn;
    struct rtpp_stream *strmp;
};

struct wipkt {
    struct rtp_packet *pkt;
    struct rtpp_dtls_gw_aux edata;
};

static struct rtpp_module_priv *rtpp_dtls_gw_ctor(const struct rtpp_cfg *);
static void rtpp_dtls_gw_dtor(struct rtpp_module_priv *);
static void rtpp_dtls_gw_worker(const struct rtpp_wthrdata *);
static int rtpp_dtls_gw_handle_command(struct rtpp_module_priv *,
  const struct rtpp_subc_ctx *);
static bool is_dtls_packet(const struct rtp_packet *);

#ifdef RTPP_CHECK_LEAKS
#include "rtpp_memdeb_internal.h"

RTPP_MEMDEB_APP_STATIC;
#endif

struct rtpp_minfo rtpp_module = {
    .descr.name = "dtls_gw",
    .descr.ver = MI_VER_INIT(),
    .descr.module_id = 4,
    .proc.ctor = rtpp_dtls_gw_ctor,
    .proc.dtor = rtpp_dtls_gw_dtor,
    .wapi = &(const struct rtpp_wthr_handlers){.main_thread = rtpp_dtls_gw_worker},
    .capi = &(const struct rtpp_cplane_handlers){.ul_subc_handle = rtpp_dtls_gw_handle_command},
#ifdef RTPP_CHECK_LEAKS
    .memdeb_p = &MEMDEB_SYM
#endif
};

static void
rtpp_dtls_gw_worker(const struct rtpp_wthrdata *wp)
{
    struct rtpp_wi *wi;
    struct wipkt *wip;

    for (;;) {
        wi = rtpp_queue_get_item(wp->mod_q, 0);
        if (wi == wp->sigterm) {
            break;
        }
        wip = rtpp_wi_data_get_ptr(wi, sizeof(*wip), sizeof(*wip));
        switch (wip->edata.direction) {
        case DTLS_IN:
            RTPP_LOG(rtpp_module.log, RTPP_LOG_DBUG, "Packet from DTLS");
            CALL_METHOD(wip->edata.dtls_conn, dtls_recv, wip->pkt);
            break;
        case SRTP_IN:
            RTPP_LOG(rtpp_module.log, RTPP_LOG_DBUG, "DTLS: packet SRTP->RTP");
            CALL_METHOD(wip->edata.dtls_conn, srtp_recv, wip->pkt);
            break;
        case RTP_OUT:
            RTPP_LOG(rtpp_module.log, RTPP_LOG_DBUG, "DTLS: packet RTP->SRTP");
            CALL_METHOD(wip->edata.dtls_conn, rtp_send, wip->pkt);
            break;
        default:
            abort();
        }
        RTPP_OBJ_DECREF(wip->edata.strmp);
        RTPP_OBJ_DECREF(wip->edata.dtls_conn);
        RTPP_OBJ_DECREF(wip->pkt);
        CALL_METHOD(wi, dtor);
    }
}

static void
dtls_gw_data_dtor(struct dtls_gw_stream_cfg *pvt)
{

    RTPP_OBJ_DECREF(pvt->dtls_conn);
    free(pvt);
    RC_DECREF(rtpp_module.module_rcnt);
}

static struct dtls_gw_stream_cfg *
dtls_gw_data_ctor(struct rtpp_module_priv *pvt, struct rtpp_stream *dtls_strmp,
  struct rtpp_stream *rtp_strmp)
{
    struct dtls_gw_stream_cfg *rtps_c;

    rtps_c = mod_rzmalloc(sizeof(*rtps_c), offsetof(struct dtls_gw_stream_cfg, rcnt));
    if (rtps_c == NULL) {
        goto e0;
    }
    rtps_c->dtls_conn = CALL_METHOD(pvt->dtls_ctx, newconn, dtls_strmp,
      rtp_strmp);
    if (rtps_c->dtls_conn == NULL) {
        goto e1;
    }
    RC_INCREF(rtpp_module.module_rcnt);
    CALL_SMETHOD(rtps_c->rcnt, attach, (rtpp_refcnt_dtor_t)dtls_gw_data_dtor, rtps_c);
    return (rtps_c);
e1:
    mod_free(rtps_c);
e0:
    return (NULL);
}

static int
rtpp_dtls_gw_setup_sender(struct rtpp_module_priv *pvt,
  struct rtpp_session *spa, struct rtpp_stream *dtls_strmp)
{
    int sidx, lport;
    struct rtpp_socket *fd, *fds[2];

    fd = CALL_SMETHOD(dtls_strmp, get_skt);
    if (fd != NULL) {
        RTPP_OBJ_DECREF(fd);
        return (0);
    }

    if (spa->rtp->stream[0] == dtls_strmp) {
        sidx = 0;
    } else if (spa->rtp->stream[1] == dtls_strmp) {
        sidx = 1;
    } else {
        abort();
    }

    if (rtpp_create_listener(pvt->cfsp, dtls_strmp->laddr, &lport, fds) == -1)
        return (-1);
    CALL_METHOD(pvt->cfsp->sessinfo, append, spa, sidx, fds);
    CALL_METHOD(pvt->cfsp->rtpp_proc_cf, nudge);
    RTPP_OBJ_DECREF(fds[0]);
    RTPP_OBJ_DECREF(fds[1]);
    dtls_strmp->port = lport;
    spa->rtcp->stream[sidx]->port = lport + 1;
    if (spa->complete == 0) {
        CALL_SMETHOD(pvt->cfsp->rtpp_stats, updatebyname, "nsess_complete", 1);
        CALL_METHOD(spa->rtp->stream[0]->ttl, reset_with,
          pvt->cfsp->max_ttl);
        CALL_METHOD(spa->rtp->stream[1]->ttl, reset_with,
          pvt->cfsp->max_ttl);
    }
    spa->complete = 1;
    return (0);
}

static int
rtpp_dtls_gw_handle_command(struct rtpp_module_priv *pvt,
  const struct rtpp_subc_ctx *ctxp)
{
    struct rtpp_refcnt *rtps_cnt;
    struct dtls_gw_stream_cfg *rtps_c;
    _Atomic(struct rtpp_refcnt *) *dtls_gw_datap;
    enum rtpp_dtls_mode my_mode;
    struct rdc_peer_spec rdfs, *rdfsp;
    char * const *argv = &(ctxp->subc_args->v[1]);
    int argc = ctxp->subc_args->c - 1;
    struct rtpp_stream *dtls_strmp, *rtp_strmp;
    int rlen;
    char *rcp;

    if (argc != 1 && argc != 3 && argc != 4) {
        RTPP_LOG(rtpp_module.log, RTPP_LOG_DBUG, "expected 1, 3 or 4 parameters: %d",
          argc);
        return (-1);
    }

    switch (argv[0][0] | argv[0][1]) {
    case 'a':
    case 'A':
        if (argc != 3 && argc != 4)
            goto invalmode;
        rdfs.peer_mode = RTPP_DTLS_ACTIVE;
        break;

    case 'p':
    case 'P':
        if (argc != 3 && argc != 4)
            goto invalmode;
        rdfs.peer_mode = RTPP_DTLS_PASSIVE;
        break;

    case 's':
    case 'S':
        if (argc != 1)
            goto invalmode;
        break;

    default:
        goto invalmode;
    }

    if (argc != 1) {
        rdfs.algorithm = argv[1];
        rdfs.fingerprint = argv[2];
        rdfs.ssrc = (argc == 4) ? argv[3] : NULL;
        rdfsp = &rdfs;
        dtls_strmp = ctxp->strmp_in;
        rtp_strmp = ctxp->strmp_out;
    } else {
        rdfsp = NULL;
        dtls_strmp = ctxp->strmp_out;
        rtp_strmp = ctxp->strmp_in;
    }

    dtls_gw_datap = &(dtls_strmp->pmod_datap->adp[rtpp_module.ids->module_idx]);
    rtps_cnt = atomic_load(dtls_gw_datap);
    if (rtps_cnt == NULL) {
        rtps_c = dtls_gw_data_ctor(pvt, dtls_strmp, rtp_strmp);
        if (rtps_c == NULL) {
            return (-1);
        }
        if (!atomic_compare_exchange_strong(dtls_gw_datap,
          &rtps_cnt, rtps_c->rcnt)) {
            RTPP_OBJ_DECREF(rtps_c);
            rtps_c = CALL_SMETHOD(rtps_cnt, getdata);
        }
    } else {
        rtps_c = CALL_SMETHOD(rtps_cnt, getdata);
    }
    if (rdfsp != NULL && rdfs.peer_mode == RTPP_DTLS_PASSIVE) {
        if (rtpp_dtls_gw_setup_sender(pvt, ctxp->sessp, dtls_strmp) != 0) {
            return (-1);
        }
    }
    my_mode = CALL_METHOD(rtps_c->dtls_conn, setmode, rdfsp);
    if (my_mode == RTPP_DTLS_MODERR) {
        return (-1);
    }
    if (rdfsp != NULL) {
        return (0);
    }

    rcp = ctxp->resp->buf_t;
    rlen = sizeof(ctxp->resp->buf_t);

    switch (my_mode) {
    case RTPP_DTLS_ACTPASS:
        strlcpy(rcp, "actpass ", rlen);
        rcp += strlen("actpass ");
        rlen -= strlen("actpass ");
        break;

    case RTPP_DTLS_ACTIVE:
        strlcpy(rcp, "active ", rlen);
        rcp += strlen("active ");
        rlen -= strlen("active ");
        break;

    case RTPP_DTLS_PASSIVE:
        strlcpy(rcp, "passive ", rlen);
        rcp += strlen("passive ");
        rlen -= strlen("passive ");
        break;

    default:
        abort();
    }

    strlcpy(rcp, pvt->dtls_ctx->fingerprint, rlen);
    return (0);

invalmode:
    RTPP_LOG(rtpp_module.log, RTPP_LOG_ERR, "invalid mode: \"%s\"",
      argv[0]);
    return (-1);
}

static int
rtp_packet_is_dtls(struct pkt_proc_ctx *pktx)
{
    struct rtpp_refcnt *rtps_cnt;
    struct dtls_gw_stream_cfg *rtps_c;
    _Atomic(struct rtpp_refcnt *) *dtls_gw_datap;
    static __thread struct rtpp_dtls_gw_aux dtls_in = {.direction = DTLS_IN};
    static __thread struct rtpp_dtls_gw_aux strp_in = {.direction = SRTP_IN};
    static __thread struct rtpp_dtls_gw_aux rtp_out = {.direction = RTP_OUT};
    struct rtpp_dtls_gw_aux *rdgap;

    dtls_gw_datap = &(pktx->strmp_in->pmod_datap->adp[rtpp_module.ids->module_idx]);
    rtps_cnt = atomic_load(dtls_gw_datap);
    if (rtps_cnt != NULL) {
        if (!is_dtls_packet(pktx->pktp))
            rdgap = &strp_in;
        else
            rdgap = &dtls_in;
        rtps_c = CALL_SMETHOD(rtps_cnt, getdata);
        rdgap->dtls_conn = rtps_c->dtls_conn;
        rdgap->strmp = pktx->strmp_in;
        pktx->auxp = rdgap;
        return (1);
    }
    if (pktx->strmp_out == NULL)
        return (0);
    dtls_gw_datap = &(pktx->strmp_out->pmod_datap->adp[rtpp_module.ids->module_idx]);
    rtps_cnt = atomic_load(dtls_gw_datap);
    if (rtps_cnt != NULL) {
        rtps_c = CALL_SMETHOD(rtps_cnt, getdata);
        rtp_out.dtls_conn = rtps_c->dtls_conn;
        rtp_out.strmp = pktx->strmp_out;
        pktx->auxp = &rtp_out;
        return (1);
    }

    return (0);
}

static enum pproc_action
rtpp_dtls_gw_enqueue(void *arg, const struct pkt_proc_ctx *pktx)
{
    struct rtpp_dtls_gw_aux *edata;
    struct rtpp_wi *wi;
    struct wipkt *wip;

    edata = (struct rtpp_dtls_gw_aux *)pktx->auxp;
    wi = rtpp_wi_malloc_udata((void **)&wip, sizeof(struct wipkt));
    if (wi == NULL)
        return (PPROC_TAKE);
    RTPP_OBJ_INCREF(pktx->pktp);
    wip->edata = *edata;
    RTPP_OBJ_INCREF(edata->dtls_conn);
    wip->pkt = pktx->pktp;
    RTPP_OBJ_INCREF(edata->strmp);
    rtpp_queue_put_item(wi, rtpp_module.wthr.mod_q);

    return (PPROC_TAKE);
}

static struct rtpp_module_priv *
rtpp_dtls_gw_ctor(const struct rtpp_cfg *cfsp)
{
    struct rtpp_module_priv *pvt;
    struct packet_processor_if dtls_poi;

    pvt = mod_zmalloc(sizeof(struct rtpp_module_priv));
    if (pvt == NULL) {
        goto e0;
    }
    pvt->dtls_ctx = rtpp_dtls_ctor(cfsp);
    if (pvt->dtls_ctx == NULL) {
        goto e1;
    }
    if (srtp_init() != 0) {
        goto e2;
    }
    dtls_poi.taste = rtp_packet_is_dtls;
    dtls_poi.enqueue = rtpp_dtls_gw_enqueue;
    dtls_poi.arg = pvt;
    if (CALL_METHOD(cfsp->pproc_manager, reg, &dtls_poi) < 0)
        goto e3;
    pvt->cfsp = cfsp;
    return (pvt);
e3:
    srtp_shutdown();
e2:
    RTPP_OBJ_DECREF(pvt->dtls_ctx);
e1:
    mod_free(pvt);
e0:
    return (NULL);
}

static void
rtpp_dtls_gw_dtor(struct rtpp_module_priv *pvt)
{

    srtp_shutdown();
    RTPP_OBJ_DECREF(pvt->dtls_ctx);
    mod_free(pvt);
    return;
}

static bool
is_dtls_packet(const struct rtp_packet *pktp)
{
    uint8_t b;

    if (pktp->size < 13)
        return false;

    b = pktp->data.buf[0];

    return (19 < b && b < 64);
}
