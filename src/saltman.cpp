#include "saltman.hpp"
#include "pipeline.hpp"
#include "node.hpp"
#include "mario_sql.h"

#define SALT_SERVER_IP "10.10.10.19"
#define SALT_SERVER_PORT 8000
#define SALT_USERNAME "salt-test"
#define SALT_PASSWORD "salt-test"



namespace itat {
extern bool g_run_check_timer_out;

bool vec_find(std::vector<int> &vec, int a) {
  for (auto &p : vec)
    if (p == a)
      return true;
  return false;
}

void vec_insert(std::vector<int> &vec, int a) {
  if (!vec_find(vec, a))
    vec.emplace_back(a);
}

void vec_erase(std::vector<int> &vec, int a) {
  for (auto it = vec.begin(); it != vec.end(); it++) {
    if (*it == a) {
      vec.erase(it);
      break;
    }
  }
}


saltman::saltman() {
  param_check_.set_param(SALT_SERVER_IP, SALT_SERVER_PORT, parse_token_fn,
                         &map_job_, nullptr);
  salt_api_login(&param_check_, SALT_USERNAME, SALT_PASSWORD);
}

saltman::~saltman() {
  for (auto &p : map_job_) {
    SALT_JOB *job = p.second;
    for (auto &q : job->minion_ret) {
      SALT_JOB_RET *jr = q.second;
      if (jr)
        delete jr;
    }
    job->minion_ret.clear();
    job->minions.clear();
    delete job;
  }
  map_job_.clear();
}

int saltman::start() {
  return 0;
}

void saltman::stop() {
#ifdef _DEBUG_
  cout << "Saltman Stoping .... \n";
#endif //_DEBUG

  if (param_event_.sockfd > 0) {
    close(param_event_.sockfd);
    thread_event_.join();
  }

  if (g_run_check_timer_out) {
    g_run_check_timer_out = false;
    thread_timeout_.join();
  }

#ifdef _DEBUG_
  cout << "Stoped\n";
#endif //_DEBUG
}

void saltman::dump_jobmap() {
  for (auto &p : map_job_) {
    SALT_JOB *job = p.second;
    assert(job != nullptr);
    std::cout << *job << std::endl;
  }
}

void saltman::check_time_out() {}

STATE_TYPE saltman::check_node(iNode *node) {
  assert(node != nullptr);
  MARIO_NODE *n = node->get_mario_node();
  assert(n != nullptr);
  STATE_TYPE state = ST_checked_ok;

  // 1. test_ping
  param_check_.rf = parse_salt_testping_ret;
  SALT_JOB_RET jobret;
  param_check_.param1 = &jobret;
  param_check_.param2 = (void *)(n->minion_id.c_str());
  int ret = salt_api_testping(&param_check_, n->minion_id.c_str());
  if (ret)
    state = ST_checked_herr;
  else {
    // 2. find the script
    auto script_path = split(n->command, ' ');
    if ((ret = salt_api_file_exists(&param_check_, n->minion_id.c_str(),
                                    script_path[0].c_str())))
      state = ST_checked_serr;
  }
  return state;
}

STATE_TYPE saltman::run_node(iNode *node) {
  assert(node != nullptr);
  STATE_TYPE state = ST_running;
  param_run_.param1 = &map_job_;
  param_run_.param2 = node;

  if (salt_api_async_cmd_runall(&param_run_,
                                node->get_mario_node()->minion_id.c_str(),
                                node->get_mario_node()->command.c_str()))
    state = ST_error;

  return state;
}

void saltman::init(Pipeline *pl) {
  pl_ = pl;
  g_ = pl->get_igraph();
  nodeset_ = pl->get_nodeset();


  map_job_.clear();
  param_event_.set_param(SALT_SERVER_IP, SALT_SERVER_PORT, parse_job, &map_job_,
                         nullptr);
  param_run_.set_param(SALT_SERVER_IP, SALT_SERVER_PORT,
                       parse_salt_myjob_jobmap, &map_job_, nullptr);

  int ret = 0;
  if (!ret) {
    g_run_check_timer_out = true;
    thread_timeout_ = std::thread(itat::thread_check_timer_out, &map_job_);
  }

  if (!ret) {
    thread_event_ = std::thread(itat::salt_api_events, &param_event_);
  }

  std::this_thread::sleep_for(std::chrono::seconds(5));
  // return ret;
}
}
