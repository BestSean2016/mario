#include <gtest/gtest.h>
#include <iostream>
#include <thread>
#include <chrono>
#include "httpapi.hpp"
#include "saltapi.hpp"

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



static const char* send_str[] = {
    "GET /api/pyaxa/foo HTTP/1.1\r\n"
    "Host: localhost:8000\r\n"
    "Accept: */*\r\n"
    "Content-Length: 28\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "\r\n"
    "aaa=bbb&action=get&user=sean",

    "POST /api/pyaxa/foo HTTP/1.1\r\n"
    "Host: localhost:8000\r\n"
    "Accept: */*\r\n"
    "Content-Length: 40\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "\r\n"
    "aaa=bbb&action=post&user=sean&pass=ooooo",

    "DELETE /api/pyaxa/foo HTTP/1.1\r\n"
    "Host: localhost:8000\r\n"
    "Accept: */*\r\n"
    "Content-Length: 28\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "\r\n"
    "aaa=bbb&action=get&user=sean",

    "POST /api/pyaxa/foo HTTP/1.1\r\n"
    "Host: localhost:8000\r\n"
    "Accept: */*\r\n"
    "Content-Length: 28\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "\r\n"
    "aaa=bbb&user=sean&pass=ooooo",


    "POST /event/pyaxa HTTP/1.1\r\n"
    "Host: localhost:8000\r\n"
    "Accept: */*\r\n"
    "Content-Length: 41\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "\r\n"
    "aaa=bbb&action=event&user=sean&pass=ooooo",

    nullptr,
};



static int myfun (const char* data, size_t len, PARAM param1, PARAM param2) {
  (void)param1;
  (void)param2;
  show_cstring(data, len);
  return 0;
}

static void run_event_client() {
    char buf[BUFSIZE];
    itat_httpc("127.0.0.1", 32001, buf, send_str[4], myfun, nullptr, nullptr);
}

TEST(itat_httpd, server) {
  g_run = 1;

  SetUri uriEvents;
  uriEvents.insert("/event/pyaxa");

  SetUri uriApis;
  uriApis.insert("/api/pyaxa");

  URI_REQUEST urireq;
  urireq.insert(std::make_pair("event", uriEvents));
  urireq.insert(std::make_pair("api", uriApis));

  std::thread t(itat_httpd, 32001, &urireq);
  std::thread t_event_sender(event_sender);

  std::this_thread::sleep_for(std::chrono::seconds(5));

  //std::thread tEventClent(run_event_client);

  std::this_thread::sleep_for(std::chrono::seconds(30000));

  g_run = 0;

  //tEventClent.join();
  t_event_sender.join();
  t.join();
}

TEST(itat_salt, SALT_JOB) {
  MapStr2Ptr<SALT_JOB> jobs;
  SALT_JOB* job = new SALT_JOB;
  SALT_JOB_RET* ret = new SALT_JOB_RET;
  job->minion_ret.insert(std::make_pair("hahah", ret));

  jobs.insert(std::make_pair("aaa", job));
}


TEST(itat_salt, salt_api_login) {
  set_default_callback();
  std::cout << "return code " << salt_api_login("10.10.10.19", 8000, "sean1", "hongt@8a51") << std::endl;
}


TEST(itat_salt, salt_api_testping) {
  set_default_callback();
  //std::cout << "return code " << salt_api_testping("10.10.10.19", 8000, nullptr, nullptr) << std::endl;
}
