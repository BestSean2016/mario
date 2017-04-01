#ifndef DATAFLOW_GRAPH_HPP
#define DATAFLOW_GRAPH_HPP


#include <igraph/igraph.h>
#include <vector>

namespace itat {

class dfnode;

class dfgraph {
public:
  dfgraph();
  ~dfgraph();

  /**
   * @brief diamod_simulator generate a graph
   * @param n number of nodes
   * @param b number of branches
   * @return 0 for good
   */
  int diamod_simulator(int node_num, int branch_num);
  dfnode* get_node(int i) {return (i >= 0 && i < (int)node_.size()) ? node_[i] : nullptr;}

  /**
   * @brief gen_piple_graph generate the graph of piple from db
   * @return 0 for good
   */
  int gen_piple_graph();

private:
  igraph_t ig_;
  std::vector<dfnode*> node_;

private:
  int gen_diamod_graph_(int node_num, int branch_num);
  int gen_node_(bool mock = false);
  /**
   * @brief load_pipe_line_from_db
   * @return 0 for good
   */
  int load_pipe_line_from_db_();
  int gen_piple_graph_();

};

} //namespace itat

#endif // DATAFLOW_GRAPH_HPP
