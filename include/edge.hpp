#ifndef EDGE_HPP
#define EDGE_HPP

#include "igraph/igraph.h"

namespace itat {
class iedge {
    iedge() {}
    iedge(igraph_t* g) : g_(g) {}
    virtual ~iedge() {}

private:
    igraph_t* g_ = nullptr;
};
} //namespace itat

#endif // EDGE_HPP
