#ifndef MARIO_PLUMBER_H
#define MARIO_PLUMBER_H

#include "itat_global.h"
#include "itat.h"
#include "state.hpp"
#include <boost/python.hpp>
#include "pipeline.h"

namespace itat {

class Mario {
public:
  Mario() {}
  Mario(int plid);
  ~Mario();

  //
  // Interface to Bill, Python
  //
  int initial(int real_run, const char *py_message_path, int node_num,
              int branch_num);

  int check();
  int run(int start_id);
  int run_node(int node_id);
  int pause();
  int go_on();
  int stop();
  int confirm(int node_id);

  int test_int(int test) { return test; }

  int get_plid();

  // test, set up simulator
  /**
   * @brief test_setup setup the simulator
   * @param node_num the amount of node
   * @param branch_num the branch of echo node
   * @param check the result of check
   * @param run the result of run
   * @param check_err_id set the check error occured at id if the id is not -1
   * @param run_err_id set the run error occured at id if the id is not -1
   * @param pause_id set the pause occured at id if the id is not -1
   * @param stop_id set the stop occured at id if the id is not -1
   * @param confirm_id set the confirm occured at id if the id is not -1
   */
  void test_setup(int check = 1, int run = 1, int check_err_id = -1,
                  int run_err_id = -1, int timeout_id = -1, int pause_id = -1,
                  int stop_id = -1, int confirm_id = -1,
                  int sleep_interval = 1000);

private:
  Pipeline *g_ = nullptr; /// the graph that will be genereted
};


} // namespace itat

extern int64_t new_mario(int plid);
extern int get_back_mario(int64_t id);
extern void kill_mario(int64_t id);
extern int initial(int64_t id, int real_run, const char *bill_message,
                   int node_num, int branch_num);
extern int run_mario(int64_t id, int start_id);
extern int stop_mario(int64_t id);
extern int test_setup(int64_t id, int check, int run, int check_err_id,
                      int run_err_id, int timeout_id, int pause_id, int stop_id,
                      int confirm_id, int sleep_interval);
#endif // MARIO_PLUMBER_H
