#include "mario_data.h"
#include "salt_api.h"
#include <iostream>
#include <thread>
#include <curl/curl.h>
#include "http_client.h"

#include <pthread.h>

using namespace std;


int main(int argc, char *argv[]) {
  int run = 1;
  (void)argc;
  (void)argv;

  // curl_get_token();
  // curl_salt_event();


  salt_api_login("10.10.10.19", 8000);

  // std::thread t1(salt_api_testrun, "10.10.10.19", 8000);
  // std::thread t2(salt_api_testping, "10.10.10.19", 8000);
  // std::thread t3(salt_api_testrun, "10.10.10.19", 8000);
  // std::thread t4(salt_api_testrun, "10.10.10.19", 8000);
  // std::thread t5(salt_api_testping, "10.10.10.19", 8000);
  // std::thread t6(salt_api_testrun, "10.10.10.19", 8000);
  //
  // t1.join();
  // t2.join();
  // t3.join();
  // t4.join();
  // t5.join();
  // t6.join();

  std::thread t1(salt_api_events, "10.10.10.19", 8000, &run);

  std::this_thread::sleep_for(std::chrono::seconds(30));
  run = 0;

  t1.join();

  return 0;
}
