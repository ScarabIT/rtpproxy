/* Auto-generated by genfincode_stat.sh - DO NOT EDIT! */
#if !defined(_rtpp_timed_fin_h)
#define _rtpp_timed_fin_h
#if !defined(RTPP_AUTOTRAP)
#define RTPP_AUTOTRAP() abort()
#else
extern int _naborts;
#endif
void rtpp_timed_fin(struct rtpp_timed *);
#if defined(RTPP_FINTEST)
void rtpp_timed_fintest(void);
#endif /* RTPP_FINTEST */
#endif /* _rtpp_timed_fin_h */