#include <iostream>
#include "pipeline.h"
#include "http_client.h"

#include <thread>
#include <signal.h>

using namespace std;

static int run = 1;

void got_signal(int sig) {
  printf("got signal %d\n", sig);
  run = 0;
}

void run_test_cmd() {
  int64_t test_ping_pid = 24683579;
  int i = 0;
  while(run) {
    std::this_thread::sleep_for(std::chrono::seconds(60));
    salt_api_testping("10.10.10.19", 8000, test_ping_pid);
    if (!(++i % 60)) {
      salt_api_login("10.10.10.19", 8000);
      i = 0;
    }
  }
}

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  if (0 != gen_diamond_pipeline(120, 4))
      return -1;

  signal(SIGINT , got_signal);
  signal(SIGKILL, got_signal);
  signal(SIGSTOP, got_signal);

  salt_api_login("10.10.10.19", 8000);


  std::thread tEvent(salt_api_events, "10.10.10.19", 8000, &run);
  std::this_thread::sleep_for(std::chrono::seconds(10));
  std::thread tTimerOut(thread_check_timer_out, &run);
  // std::thread tTestPing(run_test_cmd);

  run_pipeline(&run);
  run = 0;

  std::this_thread::sleep_for(std::chrono::seconds(20));

  // tTestPing.join();
  tTimerOut.join();
  tEvent.join();

  jobmap_cleanup(&gjobmap);

  release_pipeline();

  return 0;
}

