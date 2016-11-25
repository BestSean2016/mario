#ifndef UTILITY_H
#define UTILITY_H


#include <stdio.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#ifdef _UNIX_
extern char* strcat_s(char* buf, const size_t buflen, const char* src);
extern char* strcpy_s(char* buf, const size_t buflen, const char* str);
extern char* strncpy_s(char* buf, const size_t buflen, const char* str, const int n);
extern void itoa(unsigned long val, char *buf, unsigned radix);
extern int fopen_s(FILE** fp, const char* filepath, const char* mode);
extern void localtime_s(struct tm* ttm, time_t* tt);

#endif //UNIX

extern char* strtrim(char* buf);
extern char* strltrim(char* buf);
extern char* strrtrim(char* buf);
extern void itoa_s(int num, char* buf, size_t len, int base);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // UTILITY_H
