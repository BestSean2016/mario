#include "chinese.h"
#include "curl_helper.h"
#include "mario_data.h"
#include "mario_mysql.h"
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


const char* str = {
  "data: {\"tag\": \"salt/job/20161125224350449846/ret/old080027539807\", \"data\": {\"_stamp\": \"2016-11-25T14:43:50.710145\", \"return\": true, \"retcode\": 0, \"success\": true, \"cmd\": \"_return\", \"jid\": \"20161125224350449846\", \"fun\": \"test.ping\", \"id\": \"old080027539807\"}}\n"
  "\n"
  "\n"
  "37\n"
  "tag: salt/job/20161125224350449846/ret/old080027713729\n"
  "\n"
  "103\n"
  "data: {\"tag\": \"salt/job/20161125224350449846/ret/old080027713729\", \"data\": {\"_stamp\": \"2016-11-25T14:43:50.710178\", \"return\": true, \"retcode\": 0, \"success\": true, \"cmd\": \"_return\", \"jid\": \"20161125224350449846\", \"fun\": \"test.ping\", \"id\": \"old080027713729\"}}\n"
  "\n"
  "\n"
  "\n"
};


TEST(Str, get_next_line) {
  char* buf = (char*)str;
  char* tmp = buf;
  int lines = 0;
  while(nullptr != (tmp = get_line(buf, &lines))) {
    cout << lines << ": ";
    for (int i = 0; i < tmp - buf; ++i)
      cout << buf[i];
    buf = tmp;
  }
}

TEST(Str, get_next_line_2) {
  char* buf = (char*)str;
  char* tmp = buf;
  while(nullptr != (tmp = get_line(buf))) {
    cout << "length " << tmp - buf << "->";
    for (int i = 0; i < tmp - buf; ++i)
      cout << buf[i];
    buf = tmp;
  }
}

//
// TEST(convert, utf8_to_ucs2) {
//   char hello[100] = "abc 123 大家好！";
//   char temp[100];
//   memset(temp, 0, 100);
//   EXPECT_EQ(0, convert(hello, temp, strlen(hello), 100, "UTF-8", "UCS-2"));
//
//   // dumpbuf("hello", hello, 100);
//   // dumpbuf("temp", temp, 100);
//   unsigned char t1[] = {0x61, 0x00, 0x62, 0x00, 0x63, 0x00, 0x20, 0x00, 0x31,
//                         0x00, 0x32, 0x00, 0x33, 0x00, 0x20, 0x00, 0x27, 0x59,
//                         0xB6, 0x5B, 0x7D, 0x59, 0x01, 0xFF, 0x00, 0x00};
//   EXPECT_EQ(memcmp(temp, t1, sizeof(t1)), 0);
// }
//
// TEST(convert, utf8_to_ucs4) {
//   char hello[100] = "abc 123 大家好！";
//   char temp[200];
//   memset(temp, 0, 200);
//   EXPECT_EQ(0, convert(hello, temp, strlen(hello), 200, "UTF-8", "UCS-4"));
//
//   // dumpbuf("hello", hello, 100);
//   // dumpbuf("temp", temp, 100);
//
//   unsigned char t2[] = {0x00, 0x00, 0x00, 0x61, 0x00, 0x00, 0x00, 0x62, 0x00,
//                         0x00, 0x00, 0x63, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00,
//                         0x00, 0x31, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00,
//                         0x33, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x59, 0x27,
//                         0x00, 0x00, 0x5B, 0xB6, 0x00, 0x00, 0x59, 0x7D, 0x00,
//                         0x00, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00};
//
//   // printf("sizeof t2 %lu\n", sizeof(t2));
//   EXPECT_EQ(memcmp(temp, t2, sizeof(t2)), 0);
//
//   // for (int k = 1; k <= 100; k++)
//   //     for (int j = 1; j <= 100; j++) {
//   //         int ret = convert(hello, temp, k, j, "UTF-8", "UCS-4");
//   //         if (ret == 0) printf("%d %d %d\n", k, j, ret);
//   //     }
// }
//
// TEST(convert, utf8_to_webucs2) {
//   char hello[100] = "abc 123 大家好！";
//   char tmp[200];
//   memset(tmp, 0, 200);
//   EXPECT_EQ(0, utf8_2_webucs2(hello, tmp, strlen(hello), 200, "&#", ";"));
//   EXPECT_EQ(strcmp(tmp, "abc 123 &#22823;&#23478;&#22909;&#65281;"), 0);
// }
//
// TEST(convert, gbk_to_webucs2) {
//   char hello[100] = "abc 123 大家好！";
//   char tmp[200];
//   char gbk[200];
//
//   memset(gbk, 0, 200);
//   EXPECT_EQ(0, convert(hello, gbk, strlen(hello), 200, "UTF-8", "GBK"));
//
//   memset(tmp, 0, 200);
//   EXPECT_EQ(0, gbk_2_webucs2(gbk, tmp, strlen(gbk), 200, "&#", ";"));
//   EXPECT_EQ(strcmp(tmp, "abc 123 &#22823;&#23478;&#22909;&#65281;"), 0);
// }
//
// TEST(convert, webucs_2_gbk) {
//   char hello[100] = "abc 123 大家好！";
//   char gbk[200];
//   char webucs_2[] = "abc 123 &#22823;&#23478;&#22909;&#65281;";
//
//   char tmp[100];
//   memset(tmp, 0, 100);
//   EXPECT_EQ(0, convert(hello, tmp, strlen(hello), 100, "UTF-8", "GBK"));
//
//   memset(gbk, 0, 200);
//   EXPECT_EQ(0,
//             webucs_2_gbk(webucs_2, gbk, strlen(webucs_2) + 1, 200, "&#", ";"));
//   EXPECT_EQ(strcmp(gbk, tmp), 0);
// }
//
// TEST(convert, webucs_2_utf8) {
//   char hello[100] = "abc 123 大家好！";
//   char utf8[200];
//   char webucs_2[] = "abc 123 &#22823;&#23478;&#22909;&#65281;";
//
//   memset(utf8, 0, 200);
//   EXPECT_EQ(
//       0, webucs_2_utf8(webucs_2, utf8, strlen(webucs_2) + 1, 200, "&#", ";"));
//   EXPECT_EQ(strcmp(utf8, hello), 0);
// }
//
// TEST(convert, webucs16_2_utf8) {
//   char hello[100] = "abc 123 大家好！";
//   char utf8[200];
//   char webucs_2[] = "abc 123 &#x5927;&#x5bb6;&#x597d;&#xff01;";
//
//   memset(utf8, 0, 200);
//   EXPECT_EQ(0, webucs16_2_utf8(webucs_2, utf8, strlen(webucs_2) + 1, 200, "&#x",
//                                ";"));
//   EXPECT_EQ(strcmp(utf8, hello), 0);
// }
//
// TEST(convert, webucs16_2_gbk) {
//   char hello[100] = "abc 123 大家好！";
//   char gbk[200];
//   char webucs_2[] = "abc 123 &#x5927;&#x5bb6;&#x597d;&#xff01;";
//
//   char tmp[100];
//   memset(tmp, 0, 100);
//   EXPECT_EQ(0, convert(hello, tmp, strlen(hello), 100, "UTF-8", "GBK"));
//
//   memset(gbk, 0, 200);
//   EXPECT_EQ(
//       0, webucs16_2_utf8(webucs_2, gbk, strlen(webucs_2) + 1, 200, "&#x", ";"));
//   EXPECT_EQ(strcmp(gbk, hello), 0);
// }
//
// TEST(curl, authentication) {
//   CURL *curl;
//   CURLcode res;
//   int rspcode = 0;
//
//   struct cstring s;
//   init_string(&s);
//
//   /* In windows, this will init the winsock stuff */
//   EXPECT_EQ(0, curl_global_init(CURL_GLOBAL_ALL));
//
//   /* get a curl handle */
//   curl = curl_easy_init();
//   EXPECT_NE(curl, (void *)NULL);
//   if (curl) {
//     /* First set the URL that is about to receive our POST. This URL can
//      * just as well be a https:// URL if that is what should receive the
//      * data. */
//     EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_URL,
//                                   "http://10.10.10.19:8000/login"));
//     /* Now specify the POST data */
//     EXPECT_EQ(0,
//               curl_easy_setopt(curl, CURLOPT_POSTFIELDS,
//                                "username=sean&password=hongt@8a51&eauth=pam"));
//     EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc));
//     EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s));
//
//     struct curl_slist *headers = NULL; /* init to NULL is important */
//     EXPECT_NE((void *)0,
//               headers = curl_slist_append(headers, "Accept: application/json"));
//
//     /* pass our list of custom made headers */
//     EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers));
//
//     /* Perform the request, res will get the return code */
//     EXPECT_EQ(0, res = curl_easy_perform(curl));
//     /* Check for errors */
//     // if (res != CURLE_OK)
//     //   fprintf(stderr, "curl_easy_perform() failed: %s\n",
//     //           curl_easy_strerror(res));
//     // else
//     // fprintf(stdout, "%s\n", s.ptr);
//     //
//     EXPECT_EQ(0, curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &rspcode));
//     EXPECT_EQ(200, rspcode);
//     // printf("RESPONSE %d\n\n", rspcode);
//
//     /* Now specify the POST data */
//     EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_POSTFIELDS,
//                                   "username=sean&password=hongt&eauth=pam"));
//
//     /* Perform the request, res will get the return code */
//     EXPECT_EQ(0, (res = curl_easy_perform(curl)));
//     EXPECT_EQ(0, curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &rspcode));
//     EXPECT_EQ(401, rspcode);
//
//     EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_URL,
//                                   "http://10.10.10.19:8000/loginabc"));
//     /* Perform the request, res will get the return code */
//     EXPECT_EQ(0, (res = curl_easy_perform(curl)));
//     /* Check for errors */
//     EXPECT_EQ(0, curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &rspcode));
//     EXPECT_EQ(404, rspcode);
//
//     /* always cleanup */
//     curl_slist_free_all(headers);
//     curl_easy_cleanup(curl);
//   }
//   curl_global_cleanup();
//   free_string(&s);
// }
//
// TEST(curl, cookie) {
//   CURL *curl;
//   CURLcode res;
//   int rspcode = 0;
//
//   struct cstring s;
//   init_string(&s);
//
//   /* get a curl handle */
//   curl = curl_easy_init();
//   EXPECT_NE(curl, (void *)NULL);
//   if (curl) {
//     /* First set the URL that is about to receive our POST. This URL can
//      * just as well be a https:// URL if that is what should receive the
//      * data. */
//     EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_URL,
//                                   "http://10.10.10.19:8000/login"));
//     /* Now specify the POST data */
//     EXPECT_EQ(0,
//               curl_easy_setopt(curl, CURLOPT_POSTFIELDS,
//                                "username=sean&password=hongt@8a51&eauth=pam"));
//     EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeone));
//     EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s));
//     EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_COOKIEFILE,
//                                   "/home/sean/cookies.txt"));
//     EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_COOKIELIST, "ALL"));
//
//     struct curl_slist *headers = NULL; /* init to NULL is important */
//     EXPECT_NE((void *)0,
//               headers = curl_slist_append(headers, "Accept: application/json"));
//
//     /* pass our list of custom made headers */
//     EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers));
//
//     /* Perform the request, res will get the return code */
//     EXPECT_EQ(0, res = curl_easy_perform(curl));
//     /* Check for errors */
//     // if (res != CURLE_OK)
//     //   fprintf(stderr, "curl_easy_perform() failed: %s\n",
//     //           curl_easy_strerror(res));
//     // else
//     // fprintf(stdout, "%s\n", s.ptr);
//     //
//     EXPECT_EQ(0, curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &rspcode));
//     EXPECT_EQ(200, rspcode);
//     // printf("RESPONSE %d\n\n", rspcode);
//
//     // print_cookies(curl);
//
//     struct curl_slist *cookies;
//
//     EXPECT_EQ(0, res = curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies));
//     if (res != CURLE_OK) {
//       fprintf(stderr, "Curl curl_easy_getinfo failed: %s\n",
//               curl_easy_strerror(res));
//       exit(1);
//     }
//     EXPECT_EQ(0, strncmp(cookies->data, "10.10.10.19", strlen("10.10.10.19")));
//
//     EXPECT_EQ(0,
//               curl_easy_setopt(curl, CURLOPT_URL, "http://10.10.10.19:8000"));
//     /* Now specify the POST data */
//     EXPECT_EQ(
//         0, curl_easy_setopt(curl, CURLOPT_POSTFIELDS,
//                             "client=local&tgt=old0800272CDDF5&fun=test.ping"));
//     curl_easy_setopt(curl, CURLOPT_COOKIE, cookies->data);
//     EXPECT_EQ(0, res = curl_easy_perform(curl));
//     EXPECT_EQ(0, curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &rspcode));
//     EXPECT_EQ(200, rspcode);
//     EXPECT_EQ(0, strcmp("{\"return\": [{\"old0800272CDDF5\": true}]}", s.ptr));
//
//     curl_slist_free_all(cookies);
//
//     /* always cleanup */
//     curl_slist_free_all(headers);
//     curl_easy_cleanup(curl);
//   }
//
//   free_string(&s);
// }
//
void test_curl_event() {
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

  free_string(&s);
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

// TEST(curl, events) {
//   //test_curl_event();
//   EXPECT_EQ(0, curl_salt_event());
// }

// TEST(mysql, connect) {
//   DBHANDLE dbh = connect_db("localhost", 3306, "mario", "mario", "chaojimali");
//   EXPECT_NE(nullptr, dbh);
//   disconnect_db(dbh);
// }
//
// template <typename T>
// void read_table(std::vector<T *> &array, const char *sql,
//                 get_fields_callback get_fields) {
//   DBHANDLE dbh = connect_db("localhost", 3306, "mario", "mario", "chaojimali");
//   EXPECT_NE(nullptr, dbh);
//
//   EXPECT_GE(get_array_from_db(array, dbh, sql, get_fields), 0);
//
//   show_ptr_array(array, std::cout);
//
//   free_ptr_array(array);
//
//   disconnect_db(dbh);
// }
//
// TEST(mysql, read_hosts) {
//   HostArray hosts;
//   read_table(hosts, select_host_from_db, get_hosts);
// }
//
// TEST(mysql, read_pipeline) {
//   PipeLineArray pls;
//   read_table(pls, select_pipeline_from_db, get_pipeline);
// }
//
// TEST(mysql, read_script) {
//   std::vector<MR_SCRIPT *> array;
//
//   DBHANDLE dbh = connect_db("localhost", 3306, "mario", "mario", "chaojimali");
//   EXPECT_NE(nullptr, dbh);
//
//   EXPECT_GE(get_array_from_db(array, dbh, select_script_from_db, get_script),
//             0);
//
//   show_ptr_array(array, std::cout);
//
//   free_script_array(array);
//
//   disconnect_db(dbh);
// }
//
// TEST(mysql, read_pipeline_edge) {
//   std::vector<MR_PIPELINE_EDGE *> edges;
//   read_table(edges, select_edge_from_db, get_edge);
// }
//
// TEST(mysql, read_pipeline_exec) {
//   std::vector<MR_PIPELINE_EXEC *> ples;
//   read_table(ples, select_plexec_from_db, get_pipeline_exec);
// }
//
// TEST(mysql, read_plen_exec) {
//   std::vector<MR_PIPELINE_NODE_EXEC *> plens;
//   read_table(plens, select_plen_from_db, get_plen_exec);
// }
//
// TEST(mysql, read_host_status) {
//   std::vector<MR_HOST_STATUS *> status;
//   read_table(status, select_host_status_from_db, get_host_status);
// }
//
// TEST(mysql, read_all) {
//   DBHANDLE dbh = connect_db("localhost", 3306, "mario", "mario", "chaojimali");
//   EXPECT_NE(nullptr, dbh);
//
//   HostArray hosts;
//   PipeLineArray pls;
//   std::vector<MR_SCRIPT *> scripts;
//   std::vector<MR_PIPELINE_EDGE *> edges;
//   std::vector<MR_PIPELINE_EXEC *> ples;
//   std::vector<MR_PIPELINE_NODE_EXEC *> plens;
//   std::vector<MR_HOST_STATUS *> status;
//
//   EXPECT_GE(get_array_from_db(hosts, dbh, select_host_from_db, get_hosts), 0);
//   EXPECT_GE(get_array_from_db(pls, dbh, select_pipeline_from_db, get_pipeline),
//             0);
//   EXPECT_GE(get_array_from_db(scripts, dbh, select_script_from_db, get_script),
//             0);
//   EXPECT_GE(get_array_from_db(edges, dbh, select_edge_from_db, get_edge), 0);
//   EXPECT_GE(
//       get_array_from_db(ples, dbh, select_plexec_from_db, get_pipeline_exec),
//       0);
//   EXPECT_GE(get_array_from_db(plens, dbh, select_plen_from_db, get_plen_exec),
//             0);
//   EXPECT_GE(
//       get_array_from_db(status, dbh, select_pipeline_from_db, get_host_status),
//       0);
//
//   free_ptr_array(status);
//   free_ptr_array(plens);
//   free_ptr_array(ples);
//   free_ptr_array(edges);
//   free_ptr_array(pls);
//   free_ptr_array(hosts);
//   free_script_array(scripts);
//
//   disconnect_db(dbh);
// }
//
// #define offset(T, F) ((char *)&(F) - (char *)&(T))
//
// TEST(mario_data, set_maps) {
//   DBHANDLE dbh = connect_db("localhost", 3306, "mario", "mario", "chaojimali");
//   EXPECT_NE(nullptr, dbh);
//
//   HostArray hosts;
//   PipeLineArray pls;
//   std::vector<MR_SCRIPT *> scripts;
//   std::vector<MR_PIPELINE_EDGE *> edges;
//   std::vector<MR_PIPELINE_EXEC *> ples;
//   std::vector<MR_PIPELINE_NODE_EXEC *> plens;
//   std::vector<MR_HOST_STATUS *> status;
//
//   EXPECT_GE(get_array_from_db(hosts, dbh, select_host_from_db, get_hosts), 0);
//   EXPECT_GE(get_array_from_db(pls, dbh, select_pipeline_from_db, get_pipeline),
//             0);
//   EXPECT_GE(get_array_from_db(scripts, dbh, select_script_from_db, get_script),
//             0);
//   EXPECT_GE(get_array_from_db(edges, dbh, select_edge_from_db, get_edge), 0);
//   EXPECT_GE(
//       get_array_from_db(ples, dbh, select_plexec_from_db, get_pipeline_exec),
//       0);
//   EXPECT_GE(get_array_from_db(plens, dbh, select_plen_from_db, get_plen_exec),
//             0);
//   EXPECT_GE(get_array_from_db(status, dbh, select_host_status_from_db,
//                               get_host_status),
//             0);
//
//   MapId2Ptr mapHosts, mapPipeline, mapScript, mapEdges, mapPipelineExec,
//       mapPipelineNodeExec, mapHostStatus;
//   insert_int2ptr_map(mapHosts, hosts);
//   insert_int2ptr_map(mapPipeline, pls);
//   insert_int2ptr_map(mapScript, scripts);
//   insert_int2ptr_map(mapEdges, edges);
//   insert_int2ptr_map(mapPipelineExec, ples);
//   insert_int2ptr_map(mapPipelineNodeExec, plens);
//   insert_int2ptr_map(mapHostStatus, status);
//
//   EXPECT_NE(nullptr, mapHosts[0]);
//   EXPECT_NE(nullptr, mapHosts[1]);
//   EXPECT_EQ(mapHosts[0], hosts[0]);
//   EXPECT_EQ(mapHosts[1], hosts[1]);
//   EXPECT_EQ(mapHosts[10552], hosts[2]);
//   EXPECT_EQ(nullptr, mapHosts[100000000]);
//
//   MapStr2Ptr mapHostMinion;
//   MR_HOST host_;
//   insert_str2_ptr_map(mapHostMinion, hosts, offset(host_, host_.minion_id));
//   EXPECT_NE(mapHostMinion["new080027ADF439"], nullptr);
//   EXPECT_EQ(0,
//             strcmp(((MR_HOST *)(mapHostMinion["new080027ADF439"]))->minion_id,
//                    "new080027ADF439"));
//   EXPECT_EQ(mapHostMinion["*"], hosts[0]);
//   EXPECT_EQ(mapHostMinion["---"], hosts[1]);
//
//   {
//     MR_HOST_STATUS s_;
//     set_key_to_ptr(mapHosts, status, offset(s_, s_.host_id),
//                    offset(s_, s_.host));
//     set_key_to_ptr(mapPipelineExec, status, offset(s_, s_.pe_id),
//                    offset(s_, s_.pe));
//   }
//
//   {
//     MR_PIPELINE_NODE_EXEC p_;
//     set_key_to_ptr(mapPipelineExec, plens, offset(p_, p_.pe_id),
//                    offset(p_, p_.ple));
//     set_key_to_ptr(mapEdges, plens, offset(p_, p_.edge_id),
//                    offset(p_, p_.edge));
//     for (auto &p : plens)
//       p->host = reinterpret_cast<MR_HOST *>(mapHostMinion[p->minion_id]);
//   }
//
//   {
//     MR_PIPELINE_EXEC p_;
//     set_key_to_ptr(mapPipeline, ples, offset(p_, p_.pl_id),
//                    offset(p_, p_.pipeline));
//   }
//
//   {
//     for (auto &p : edges) {
//       p->pipeline = reinterpret_cast<MR_PIPELINE *>(mapPipeline[p->pl_id]);
//       p->src_host = reinterpret_cast<MR_HOST *>(mapHostMinion[p->src_min_id]);
//       p->trg_host = reinterpret_cast<MR_HOST *>(mapHostMinion[p->trg_min_id]);
//
//       p->src_node_ptr = (p->src_type == NODE_TYPE_SCRIPT)
//                             ? mapScript[p->src_id]
//                             : mapPipeline[p->src_id];
//       p->trg_node_ptr = (p->trg_type == NODE_TYPE_SCRIPT)
//                             ? mapScript[p->trg_id]
//                             : mapPipeline[p->trg_id];
//     }
//     MR_PIPELINE_EDGE *p = (MR_PIPELINE_EDGE *)edges[0];
//     EXPECT_EQ(strcmp(p->src_host->minion_id, p->src_min_id), 0);
//     EXPECT_EQ(strcmp(p->trg_host->minion_id, p->trg_min_id), 0);
//     EXPECT_EQ(p->pl_id, p->pipeline->id);
//     EXPECT_EQ(((MR_SCRIPT *)(p->src_node_ptr))->id, p->src_id);
//     EXPECT_EQ(((MR_SCRIPT *)(p->trg_node_ptr))->id, p->trg_id);
//
//     p = (MR_PIPELINE_EDGE *)edges[1];
//     EXPECT_EQ(strcmp(p->src_host->minion_id, p->src_min_id), 0);
//     EXPECT_EQ(strcmp(p->trg_host->minion_id, p->trg_min_id), 0);
//     EXPECT_EQ(p->pl_id, p->pipeline->id);
//     EXPECT_EQ(((MR_SCRIPT *)(p->src_node_ptr))->id, p->src_id);
//     EXPECT_EQ(((MR_SCRIPT *)(p->trg_node_ptr))->id, p->trg_id);
//   }
//
//   for (auto &p : scripts)
//     p->host = reinterpret_cast<MR_HOST *>(mapHosts[p->host_id]);
//
//   free_ptr_array(status);
//   free_ptr_array(plens);
//   free_ptr_array(ples);
//   free_ptr_array(edges);
//   free_ptr_array(pls);
//   free_ptr_array(hosts);
//   free_script_array(scripts);
//
//   disconnect_db(dbh);
// }
//
// #include "salt_api.h"
//
// TEST(salt, parse_json_job_new) {
//   SALT_JOB_NEW jobnew;
//   const char *json_data = {
//       "data: {\"tag\": \"salt/job/20161125075810733967/new\", \"data\": "
//       "{\"tgt_type\": \"glob\", \"jid\": \"20161125075810733967\", \"tgt\": "
//       "\"*\", \"_stamp\": \"2016-11-24T23:58:10.735307\", \"user\": \"root\", "
//       "\"arg\": [], \"fun\": \"test.ping\", \"minions\": [\"minion1\", "
//       "\"minion2\", \"minion3\", \"minion4\", \"minion5\", \"minion6\", "
//       "\"minion7\", \"minion8\", \"new080027006B3F\", \"new08002700A6BA\", "
//       "\"new08002701AABF\", \"new0800270247DC\", \"new080027026A50\", "
//       "\"new08002702F3BC\", \"new080027032B94\", \"new080027036BBF\", "
//       "\"new080027037DCB\", \"new08002703D960\", \"new080027051576\", "
//       "\"new08002705D9EC\", \"new08002706D144\", \"new0800270795D5\", "
//       "\"new08002709048A\", \"new080027094DD8\", \"new080027098A92\", "
//       "\"new08002709E6AE\", \"new08002709EAA1\", \"new0800270A5890\", "
//       "\"new0800270B05DF\", \"new0800270C00F4\", \"new0800270C6BFC\", "
//       "\"new0800270C7E44\", \"new0800270C8919\", \"new0800270E808E\", "
//       "\"new0800270F1B1F\", \"new0800270F7687\", \"new0800271142DC\", "
//       "\"new08002712E318\", \"new08002713723E\", \"new08002713E8D7\", "
//       "\"new080027148D9D\", \"new08002714A247\", \"new08002714CF73\", "
//       "\"new080027152097\", \"new08002715EA79\", \"new08002715F727\", "
//       "\"new08002715F837\", \"new0800271728E4\", \"new080027183037\", "
//       "\"new080027186629\", \"new08002719EB54\", \"new0800271D1C53\", "
//       "\"new0800271D6A4B\", \"new0800271FF76C\", \"new080027206C15\", "
//       "\"new08002720B77C\", \"new08002721DEB8\", \"new080027225131\", "
//       "\"new080027237D6A\", \"new0800272A1036\", \"new0800272AE70E\", "
//       "\"new0800272C6822\", \"new0800272CE050\", \"new0800272CEC6C\", "
//       "\"new0800272EB608\", \"new0800272EF16C\", \"new0800272F414E\", "
//       "\"new0800273070B2\", \"new08002731E083\", \"new0800273209BE\", "
//       "\"new0800273314F8\", \"new0800273327F8\", \"new080027344053\", "
//       "\"new08002734940D\", \"new08002734BD44\", \"new08002734EF5F\", "
//       "\"new080027356819\", \"new080027369BC5\", \"new08002736C32F\", "
//       "\"new0800273964FB\", \"new08002739FBC9\", \"new0800273AA955\", "
//       "\"new0800273B51A0\", \"new0800273C1D97\", \"new0800273C3481\", "
//       "\"new0800273D88D6\", \"new0800273D97A4\", \"new0800273DABB2\", "
//       "\"new0800274023ED\", \"new08002742C968\", \"new080027478C01\", "
//       "\"new08002748030F\", \"new08002749D32D\", \"new08002749F0E1\", "
//       "\"new0800274A78B6\", \"new0800274C64EE\", \"new0800274CDE22\", "
//       "\"new0800274E0986\", \"new0800274E22A3\", \"new08002750887B\", "
//       "\"new0800275247B1\", \"new080027538B56\", \"new08002753AF1A\", "
//       "\"new0800275431FC\", \"new080027557788\", \"new08002756A517\", "
//       "\"new08002756BF1C\", \"new08002756F0BE\", \"new08002757BB77\", "
//       "\"new0800275850EC\", \"new080027590EAC\", \"new08002759712F\", "
//       "\"new0800275A2A40\", \"new0800275B5B3D\", \"new0800275C9EA9\", "
//       "\"new0800275D7D4B\", \"new0800275F07FC\", \"new0800275F1235\", "
//       "\"new0800275F127A\", \"new0800275FE53F\", \"new080027609149\", "
//       "\"new08002761163E\", \"new08002761178C\", \"new08002762E240\", "
//       "\"new08002763D2D7\", \"new080027647E08\", \"new08002764DF81\", "
//       "\"new08002765FEF8\", \"new080027663D1E\", \"new080027664D98\", "
//       "\"new0800276663A1\", \"new08002766D5ED\", \"new08002767562E\", "
//       "\"new08002767A8A4\", \"new080027689F7B\", \"new080027697AAB\", "
//       "\"new0800276D15E9\", \"new0800276DB869\", \"new0800276EE7BE\", "
//       "\"new0800276F5E5F\", \"new080027703081\", \"new080027707B7D\", "
//       "\"new08002770A3F7\", \"new080027718E04\", \"new08002771BB7C\", "
//       "\"new08002772925E\", \"new08002773AB32\", \"new08002773E906\", "
//       "\"new0800277490AF\", \"new08002775CB70\", \"new080027761C7F\", "
//       "\"new080027763E36\", \"new08002776A472\", \"new0800277812D2\", "
//       "\"new08002779019E\", \"new0800277989D2\", \"new080027799F00\", "
//       "\"new0800277A77ED\", \"new0800277AD46F\", \"new0800277B6709\", "
//       "\"new0800277D075C\", \"new0800277D938D\", \"new0800277E6CF0\", "
//       "\"new0800277ED0C7\", \"new08002780B8F9\", \"new08002781425D\", "
//       "\"new080027817B05\", \"new080027817C8E\", \"new080027819354\", "
//       "\"new080027823FE1\", \"new08002783192A\", \"new08002784BB85\", "
//       "\"new080027856E62\", \"new08002785BD75\", \"new080027860204\", "
//       "\"new080027873965\", \"new0800278798FC\", \"new08002788E52E\", "
//       "\"new08002788FA42\", \"new0800278941C4\", \"new0800278988EC\", "
//       "\"new08002789A314\", \"new0800278B4BC2\", \"new0800278C122E\", "
//       "\"new0800278C340B\", \"new0800278F78BB\", \"new08002790058E\", "
//       "\"new08002792F405\", \"new08002793058D\", \"new08002793CC8A\", "
//       "\"new08002793FA1E\", \"new08002794627D\", \"new080027955089\", "
//       "\"new080027965E33\", \"new080027965E3B\", \"new08002796E83E\", "
//       "\"new0800279725CD\", \"new080027983D24\", \"new0800279C65EC\", "
//       "\"new0800279C86BA\", \"new0800279CA2D3\", \"new0800279CAC4D\", "
//       "\"new0800279DDDA6\", \"new0800279EE5F8\", \"new0800279FDCB0\", "
//       "\"new080027A12E30\", \"new080027A145AC\", \"new080027A24AEB\", "
//       "\"new080027A2A128\", \"new080027A39400\", \"new080027A3C893\", "
//       "\"new080027A40240\", \"new080027A490BE\", \"new080027A4C5F5\", "
//       "\"new080027A58B4C\", \"new080027A6BE9C\", \"new080027A6E229\", "
//       "\"new080027A6F335\", \"new080027A70AAF\", \"new080027A7F668\", "
//       "\"new080027A89C44\", \"new080027A97E41\", \"new080027AA6849\", "
//       "\"new080027ABE7D6\", \"new080027AC9609\", \"new080027ACF39F\", "
//       "\"new080027AD5721\", \"new080027ADC674\", \"new080027ADF439\", "
//       "\"new080027AF1F2C\", \"new080027B14207\", \"new080027B150F7\", "
//       "\"new080027B15A5B\", \"new080027B15CC3\", \"new080027B210E2\", "
//       "\"new080027B343EF\", \"new080027B368CD\", \"new080027B4AAAC\", "
//       "\"new080027B4E010\", \"new080027B53013\", \"new080027B58DCE\", "
//       "\"new080027B5F82D\", \"new080027B8257A\", \"new080027B82884\", "
//       "\"new080027B834F0\", \"new080027B86A13\", \"new080027B93F8E\", "
//       "\"new080027B9C8C2\", \"new080027BB38EA\", \"new080027BB4DAF\", "
//       "\"new080027BCCF7B\", \"new080027BCD47D\", \"new080027BF894A\", "
//       "\"new080027BFB11F\", \"new080027C076FA\", \"new080027C07FB0\", "
//       "\"new080027C0B8A4\", \"new080027C0EE66\", \"new080027C14A65\", "
//       "\"new080027C1976C\", \"new080027C22850\", \"new080027C252A8\", "
//       "\"new080027C272F3\", \"new080027C33408\", \"new080027C66FF1\", "
//       "\"new080027C74050\", \"new080027C7C27C\", \"new080027C7E2E1\", "
//       "\"new080027C7FC17\", \"new080027C8061F\", \"new080027C91EBC\", "
//       "\"new080027CA0124\", \"new080027CA866F\", \"new080027CA8DD1\", "
//       "\"new080027CD3D31\", \"new080027CEF62B\", \"new080027CFFB44\", "
//       "\"new080027D062CB\", \"new080027D2A4FB\", \"new080027D330ED\", "
//       "\"new080027D3BBCB\", \"new080027D48D22\", \"new080027D494C3\", "
//       "\"new080027D610D8\", \"new080027D63549\", \"new080027D707CC\", "
//       "\"new080027D79189\", \"new080027D9BCE6\", \"new080027D9EB92\", "
//       "\"new080027DA4553\", \"new080027DF3D9D\", \"new080027DF4D02\", "
//       "\"new080027DFC969\", \"new080027E151F8\", \"new080027E27959\", "
//       "\"new080027E2D43B\", \"new080027E2FE17\", \"new080027E44063\", "
//       "\"new080027E44860\", \"new080027E631D3\", \"new080027E655DB\", "
//       "\"new080027E69D6D\", \"new080027E888A4\", \"new080027E96AEF\", "
//       "\"new080027ED3EFB\", \"new080027EF0ED5\", \"new080027EF4377\", "
//       "\"new080027EF53F6\", \"new080027EF749F\", \"new080027F00919\", "
//       "\"new080027F02D44\", \"new080027F0FCA0\", \"new080027F19E03\", "
//       "\"new080027F20221\", \"new080027F30C0F\", \"new080027F31340\", "
//       "\"new080027F397AE\", \"new080027F52531\", \"new080027F578F3\", "
//       "\"new080027F58826\", \"new080027F81528\", \"new080027F91C29\", "
//       "\"new080027F9F307\", \"new080027FA2E8C\", \"new080027FC0FFC\", "
//       "\"new080027FCF7AD\", \"new080027FE808C\", \"new080027FF8AD0\", "
//       "\"old08002700F858\", \"old080027017DF5\", \"old08002701FEF1\", "
//       "\"old08002702B57A\", \"old080027033F76\", \"old080027053A33\", "
//       "\"old08002706C48F\", \"old08002706FE99\", \"old08002707A77C\", "
//       "\"old08002708AD8F\", \"old08002709C1DC\", \"old08002709EAA8\", "
//       "\"old0800270A9348\", \"old0800270ABFCB\", \"old0800270BED53\", "
//       "\"old0800270C0EA8\", \"old0800270C60EE\", \"old0800270D3ED7\", "
//       "\"old0800270E6BA3\", \"old0800270E6DBB\", \"old0800270E6F04\", "
//       "\"old0800270E9DE3\", \"old0800270EA39C\", \"old0800270EADC7\", "
//       "\"old080027106D96\", \"old08002710DACC\", \"old08002713B527\", "
//       "\"old080027146AF3\", \"old080027151C4E\", \"old080027158246\", "
//       "\"old08002716E8BB\", \"old08002716EA59\", \"old080027170D70\", "
//       "\"old080027181BEF\", \"old08002719110D\", \"old0800271A0181\", "
//       "\"old0800271A160E\", \"old0800271ABC4B\", \"old0800271BB437\", "
//       "\"old0800271C2535\", \"old0800271C305B\", \"old0800271C82FC\", "
//       "\"old0800271CA0E6\", \"old0800271DB2C3\", \"old0800271DDFBD\", "
//       "\"old0800272036B6\", \"old080027207600\", \"old08002720AEA3\", "
//       "\"old08002721735F\", \"old080027224C8E\", \"old08002722F118\", "
//       "\"old0800272358D5\", \"old0800272367BA\", \"old08002723B8A6\", "
//       "\"old080027249540\", \"old08002724CB79\", \"old08002724E6A1\", "
//       "\"old080027273A0E\", \"old080027276A54\", \"old08002727AA2A\", "
//       "\"old08002729F6D7\", \"old0800272A6E89\", \"old0800272CDDF5\", "
//       "\"old0800272E6F47\", \"old0800272F0DE3\", \"old0800272F4F72\", "
//       "\"old080027311B7A\", \"old08002731A10F\", \"old08002731FD42\", "
//       "\"old08002732260A\", \"old080027332E8D\", \"old08002733B25A\", "
//       "\"old0800273464A7\", \"old080027354919\", \"old08002735C9EA\", "
//       "\"old080027365607\", \"old080027370BF3\", \"old08002738C55E\", "
//       "\"old0800273906AC\", \"old0800273B868C\", \"old0800273DA0A2\", "
//       "\"old0800273E0C5C\", \"old0800273E6ACD\", \"old0800273FE152\", "
//       "\"old0800274033F9\", \"old080027407D0D\", \"old08002740FFA6\", "
//       "\"old080027415422\", \"old08002741DA35\", \"old080027427F66\", "
//       "\"old08002742C1C2\", \"old0800274353F3\", \"old08002746F846\", "
//       "\"old080027491B5A\", \"old08002749CB7B\", \"old0800274A431F\", "
//       "\"old0800274B3BCF\", \"old0800274C064E\", \"old0800274C0C96\", "
//       "\"old0800274EBA36\", \"old080027503EF5\", \"old0800275048CE\", "
//       "\"old08002750C0DB\", \"old080027515116\", \"old080027522A97\", "
//       "\"old080027524631\", \"old0800275352CA\", \"old0800275375CD\", "
//       "\"old08002753900C\", \"old080027539807\", \"old08002753C419\", "
//       "\"old08002753D0B2\", \"old0800275534E5\", \"old08002755483D\", "
//       "\"old0800275668B5\", \"old080027577EF5\", \"old08002759F481\", "
//       "\"old08002759F4B6\", \"old0800275B1476\", \"old0800275B3593\", "
//       "\"old0800275B9FA3\", \"old0800275BD820\", \"old0800275C065E\", "
//       "\"old0800275D1CBE\", \"old0800275F60CB\", \"old080027606181\", "
//       "\"old08002760F346\", \"old08002762B0A1\", \"old08002765B525\", "
//       "\"old08002765F5BA\", \"old08002766A5BF\", \"old08002769DE31\", "
//       "\"old0800276A49FF\", \"old0800276A60B8\", \"old0800276AE6A4\", "
//       "\"old0800276C8E95\", \"old0800276CA03F\", \"old0800276CABFA\", "
//       "\"old0800276E5ACA\", \"old0800276EB34D\", \"old0800276F03FC\", "
//       "\"old0800276F4445\", \"old0800276F5086\", \"old08002770A017\", "
//       "\"old080027713729\", \"old080027715E23\", \"old080027716D9B\", "
//       "\"old080027720DC9\", \"old080027722C97\", \"old080027731D40\", "
//       "\"old08002773AA8E\", \"old08002773B0F0\", \"old0800277462E8\", "
//       "\"old08002774E5C4\", \"old08002774F76E\", \"old080027761BFB\", "
//       "\"old08002777DED0\", \"old080027789636\", \"old080027789CA4\", "
//       "\"old080027794FD7\", \"old0800277A26BD\", \"old0800277AB267\", "
//       "\"old0800277ACE55\", \"old0800277AF5BE\", \"old0800277B8085\", "
//       "\"old0800277BC60C\", \"old0800277E436B\", \"old0800277F4D3D\", "
//       "\"old0800277F6D15\", \"old080027807D65\", \"old08002781DB03\", "
//       "\"old080027834986\", \"old08002783ED15\", \"old080027844C78\", "
//       "\"old080027856310\", \"old080027856836\", \"old08002786E5C8\", "
//       "\"old080027871707\", \"old08002788B195\", \"old0800278D6F21\", "
//       "\"old0800278E33D5\", \"old0800278EFF7A\", \"old0800278F99C0\", "
//       "\"old08002790079C\", \"old0800279076C6\", \"old080027911120\", "
//       "\"old08002793BC6F\", \"old08002794AF1B\", \"old080027954B44\", "
//       "\"old08002795F08D\", \"old08002795F97E\", \"old080027961E90\", "
//       "\"old08002796B693\", \"old080027974E91\", \"old08002798BF64\", "
//       "\"old08002798D7C6\", \"old08002799196B\", \"old080027998ED2\", "
//       "\"old0800279A46D7\", \"old0800279CDA51\", \"old0800279D27B5\", "
//       "\"old0800279DB720\", \"old0800279EA80D\", \"old0800279EEA0A\", "
//       "\"old0800279F6BDC\", \"old0800279FDEB4\", \"old080027A207FF\", "
//       "\"old080027A337AF\", \"old080027A3F1E8\", \"old080027A631F7\", "
//       "\"old080027A63244\", \"old080027A640AF\", \"old080027A6AEFE\", "
//       "\"old080027A7AFA0\", \"old080027A8190B\", \"old080027A83B11\", "
//       "\"old080027A86865\", \"old080027AA2545\", \"old080027AA9E30\", "
//       "\"old080027AAA8A3\", \"old080027AB4205\", \"old080027AB6698\", "
//       "\"old080027AB70FB\", \"old080027AB9A89\", \"old080027AC7211\", "
//       "\"old080027ACF4F1\", \"old080027AF9702\", \"old080027AFB1CE\", "
//       "\"old080027B001A6\", \"old080027B0F926\", \"old080027B2FA4F\", "
//       "\"old080027B4A151\", \"old080027B4C3C5\", \"old080027B4E438\", "
//       "\"old080027B5704A\", \"old080027B73874\", \"old080027B7E964\", "
//       "\"old080027B929E0\", \"old080027B99898\", \"old080027B9CB5C\", "
//       "\"old080027B9FF2F\", \"old080027BA9479\", \"old080027BB6313\", "
//       "\"old080027BB8F4C\", \"old080027BB9468\", \"old080027BBDB4F\", "
//       "\"old080027BD47B1\", \"old080027BD834A\", \"old080027BEA97F\", "
//       "\"old080027BFCFDF\", \"old080027C0C9B4\", \"old080027C0CE51\", "
//       "\"old080027C162DF\", \"old080027C16FEE\", \"old080027C17F02\", "
//       "\"old080027C234D6\", \"old080027C2F823\", \"old080027C39ECF\", "
//       "\"old080027C3C451\", \"old080027C4E92B\", \"old080027C5368B\", "
//       "\"old080027C549EC\", \"old080027C5914D\", \"old080027C60F7B\", "
//       "\"old080027C7BE22\", \"old080027C82FE8\", \"old080027C8AACB\", "
//       "\"old080027C8BFA4\", \"old080027CA2B61\", \"old080027CA957B\", "
//       "\"old080027CD4664\", \"old080027CD9A49\", \"old080027CED61A\", "
//       "\"old080027CF44AA\", \"old080027D1533E\", \"old080027D198D2\", "
//       "\"old080027D2E855\", \"old080027D44E5E\", \"old080027D55007\", "
//       "\"old080027D6217B\", \"old080027D73795\", \"old080027D76755\", "
//       "\"old080027D7974D\", \"old080027D8A76D\", \"old080027DB2747\", "
//       "\"old080027DB31DB\", \"old080027DB4144\", \"old080027DBBCE5\", "
//       "\"old080027DC74A6\", \"old080027DCA047\", \"old080027DE246C\", "
//       "\"old080027DE487F\", \"old080027DF70F1\", \"old080027DFB8E8\", "
//       "\"old080027DFF54D\", \"old080027E20FFE\", \"old080027E58C05\", "
//       "\"old080027E5B5BA\", \"old080027E6D6A9\", \"old080027E867AB\", "
//       "\"old080027E8DDBB\", \"old080027E8DFC3\", \"old080027E936D6\", "
//       "\"old080027E99725\", \"old080027EBA277\", \"old080027ED2BBA\", "
//       "\"old080027EE13E3\", \"old080027EE5925\", \"old080027EFF09B\", "
//       "\"old080027F0FBBA\", \"old080027F6C8DA\", \"old080027F70711\", "
//       "\"old080027F7659B\", \"old080027F7ED5D\", \"old080027F8BD48\", "
//       "\"old080027F97826\", \"old080027FAFD2E\", \"old080027FCBA25\", "
//       "\"old080027FCC315\", \"old080027FEC857\", \"old080027FF5284\", "
//       "\"Server178\", \"Server179\"]}}"};
//   EXPECT_EQ(0, parse_salt_job_new(&jobnew, (char*)json_data + 6));
//   EXPECT_EQ(jobnew.fun, "test.ping");
//   EXPECT_EQ(jobnew.jid, "20161125075810733967");
//   EXPECT_EQ(jobnew.stamp, "2016-11-24T23:58:10.735307");
//   EXPECT_EQ(jobnew.tag, "salt/job/20161125075810733967/new");
//   EXPECT_EQ(jobnew.tgt, "*");
//   EXPECT_EQ(jobnew.tgt_type, "glob");
//   EXPECT_EQ(jobnew.user, "root");
//   EXPECT_EQ(jobnew.fun, "test.ping");
//   EXPECT_EQ(jobnew.arg.size(), 0);
//   EXPECT_EQ(jobnew.minions.size(), 651);
//
//   SALT_JOB job;
//   EXPECT_EQ(0, parse_salt_job(job, json_data));
//   EXPECT_EQ(job.type, SALT_JOB_TYPE_NEW);
//   SALT_JOB_NEW* jn = (SALT_JOB_NEW*)job.ptr;
//   EXPECT_EQ(jn->fun, "test.ping");
//   EXPECT_EQ(jn->jid, "20161125075810733967");
//   EXPECT_EQ(jn->stamp, "2016-11-24T23:58:10.735307");
//   EXPECT_EQ(jn->tag, "salt/job/20161125075810733967/new");
//   EXPECT_EQ(jn->tgt, "*");
//   EXPECT_EQ(jn->tgt_type, "glob");
//   EXPECT_EQ(jn->user, "root");
//   EXPECT_EQ(jn->fun, "test.ping");
//   EXPECT_EQ(jn->arg.size(), 0);
//   EXPECT_EQ(jn->minions.size(), 651);
//   free_salt_job(job);
//
//
//   const char *json_data_2 = {
//       "data: "
//       "{                                                "
//       "  \"tag\": \"salt/job/20161123065056424864/new\","
//       "  \"data\": {                                    "
//       "    \"tgt_type\": \"glob\",                      "
//       "    \"jid\": \"20161123065056424864\",           "
//       "    \"tgt\": \"old08002759F4B6\",                "
//       "    \"_stamp\": \"2016-11-22T22:50:56.426503\",  "
//       "    \"user\": \"root\",                          "
//       "    \"arg\": [                                   "
//       "      \"c:new-saltExecClient.exe abcd\"          "
//       "    ],                                           "
//       "    \"fun\": \"cmd.run_all\",                    "
//       "    \"minions\": [                               "
//       "      \"old08002759F4B6\"                        "
//       "    ]                                            "
//       "  }                                              "
//       "}                                                "
//   };
//
//   jobnew.arg.clear();
//   jobnew.minions.clear();
//
//   EXPECT_EQ(0, parse_salt_job_new(&jobnew, (char*)json_data_2 + 6));
//   EXPECT_EQ(jobnew.fun, "cmd.run_all");
//   EXPECT_EQ(jobnew.jid, "20161123065056424864");
//   EXPECT_EQ(jobnew.stamp, "2016-11-22T22:50:56.426503");
//   EXPECT_EQ(jobnew.tag, "salt/job/20161123065056424864/new");
//   EXPECT_EQ(jobnew.tgt, "old08002759F4B6");
//   EXPECT_EQ(jobnew.tgt_type, "glob");
//   EXPECT_EQ(jobnew.user, "root");
//   EXPECT_EQ(jobnew.fun, "cmd.run_all");
//   EXPECT_EQ(jobnew.arg.size(), 1);
//   EXPECT_EQ(jobnew.minions.size(), 1);
//   EXPECT_EQ(jobnew.arg[0], "c:new-saltExecClient.exe abcd");
//   EXPECT_EQ(jobnew.minions[0], "old08002759F4B6");
//
//   EXPECT_EQ(0, parse_salt_job(job, json_data_2));
//   EXPECT_EQ(job.type, SALT_JOB_TYPE_NEW);
//   jn = (SALT_JOB_NEW*)job.ptr;
//   EXPECT_EQ(jn->fun, "cmd.run_all");
//   EXPECT_EQ(jn->jid, "20161123065056424864");
//   EXPECT_EQ(jn->stamp, "2016-11-22T22:50:56.426503");
//   EXPECT_EQ(jn->tag, "salt/job/20161123065056424864/new");
//   EXPECT_EQ(jn->tgt, "old08002759F4B6");
//   EXPECT_EQ(jn->tgt_type, "glob");
//   EXPECT_EQ(jn->user, "root");
//   EXPECT_EQ(jn->fun, "cmd.run_all");
//   EXPECT_EQ(jn->arg.size(), 1);
//   EXPECT_EQ(jn->minions.size(), 1);
//   EXPECT_EQ(jn->arg[0], "c:new-saltExecClient.exe abcd");
//   EXPECT_EQ(jn->minions[0], "old08002759F4B6");
//   free_salt_job(job);
// }
//
// TEST(salt, parse_json_job_ret) {
//   SALT_JOB_RET ret;\
//   const char* json_data = {
//       "data: {\"tag\": \"salt/job/20161123065056424864/ret/old08002759F4B6\", \"data\":             "
//       "{\"_stamp\": \"2016-11-22T22:50:56.749570\", \"return\": {\"pid\": 2976, \"retcode\": 0,     "
//       "\"stderr\": \"aaa\", \"stdout\": \"bbb\"}, \"retcode\": 0, \"success\": true, \"cmd\": \"_return\","
//       "\"jid\": \"20161123065056424864\", \"fun\": \"cmd.run_all\", \"id\": \"old08002759F4B6\"}}   "
//   };
//
//   EXPECT_EQ(0, parse_salt_job_ret(&ret, (char*)json_data + 6));
//   EXPECT_EQ(ret.tag, "salt/job/20161123065056424864/ret/old08002759F4B6");
//   EXPECT_EQ(ret.stamp, "2016-11-22T22:50:56.749570");
//   EXPECT_EQ(ret.pid, 2976);
//   EXPECT_EQ(ret.retcode, 0);
//   EXPECT_EQ(ret.stderr, "aaa");
//   EXPECT_EQ(ret.stdout, "bbb");
//   EXPECT_EQ(ret.success, true);
//   EXPECT_EQ(ret.cmd, "_return");
//   EXPECT_EQ(ret.jid, "20161123065056424864");
//   EXPECT_EQ(ret.fun, "cmd.run_all");
//   EXPECT_EQ(ret.minion_id, "old08002759F4B6");
//   EXPECT_EQ(ret.rettype, RETURN_TYPE_OBJECT);
//
//   SALT_JOB job;
//   EXPECT_EQ(0, parse_salt_job(job, json_data));
//   EXPECT_EQ(job.type, SALT_JOB_TYPE_RET);
//   SALT_JOB_RET* jr = (SALT_JOB_RET*)job.ptr;
//   EXPECT_EQ(jr->tag, "salt/job/20161123065056424864/ret/old08002759F4B6");
//   EXPECT_EQ(jr->stamp, "2016-11-22T22:50:56.749570");
//   EXPECT_EQ(jr->pid, 2976);
//   EXPECT_EQ(jr->retcode, 0);
//   EXPECT_EQ(jr->stderr, "aaa");
//   EXPECT_EQ(jr->stdout, "bbb");
//   EXPECT_EQ(jr->success, true);
//   EXPECT_EQ(jr->cmd, "_return");
//   EXPECT_EQ(jr->jid, "20161123065056424864");
//   EXPECT_EQ(jr->fun, "cmd.run_all");
//   EXPECT_EQ(jr->minion_id, "old08002759F4B6");
//   EXPECT_EQ(jr->rettype, RETURN_TYPE_OBJECT);
//   free_salt_job(job);
//
//
//   const char* json_data_2 = {
//       "data: {\"tag\": \"salt/job/20161123065414567343/ret/old0800277AF5BE\", \"data\":    "
//       "{\"_stamp\": \"2016-11-22T22:54:14.580881\", \"return\": true, \"retcode\": 123,      "
//       "\"success\": false, \"cmd\": \"_return\", \"jid\": \"20161123065414567343\", \"fun\":"
//       "\"test.ping\", \"id\": \"old0800277AF5BE\"}}                                        "
//   };
//
//   EXPECT_EQ(0, parse_salt_job_ret(&ret, (char*)json_data_2 + 6));
//   EXPECT_EQ(ret.tag, "salt/job/20161123065414567343/ret/old0800277AF5BE");
//   EXPECT_EQ(ret.stamp, "2016-11-22T22:54:14.580881");
//   EXPECT_EQ(ret.pid, 0);
//   EXPECT_EQ(ret.retcode, 123);
//   EXPECT_EQ(ret.stderr, "");
//   EXPECT_EQ(ret.stdout, "");
//   EXPECT_EQ(ret.success, false);
//   EXPECT_EQ(ret.cmd, "_return");
//   EXPECT_EQ(ret.jid, "20161123065414567343");
//   EXPECT_EQ(ret.fun, "test.ping");
//   EXPECT_EQ(ret.minion_id, "old0800277AF5BE");
//   EXPECT_EQ(ret.rettype, RETURN_TYPE_BOOL);
//
//   EXPECT_EQ(0, parse_salt_job(job, json_data_2));
//   EXPECT_EQ(job.type, SALT_JOB_TYPE_RET);
//   jr = (SALT_JOB_RET*)job.ptr;
//   EXPECT_EQ(jr->tag, "salt/job/20161123065414567343/ret/old0800277AF5BE");
//   EXPECT_EQ(jr->stamp, "2016-11-22T22:54:14.580881");
//   EXPECT_EQ(jr->pid, 0);
//   EXPECT_EQ(jr->retcode, 123);
//   EXPECT_EQ(jr->stderr, "");
//   EXPECT_EQ(jr->stdout, "");
//   EXPECT_EQ(jr->success, false);
//   EXPECT_EQ(jr->cmd, "_return");
//   EXPECT_EQ(jr->jid, "20161123065414567343");
//   EXPECT_EQ(jr->fun, "test.ping");
//   EXPECT_EQ(jr->minion_id, "old0800277AF5BE");
//   EXPECT_EQ(jr->rettype, RETURN_TYPE_BOOL);
//   free_salt_job(job);
// }
//
