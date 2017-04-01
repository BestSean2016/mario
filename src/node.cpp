#include <vector>
#include <iostream>
#include "state.hpp"
#include "graph.hpp"
#include "node.hpp"


namespace itat {

dfnode::dfnode(dfgraph* g) : g_(g) {
  sm_ = new dfnode_state_machine(g_, this);
}

dfnode::~dfnode() {
  if (sm_) delete sm_;
  if (plnode_) delete plnode_;
}

void dfnode::set_pipline_node(mr_pl_node *plnode) {
  assert(plnode != nullptr);
  plnode_ = plnode;
}



} // namespace dfgraph
