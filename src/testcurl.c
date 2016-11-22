#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <curl/curl.h>


struct cstring {
  char *ptr;
  size_t len;
};

void init_string(struct cstring *s) {
  s->len = 0;
  s->ptr = malloc(s->len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  s->ptr[0] = '\0';
}

void free_string(struct cstring* s) {
  if (s->ptr) free(s->ptr);
  s->ptr = 0;
  s->len = 0;
}


size_t writefunc(void *ptr, size_t size, size_t nmemb, struct cstring *s)
{
  size_t new_len = s->len + size*nmemb;
  printf("hahah: ");
  s->ptr = realloc(s->ptr, new_len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "realloc() failed\n");
    exit(EXIT_FAILURE);
  }
  memcpy(s->ptr+s->len, ptr, size*nmemb);
  s->ptr[new_len] = '\0';
  s->len = new_len;

  return size*nmemb;
}
 
int test_response(void)
{
  CURL *curl;
  CURLcode res;
  int rspcode = 0;

  struct cstring s;
  init_string(&s);

  /* In windows, this will init the winsock stuff */ 
  curl_global_init(CURL_GLOBAL_ALL);
 
  /* get a curl handle */ 
  curl = curl_easy_init();
  if(curl) {
    /* First set the URL that is about to receive our POST. This URL can
 *        just as well be a https:// URL if that is what should receive the
 *               data. */ 
    curl_easy_setopt(curl, CURLOPT_URL, "http://10.10.10.19:8000/login");
    /* Now specify the POST data */ 
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "username=sean&password=hongt@8a51&eauth=pam");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

    struct curl_slist *headers=NULL; /* init to NULL is important */
    headers = curl_slist_append(headers, "Accept: application/json");
 
    /* pass our list of custom made headers */
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
 
    /* Perform the request, res will get the return code */ 
    res = curl_easy_perform(curl);
    /* Check for errors */ 
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
    else
      fprintf(stdout, "%s\n", s.ptr);

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &rspcode);
    printf("RESPONSE %d\n\n", rspcode);

 
    /* Now specify the POST data */
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "username=sean&password=hongt&eauth=pam");

    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
    else
      fprintf(stdout, "%s\n", s.ptr);


    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &rspcode);
    printf("RESPONSE %d\n\n", rspcode);

    curl_easy_setopt(curl, CURLOPT_URL, "http://10.10.10.19:8000/loginabc");
    /* Now specify the POST data */
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "username=sean&password=hongt@8a51&eauth=pam");

    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
    else
      fprintf(stdout, "%s\n", s.ptr);

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &rspcode);
    printf("RESPONSE %d\n\n", rspcode);


    free_string(&s);
    /* always cleanup */
    curl_easy_cleanup(curl);




  }
  curl_global_cleanup();
  return 0;
}



static void
print_cookies(CURL *curl)
{
  CURLcode res;
  struct curl_slist *cookies;
  struct curl_slist *nc;
  int i;
 
  printf("Cookies, curl knows:\n");
  res = curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);
  if(res != CURLE_OK) {
    fprintf(stderr, "Curl curl_easy_getinfo failed: %s\n",
            curl_easy_strerror(res));
    exit(1);
  }
  nc = cookies, i = 1;
  while(nc) {
    printf("[%d]: %s\n", i, nc->data);
    nc = nc->next;
    i++;
  }
  if(i == 1) {
    printf("(none)\n");
  }
  curl_slist_free_all(cookies);
}
 
int
test_cookies(void)
{
  CURL *curl;
  CURLcode res;
 
  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();
  if(curl) {
    char nline[256];
 
    curl_easy_setopt(curl, CURLOPT_URL, "http://10.10.10.19:8000/login");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "username=sean&password=hongt@8a51&eauth=pam");
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "/home/sean/cookies.txt"); /* start cookie engine */ 
    res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
      fprintf(stderr, "Curl perform failed: %s\n", curl_easy_strerror(res));
      return 1;
    }
 
    print_cookies(curl);
 
    printf("Erasing curl's knowledge of cookies!\n");
    curl_easy_setopt(curl, CURLOPT_COOKIELIST, "ALL");
 
    print_cookies(curl);
 
    printf("-----------------------------------------------\n"
           "Setting a cookie \"PREF\" via cookie interface:\n");
#ifdef WIN32
#define snprintf _snprintf
#endif
    /* Netscape format cookie */ 
    snprintf(nline, sizeof(nline), "%s\t%s\t%s\t%s\t%lu\t%s\t%s",
             ".google.com", "TRUE", "/", "FALSE",
             (unsigned long)time(NULL) + 31337UL,
             "PREF", "hello google, i like you very much!");
    res = curl_easy_setopt(curl, CURLOPT_COOKIELIST, nline);
    if(res != CURLE_OK) {
      fprintf(stderr, "Curl curl_easy_setopt failed: %s\n",
              curl_easy_strerror(res));
      return 1;
    }
 
    /* HTTP-header style cookie. If you use the Set-Cookie format and don't
 *     specify a domain then the cookie is sent for any domain and will not be
 *         modified, likely not what you intended. Starting in 7.43.0 any-domain
 *             cookies will not be exported either. For more information refer to the
 *                 CURLOPT_COOKIELIST documentation.
 *                     */ 
    snprintf(nline, sizeof(nline),
      "Set-Cookie: OLD_PREF=3d141414bf4209321; "
      "expires=Sun, 17-Jan-2038 19:14:07 GMT; path=/; domain=.google.com");
    res = curl_easy_setopt(curl, CURLOPT_COOKIELIST, nline);
    if(res != CURLE_OK) {
      fprintf(stderr, "Curl curl_easy_setopt failed: %s\n",
              curl_easy_strerror(res));
      return 1;
    }
 
    print_cookies(curl);
 
    res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
      fprintf(stderr, "Curl perform failed: %s\n", curl_easy_strerror(res));
      return 1;
    }
 
    curl_easy_cleanup(curl);
  }
  else {
    fprintf(stderr, "Curl init failed!\n");
    return 1;
  }
 
  curl_global_cleanup();
  return 0;
}


int main(void) {
  test_cookies();
  return (0);
}

