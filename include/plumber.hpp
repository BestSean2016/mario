#ifndef PLUMBER_H
#define PLUMBER_H

#include <cstdlib>

namespace itat {

class igraph;
class node;
class dfgraph_state_machine;

class mario {
public:
  mario() {}
  mario(int64_t plid);
  ~mario();


  //
  // Interface to Python Bill
  //
  int check();
  int run(int start_id);
  int run_node(int node_id);
  int pause();
  int goon();
  int stop();
  int skip();
  int redo();

  /**
   * @brief simulator generate a diamod graph with n nodes and b braches
   * @param node_num the number of node
   * @param branch_num the number of branch
   * @return 0 for good
   */
  int simulator(int node_num, int branch_num);

private:
  igraph *g_ = nullptr;  ///the graph that will be genereted
  int64_t plid_ = 0;    ///id of pipeline
  dfgraph_state_machine* state_ = nullptr;



private:

  /**
   * @brief register_pyfun register python callback for check/run pipeline
   * @return 0 for good
   */
  int register_pyfun();
  int finish_job();

};

} // namespace itat

#endif // PLUMBER_H
