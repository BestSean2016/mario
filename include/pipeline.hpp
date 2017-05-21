#ifndef DATAFLOW_GRAPH_HPP
#define DATAFLOW_GRAPH_HPP

#include "itat.h"
#include "itat_global.h"
#include "state.hpp"
#include "mario_sql.h"
#include "saltapi.hpp"

#include <igraph/igraph.h>
#include "saltman.hpp"

// template<typename T>
// int vec_find(std::vector<T>& vec, T& a) {
//     for(auto&p : vec)
//         if (p == a) return true;
//     return false;
// }
//
// template<typename T>
// void vec_insert(std::vector<T>& vec, T& a) {
//     if (!vec_find(vec, a))
//         vec.emplace_back(a);
// }
//
// template<typename T>
// void vec_erase(std::vector<T>& vec, T& a) {
//     for(auto it = vec.begin(); it != vec.end(); it++)
//         if (*it == a) vec.erase(it);
// }
//


namespace itat {


class Pipeline {
public:
  Pipeline(int plid);
  ~Pipeline();

public: // generic interface to manuplate the pipeline
  iNode *get_node_by_id(int i) {
    return (i >= 0 && i < (int)node_.size()) ? node_[i] : nullptr;
  }
  /**
   * @brief gen_piple_graph generate the graph of piple from db
   * @return 0 for good
   */
  int gen_piple_graph();
  iNode *get_node_by_jid(std::string &jid);
  int initial(int real_run, int node_num = 20, int branch_num = 2);
  int is_running() { return (tsim_.joinable()); }
  STATE_TYPE get_state() { return state_; }
  STATE_TYPE get_chk_state() { return chk_state_; }
  int get_plid() { return plid_; }
  int get_pl_exe_id() { return pleid_; }
  int get_amount_node() { return ig_.n; }
  igraph_t* get_igraph() { return &ig_; }
  saltman* get_saltman() { return saltman_; }
  NODESET* get_nodeset() { return &nodeset_; }

  // test, set up simulator
  /**
   * @brief test_setup setup the simulator
   * @param node_num the amount of node
   * @param branch_num the branch of echo node
   * @param check the result of check
   * @param run the result of run
   * @param check_err_id set the check error occured at id if the id is not -1
   * @param timeout_id set the run status to timeout if the is is not -1 at the
   * timeout_id
   * @param run_err_id set the run error occured at id if the id is not -1
   * @param pause_id set the pause occured at id if the id is not -1
   * @param stop_id set the stop occured at id if the id is not -1
   * @param confirm_id set the confirm occured at id if the id is not -1
   */
  void test_setup(SIMULATE_RESULT_TYPE check = SIMULATE_RESULT_TYPE_OK,
                  SIMULATE_RESULT_TYPE run = SIMULATE_RESULT_TYPE_OK,
                  int check_err_id = -1, int run_err_id = -1,
                  int timeout_id = -1, int pause_id = -1, int stop_id = -1,
                  int confirm_id = -1, int sleep_interval = 1000);

public: // the action from user
  int check();
  int run(int start_id, int pleid);
  int run_node(int node_id);
  int pause();
  int go_on();
  int stop();
  int user_confirm(int node_id);


  int on_run_error(FUN_PARAM node);
  int on_run_timeout(FUN_PARAM node);
  int on_run_ok(FUN_PARAM node);

private: // the  state machine front and back
  // Check Action
  int do_check_front_(FUN_PARAM);
  int do_check_back_(FUN_PARAM);
  int real_do_check__();
  // the checking callback function for igraph's visitor
  static int do_node_check_cb_(const igraph_t *graph, igraph_integer_t vid,
                               igraph_integer_t, igraph_integer_t,
                               igraph_integer_t, igraph_integer_t, void *extra);

  // Run Action
  int do_run_front_(FUN_PARAM node);
  int do_run_back_(FUN_PARAM node_id);
  void do_run_node_http_request_(iNode *node);
  static int thread_simulator_(Pipeline *pl);
  static int thread_simulator_ex_(Pipeline *pl);
  int thread_simulator_ex_ex_();

  // run one node
  int do_run_one_front_(FUN_PARAM node);
  int do_run_one_back_(FUN_PARAM node);
  // pause
  int do_pause_front_(FUN_PARAM);
  int do_pause_back_(FUN_PARAM);
  // go_on
  int do_go_on_front_(FUN_PARAM);
  int do_go_on_back_(FUN_PARAM);
  // stop
  int do_stop_front_(FUN_PARAM);
  int do_stop_back_(FUN_PARAM);
  // confirm
  int do_user_confirm_front_(FUN_PARAM node_id);
  int do_user_confirm_back_(FUN_PARAM node_id);

private:
  // internal event action
  // run ......................................
  int on_run_error_front_(FUN_PARAM node);
  int on_run_error_back_(FUN_PARAM node);

  int on_run_timeout_front_(FUN_PARAM node);
  int on_run_timeout_back_(FUN_PARAM node);

  int on_run_ok_front_(FUN_PARAM node);
  int on_run_ok_back_(FUN_PARAM node);

  int on_run_allok_(FUN_PARAM);
  int on_run_allok_front_(FUN_PARAM);
  int on_run_allok_back_(FUN_PARAM);

  // run one node ..............................
  int on_run_one_error_(FUN_PARAM node);
  int on_run_one_error_front_(FUN_PARAM node);
  int on_run_one_error_back_(FUN_PARAM node);

  int on_run_one_timeout_(FUN_PARAM node);
  int on_run_one_timeout_front_(FUN_PARAM node);
  int on_run_one_timeout_back_(FUN_PARAM node);

  int on_run_one_ok_(FUN_PARAM node);
  int on_run_one_ok_front_(FUN_PARAM node);
  int on_run_one_ok_back_(FUN_PARAM node);
  // run one node ..............................

  // paused ..............................
  int on_paused_(FUN_PARAM);
  int on_paused_front_(FUN_PARAM);
  int on_paused_back_(FUN_PARAM);

  // stoped ..............................
  int on_stoped_(FUN_PARAM);
  int on_stoped_front_(FUN_PARAM);
  int on_stoped_back_(FUN_PARAM);

  // confirm .............................
  int on_waitin_confirm_(FUN_PARAM node);
  int on_waitin_confirm_front_(FUN_PARAM node);
  int on_waitin_confirm_back_(FUN_PARAM node);

  static int run_run_run__(Pipeline* pl, int node_id);
  static int chk_chek_chk__(Pipeline* pl);

public:
  // interface for test
  int test_1(FUN_PARAM);
  int test_2(FUN_PARAM);

private:
  STATE_TYPE state_ = ST_initial;
  STATE_TYPE chk_state_ = ST_initial;
  iGraphStateMachine *gsm_ = nullptr;
  iNodeStateMachine *nsm_ = nullptr;

  int plid_ = 0;
  int pleid_ = 0;
  igraph_t ig_;
  std::vector<iNode *> node_;

  MapStr2Ptr<iNode> jid_2_node_;

  NODESET nodeset_;

  std::thread tsim_;
  std::thread tchk_;

  TEST_PARAM *test_param_ = nullptr;

  saltman* saltman_ = nullptr;
private:
  int gen_diamod_graph_(int node_num, int branch_num);
  int gen_node_(vector<MARIO_NODE> *nodes);
  int setup_state_machine_();
  int get_start_node_id();

  /**
   * @brief load_pipe_line_from_db
   * @return 0 for good
   */
  int load_pipe_line_from_db_(vector<MARIO_NODE> &pl_node,
                              vector<MARIO_EDGE> &pl_edge);

  void find_node_to_run_(int ig_node_id);

  bool is_all_done_();

  /**
   * @brief diamod_simulator generate a graph
   * @param n number of nodes
   * @param b number of branches
   * @return 0 for good
   */
  int diamod_simulator_(int node_num, int branch_num);

  int simulator_check_(Pipeline* pl);
};

} // namespace itat

#endif // DATAFLOW_GRAPH_HPP
