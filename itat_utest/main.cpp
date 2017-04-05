#include <functional>
#include <gtest/gtest.h>
#include <iostream>
#include <thread>
#include <chrono>
#include "mario.hpp"
#include "httpapi.hpp"
#include "saltapi.hpp"

using namespace std;
using namespace itat;



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

//
// static const char* send_str[] = {
//     "GET /api/pyaxa/foo HTTP/1.1\r\n"
//     "Host: localhost:8000\r\n"
//     "Accept: */*\r\n"
//     "Content-Length: 28\r\n"
//     "Content-Type: application/x-www-form-urlencoded\r\n"
//     "\r\n"
//     "aaa=bbb&action=get&user=sean",
//
//     "POST /api/pyaxa/foo HTTP/1.1\r\n"
//     "Host: localhost:8000\r\n"
//     "Accept: */*\r\n"
//     "Content-Length: 40\r\n"
//     "Content-Type: application/x-www-form-urlencoded\r\n"
//     "\r\n"
//     "aaa=bbb&action=post&user=sean&pass=ooooo",
//
//     "DELETE /api/pyaxa/foo HTTP/1.1\r\n"
//     "Host: localhost:8000\r\n"
//     "Accept: */*\r\n"
//     "Content-Length: 28\r\n"
//     "Content-Type: application/x-www-form-urlencoded\r\n"
//     "\r\n"
//     "aaa=bbb&action=get&user=sean",
//
//     "POST /api/pyaxa/foo HTTP/1.1\r\n"
//     "Host: localhost:8000\r\n"
//     "Accept: */*\r\n"
//     "Content-Length: 28\r\n"
//     "Content-Type: application/x-www-form-urlencoded\r\n"
//     "\r\n"
//     "aaa=bbb&user=sean&pass=ooooo",
//
//
//     "POST /event/pyaxa HTTP/1.1\r\n"
//     "Host: localhost:8000\r\n"
//     "Accept: */*\r\n"
//     "Content-Length: 41\r\n"
//     "Content-Type: application/x-www-form-urlencoded\r\n"
//     "\r\n"
//     "aaa=bbb&action=event&user=sean&pass=ooooo",
//
//     nullptr,
// };
//

/*
static int myfun (const char* data, size_t len, PARAM param1, PARAM param2) {
  (void)param1;
  (void)param2;
  itat::show_cstring(data, len);
  return 0;
}

static void run_event_client() {
    char buf[BUFSIZE];
    HTTP_API_PARAM param("127.0.0.1", 32001, myfun, nullptr, nullptr);
    itat::itat_httpc(param, buf, send_str[4]);
}


TEST(itat_salt, SALT_JOB) {
  MapStr2Ptr<SALT_JOB> jobs;
  SALT_JOB* job = new SALT_JOB;
  SALT_JOB_RET* ret = new SALT_JOB_RET;
  job->minion_ret.insert(std::make_pair("hahah", ret));

  jobs.insert(std::make_pair("aaa", job));
}

*/
TEST(itat_salt, salt_api_login) {
  set_default_callback();
  HTTP_API_PARAM param("10.10.10.19", 8000, parse_token_fn, nullptr, nullptr);
  EXPECT_EQ(0, salt_api_login(&param, "salt-test", "hongt@8a51"));
}


TEST(itat_salt, salt_api_testping) {
  set_default_callback();
  HTTP_API_PARAM param("10.10.10.19", 8000, nullptr, nullptr, nullptr);
  EXPECT_EQ(0, salt_api_testping(&param, "old*"));
}


TEST(itat_ssalt, salt_api_async_cmd_runall) {
  set_default_callback();
  HTTP_API_PARAM param("10.10.10.19", 8000, nullptr, nullptr, nullptr);
  EXPECT_EQ(0, salt_api_async_cmd_runall(&param, "old*", "dir"));
}

void events() {
  HTTP_API_PARAM param("10.10.10.19", 8000, nullptr, nullptr, nullptr);
  EXPECT_EQ(0, salt_api_events(&param));
}

TEST(itat_salt, salt_api_events) {
  set_default_callback();
  g_run = 1;
//  std::thread t(events);
  HTTP_API_PARAM param_event("10.10.10.19", 8000, nullptr, nullptr, nullptr);
  std::thread t(salt_api_events, &param_event);

  std::this_thread::sleep_for(std::chrono::seconds(5));

  HTTP_API_PARAM param("10.10.10.19", 8000, parse_token_fn, nullptr, nullptr);
  EXPECT_EQ(0, salt_api_login(&param, "salt-test", "hongt@8a51"));
  param.rf = nullptr;
  EXPECT_EQ(0, salt_api_testping(&param, "old*"));
  EXPECT_EQ(0, salt_api_async_cmd_runall(&param,
                                   "old*",
                                   "dir"));
  g_run = 0;
  t.join();
}


TEST(functional, get_rule) {
  EXPECT_EQ(1, 1);
}


