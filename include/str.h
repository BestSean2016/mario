#ifndef STR_H
#define STR_H

#include "itat_global.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#ifdef _UNIX
extern ITAT_API char* strcat_s(char* buf, const size_t buflen, const char* src);
extern ITAT_API char* strcpy_s(char* buf, const size_t buflen, const char* str);
extern ITAT_API char* strncpy_s(char* buf, const size_t buflen, const char* str, const int n);
extern ITAT_API void itoa(unsigned long val, char *buf, unsigned radix);
extern ITAT_API int fopen_s(FILE** fp, const char* filepath, const char* mode);
extern ITAT_API void localtime_s(struct tm* ttm, time_t* tt);

#endif //UNIX

extern ITAT_API char* strtrim(char* buf);
extern ITAT_API char* strltrim(char* buf);
extern ITAT_API char* strrtrim(char* buf);
extern ITAT_API void itoa_s(int num, char* buf, size_t len, int base);

#ifdef __cplusplus
}
#endif //__cplusplus
#endif // STR_H
