#ifndef CHINESE_H
#define CHINESE_H

#ifdef max
#undef max
#endif // define max

#ifdef min
#undef min
#endif // define min

#define max(a, b)                                                              \
  ({                                                                           \
    __typeof__(a) _a = (a);                                                    \
    __typeof__(b) _b = (b);                                                    \
    _a > _b ? _a : _b;                                                         \
  })

#define min(a, b)                                                              \
  ({                                                                           \
    __typeof__(a) _a = (a);                                                    \
    __typeof__(b) _b = (b);                                                    \
    _a < _b ? _a : _b;                                                         \
  })

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

extern void dumpbuf(const char *tip, char *p, size_t len);
extern int convert(char *src, char *des, int srclen, int deslen,
                   const char *srctype, const char *destype);
extern int utf8_2_webucs2(char *src, char *des, size_t srclen, size_t deslen,
                          const char *prefix, const char *postfix);

extern int gbk_2_webucs2(char *src, char *des, size_t srclen, size_t deslen,
                         const char *prefix, const char *postfix);

extern int webucs_2_gbk(char *src, char *des, size_t srclen, size_t deslen,
                        const char *prefix, const char *postfix);

extern int webucs_2_utf8(char *src, char *des, size_t srclen, size_t deslen,
                         const char *prefix, const char *postfix);


extern int webucs16_2_gbk(char *src, char *des, size_t srclen, size_t deslen,
                        const char *prefix, const char *postfix);

extern int webucs16_2_utf8(char *src, char *des, size_t srclen, size_t deslen,
                         const char *prefix, const char *postfix);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // CHINESE_H
