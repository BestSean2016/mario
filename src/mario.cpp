#include "mario.hpp"
#include "edge.hpp"
#include "node.hpp"
#include "pipeline.hpp"
#include "djangoapi.hpp"

namespace itat {

Mario::Mario(int plid) {
  g_ = new Pipeline(plid);
  // state_ = new dfGraphStateMachine(g_);
}

Mario::~Mario() { SafeDeletePtr(g_); }

void Mario::test_setup(int check, int run, int check_err_id, int run_err_id,
                       int timeout_id, int pause_id, int stop_id,
                       int confirm_id, int sleep_interval) {
  assert(g_ != nullptr);
  g_->test_setup((SIMULATE_RESULT_TYPE)check, (SIMULATE_RESULT_TYPE)run,
                 check_err_id, run_err_id, timeout_id, pause_id, stop_id,
                 confirm_id, sleep_interval);
}

int Mario::initial(int real_run, const char *py_message_path, int node_num,
                   int branch_num) {
  UNUSE(py_message_path);
  assert(g_ != nullptr);

  return g_->initial(real_run, node_num, branch_num);
}


void Mario::set_user(int userid) {
  assert(g_ != nullptr);
  g_->get_django()->set_user(userid);
}

int Mario::check() {
  assert(g_ != nullptr);
  return g_->check();
}

int Mario::run(int start_id, int pleid) {
  assert(g_ != nullptr);
  return g_->run(start_id, pleid);
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

int Mario::stop(int code, const char *why) {
  assert(g_ != nullptr);
  cout << "STOPSTOPSTOP\n";
  return g_->stop(code, why);
}

int Mario::confirm(int node_id) {
  assert(g_ != nullptr);
  return g_->user_confirm(node_id);
}

int Mario::get_plid() {
  assert(g_ != nullptr);
  return g_->get_plid();
}

int Mario::is_done() {
  assert(g_ != nullptr);
  return (g_->get_state() == ST_succeed || g_->get_state() == ST_checked_err ||
          g_->get_state() == ST_stoped);
}

} // namespace itat
