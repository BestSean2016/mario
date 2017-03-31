#ifndef NODE_HPP
#define NODE_HPP



namespace itat {

class dfgraph;
class dfnode_state_machine;

class dfnode {
public:
  dfnode() {}
  dfnode(dfgraph* g);
  virtual ~dfnode();

  dfnode_state_machine* get_state_machine() {return sm_;}
private:
  dfgraph* g_ = nullptr;
  dfnode_state_machine* sm_ = nullptr;
};

} //namespace itat
//extern int generate_dataflow_nodes(igraph_t* g);

#endif // NODE_HPP
