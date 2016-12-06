#include "mario_data.h"
#include "salt_api.h"
#include <iostream>
#include "http_client.h"
#include <thread>
#include <signal.h>

using namespace std;

int run = 1;
int * event_socket = 0;
std::thread* tEvent = 0;
char server_ip[16] = {0};
int server_port = 0;
struct DataSet<MR_REAL_NODE> g_nodes;

void got_signal(int sig) {
  printf("got signal %d\n", sig);
  run = 0;
}

static uint64_t pid = 10000;
void run_test_cmd() {
  //while(run)
  {
    std::this_thread::sleep_for(std::chrono::seconds(20));
    std::thread tTestRunCmd(salt_api_test_cmdrun, "10.10.10.19", 8000, pid++, -1);
    std::thread tTestPing(salt_api_testping, "10.10.10.19", 8000, pid++, -1);


    tTestRunCmd.join();
    tTestPing.join();
    if (!(pid % 101)) //renew token
      salt_api_login("10.10.10.19", 8000);
    std::this_thread::sleep_for(std::chrono::seconds(20));
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

  std::thread tTimerOut(thread_check_timer_out);

  tEvent = new std::thread(salt_api_events, "10.10.10.19", 8000);

  run_test_cmd();
  run = 0;

  tEvent->detach();
  delete tEvent;

  tTimerOut.join();

  jobmap_cleanup(&gjobmap);

  return 0;
}

int node_job_finished(SALT_JOB* job, std::vector<int>* vec)
{
  (void)job;
  (void)vec;
  return 0;
}

int run_something(std::vector<int>* vec) {
  (void)vec;
  return 0;
}
