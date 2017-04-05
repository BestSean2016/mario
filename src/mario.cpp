#include "state.hpp"
#include "node.hpp"
#include "edge.hpp"
#include "graph.hpp"
#include "mario.hpp"
#include <iostream>
#include <thread>

namespace itat {
  Mario::Mario(int64_t plid)
    : plid_(plid) {
    g_ = new iGraph();
    state_ = new dfGraphStateMachine(g_);
  }

  Mario::~Mario() {
    delete state_;
    delete g_;
  }


  int Mario::simulator(int node_num, int branch_num) {
    assert(g_ != nullptr);
    int ret = g_->diamod_simulator(node_num, branch_num);
    return ret;
  }
} //namespace itat
