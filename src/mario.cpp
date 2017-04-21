#include "mario.hpp"
#include "edge.hpp"
#include "node.hpp"
#include "pipeline.hpp"
#include "djangoapi.hpp"


namespace itat {
extern DjangoAPI dj_;

Mario::Mario(int plid) {
  g_ = new Pipeline(plid);
  // state_ = new dfGraphStateMachine(g_);
}

Mario::~Mario() {
    SafeDeletePtr(g_);
}

void Mario::test_setup(int node_num,
                      int branch_num,
                      int check,
                      int run,
                      int check_err_id,
                      int run_err_id,
                      int timeout_id,
                      int pause_id,
                      int stop_id ,
                      int confirm_id,
                      int sleep_interval) {
  assert(g_ != nullptr);
  g_->test_setup(node_num,
                 branch_num,
                 (SIMULATE_RESULT_TYPE)check,
                 (SIMULATE_RESULT_TYPE)run,
                 check_err_id,
                 run_err_id,
                 timeout_id,
                 pause_id,
                 stop_id ,
                 confirm_id,
                 sleep_interval);
}

int Mario::initial(int real_run, const char* py_message_path) {
  assert(g_ != nullptr);
  dj_.init(py_message_path);
  return g_->initial(real_run);
}

int Mario::check() {
  assert(g_ != nullptr);
  return g_->check();
}

int Mario::run(int start_id) {
  assert(g_ != nullptr);
  return g_->run(start_id);
}

int Mario::run_node(int node_id) {
  assert(g_ != nullptr);
  return g_->run_node(node_id);
}

int Mario::pause() {
  assert(g_ != nullptr);
  return g_->pause();
}

int Mario::go_on() {
  assert(g_ != nullptr);
  return g_->go_on();
}

int Mario::stop() {
  assert(g_ != nullptr);
  return g_->stop();
}

int Mario::confirm(int node_id) {
  assert(g_ != nullptr);
  return g_->user_confirm(node_id);
}


} // namespace itat
