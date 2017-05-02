#ifndef NODE_HPP
#define NODE_HPP

#include "itat.h"
#include "itat_global.h"
#include "state.hpp"
#include "mario_sql.h"

namespace itat {

class Pipeline;
// class dfNodeStateMachine;

typedef enum NODE_TYPE {
  NODE_TYPE_UNKNOW = -1,
  NODE_TYPE_SCRIPT,
  NODE_TYPE_PIPELIEN,
  NODE_TYPE_START,
  NODE_TYPE_END,
  NODE_TYPE_LOGIC,
  NODE_TYPE_CONFIRM,
  NODE_TYPE_INPUT,
  NODE_TYPE_POWEROFF,
} NODE_TYPE;

class iNode {
public:
  iNode() {srand(time(0));}
  iNode(Pipeline *g);
  virtual ~iNode();

  void set_node_stype(NODE_TYPE type) { type_ = type; }
  // void set_pipline_node(mr_pl_node *plnode);
  // void set_simulate(SIMULATE_RESULT_TYPE type) { simret_type_ = type; }

  // dfNodeStateMachine *get_state_machine() { return sm_; }
  void gen_pl_node(int nodeid, MR_BILL_PIPELINE_NODE* node = nullptr) {
    if (plnode_)
      delete plnode_;
    if (node)
        plnode_ = MR_BILL_PIPELINE_NODE::clone(node);
    else {
        plnode_ = new MR_BILL_PIPELINE_NODE;
        plnode_->id = nodeid;
    }
  }
  STATE_TYPE get_state() { return state_; }
  // STATE_TYPE get_chk_state() { return chk_state_; }

  Pipeline* get_pipeline() { return g_; }
  int get_id() {return (int)id_; }
  RUN_TYPE get_run_type() { return run_type_; }

  void set_test_param(TEST_PARAM* param) { test_param_ = param; }

public:
  //user's action
  int init(int64_t i, MR_BILL_PIPELINE_NODE *node, iGraphStateMachine *gsm, iNodeStateMachine *nsm);
  int check();
  int run();
  int user_confirm();

private:
  //state machine's action from user
  int do_check_front_(FUN_PARAM);
  int do_check_back_(FUN_PARAM);
  int do_run_front_(FUN_PARAM);
  int do_run_back_(FUN_PARAM);
  int do_user_confirm_front_(FUN_PARAM confirmd);
  int do_user_confirm_back_(FUN_PARAM confirmd);

private:
  //internal action spipeline
  int on_run_error_(FUN_PARAM);
  int on_run_error_front_(FUN_PARAM);
  int on_run_error_back_(FUN_PARAM);
  int on_run_timeout_(FUN_PARAM);
  int on_run_timeout_front_(FUN_PARAM);
  int on_run_timeout_back_(FUN_PARAM);
  int on_run_ok_(FUN_PARAM);
  int on_run_ok_front_(FUN_PARAM);
  int on_run_ok_back_(FUN_PARAM);


private:
  NODE_TYPE type_ = NODE_TYPE_SCRIPT;
  STATE_TYPE state_ = ST_initial;
  //STATE_TYPE chk_state_ = ST_initial;
  int64_t id_ = -1;
  Pipeline *g_ = nullptr;

  iGraphStateMachine *gsm_ = nullptr;
  iNodeStateMachine *nsm_ = nullptr;

  struct MR_BILL_PIPELINE_NODE *plnode_ = nullptr;

  TEST_PARAM * test_param_ = nullptr;
  RUN_TYPE run_type_ = RUN_TYPE_ASYNC ;

private:
  void setup_state_machine_();
  void simu_check__();
  void simu_run__();

};

} // namespace itat
// extern int generate_dataflow_nodes(igraph_t* g);

#endif // NODE_HPP
