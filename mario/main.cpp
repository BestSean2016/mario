#include <iostream>
#include "pipeline.h"
#include "http_client.h"
#include <unistd.h>
#include <thread>
#include <signal.h>
#include "utility.h"
#include "threadpool.h"

extern threadpool_t* thpool;

using namespace std;

int run = 1;
int *event_socket = 0;

char server_ip[16] = {0};
int server_port = 0;

void got_signal(int sig) {
  printf("got signal %d\n", sig);
  run = 0;
}

void run_test_cmd() {
  int64_t test_ping_pid = 24683579;
  int i = 0;
  while(run) {
    std::this_thread::sleep_for(std::chrono::seconds(60));
    salt_api_testping("10.10.10.19", 8000, test_ping_pid, -1);
    if (!(++i % 60)) {
      salt_api_login("10.10.10.19", 8000);
      i = 0;
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc != 5) {
    std::cout << "Usage: mario <amount of node> <amont of branch> <server ip> <server port>\n";
    return 0;
  }

  thpool = threadpool_create(5, atoi(argv[1]) * 3, 0);
  if (0 != gen_diamond_pipeline(atoi(argv[1]), atoi(argv[2])))
      return -1;

  signal(SIGINT , got_signal);
  signal(SIGKILL, got_signal);
  signal(SIGSTOP, got_signal);

  strcpy_s(server_ip, 16, argv[3]);
  server_port = atoi(argv[4]);
  std::cout << "Login to server, ";
  salt_api_login(server_ip, server_port);

  std::thread tEvent(salt_api_events, server_ip, server_port);
  std::this_thread::sleep_for(std::chrono::seconds(5));
  std::thread tTimerOut(thread_check_timer_out);
  // std::thread tTestPing(run_test_cmd);

  run_pipeline(&run);

  // tTestPing.join();
  tTimerOut.join();

  std::this_thread::sleep_for(std::chrono::seconds(5));
  tEvent.join();

  jobmap_cleanup(&gjobmap);

  release_pipeline();

  threadpool_destroy(thpool, 0);
  return 0;
}
