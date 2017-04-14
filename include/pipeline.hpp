#ifndef DATAFLOW_GRAPH_HPP
#define DATAFLOW_GRAPH_HPP

#include "itat.h"
#include "itat_global.h"
#include "state.hpp"
#include "threadpool.h"

#include <igraph/igraph.h>

// template<typename T>
// bool vec_find(std::vector<T>& vec, T& a) {
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

extern bool vec_find(std::vector<int>& vec, int a);
extern void vec_insert(std::vector<int>& vec, int a);
extern void vec_erase(std::vector<int>& vec, int a);

namespace itat {

// class Pipeline;
//
// class GraphState {
//   MARIO_STATE_TYPE state_ = ST_initial;
//   MARIO_STATE_TYPE chk_state_ = ST_initial;
//   iGraphStateMachine* gsm_ = nullptr;
//   iNodeStateMachine* nsm_ = nullptr;
// };

class Pipeline {
public:
  Pipeline(int64_t plid);
  ~Pipeline();

  /**
   * @brief diamod_simulator generate a graph
   * @param n number of nodes
   * @param b number of branches
   * @return 0 for good
   */
  int diamod_simulator(int node_num, int branch_num, SIMULATE_RESULT_TYPE type);
  iNode *get_node_by_id(int i) {
    return (i >= 0 && i < (int)node_.size()) ? node_[i] : nullptr;
  }

  /**
   * @brief gen_piple_graph generate the graph of piple from db
   * @return 0 for good
   */
  int gen_piple_graph();

  iNode *get_node_by_jid(std::string &jid);

  int init(bool real_run);
  int check();
  int run(int start_id);
  int run_node(int node_id);
  int pause();
  int go_on();
  int stop();
  int redo(int node_id);


  bool is_running() {return (tsim_.joinable() || tevent_.joinable()); }

private:
  //internal event action
  int on_run_error_(FUN_PARAM node);
  int on_run_timeout_(FUN_PARAM node);

  int on_run_ok_(FUN_PARAM node);
  int on_run_ok_event_(FUN_PARAM node);
  int on_run_after_ok_(FUN_PARAM node);

  int on_run_allok_(FUN_PARAM);
  int on_run_allok_event_(FUN_PARAM);
  int on_run_after_allok_(FUN_PARAM);

  int on_pause();
  int on_continue();
  int on_stop();
  int on_wait_for_user(int64_t node_id);
  int on_user_confirmed(int64_t node_id);
  int on_wait_ror_run();

public:
  //interface for test
  int test_run_error(int node_id, int code, std::string &stdout,
                     std::string &stderr);
  int test_run_ok(int node_id, int code, std::string &stdout,
                  std::string &stderr);
  int test_user_confirmed(int node_id);

  int test_1(FUN_PARAM);
  int test_2(FUN_PARAM);

  STATE_TYPE get_state() { return state_; }
  STATE_TYPE get_chk_state() {return chk_state_; }

  RUN_TYPE get_run_type() { return run_type_; }

  int64_t get_graph_id() { return plid_; }

private:
  STATE_TYPE state_ = ST_initial;
  STATE_TYPE chk_state_ = ST_initial;
  iGraphStateMachine* gsm_ = nullptr;
  iNodeStateMachine* nsm_ = nullptr;


  int64_t plid_ = 0;
  igraph_t ig_;
  std::vector<iNode *> node_;

  MapStr2Ptr<iNode> jid_2_node_;

  std::vector<int> prepare_to_run_;
  std::vector<int> running_set_;
  std::vector<int> run_set_;
  std::vector<int> done_set_;

  SIMULATE_RESULT_TYPE simret_type_ = SIMULATE_RESULT_TYPE_OK;

  threadpool_t *thpool_ = nullptr;
  RUN_TYPE run_type_ = RUN_TYPE_ASYNC;
  std::thread tsim_;
  std::thread tevent_;

private:
  int gen_diamod_graph_(int node_num, int branch_num);
  int gen_node_(SIMULATE_RESULT_TYPE type);
  int setup_state_machine_();

  /**
   * @brief load_pipe_line_from_db
   * @return 0 for good
   */
  int load_pipe_line_from_db_();
  int gen_piple_graph_();

  //Check Action
  int do_check_(FUN_PARAM);
  int do_checking_(FUN_PARAM);
  //the checking callback function for igraph's visitor
  static int do_node_check_cb_(const igraph_t *graph, igraph_integer_t vid,
                            igraph_integer_t, igraph_integer_t,
                            igraph_integer_t, igraph_integer_t, void *extra);

  //Run Action
  int do_run_(FUN_PARAM node_id);
  int do_running_(FUN_PARAM node_id);
  static void do_thread_run_node_(FUN_PARAM node_ptr);
  //the running callback function for igraph's visitor
  static int do_node_run_cb_(const igraph_t *graph, igraph_integer_t vid,
                             igraph_integer_t, igraph_integer_t,
                             igraph_integer_t, igraph_integer_t, void *extra);

  static int thread_simulator_(Pipeline* pl);

  void find_node_to_run_(int node_id, bool after);


  bool is_all_done_();
};

} // namespace itat

#endif // DATAFLOW_GRAPH_HPP
