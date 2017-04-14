#include "mario.hpp"
#include "edge.hpp"
#include "node.hpp"
#include "pipeline.hpp"

namespace itat {
Mario::Mario(int64_t plid) {
  g_ = new Pipeline(plid);
  // state_ = new dfGraphStateMachine(g_);
}

Mario::~Mario() { SafeDeletePtr(g_); }

int Mario::simulator_pipeline(int node_num, int branch_num, SIMULATE_RESULT_TYPE type) {
  assert(g_ != nullptr);
  int ret = g_->diamod_simulator(node_num, branch_num, type);
  return ret;
}

int Mario::init(bool real_run) {
  assert(g_ != nullptr);
  return g_->init(real_run);
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

int Mario::redo(int64_t node_id) {
  assert(g_ != nullptr);
  return g_->redo(node_id);
}

} // namespace itat
