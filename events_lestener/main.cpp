#include "mario_data.h"
#include "salt_api.h"
#include <iostream>
#include <thread>
#include <curl/curl.h>

#include <pthread.h>

using namespace std;
extern char gtoken[32];

static const char* cmd_str[2] = {
  "client=local_async&fun=test.ping&tgt=*",
  "client=local_async&fun=cmd.run_all&tgt=old08002759F4B6&arg=\"c:\new_salt\ExecClient.exe abcd\""
};

void* test_run_cmd(void* param) {
  int64_t cmd_index = (int64_t)param;
  CURL *curl = 0;
  //CURLcode res = CURLE_OK;
  int rspcode = 0;
  struct curl_slist *headers = NULL; /* init to NULL is important */
  cstring s;
  init_string(&s);
  char x_token[128];

  /* get a curl handle */
  curl = curl_easy_init();
  if (!curl) return 0;
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "http://10.10.10.19:8000");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, cmd_str[cmd_index]);
    // EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, print_one));
    // EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s));

    headers = curl_slist_append(headers, "Accept: application/json");
    snprintf(x_token, 128, "X-Auth-Token: %s", gtoken);
    std::cout << gtoken << std::endl;
    headers = curl_slist_append(headers, x_token);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &rspcode);

    /* always cleanup */
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
  }

  free_string(&s);
  return 0;
}

#define PAGE_SIZE 4096
#define STK_SIZE (1000 * PAGE_SIZE)

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  curl_get_token();
  curl_run_cmd(0);
  curl_run_cmd(1);

  return 0;
}
