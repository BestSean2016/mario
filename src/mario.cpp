#include "node.hpp"
#include "edge.hpp"
#include "pipeline.hpp"
#include "mario.hpp"
#include <iostream>
#include <thread>

namespace itat {
Mario::Mario(int64_t plid)
  : plid_(plid) {
  g_ = new Pipeline();
  //state_ = new dfGraphStateMachine(g_);
}

Mario::~Mario() {
  //delete state_;
  delete g_;
}


int Mario::simulator_pipeline(int node_num, int branch_num) {
  assert(g_ != nullptr);
  int ret = g_->diamod_simulator(node_num, branch_num);
  return ret;
}

int Mario::check() {
    g_->check();
    return 0;
}

int Mario::run(int start_id) {
    return 0;
}


int Mario::run_node(int node_id) {
    return 0;
}

int Mario::pause() {
    return 0;
}

int Mario::goon() {
    return 0;
}

int Mario::stop() {
    return 0;
}

int Mario::skip() {
    return 0;
}

int Mario::redo() {
    return 0;
}

} //namespace itat
