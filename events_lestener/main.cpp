#include <iostream>
#include "mario_data.h"
#include "salt_api.h"
#include <thread>

using namespace std;


int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  curl_get_token();

  std::thread t(thread_check_timer_out);
  curl_salt_event();
  t.join();
  return 0;
}
