#include <gtest/gtest.h>
#include <iostream>
#include "itat.h"
#include "http_api.hpp"
#include <thread>
#include <chrono>
#include <zmq.h>

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



static const char* send_str2[] = {
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


TEST(itat_httpd, server) {
  g_run = 1;
  std::thread t(itat_httpd, 32001);

  std::thread t_event_sender(event_sender);

  std::this_thread::sleep_for(std::chrono::seconds(50));

  //char buf[BUFSIZE];
  //itat_httpc("127.0.0.1", 32001, buf, BUFSIZE, send_str2[4]);

  g_run = 0;
  t_event_sender.join();

  t.join();
}
