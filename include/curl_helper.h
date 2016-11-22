#ifndef CURL_HELPER_H
#define CURL_HELPER_H
#include <curl/curl.h>

typedef struct cstring {
  char *ptr;
  size_t len;
} CSTRING;


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus



extern void init_string(struct cstring *s);
extern void free_string(struct cstring* s);
extern size_t writeone(void *ptr, size_t size, size_t nmemb, struct cstring *s);
extern size_t writefunc(void *ptr, size_t size, size_t nmemb, struct cstring *s);
extern size_t print_one(void *ptr, size_t size, size_t nmemb, struct cstring *s);
extern void print_cookies(CURL *curl);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // CURL_HELPER_H
