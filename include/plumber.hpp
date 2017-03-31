#ifndef PLUMBER_H
#define PLUMBER_H

namespace itat {

class graph;
class node;
class state;

class plumber {
public:
    plumber() {}
    plumber(graph* g) : g_(g) {}
    ~plumber() {}


private:
    graph* g_ = nullptr;
};


} //namespace itat

#endif // PLUMBER_H
