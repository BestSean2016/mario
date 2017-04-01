#include "state.hpp"
#include "node.hpp"
#include "edge.hpp"
#include "graph.hpp"
#include "plumber.hpp"
#include <iostream>
#include <thread>

namespace itat {
  mario::mario(int64_t plid)
    : plid_(plid) {
    g_ = new dfgraph();
    state_ = new dfgraph_state_machine(g_);
  }

  mario::~mario() {
    delete state_;
    delete g_;
  }


  int mario::simulator(int node_num, int branch_num) {
    assert(g_ != nullptr);
    int ret = g_->diamod_simulator(node_num, branch_num);
    return ret;
  }
} //namespace itat
