#include <errno.h>
#include <error.h>
#include <iconv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chinese.h"

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

void dumpbuf(const char *tip, char *p, size_t len) {
  size_t i = 0;

  if (tip)
    printf("%s: ", tip);
  for (; i < len; i++) {
    printf("%02X ", (unsigned char)p[i]);
    if (!(i + 1) % 16)
      printf("\n");
  }
  printf("\n");
}

int convert(char *src, char *des, int srclen, int deslen, const char *srctype,
            const char *destype) {
  iconv_t conv = 0;
  if (strcmp(srctype, destype) == 0) {
    memcpy(des, src, min(srclen, deslen));
    return 0;
  }
  conv = iconv_open(destype, srctype);
  if (conv == (iconv_t)-1) {
    printf("iconvopen err\n");
    return -1;
  }

  size_t avail = deslen;
  size_t insize = srclen;
  char *wrptr = des;
  char *inptr = src;
  int ret = 0;

  {
    size_t nconv;
    // printf("avail:%lu\n", avail);
    /* Do the conversion.  */
    nconv = iconv(conv, &inptr, &insize, &wrptr, &avail);
    if (nconv == (size_t)-1) {
      /* Not everything went right.  It might only be
         an unfinished byte sequence at the end of the
         buffer.  Or it is a real problem.  */
      if (errno == EINVAL) {
        /* This is harmless.  Simply move the unused
           bytes to the beginning of the buffer so that
           they can be used in the next round.  */
        // memmove (inbuf, inptr, insize);
        // printf("EINVAL\n");
      } else {
        /* It is a real problem.  Maybe we ran out of
           space in the output buffer or we have invalid
           input.  In any case back the file pointer to
           the position of the last processed byte.  */
        ret = errno;
        // printf("error %d, %s\n", errno, strerror(errno));
      }
    }
  }
  iconv_close(conv);
  return ret;
}

int utf8_2_webucs2(char *src, char *des, size_t srclen, size_t deslen,
                   const char *prefix, const char *postfix) {
  char *tmp = 0;
  int ret = 0;
  char *srcptr = src;

  int dlen = 0, ulen = 0;
  while (*srcptr) {
    if (*srcptr > 0)
      ++dlen;
    else
      ++ulen;
    ++srcptr;
  }

  // 5bytes for digital in base10, 4bytes for base16
  // each 'word' has 5 + len(prefix) + len(postfix) bytes
  // and all word has dlen + ulen/3
  // and plus one zero for end string
  if (deslen < (5 + strlen(prefix) + strlen(postfix)) * (dlen + ulen / 3 + 1))
    return -1000;

  // 2 bytes for base16
  tmp = (char *)calloc(sizeof(char), (dlen + ulen + 1) * 2);
  ret = convert(src, tmp, srclen, srclen * 4, "UTF-8", "UCS-2");
  // dumpbuf("a??", tmp, (dlen + ulen + 1) *  4);
  if (ret == 0) {
    unsigned short int *intptr = (unsigned short int *)tmp;
    char *desptr = des;

    memset(des, 0, deslen);

    while (*intptr) {
      // dumpbuf("int", intptr, sizeof(short int));
      if (*intptr > 128)
        snprintf(desptr, 5 + strlen(prefix) + strlen(postfix) + 1, "%s%d%s",
                 prefix, *intptr, postfix);
      else
        snprintf(desptr, 5 + strlen(prefix) + strlen(postfix) + 1, "%c",
                 *(char *)intptr);
      desptr += strlen(desptr);
      ++intptr;
    }
  }
  free(tmp);
  return ret;
}

int gbk_2_webucs2(char *src, char *des, size_t srclen, size_t deslen,
                  const char *prefix, const char *postfix) {
  char *tmp = 0;
  int ret = 0;
  char *srcptr = src;

  int dlen = 0, ulen = 0;
  while (*srcptr) {
    if (*srcptr > 0)
      ++dlen;
    else
      ++ulen;
    ++srcptr;
  }

  // 5bytes for digital in base10
  // each 'word' has 5 + len(prefix) + len(postfix) bytes
  // and all word has dlen + ulen/2
  // and plus one zero for end string
  if (deslen < (5 + strlen(prefix) + strlen(postfix)) * (dlen + ulen / 2 + 1))
    return -1000;

  // 2 bytes for base16
  tmp = (char *)calloc(sizeof(char), (dlen + ulen + 1) * 2);
  ret = convert(src, tmp, srclen, srclen * 4, "GBK", "UCS-2");
  // dumpbuf("a??", tmp, (dlen + ulen + 1) *  4);
  if (ret == 0) {
    unsigned short int *intptr = (unsigned short int *)tmp;
    char *desptr = des;

    memset(des, 0, deslen);

    while (*intptr) {
      // dumpbuf("int", intptr, sizeof(short int));
      if (*intptr > 128)
        snprintf(desptr, 5 + strlen(prefix) + strlen(postfix) + 1, "%s%d%s",
                 prefix, *intptr, postfix);
      else
        snprintf(desptr, 5 + strlen(prefix) + strlen(postfix) + 1, "%c",
                 *(char *)intptr);
      desptr += strlen(desptr);
      ++intptr;
    }
  }
  free(tmp);
  return ret;
}



static int ucs2_2_str(char *src, char *des, size_t srclen, size_t deslen,
                      const char *prefix, const char *postfix, const char* to, const int base) {
    unsigned short int *tmp = 0, *buf = 0;
    int ret = 0;
    char* srcptr = src;
    size_t dlen = 0, ulen = 0;

    // 2 bytes for base16
    tmp = (unsigned short int *)calloc(sizeof(char), srclen + 2);
    buf = tmp;
    while (*srcptr) {
      if (!strncmp(srcptr, prefix, strlen(prefix))) {
        char* postptr = strstr(srcptr, postfix);
        if (!postptr) return (-2);

        srcptr += strlen(prefix);
        *buf = (unsigned short int)strtol(srcptr, NULL, base);
        srcptr = postptr + strlen(postfix);
        ++ulen;
      } else {
        *buf = (unsigned short int)(*srcptr);
        srcptr++;
        ++dlen;
      }

      ++buf;
    }


    if (deslen < (dlen * 2 + ulen * 3)) return -1000;
    memset(des, 0, deslen);
    ret = convert((char*)tmp, des, (dlen + ulen)* 2, deslen, "UCS-2", to);
    free(tmp);
    return ret;

}


int webucs16_2_gbk(char *src, char *des, size_t srclen, size_t deslen,
                        const char *prefix, const char *postfix) {
  return ucs2_2_str(src, des, srclen, deslen, prefix, postfix, "GBK", 16);
}

int webucs16_2_utf8(char *src, char *des, size_t srclen, size_t deslen,
                         const char *prefix, const char *postfix) {
  return ucs2_2_str(src, des, srclen, deslen, prefix, postfix, "UTF8", 16);
}


int webucs_2_gbk(char *src, char *des, size_t srclen, size_t deslen,
                 const char *prefix, const char *postfix) {
  return ucs2_2_str(src, des, srclen, deslen, prefix, postfix, "GBK", 10);
}



int webucs_2_utf8(char *src, char *des, size_t srclen, size_t deslen,
                 const char *prefix, const char *postfix) {
  return ucs2_2_str(src, des, srclen, deslen, prefix, postfix, "UTF8", 10);
}
