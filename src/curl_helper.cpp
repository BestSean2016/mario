#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "curl_helper.h"

void init_string(struct cstring *s) {
  s->len = 0;
  s->ptr = (char*)malloc(s->len + 1);
  if (s->ptr == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  s->ptr[0] = '\0';
}

void free_string(struct cstring *s) {
  if (s->ptr)
    free(s->ptr);
  s->ptr = 0;
  s->len = 0;
}

size_t writeone(void *ptr, size_t size, size_t nmemb, struct cstring *s) {
  size_t new_len = size * nmemb;
  s->ptr = (char*)realloc(s->ptr, new_len + 1);
  if (s->ptr == NULL) {
    fprintf(stderr, "realloc() failed\n");
    exit(EXIT_FAILURE);
  }
  memcpy(s->ptr, ptr, size * nmemb);
  s->ptr[new_len] = '\0';
  s->len = new_len;

  return size * nmemb;
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct cstring *s) {
  size_t new_len = s->len + size * nmemb;
  s->ptr = (char*)realloc(s->ptr, new_len + 1);
  if (s->ptr == NULL) {
    fprintf(stderr, "realloc() failed\n");
    exit(EXIT_FAILURE);
  }
  memcpy(s->ptr + s->len, ptr, size * nmemb);
  s->ptr[new_len] = '\0';
  s->len = new_len;

  return size * nmemb;
}

size_t print_one(void *ptr, size_t size, size_t nmemb, struct cstring *s) {
  (void)s;
  char* pos = strstr((char*)ptr, "data: {\"tag\": \"salt/job/");
  if (pos)
    fprintf(stdout, "%s", (char *)pos);
  return size * nmemb;
}

char* get_line(const char* buf, int* lines) {
  if (!buf || !(*buf)) return nullptr;
  char* ptr = (char*)buf;

  while(ptr && *ptr && *ptr != '\n' && *ptr != '\r')
    ++ptr;

  ++*lines;
  return ptr + 1;
}

char* get_line(const char* buf) {
  if (!buf || !(*buf)) return nullptr;
  char* ptr = (char*)buf;

  while(ptr && *ptr && *ptr != '\n' && *ptr != '\r')
    ++ptr;

  return ptr + 1;
}

size_t parse_json(void *ptr, size_t size, size_t nmemb, SALT_JOB* job) {
  //data: {\"tag\": \"salt/job/
  char* buf = (char*)ptr;
  char* tmp = buf;
  while (nullptr != (tmp = get_line(buf))) {
    if (!strncmp(buf, "data: ", 6)) {
      free_salt_job(job);
      //fprintf(stdout, "%s", (char *)pos);
      parse_salt_job(job, (const char*)buf + 6, tmp - buf - 6);
      show_job(job);
    }
    buf = tmp;
  }
  return size * nmemb;
}


void print_cookies(CURL *curl) {
  CURLcode res;
  struct curl_slist *cookies;
  struct curl_slist *nc;
  int i;

  printf("Cookies, curl knows:\n");
  res = curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);
  if (res != CURLE_OK) {
    fprintf(stderr, "Curl curl_easy_getinfo failed: %s\n",
            curl_easy_strerror(res));
    exit(1);
  }
  nc = cookies, i = 1;
  while (nc) {
    printf("[%d]: %s\n", i, nc->data);
    nc = nc->next;
    i++;
  }
  if (i == 1) {
    printf("(none)\n");
  }
  curl_slist_free_all(cookies);
}


#ifdef EXPECT_NE
#undef EXPECT_NE
#endif //EXPECT_NE
#define EXPECT_NE(x, y) if ((x) == (y)) { printf("%s %d\n", __FILE__, __LINE__); return -1; }

#ifdef EXPECT_EQ
#undef EXPECT_EQ
#endif //EXPECT_EQ
#define EXPECT_EQ(x, y) if ((x) != (y)) { printf("%s %d\n", __FILE__, __LINE__); return -1; }

int curl_salt_event() {
  CURL *curl;
  CURLcode res;
  int rspcode = 0;

  SALT_JOB job;

  /* get a curl handle */
  curl = curl_easy_init();
  EXPECT_NE(curl, (void *)NULL);
  if (curl) {
    /* First set the URL that is about to receive our POST. This URL can
     * just as well be a https:// URL if that is what should receive the
     * data. */
    EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_URL,
                                  "http://10.10.10.19:8000/login"));
    /* Now specify the POST data */
    EXPECT_EQ(0,
              curl_easy_setopt(curl, CURLOPT_POSTFIELDS,
                               "username=sean&password=hongt@8a51&eauth=pam"));
    // EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, print_one));
    // EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s));
    EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, parse_json));
    EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_WRITEDATA, &job));

    EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_COOKIEFILE,
                                  "/home/sean/cookies.txt"));
    EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_COOKIELIST, "ALL"));

    struct curl_slist *headers = NULL; /* init to NULL is important */
    EXPECT_NE((void *)0,
              headers = curl_slist_append(headers, "Accept: application/json"));

    /* pass our list of custom made headers */
    EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers));

    /* Perform the request, res will get the return code */
    EXPECT_EQ(0, res = curl_easy_perform(curl));
    EXPECT_EQ(0, curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &rspcode));
    EXPECT_EQ(200, rspcode);
    struct curl_slist *cookies;

    EXPECT_EQ(0, res = curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies));
    EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_URL,
                                  "http://10.10.10.19:8000/events"));
    /* Now specify the POST data */
    EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_POSTFIELDS, 0));
    EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_COOKIE, cookies->data));

    curl_easy_setopt(curl, (CURLoption)43, 1L);
    curl_easy_setopt(curl, (CURLoption)42, 0L);
    curl_easy_setopt(curl, (CURLoption)61, 0L);
    curl_easy_setopt(curl, (CURLoption)45, 0L);
    curl_easy_setopt(curl, (CURLoption)46, 0L);
    curl_easy_setopt(curl, (CURLoption)48, 0L);
    curl_easy_setopt(curl, (CURLoption)50, 0L);
    curl_easy_setopt(curl, (CURLoption)53, 0L);
    curl_easy_setopt(curl, (CURLoption)155, 0L);
    curl_easy_setopt(curl, (CURLoption)52, 0L);
    curl_easy_setopt(curl, (CURLoption)105, 0L);
    curl_easy_setopt(curl, (CURLoption)58, 0L);
    curl_easy_setopt(curl, (CURLoption)68, 50L);
    curl_easy_setopt(curl, (CURLoption)161, 0L);
    curl_easy_setopt(curl, (CURLoption)19, 0L);
    curl_easy_setopt(curl, (CURLoption)20, 0L);
    curl_easy_setopt(curl, (CURLoption)64, 1L);
    curl_easy_setopt(curl, (CURLoption)27, 0L);
    curl_easy_setopt(curl, (CURLoption)96, 0L);
    curl_easy_setopt(curl, (CURLoption)34, 0L);
    curl_easy_setopt(curl, (CURLoption)156, 0L);
    curl_easy_setopt(curl, (CURLoption)110, 0L);
    curl_easy_setopt(curl, (CURLoption)113, 0L);
    curl_easy_setopt(curl, (CURLoption)136, 0L);
    curl_easy_setopt(curl, (CURLoption)137, 0L);
    curl_easy_setopt(curl, (CURLoption)138, 0L);
    curl_easy_setopt(curl, (CURLoption)213, 1L);

    curl_easy_setopt(curl, (CURLoption)30145, 0L);
    curl_easy_setopt(curl, (CURLoption)30146, 0L);
    curl_easy_setopt(curl, (CURLoption)30116, 0L);

    curl_easy_setopt(curl, (CURLoption)10004, 0);
    curl_easy_setopt(curl, (CURLoption)10006, 0);
    curl_easy_setopt(curl, (CURLoption)10177, 0);
    curl_easy_setopt(curl, (CURLoption)10005, 0);
    curl_easy_setopt(curl, (CURLoption)10007, 0);
    curl_easy_setopt(curl, (CURLoption)10016, 0);
    curl_easy_setopt(curl, (CURLoption)10017, 0);
    curl_easy_setopt(curl, (CURLoption)10026, 0);
    curl_easy_setopt(curl, (CURLoption)10153, 0);
    curl_easy_setopt(curl, (CURLoption)10152, 0);
    curl_easy_setopt(curl, (CURLoption)10162, 0);
    curl_easy_setopt(curl, (CURLoption)10025, 0);
    curl_easy_setopt(curl, (CURLoption)10086, 0);
    curl_easy_setopt(curl, (CURLoption)10087, 0);
    curl_easy_setopt(curl, (CURLoption)10088, 0);
    curl_easy_setopt(curl, (CURLoption)10036, 0);
    curl_easy_setopt(curl, (CURLoption)10062, 0);
    curl_easy_setopt(curl, (CURLoption)10063, 0);
    curl_easy_setopt(curl, (CURLoption)10076, 0);
    curl_easy_setopt(curl, (CURLoption)10077, 0);
    curl_easy_setopt(curl, (CURLoption)10134, 0);
    curl_easy_setopt(curl, (CURLoption)10147, 0);

    EXPECT_EQ(0, res = curl_easy_perform(curl));
    EXPECT_EQ(0, curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &rspcode));
    EXPECT_EQ(200, rspcode);
    curl_slist_free_all(cookies);

    /* always cleanup */
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
  }

  //free_string(&s);
  free_salt_job(&job);

  return (0);
}


/*
curl_easy_setopt lval 43 1L
curl_easy_setopt lval 42 0L
curl_easy_setopt lval 61 0L
curl_easy_setopt lval 45 0L
curl_easy_setopt lval 46 0L
curl_easy_setopt lval 48 0L
curl_easy_setopt lval 50 0L
curl_easy_setopt lval 53 0L
curl_easy_setopt lval 155 0L
curl_easy_setopt lval 52 0L
curl_easy_setopt lval 105 0L
curl_easy_setopt lval 58 0L
curl_easy_setopt lval 68 50L
curl_easy_setopt lval 161 0L
curl_easy_setopt lval 19 0L
curl_easy_setopt lval 20 0L
curl_easy_setopt lval 64 1L
curl_easy_setopt lval 27 0L
curl_easy_setopt lval 96 0L
curl_easy_setopt lval 34 0L
curl_easy_setopt lval 156 0L
curl_easy_setopt lval 110 0L
curl_easy_setopt lval 113 0L
curl_easy_setopt lval 136 0L
curl_easy_setopt lval 137 0L
curl_easy_setopt lval 138 0L
curl_easy_setopt lval 213 1L


curl_easy_setopt oval 30145 0
curl_easy_setopt oval 30146 0
curl_easy_setopt oval 30116 0


curl_easy_setopt pval 10001 7fffffffd8a0
curl_easy_setopt pval 10195 7fffffffd8a0
curl_easy_setopt pval 20011 40431d
curl_easy_setopt pval 10009 7fffffffd9c0
curl_easy_setopt pval 20012 403f7c
curl_easy_setopt pval 10168 7fffffffd9c0
curl_easy_setopt pval 20167 4040bc
curl_easy_setopt pval 10002 668278
curl_easy_setopt pval 10010 7fffffffda90
curl_easy_setopt pval 10018 666e98
curl_easy_setopt pval 10183 668358
curl_easy_setopt pval 10031 666e68
curl_easy_setopt pval 10037 7ffff72bd1c0
curl_easy_setopt pval 20079 4034ec
curl_easy_setopt pval 10029 7fffffffda30
curl_easy_setopt pval 10004 0
curl_easy_setopt pval 10006 0
curl_easy_setopt pval 10177 0
curl_easy_setopt pval 10005 0
curl_easy_setopt pval 10007 0
curl_easy_setopt pval 10016 0
curl_easy_setopt pval 10017 0
curl_easy_setopt pval 10026 0
curl_easy_setopt pval 10153 0
curl_easy_setopt pval 10152 0
curl_easy_setopt pval 10162 0
curl_easy_setopt pval 10025 0
curl_easy_setopt pval 10086 0
curl_easy_setopt pval 10087 0
curl_easy_setopt pval 10088 0
curl_easy_setopt pval 10036 0
curl_easy_setopt pval 10062 0
curl_easy_setopt pval 10063 0
curl_easy_setopt pval 10076 0
curl_easy_setopt pval 10077 0
curl_easy_setopt pval 10134 0
curl_easy_setopt pval 10147 0
*/

