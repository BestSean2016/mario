#ifndef DATAFLOW_GRAPH_HPP
#define DATAFLOW_GRAPH_HPP

#include "itat.h"
#include "itat_global.h"
#include "state.hpp"

#include <igraph/igraph.h>

namespace itat {

// class Pipeline;
//
// class GraphState {
// MARIO_STATE_TYPE state_ = ST_initial;
// MARIO_STATE_TYPE chk_state_ = ST_initial;
// iGraphStateMachine* gsm_ = nullptr;
// iNodeStateMachine* nsm_ = nullptr;
// };

class Pipeline {
public:
  Pipeline();
  ~Pipeline();

  /**
   * @brief diamod_simulator generate a graph
   * @param n number of nodes
   * @param b number of branches
   * @return 0 for good
   */
  int diamod_simulator(int node_num, int branch_num);
  iNode *get_node_by_id(int i) {
    return (i >= 0 && i < (int)node_.size()) ? node_[i] : nullptr;
  }

  /**
   * @brief gen_piple_graph generate the graph of piple from db
   * @return 0 for good
   */
  int gen_piple_graph();

  iNode *get_node_by_jid(std::string &jid);

  int init();
  int check();
  int run(int start_id);
  int run_node(int node_id);
  int pause();
  int goon();
  int stop();
  int skip();
  int redo();

  int on_check_start();
  int on_check_error(int64_t node_id, bool hosterr, std::string &errmsg);
  int on_check_ok(int64_t node_id, bool hosterr, std::string &errmsg);
  int on_check_allok();

  int on_run_start();
  int on_run_error(int64_t node_id, int code, std::string &stdout,
                   std::string &stderr);
  int on_run_ok(int64_t node_id, int code, std::string &stdout,
                std::string &stderr);
  int on_run_allok();

  int on_pause();
  int on_continue();
  int on_stop();
  int on_wait_for_user(int64_t node_id);
  int on_user_confirmed(int64_t node_id);
  int on_wait_ror_run();

  int test_check_error(int node_id, bool hosterr, std::string &errmsg);
  int test_check_ok(int node_id, bool hosterr, std::string &errmsg);
  int test_run_error(int node_id, int code, std::string &stdout,
                     std::string &stderr);
  int test_run_ok(int node_id, int code, std::string &stdout,
                  std::string &stderr);
  int test_user_confirmed(int node_id);

  int test_1(FUN_PARAM);
  int test_2(FUN_PARAM);

  MARIO_STATE_TYPE get_state() { return state_; }
  MARIO_STATE_TYPE state_ = ST_initial;
  MARIO_STATE_TYPE chk_state_ = ST_initial;
  iGraphStateMachine* gsm_ = nullptr;
  iNodeStateMachine* nsm_ = nullptr;

  int64_t get_graph_id() { return graph_id_; }

private:
  int64_t graph_id_ = 0;
  igraph_t ig_;
  std::vector<iNode *> node_;

  MapStr2Ptr<iNode> jid_2_node_;

  std::set<int> running_set_;
  std::set<int> done_st_;

private:
  int gen_diamod_graph_(int node_num, int branch_num);
  int gen_node_();
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
  static int do_node_check_(const igraph_t *graph, igraph_integer_t vid,
                            igraph_integer_t, igraph_integer_t,
                            igraph_integer_t, igraph_integer_t, void *extra);

  //Run Action
  int do_run_(FUN_PARAM node_id);
  int do_running_(FUN_PARAM node_id);
  //the running callback function for igraph's visitor
  static int do_node_run_(const igraph_t *graph, igraph_integer_t vid,
                          igraph_integer_t, igraph_integer_t,
                          igraph_integer_t, igraph_integer_t, void *extra);
};

} // namespace itat

#endif // DATAFLOW_GRAPH_HPP
