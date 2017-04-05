#ifndef EDGE_HPP
#define EDGE_HPP
#include "itat_global.h"
#include "itat.h"

#include "igraph/igraph.h"

namespace itat {
class ITAT_API iEdge {
    iEdge() {}
    iEdge(igraph_t* g) : g_(g) {}
    virtual ~iEdge() {}

private:
    igraph_t* g_ = nullptr;
};
} //namespace itat

#endif // EDGE_HPP
