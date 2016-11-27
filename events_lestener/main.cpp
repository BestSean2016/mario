#include <iostream>
#include "mario_data.h"
#include "salt_api.h"
#include <thread>

using namespace std;


int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  curl_get_token();
//  std::thread tCheck(thread_check_timer_out);
  std::thread tRun(thread_run_pipeline);

  //curl_salt_event();

  tRun.join();
  // tCheck.join();
  return 0;
}
