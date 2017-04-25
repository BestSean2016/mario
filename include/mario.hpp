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
  int initial(int real_run, const char *py_message_path);

  int check();
  int run(int start_id);
  int run_node(int node_id);
  int pause();
  int go_on();
  int stop();
  int confirm(int node_id);

  int test_int(int test) { return test; }

  int get_plid();

  //test, set up simulator
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
  void test_setup(int node_num = 20,
                  int branch_num = 2,
                  int check = 1,
                  int run = 1,
                  int check_err_id = -1,
                  int run_err_id = -1,
                  int timeout_id = -1,
                  int pause_id = -1,
                  int stop_id = -1,
                  int confirm_id = -1,
                  int sleep_interval  = 1000);

private:
  Pipeline *g_ = nullptr;  ///the graph that will be genereted
};

//
// struct mario_pickle : public boost::python::pickle_suite {
//     static boost::python::tuple getinitargs(Mario const& m);
//     static boost::python::tuple getstate(Mario const& m);
//     static void setstate(Mario& m, boost::python::tuple state);
// };
//
} // namespace itat

#endif // MARIO_PLUMBER_H
