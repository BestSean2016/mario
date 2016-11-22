#include <gtest/gtest.h>
#include <iostream>

using namespace std;

int main(int argc, char **argv) {

#ifdef _WINDOWS
#ifdef _DEBUG_
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
  _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
  _CrtDumpMemoryLeaks();
#endif //_DEBUG_
#endif //_WINDOWS

  // if (argc != 3) {
  //   printf("Usage testing <json config file> <json config file2>\n");
  //   return (0);
  // }

  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}

#include "chinese.h"

TEST(convert, utf8_to_ucs2) {
  char hello[100] = "abc 123 大家好！";
  char temp[100];
  memset(temp, 0, 100);
  EXPECT_EQ(0, convert(hello, temp, strlen(hello), 100, "UTF-8", "UCS-2"));

  // dumpbuf("hello", hello, 100);
  // dumpbuf("temp", temp, 100);
  unsigned char t1[] = {0x61, 0x00, 0x62, 0x00, 0x63, 0x00, 0x20, 0x00, 0x31,
                        0x00, 0x32, 0x00, 0x33, 0x00, 0x20, 0x00, 0x27, 0x59,
                        0xB6, 0x5B, 0x7D, 0x59, 0x01, 0xFF, 0x00, 0x00};
  EXPECT_EQ(memcmp(temp, t1, sizeof(t1)), 0);
}

TEST(convert, utf8_to_ucs4) {
  char hello[100] = "abc 123 大家好！";
  char temp[200];
  memset(temp, 0, 200);
  EXPECT_EQ(0, convert(hello, temp, strlen(hello), 200, "UTF-8", "UCS-4"));

  // dumpbuf("hello", hello, 100);
  // dumpbuf("temp", temp, 100);

  unsigned char t2[] = {0x00, 0x00, 0x00, 0x61, 0x00, 0x00, 0x00, 0x62, 0x00,
                        0x00, 0x00, 0x63, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00,
                        0x00, 0x31, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00,
                        0x33, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x59, 0x27,
                        0x00, 0x00, 0x5B, 0xB6, 0x00, 0x00, 0x59, 0x7D, 0x00,
                        0x00, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00};

  // printf("sizeof t2 %lu\n", sizeof(t2));
  EXPECT_EQ(memcmp(temp, t2, sizeof(t2)), 0);

  // for (int k = 1; k <= 100; k++)
  //     for (int j = 1; j <= 100; j++) {
  //         int ret = convert(hello, temp, k, j, "UTF-8", "UCS-4");
  //         if (ret == 0) printf("%d %d %d\n", k, j, ret);
  //     }
}

TEST(convert, utf8_to_webucs2) {
  char hello[100] = "abc 123 大家好！";
  char tmp[200];
  memset(tmp, 0, 200);
  EXPECT_EQ(0, utf8_2_webucs2(hello, tmp, strlen(hello), 200, "&#", ";"));
  EXPECT_EQ(strcmp(tmp, "abc 123 &#22823;&#23478;&#22909;&#65281;"), 0);
}

TEST(convert, gbk_to_webucs2) {
  char hello[100] = "abc 123 大家好！";
  char tmp[200];
  char gbk[200];

  memset(gbk, 0, 200);
  EXPECT_EQ(0, convert(hello, gbk, strlen(hello), 200, "UTF-8", "GBK"));

  memset(tmp, 0, 200);
  EXPECT_EQ(0, gbk_2_webucs2(gbk, tmp, strlen(gbk), 200, "&#", ";"));
  EXPECT_EQ(strcmp(tmp, "abc 123 &#22823;&#23478;&#22909;&#65281;"), 0);
}

TEST(convert, webucs_2_gbk) {
  char hello[100] = "abc 123 大家好！";
  char gbk[200];
  char webucs_2[] = "abc 123 &#22823;&#23478;&#22909;&#65281;";

  char tmp[100];
  memset(tmp, 0, 100);
  EXPECT_EQ(0, convert(hello, tmp, strlen(hello), 100, "UTF-8", "GBK"));

  memset(gbk, 0, 200);
  EXPECT_EQ(0,
            webucs_2_gbk(webucs_2, gbk, strlen(webucs_2) + 1, 200, "&#", ";"));
  EXPECT_EQ(strcmp(gbk, tmp), 0);
}

TEST(convert, webucs_2_utf8) {
  char hello[100] = "abc 123 大家好！";
  char utf8[200];
  char webucs_2[] = "abc 123 &#22823;&#23478;&#22909;&#65281;";

  memset(utf8, 0, 200);
  EXPECT_EQ(
      0, webucs_2_utf8(webucs_2, utf8, strlen(webucs_2) + 1, 200, "&#", ";"));
  EXPECT_EQ(strcmp(utf8, hello), 0);
}

TEST(convert, webucs16_2_utf8) {
  char hello[100] = "abc 123 大家好！";
  char utf8[200];
  char webucs_2[] = "abc 123 &#x5927;&#x5bb6;&#x597d;&#xff01;";

  memset(utf8, 0, 200);
  EXPECT_EQ(0, webucs16_2_utf8(webucs_2, utf8, strlen(webucs_2) + 1, 200, "&#x",
                               ";"));
  EXPECT_EQ(strcmp(utf8, hello), 0);
}

TEST(convert, webucs16_2_gbk) {
  char hello[100] = "abc 123 大家好！";
  char gbk[200];
  char webucs_2[] = "abc 123 &#x5927;&#x5bb6;&#x597d;&#xff01;";

  char tmp[100];
  memset(tmp, 0, 100);
  EXPECT_EQ(0, convert(hello, tmp, strlen(hello), 100, "UTF-8", "GBK"));

  memset(gbk, 0, 200);
  EXPECT_EQ(
      0, webucs16_2_utf8(webucs_2, gbk, strlen(webucs_2) + 1, 200, "&#x", ";"));
  EXPECT_EQ(strcmp(gbk, hello), 0);
}

#include "mario_types.h"

#include "curl_helper.h"

TEST(curl, authentication) {
  CURL *curl;
  CURLcode res;
  int rspcode = 0;

  struct cstring s;
  init_string(&s);

  /* In windows, this will init the winsock stuff */
  EXPECT_EQ(0, curl_global_init(CURL_GLOBAL_ALL));

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
    EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc));
    EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s));

    struct curl_slist *headers = NULL; /* init to NULL is important */
    EXPECT_NE((void *)0,
              headers = curl_slist_append(headers, "Accept: application/json"));

    /* pass our list of custom made headers */
    EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers));

    /* Perform the request, res will get the return code */
    EXPECT_EQ(0, res = curl_easy_perform(curl));
    /* Check for errors */
    // if (res != CURLE_OK)
    //   fprintf(stderr, "curl_easy_perform() failed: %s\n",
    //           curl_easy_strerror(res));
    // else
    // fprintf(stdout, "%s\n", s.ptr);
    //
    EXPECT_EQ(0, curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &rspcode));
    EXPECT_EQ(200, rspcode);
    // printf("RESPONSE %d\n\n", rspcode);

    /* Now specify the POST data */
    EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_POSTFIELDS,
                                  "username=sean&password=hongt&eauth=pam"));

    /* Perform the request, res will get the return code */
    EXPECT_EQ(0, (res = curl_easy_perform(curl)));
    EXPECT_EQ(0, curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &rspcode));
    EXPECT_EQ(401, rspcode);

    EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_URL,
                                  "http://10.10.10.19:8000/loginabc"));
    /* Perform the request, res will get the return code */
    EXPECT_EQ(0, (res = curl_easy_perform(curl)));
    /* Check for errors */
    EXPECT_EQ(0, curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &rspcode));
    EXPECT_EQ(404, rspcode);

    /* always cleanup */
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
  }
  curl_global_cleanup();
  free_string(&s);
}

TEST(curl, cookie) {
  CURL *curl;
  CURLcode res;
  int rspcode = 0;

  struct cstring s;
  init_string(&s);

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
    EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeone));
    EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s));
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
    /* Check for errors */
    // if (res != CURLE_OK)
    //   fprintf(stderr, "curl_easy_perform() failed: %s\n",
    //           curl_easy_strerror(res));
    // else
    // fprintf(stdout, "%s\n", s.ptr);
    //
    EXPECT_EQ(0, curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &rspcode));
    EXPECT_EQ(200, rspcode);
    // printf("RESPONSE %d\n\n", rspcode);

    // print_cookies(curl);

    struct curl_slist *cookies;

    EXPECT_EQ(0, res = curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies));
    if (res != CURLE_OK) {
      fprintf(stderr, "Curl curl_easy_getinfo failed: %s\n",
              curl_easy_strerror(res));
      exit(1);
    }
    EXPECT_EQ(0, strncmp(cookies->data, "10.10.10.19", strlen("10.10.10.19")));

    EXPECT_EQ(0,
              curl_easy_setopt(curl, CURLOPT_URL, "http://10.10.10.19:8000"));
    /* Now specify the POST data */
    EXPECT_EQ(
        0, curl_easy_setopt(curl, CURLOPT_POSTFIELDS,
                            "client=local&tgt=old0800272CDDF5&fun=test.ping"));
    curl_easy_setopt(curl, CURLOPT_COOKIE, cookies->data);
    EXPECT_EQ(0, res = curl_easy_perform(curl));
    EXPECT_EQ(0, curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &rspcode));
    EXPECT_EQ(200, rspcode);
    EXPECT_EQ(0, strcmp("{\"return\": [{\"old0800272CDDF5\": true}]}", s.ptr));

    curl_slist_free_all(cookies);

    /* always cleanup */
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
  }

  free_string(&s);
}

TEST(curl, event) {
  CURL *curl;
  CURLcode res;
  int rspcode = 0;

  struct cstring s;
  init_string(&s);

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
    EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, print_one));
    EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s));
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
    EXPECT_EQ(0, res = curl_easy_perform(curl));
    EXPECT_EQ(0, curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &rspcode));
    EXPECT_EQ(200, rspcode);
    curl_slist_free_all(cookies);

    /* always cleanup */
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
  }

  free_string(&s);
}
