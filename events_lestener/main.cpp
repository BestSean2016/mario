#include "mario_data.h"
#include "salt_api.h"
#include <iostream>
#include "http_client.h"
#include <thread>
#include <signal.h>

using namespace std;

static int run = 1;

void got_signal(int sig) {
  printf("got signal %d\n", sig);
  run = 0;
}

static uint64_t pid = 10000;
void run_test_cmd() {
  while(run) {
    std::this_thread::sleep_for(std::chrono::seconds(20));
    std::thread t1(salt_api_test_cmdrun, "10.10.10.19", 8000, pid++);
    std::thread t2(salt_api_testping, "10.10.10.19", 8000, pid++);
    t1.join();
    t2.join();

    if (!(pid % 1000)) //renew token
      salt_api_login("10.10.10.19", 8000);
  }
}

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  // curl_get_token();
  // curl_salt_event();

  signal(SIGINT , got_signal);
  signal(SIGKILL, got_signal);
  signal(SIGSTOP, got_signal);

  salt_api_login("10.10.10.19", 8000);

  std::thread t2(thread_check_timer_out, &run);

  std::thread t1(salt_api_events, "10.10.10.19", 8000, &run);
  //std::thread::id t1_id = t1.get_id();

  run_test_cmd();

  t1.detach();
  t2.detach();

  jobmap_cleanup(&gjobmap);

  return 0;
}
