#ifndef CURL_HELPER_H
#define CURL_HELPER_H
#include <curl/curl.h>
#include "salt_api.h"

typedef struct cstring {
  char *ptr;
  size_t len;
} CSTRING;

extern char* get_line(const char* buf, int* lines);
extern char* get_line(const char* buf);


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus



extern void init_string(struct cstring *s);
extern void free_string(struct cstring* s);
extern size_t writeone(void *ptr, size_t size, size_t nmemb, struct cstring *s);
extern size_t writefunc(void *ptr, size_t size, size_t nmemb, struct cstring *s);
extern size_t print_one(void *ptr, size_t size, size_t nmemb, struct cstring *s);
extern size_t parse_json(void *ptr, size_t size, size_t nmemb, SALT_JOB* job);

extern void print_cookies(CURL *curl);

extern int curl_salt_event();

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // CURL_HELPER_H
