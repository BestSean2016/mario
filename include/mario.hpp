#ifndef MARIO_PLUMBER_H
#define MARIO_PLUMBER_H

#include "itat_global.h"
#include "itat.h"
#include "state.hpp"

namespace itat {

class Pipeline;
class iNode;
class dfGraphStateMachine;

class Mario {
public:
  Mario() {}
  Mario(int64_t plid);
  ~Mario();


  //
  // Interface to Bill, Python
  //
  int init(bool real_run);

  int check();
  int run(int start_id);
  int run_node(int node_id);
  int pause();
  int go_on();
  int stop();
  int redo(int64_t node_id);

  /**
   * @brief simulator generate a diamod graph with n nodes and b braches
   * @param node_num the number of node
   * @param branch_num the number of branch
   * @return 0 for good
   */
  int simulator_pipeline(int node_num, int branch_num, SIMULATE_RESULT_TYPE type);

private:
  Pipeline *g_ = nullptr;  ///the graph that will be genereted
};

} // namespace itat

#endif // MARIO_PLUMBER_H
