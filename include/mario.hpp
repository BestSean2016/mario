#ifndef MARIO_PLUMBER_H
#define MARIO_PLUMBER_H

#include "itat_global.h"
#include "itat.h"

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
  int simulator_pipeline(int node_num, int branch_num);

private:
  Pipeline *g_ = nullptr;  ///the graph that will be genereted
  int64_t plid_ = 0;    ///id of pipeline
  dfGraphStateMachine* state_ = nullptr;



private:

  /**
   * @brief register_pyfun register python callback for check/run pipeline
   * @return 0 for good
   */
  int register_pyfun();
  int finish_job();

};

} // namespace itat

#endif // MARIO_PLUMBER_H