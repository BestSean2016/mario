#ifndef EDGE_HPP
#define EDGE_HPP

#include "igraph/igraph.h"

namespace dfgraph {
class dfedge {
    dfedge() {}
    dfedge(igraph_t* g) : g_(g) {}
    virtual ~dfedge() {}

private:
    igraph_t* g_ = nullptr;
};
}

#endif // EDGE_HPP
