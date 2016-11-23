#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "curl_helper.h"

void init_string(struct cstring *s) {
  s->len = 0;
  s->ptr = malloc(s->len + 1);
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
  s->ptr = realloc(s->ptr, new_len + 1);
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
  s->ptr = realloc(s->ptr, new_len + 1);
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

/*







--------
tag: 20161123065056424864


--------
--------
data: {"tag": "20161123065056424864", "data": {"_stamp":
"2016-11-22T22:50:56.425385", "minions": ["old08002759F4B6"]}}



--------
--------
tag: salt/job/20161123065056424864/new


--------
--------
data: {"tag": "salt/job/20161123065056424864/new", "data": {"tgt_type": "glob",
"jid": "20161123065056424864", "tgt": "old08002759F4B6", "_stamp":
"2016-11-22T22:50:56.426503", "user": "root", "arg":
["c:\\new-salt\\ExecClient.exe abcd"], "fun": "cmd.run_all", "minions":
["old08002759F4B6"]}}



--------
--------
tag: salt/job/20161123065056424864/ret/old08002759F4B6


--------
--------
data: {"tag": "salt/job/20161123065056424864/ret/old08002759F4B6", "data":
{"_stamp": "2016-11-22T22:50:56.749570", "return": {"pid": 2976, "retcode": 0,
"stderr": "", "stdout": ""}, "retcode": 0, "success": true, "cmd": "_return",
"jid": "20161123065056424864", "fun": "cmd.run_all", "id": "old08002759F4B6"}}



--------

*/

/*
--------
tag: 20161123065414567343


--------
--------
data: {"tag": "20161123065414567343", "data": {"_stamp":
"2016-11-22T22:54:14.567905", "minions": ["minion1", "minion2", "minion3",
"minion4", "minion5", "minion6", "minion7", "minion8", "new080027006B3F",
"new08002700A6BA", "new08002701AABF", "new0800270247DC", "new080027026A50", ...
, "Server178", "Server179"]}}



--------
--------
tag: salt/job/20161123065414567343/new

30e6 data: {"tag": "salt/job/20161123065414567343/new", "data": {"tgt_type":
"glob", "jid": "20161123065414567343", "tgt": "*", "_stamp":
"2016-11-22T22:54:14.569305", "user": "root", "arg": [], "fun": "test.ping",
"minions": ["minion1", "minion2", "minion3", "minion4", "minion5", "minion6",
"minion7", "minion8", "new080027006B3F", "new08002700A6BA", ..., ]}}


37
tag: salt/job/20161123065414567343/ret/old0800277AF5BE

103 data: {"tag": "salt/job/20161123065414567343/ret/old0800277AF5BE", "data":
{"_stamp": "2016-11-22T22:54:14.580881", "return": true, "retcode": 0,
"success": true, "cmd": "_return", "jid": "20161123065414567343", "fun":
"test.ping", "id": "old0800277AF5BE"}}



--------
--------
data: {"tag": "salt/job/20161123065414567343/new", "data": {"tgt_type": "glob",
"jid": "20161123065414567343", "tgt": "*", "_stamp":
"2016-11-22T22:54:14.569305", "user": "root", "arg": [], "fun": "test.ping",
"minions": ["minion1", "minion2", "minion3", "minion4", "minion5", "minion6",
"minion7", "minion8", "new080027006B3F", "new08002700A6BA", "new08002701AABF",
"new0800277E6CF0" ....]}}


tag: salt/job/20161123065414567343/ret/old0800277AF5BE

103 data: {"tag": "salt/job/20161123065414567343/ret/old0800277AF5BE", "data":
{"_stamp": "2016-11-22T22:54:14.580881", "return": true, "retcode": 0,
"success": true, "cmd": "_return", "jid": "20161123065414567343", "fun":
"test.ping", "id": "old0800277AF5BE"}}



--------
--------
tag: salt/job/20161123065414567343/ret/old0800277AF5BE

103 data: {"tag": "salt/job/20161123065414567343/ret/old0800277AF5BE", "data":
{"_stamp": "2016-11-22T22:54:14.580881", "return": true, "retcode": 0,
"success": true, "cmd": "_return", "jid": "20161123065414567343", "fun":
"test.ping", "id": "old0800277AF5BE"}}



--------
--------
data: {"tag": "salt/job/20161123065414567343/ret/old0800277AF5BE", "data":
{"_stamp": "2016-11-22T22:54:14.580881", "return": true, "retcode": 0,
"success": true, "cmd": "_return", "jid": "20161123065414567343", "fun":
"test.ping", "id": "old0800277AF5BE"}}

*/
