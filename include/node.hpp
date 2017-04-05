#ifndef NODE_HPP
#define NODE_HPP

#include "itat_global.h"
#include "itat.h"

namespace itat {

class iGraph;
class dfNodeStateMachine;

typedef struct mr_pl_node {
  int64_t id = 0;
  std::string old_id;
  int64_t pl_id = 0;
  int64_t ref_id = 0;
  int ref_type = 0;
  std::string minion_id;
  int timeout = 60;
  std::string argv;
  std::string node_desc;
  time_t created_at = 0;
  int creator = 0;
  time_t updated_at = 0;
  int modifier = 0;
} mr_pl_node;

class iNode {
public:
  iNode() {}
  iNode(iGraph *g);
  virtual ~iNode();

  void set_pipline_node(mr_pl_node *plnode);

  dfNodeStateMachine *get_state_machine() { return sm_; }
  void gen_pl_node(int nodeid) {
    if (plnode_) delete plnode_;
    plnode_ = new mr_pl_node;
    plnode_ ->id = nodeid;
  }

private:
  iGraph *g_ = nullptr;
  dfNodeStateMachine *sm_ = nullptr;

  struct mr_pl_node *plnode_ = nullptr;
};

} // namespace itat
// extern int generate_dataflow_nodes(igraph_t* g);

#endif // NODE_HPP
