#ifndef EDGE_HPP
#define EDGE_HPP
#include "itat_global.h"
#include "itat.h"

#include "igraph/igraph.h"

namespace itat {
class ITAT_API iedge {
    iedge() {}
    iedge(igraph_t* g) : g_(g) {}
    virtual ~iedge() {}

private:
    igraph_t* g_ = nullptr;
};
} //namespace itat

#endif // EDGE_HPP
