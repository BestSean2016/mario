#ifndef NODE_HPP
#define NODE_HPP

#include <vector>
#include <iostream>

#include <igraph/igraph.h>

namespace itat {

namespace dfgraph {

class dfnode {
public:
  dfnode() {}
  dfnode(igraph_t* g) : g_(g) {}
  virtual ~dfnode() {}


private:
  igraph_t* g_ = nullptr;
};

} //namespace dfgraph
} //namespace itat
extern int generate_dataflow_nodes(igraph_t* g);

#endif // NODE_HPP
