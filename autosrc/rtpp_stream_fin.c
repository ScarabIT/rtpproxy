/* Auto-generated by genfincode_stat.sh - DO NOT EDIT! */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#define RTPP_FINCODE
#include "rtpp_types.h"
#include "rtpp_debug.h"
#include "rtpp_stream.h"
#include "rtpp_stream_fin.h"
static void rtpp_stream_finish_playback_fin(void *pub) {
    fprintf(stderr, "Method rtpp_stream@%p::finish_playback (rtpp_stream_finish_playback) is invoked after destruction\x0a", pub);
    RTPP_AUTOTRAP();
}
static void rtpp_stream_get_actor_fin(void *pub) {
    fprintf(stderr, "Method rtpp_stream@%p::get_actor (rtpp_stream_get_actor) is invoked after destruction\x0a", pub);
    RTPP_AUTOTRAP();
}
static void rtpp_stream_get_proto_fin(void *pub) {
    fprintf(stderr, "Method rtpp_stream@%p::get_proto (rtpp_stream_get_proto) is invoked after destruction\x0a", pub);
    RTPP_AUTOTRAP();
}
static void rtpp_stream_get_rem_addr_fin(void *pub) {
    fprintf(stderr, "Method rtpp_stream@%p::get_rem_addr (rtpp_stream_get_rem_addr) is invoked after destruction\x0a", pub);
    RTPP_AUTOTRAP();
}
static void rtpp_stream_get_sender_fin(void *pub) {
    fprintf(stderr, "Method rtpp_stream@%p::get_sender (rtpp_stream_get_sender) is invoked after destruction\x0a", pub);
    RTPP_AUTOTRAP();
}
static void rtpp_stream_get_skt_fin(void *pub) {
    fprintf(stderr, "Method rtpp_stream@%p::get_skt (rtpp_stream_get_skt) is invoked after destruction\x0a", pub);
    RTPP_AUTOTRAP();
}
static void rtpp_stream_get_stats_fin(void *pub) {
    fprintf(stderr, "Method rtpp_stream@%p::get_stats (rtpp_stream_get_stats) is invoked after destruction\x0a", pub);
    RTPP_AUTOTRAP();
}
static void rtpp_stream_guess_addr_fin(void *pub) {
    fprintf(stderr, "Method rtpp_stream@%p::guess_addr (rtpp_stream_guess_addr) is invoked after destruction\x0a", pub);
    RTPP_AUTOTRAP();
}
static void rtpp_stream_handle_noplay_fin(void *pub) {
    fprintf(stderr, "Method rtpp_stream@%p::handle_noplay (rtpp_stream_handle_noplay) is invoked after destruction\x0a", pub);
    RTPP_AUTOTRAP();
}
static void rtpp_stream_handle_play_fin(void *pub) {
    fprintf(stderr, "Method rtpp_stream@%p::handle_play (rtpp_stream_handle_play) is invoked after destruction\x0a", pub);
    RTPP_AUTOTRAP();
}
static void rtpp_stream_isplayer_active_fin(void *pub) {
    fprintf(stderr, "Method rtpp_stream@%p::isplayer_active (rtpp_stream_isplayer_active) is invoked after destruction\x0a", pub);
    RTPP_AUTOTRAP();
}
static void rtpp_stream_issendable_fin(void *pub) {
    fprintf(stderr, "Method rtpp_stream@%p::issendable (rtpp_stream_issendable) is invoked after destruction\x0a", pub);
    RTPP_AUTOTRAP();
}
static void rtpp_stream_latch_fin(void *pub) {
    fprintf(stderr, "Method rtpp_stream@%p::latch (rtpp_stream_latch) is invoked after destruction\x0a", pub);
    RTPP_AUTOTRAP();
}
static void rtpp_stream_latch_getmode_fin(void *pub) {
    fprintf(stderr, "Method rtpp_stream@%p::latch_getmode (rtpp_stream_latch_getmode) is invoked after destruction\x0a", pub);
    RTPP_AUTOTRAP();
}
static void rtpp_stream_latch_setmode_fin(void *pub) {
    fprintf(stderr, "Method rtpp_stream@%p::latch_setmode (rtpp_stream_latch_setmode) is invoked after destruction\x0a", pub);
    RTPP_AUTOTRAP();
}
static void rtpp_stream_locklatch_fin(void *pub) {
    fprintf(stderr, "Method rtpp_stream@%p::locklatch (rtpp_stream_locklatch) is invoked after destruction\x0a", pub);
    RTPP_AUTOTRAP();
}
static void rtpp_stream_prefill_addr_fin(void *pub) {
    fprintf(stderr, "Method rtpp_stream@%p::prefill_addr (rtpp_stream_prefill_addr) is invoked after destruction\x0a", pub);
    RTPP_AUTOTRAP();
}
static void rtpp_stream_reg_onhold_fin(void *pub) {
    fprintf(stderr, "Method rtpp_stream@%p::reg_onhold (rtpp_stream_reg_onhold) is invoked after destruction\x0a", pub);
    RTPP_AUTOTRAP();
}
static void rtpp_stream_rx_fin(void *pub) {
    fprintf(stderr, "Method rtpp_stream@%p::rx (rtpp_stream_rx) is invoked after destruction\x0a", pub);
    RTPP_AUTOTRAP();
}
static void rtpp_stream_send_pkt_fin(void *pub) {
    fprintf(stderr, "Method rtpp_stream@%p::send_pkt (rtpp_stream_send_pkt) is invoked after destruction\x0a", pub);
    RTPP_AUTOTRAP();
}
static void rtpp_stream_send_pkt_to_fin(void *pub) {
    fprintf(stderr, "Method rtpp_stream@%p::send_pkt_to (rtpp_stream_send_pkt_to) is invoked after destruction\x0a", pub);
    RTPP_AUTOTRAP();
}
static void rtpp_stream_set_skt_fin(void *pub) {
    fprintf(stderr, "Method rtpp_stream@%p::set_skt (rtpp_stream_set_skt) is invoked after destruction\x0a", pub);
    RTPP_AUTOTRAP();
}
static void rtpp_stream_update_skt_fin(void *pub) {
    fprintf(stderr, "Method rtpp_stream@%p::update_skt (rtpp_stream_update_skt) is invoked after destruction\x0a", pub);
    RTPP_AUTOTRAP();
}
static const struct rtpp_stream_smethods rtpp_stream_smethods_fin = {
    .finish_playback = (rtpp_stream_finish_playback_t)&rtpp_stream_finish_playback_fin,
    .get_actor = (rtpp_stream_get_actor_t)&rtpp_stream_get_actor_fin,
    .get_proto = (rtpp_stream_get_proto_t)&rtpp_stream_get_proto_fin,
    .get_rem_addr = (rtpp_stream_get_rem_addr_t)&rtpp_stream_get_rem_addr_fin,
    .get_sender = (rtpp_stream_get_sender_t)&rtpp_stream_get_sender_fin,
    .get_skt = (rtpp_stream_get_skt_t)&rtpp_stream_get_skt_fin,
    .get_stats = (rtpp_stream_get_stats_t)&rtpp_stream_get_stats_fin,
    .guess_addr = (rtpp_stream_guess_addr_t)&rtpp_stream_guess_addr_fin,
    .handle_noplay = (rtpp_stream_handle_noplay_t)&rtpp_stream_handle_noplay_fin,
    .handle_play = (rtpp_stream_handle_play_t)&rtpp_stream_handle_play_fin,
    .isplayer_active = (rtpp_stream_isplayer_active_t)&rtpp_stream_isplayer_active_fin,
    .issendable = (rtpp_stream_issendable_t)&rtpp_stream_issendable_fin,
    .latch = (rtpp_stream_latch_t)&rtpp_stream_latch_fin,
    .latch_getmode = (rtpp_stream_latch_getmode_t)&rtpp_stream_latch_getmode_fin,
    .latch_setmode = (rtpp_stream_latch_setmode_t)&rtpp_stream_latch_setmode_fin,
    .locklatch = (rtpp_stream_locklatch_t)&rtpp_stream_locklatch_fin,
    .prefill_addr = (rtpp_stream_prefill_addr_t)&rtpp_stream_prefill_addr_fin,
    .reg_onhold = (rtpp_stream_reg_onhold_t)&rtpp_stream_reg_onhold_fin,
    .rx = (rtpp_stream_rx_t)&rtpp_stream_rx_fin,
    .send_pkt = (rtpp_stream_send_pkt_t)&rtpp_stream_send_pkt_fin,
    .send_pkt_to = (rtpp_stream_send_pkt_to_t)&rtpp_stream_send_pkt_to_fin,
    .set_skt = (rtpp_stream_set_skt_t)&rtpp_stream_set_skt_fin,
    .update_skt = (rtpp_stream_update_skt_t)&rtpp_stream_update_skt_fin,
};
void rtpp_stream_fin(struct rtpp_stream *pub) {
    RTPP_DBG_ASSERT(pub->smethods->finish_playback != (rtpp_stream_finish_playback_t)NULL);
    RTPP_DBG_ASSERT(pub->smethods->get_actor != (rtpp_stream_get_actor_t)NULL);
    RTPP_DBG_ASSERT(pub->smethods->get_proto != (rtpp_stream_get_proto_t)NULL);
    RTPP_DBG_ASSERT(pub->smethods->get_rem_addr != (rtpp_stream_get_rem_addr_t)NULL);
    RTPP_DBG_ASSERT(pub->smethods->get_sender != (rtpp_stream_get_sender_t)NULL);
    RTPP_DBG_ASSERT(pub->smethods->get_skt != (rtpp_stream_get_skt_t)NULL);
    RTPP_DBG_ASSERT(pub->smethods->get_stats != (rtpp_stream_get_stats_t)NULL);
    RTPP_DBG_ASSERT(pub->smethods->guess_addr != (rtpp_stream_guess_addr_t)NULL);
    RTPP_DBG_ASSERT(pub->smethods->handle_noplay != (rtpp_stream_handle_noplay_t)NULL);
    RTPP_DBG_ASSERT(pub->smethods->handle_play != (rtpp_stream_handle_play_t)NULL);
    RTPP_DBG_ASSERT(pub->smethods->isplayer_active != (rtpp_stream_isplayer_active_t)NULL);
    RTPP_DBG_ASSERT(pub->smethods->issendable != (rtpp_stream_issendable_t)NULL);
    RTPP_DBG_ASSERT(pub->smethods->latch != (rtpp_stream_latch_t)NULL);
    RTPP_DBG_ASSERT(pub->smethods->latch_getmode != (rtpp_stream_latch_getmode_t)NULL);
    RTPP_DBG_ASSERT(pub->smethods->latch_setmode != (rtpp_stream_latch_setmode_t)NULL);
    RTPP_DBG_ASSERT(pub->smethods->locklatch != (rtpp_stream_locklatch_t)NULL);
    RTPP_DBG_ASSERT(pub->smethods->prefill_addr != (rtpp_stream_prefill_addr_t)NULL);
    RTPP_DBG_ASSERT(pub->smethods->reg_onhold != (rtpp_stream_reg_onhold_t)NULL);
    RTPP_DBG_ASSERT(pub->smethods->rx != (rtpp_stream_rx_t)NULL);
    RTPP_DBG_ASSERT(pub->smethods->send_pkt != (rtpp_stream_send_pkt_t)NULL);
    RTPP_DBG_ASSERT(pub->smethods->send_pkt_to != (rtpp_stream_send_pkt_to_t)NULL);
    RTPP_DBG_ASSERT(pub->smethods->set_skt != (rtpp_stream_set_skt_t)NULL);
    RTPP_DBG_ASSERT(pub->smethods->update_skt != (rtpp_stream_update_skt_t)NULL);
    RTPP_DBG_ASSERT(pub->smethods != &rtpp_stream_smethods_fin &&
      pub->smethods != NULL);
    pub->smethods = &rtpp_stream_smethods_fin;
}
#if defined(RTPP_FINTEST)
#include <assert.h>
#include <stddef.h>
#include "rtpp_mallocs.h"
#include "rtpp_refcnt.h"
#include "rtpp_linker_set.h"
#define CALL_TFIN(pub, fn) ((void (*)(typeof(pub)))((pub)->smethods->fn))(pub)

void
rtpp_stream_fintest()
{
    int naborts_s;

    struct {
        struct rtpp_stream pub;
    } *tp;

    naborts_s = _naborts;
    tp = rtpp_rzmalloc(sizeof(*tp), offsetof(typeof(*tp), pub.rcnt));
    assert(tp != NULL);
    assert(tp->pub.rcnt != NULL);
    static const struct rtpp_stream_smethods dummy = {
        .finish_playback = (rtpp_stream_finish_playback_t)((void *)0x1),
        .get_actor = (rtpp_stream_get_actor_t)((void *)0x1),
        .get_proto = (rtpp_stream_get_proto_t)((void *)0x1),
        .get_rem_addr = (rtpp_stream_get_rem_addr_t)((void *)0x1),
        .get_sender = (rtpp_stream_get_sender_t)((void *)0x1),
        .get_skt = (rtpp_stream_get_skt_t)((void *)0x1),
        .get_stats = (rtpp_stream_get_stats_t)((void *)0x1),
        .guess_addr = (rtpp_stream_guess_addr_t)((void *)0x1),
        .handle_noplay = (rtpp_stream_handle_noplay_t)((void *)0x1),
        .handle_play = (rtpp_stream_handle_play_t)((void *)0x1),
        .isplayer_active = (rtpp_stream_isplayer_active_t)((void *)0x1),
        .issendable = (rtpp_stream_issendable_t)((void *)0x1),
        .latch = (rtpp_stream_latch_t)((void *)0x1),
        .latch_getmode = (rtpp_stream_latch_getmode_t)((void *)0x1),
        .latch_setmode = (rtpp_stream_latch_setmode_t)((void *)0x1),
        .locklatch = (rtpp_stream_locklatch_t)((void *)0x1),
        .prefill_addr = (rtpp_stream_prefill_addr_t)((void *)0x1),
        .reg_onhold = (rtpp_stream_reg_onhold_t)((void *)0x1),
        .rx = (rtpp_stream_rx_t)((void *)0x1),
        .send_pkt = (rtpp_stream_send_pkt_t)((void *)0x1),
        .send_pkt_to = (rtpp_stream_send_pkt_to_t)((void *)0x1),
        .set_skt = (rtpp_stream_set_skt_t)((void *)0x1),
        .update_skt = (rtpp_stream_update_skt_t)((void *)0x1),
    };
    tp->pub.smethods = &dummy;
    CALL_SMETHOD(tp->pub.rcnt, attach, (rtpp_refcnt_dtor_t)&rtpp_stream_fin,
      &tp->pub);
    RTPP_OBJ_DECREF(&(tp->pub));
    CALL_TFIN(&tp->pub, finish_playback);
    CALL_TFIN(&tp->pub, get_actor);
    CALL_TFIN(&tp->pub, get_proto);
    CALL_TFIN(&tp->pub, get_rem_addr);
    CALL_TFIN(&tp->pub, get_sender);
    CALL_TFIN(&tp->pub, get_skt);
    CALL_TFIN(&tp->pub, get_stats);
    CALL_TFIN(&tp->pub, guess_addr);
    CALL_TFIN(&tp->pub, handle_noplay);
    CALL_TFIN(&tp->pub, handle_play);
    CALL_TFIN(&tp->pub, isplayer_active);
    CALL_TFIN(&tp->pub, issendable);
    CALL_TFIN(&tp->pub, latch);
    CALL_TFIN(&tp->pub, latch_getmode);
    CALL_TFIN(&tp->pub, latch_setmode);
    CALL_TFIN(&tp->pub, locklatch);
    CALL_TFIN(&tp->pub, prefill_addr);
    CALL_TFIN(&tp->pub, reg_onhold);
    CALL_TFIN(&tp->pub, rx);
    CALL_TFIN(&tp->pub, send_pkt);
    CALL_TFIN(&tp->pub, send_pkt_to);
    CALL_TFIN(&tp->pub, set_skt);
    CALL_TFIN(&tp->pub, update_skt);
    assert((_naborts - naborts_s) == 23);
    free(tp);
}
const static void *_rtpp_stream_ftp = (void *)&rtpp_stream_fintest;
DATA_SET(rtpp_fintests, _rtpp_stream_ftp);
#endif /* RTPP_FINTEST */
