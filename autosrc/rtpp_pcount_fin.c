/* Auto-generated by genfincode_stat.sh - DO NOT EDIT! */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#define RTPP_FINCODE
#include "rtpp_types.h"
#include "rtpp_debug.h"
#include "rtpp_pcount.h"
#include "rtpp_pcount_fin.h"
static void rtpp_pcount_get_stats_fin(void *pub) {
    fprintf(stderr, "Method rtpp_pcount@%p::get_stats (rtpp_pcount_get_stats) is invoked after destruction\x0a", pub);
    RTPP_AUTOTRAP();
}
static void rtpp_pcount_log_drops_fin(void *pub) {
    fprintf(stderr, "Method rtpp_pcount@%p::log_drops (rtpp_pcount_log_drops) is invoked after destruction\x0a", pub);
    RTPP_AUTOTRAP();
}
static void rtpp_pcount_reg_drop_fin(void *pub) {
    fprintf(stderr, "Method rtpp_pcount@%p::reg_drop (rtpp_pcount_reg_drop) is invoked after destruction\x0a", pub);
    RTPP_AUTOTRAP();
}
static void rtpp_pcount_reg_ignr_fin(void *pub) {
    fprintf(stderr, "Method rtpp_pcount@%p::reg_ignr (rtpp_pcount_reg_ignr) is invoked after destruction\x0a", pub);
    RTPP_AUTOTRAP();
}
static void rtpp_pcount_reg_reld_fin(void *pub) {
    fprintf(stderr, "Method rtpp_pcount@%p::reg_reld (rtpp_pcount_reg_reld) is invoked after destruction\x0a", pub);
    RTPP_AUTOTRAP();
}
static const struct rtpp_pcount_smethods rtpp_pcount_smethods_fin = {
    .get_stats = (rtpp_pcount_get_stats_t)&rtpp_pcount_get_stats_fin,
    .log_drops = (rtpp_pcount_log_drops_t)&rtpp_pcount_log_drops_fin,
    .reg_drop = (rtpp_pcount_reg_drop_t)&rtpp_pcount_reg_drop_fin,
    .reg_ignr = (rtpp_pcount_reg_ignr_t)&rtpp_pcount_reg_ignr_fin,
    .reg_reld = (rtpp_pcount_reg_reld_t)&rtpp_pcount_reg_reld_fin,
};
void rtpp_pcount_fin(struct rtpp_pcount *pub) {
    RTPP_DBG_ASSERT(pub->smethods->get_stats != (rtpp_pcount_get_stats_t)NULL);
    RTPP_DBG_ASSERT(pub->smethods->log_drops != (rtpp_pcount_log_drops_t)NULL);
    RTPP_DBG_ASSERT(pub->smethods->reg_drop != (rtpp_pcount_reg_drop_t)NULL);
    RTPP_DBG_ASSERT(pub->smethods->reg_ignr != (rtpp_pcount_reg_ignr_t)NULL);
    RTPP_DBG_ASSERT(pub->smethods->reg_reld != (rtpp_pcount_reg_reld_t)NULL);
    RTPP_DBG_ASSERT(pub->smethods != &rtpp_pcount_smethods_fin &&
      pub->smethods != NULL);
    pub->smethods = &rtpp_pcount_smethods_fin;
}
#if defined(RTPP_FINTEST)
#include <assert.h>
#include <stddef.h>
#include "rtpp_mallocs.h"
#include "rtpp_refcnt.h"
#include "rtpp_linker_set.h"
#define CALL_TFIN(pub, fn) ((void (*)(typeof(pub)))((pub)->smethods->fn))(pub)

void
rtpp_pcount_fintest()
{
    int naborts_s;

    struct {
        struct rtpp_pcount pub;
    } *tp;

    naborts_s = _naborts;
    tp = rtpp_rzmalloc(sizeof(*tp), offsetof(typeof(*tp), pub.rcnt));
    assert(tp != NULL);
    assert(tp->pub.rcnt != NULL);
    static const struct rtpp_pcount_smethods dummy = {
        .get_stats = (rtpp_pcount_get_stats_t)((void *)0x1),
        .log_drops = (rtpp_pcount_log_drops_t)((void *)0x1),
        .reg_drop = (rtpp_pcount_reg_drop_t)((void *)0x1),
        .reg_ignr = (rtpp_pcount_reg_ignr_t)((void *)0x1),
        .reg_reld = (rtpp_pcount_reg_reld_t)((void *)0x1),
    };
    tp->pub.smethods = &dummy;
    CALL_SMETHOD(tp->pub.rcnt, attach, (rtpp_refcnt_dtor_t)&rtpp_pcount_fin,
      &tp->pub);
    RTPP_OBJ_DECREF(&(tp->pub));
    CALL_TFIN(&tp->pub, get_stats);
    CALL_TFIN(&tp->pub, log_drops);
    CALL_TFIN(&tp->pub, reg_drop);
    CALL_TFIN(&tp->pub, reg_ignr);
    CALL_TFIN(&tp->pub, reg_reld);
    assert((_naborts - naborts_s) == 5);
    free(tp);
}
const static void *_rtpp_pcount_ftp = (void *)&rtpp_pcount_fintest;
DATA_SET(rtpp_fintests, _rtpp_pcount_ftp);
#endif /* RTPP_FINTEST */
