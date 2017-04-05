#include <vector>
#include <iostream>
#include "state.hpp"
#include "graph.hpp"
#include "node.hpp"


namespace itat {

iNode::iNode(iGraph* g) : g_(g) {
  sm_ = new dfNodeStateMachine(g_, this);
}

iNode::~iNode() {
  if (sm_) delete sm_;
  if (plnode_) delete plnode_;
}

void iNode::set_pipline_node(mr_pl_node *plnode) {
  assert(plnode != nullptr);
  plnode_ = plnode;
}



} // namespace dfgraph
