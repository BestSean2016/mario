#ifndef STATE_HPP
#define STATE_HPP
#include "itat.h"
#include "itat_global.h"

#define ERROR_WRONG_STATE_TO_ACTION -10000
#define ERROR_DONT_HAVE_FRONT_ACTION -10001
#define ERROR_INVAILD_NODE_ID -10002
#define ERROR_CANNOT_CREATE_THREAD_POOL -10003

#define NO_NODE -1

namespace itat {

static const char *mario_state_name[] = {
    "initial          ", "checking         ", "checked_err      ",
    "checked_ok       ", "running          ", "error            ",
    "timeout          ", "successed        ", "waiting_for_input",
    "stoped           ", "paused           ", "waiting_for_run  ",
    "running_one      ", "run_one_ok       ", "run_one_err      ",
};

typedef enum STATE_TYPE {
  ST_unknow = -1,
  ST_initial,
  ST_checking,
  ST_checked_err,
  ST_checked_serr,
  ST_checked_herr,
  ST_checked_ok,
  ST_running,
  ST_error,
  ST_timeout,
  ST_succeed,
  ST_waiting_for_input,
  ST_stoped,
  ST_stoping,
  ST_paused,
  ST_pausing,
  ST_waiting_for_run,
  ST_running_one,
  ST_run_one_ok,
  ST_run_one_err,
} STATE_TYPE;

class Pipeline;
class iNode;

typedef void *FUN_PARAM;

template <typename Action, typename Object> class STATE_TRANS {
public:
  STATE_TYPE source;
  Action front_action;
  STATE_TYPE target;
  Action back_action;

  int do_front_action(Object *o, FUN_PARAM p) {
    if (this->front_action) {
      int (Object::*front_action)(FUN_PARAM p) = this->front_action;
      return (o->*front_action)(p);
    }

    // whould never happend
    return ERROR_DONT_HAVE_FRONT_ACTION;
  }

  int do_back_action(Object *o, FUN_PARAM p) {
    if (this->back_action) {
      int (Object::*back_action)(FUN_PARAM p) = this->back_action;
      return (o->*back_action)(p);
    }
    return 0;
  }

  STATE_TRANS(STATE_TYPE s, Action f, STATE_TYPE t, Action b)
      : source(s), front_action(f), target(t), back_action(b) {}
};

template <typename Action, typename Object> class STATE_TRANS_SOURCE {
public:
  STATE_TYPE source;
  Action front_action;

  STATE_TRANS_SOURCE(STATE_TYPE s, Action f) : source(s), front_action(f) {}

  bool operator<(const STATE_TRANS_SOURCE<Action, Object> &sts) const {
    if (source < sts.source)
      return true;
    else if (source == sts.source)
      return ((uint64_t)((void *)front_action) < (uint64_t)((void *)sts.front_action));
    else
      return false;
  }

  bool operator ==(const STATE_TRANS_SOURCE<Action, Object> &sts) const {
      return ((source == sts.source)
        && (front_action == sts.front_action));
  }
};

template <typename Action, typename Object> class SateMachine {
public:
  SateMachine() {}
  virtual ~SateMachine() {}

  void add_state_trans(STATE_TYPE s, Action f, STATE_TYPE t, Action b) {
    STATE_TRANS<Action, Object> st(s, f, t, b);
    STATE_TRANS_SOURCE<Action, Object> sts(s, f);
    mapStateMachine.insert(std::make_pair(sts, st));
  }

  bool find(STATE_TYPE s, Action f) {
      STATE_TRANS_SOURCE<Action, Object> sts(s, f);
      auto iter = mapStateMachine.find(sts);
      return (iter != mapStateMachine.end());
  }

  int do_trans(STATE_TYPE s, Action f, Object *o, FUN_PARAM p) {
    STATE_TRANS_SOURCE<Action, Object> sts(s, f);
    auto iter = mapStateMachine.find(sts);
    if (iter == mapStateMachine.end())
      return ERROR_WRONG_STATE_TO_ACTION;
    STATE_TRANS<Action, Object>& st = iter->second;
    int ret = st.do_front_action(o, p);
    if (ret) return ret;
    return st.do_back_action(o, p);
  }

private:
  std::map<STATE_TRANS_SOURCE<Action, Object>, STATE_TRANS<Action, Object>>
      mapStateMachine;
};

class Pipeline;
class iNode;

typedef int (Pipeline::*graph_action)(FUN_PARAM);
typedef int (iNode::*node_action)(FUN_PARAM);

typedef STATE_TRANS<graph_action, Pipeline> iGraphStateTrans;
typedef STATE_TRANS_SOURCE<graph_action, Pipeline> iGraphTransSource;
typedef SateMachine<graph_action, Pipeline> iGraphStateMachine;

typedef STATE_TRANS<node_action, iNode> iNodeStateTrans;
typedef STATE_TRANS_SOURCE<node_action, iNode> iNodeTransSource;
typedef SateMachine<node_action, iNode> iNodeStateMachine;


typedef enum SIMULATE_RESULT_TYPE {
    SIMULATE_RESULT_TYPE_UNKNOW,
    SIMULATE_RESULT_TYPE_OK,
    SIMULATE_RESULT_TYPE_ERR,
    SIMULATE_RESULT_TYPE_RONDOM,
} SIMULATE_RESULT_TYPE;



typedef struct TEST_PARAM {
    int node_num = 20;
    int branch_num = 2;
    SIMULATE_RESULT_TYPE check_type = SIMULATE_RESULT_TYPE_OK;
    SIMULATE_RESULT_TYPE run_type = SIMULATE_RESULT_TYPE_OK;
    int check_err_id = -1;
    int run_err_id = -1;
    int timeout_id = -1;
    int pause_id = -1;
    int stop_id = -1;
    int confirm_id = -1;
    int sleep_interval = 1000;  //in macro-second
} TEST_PARAM;


typedef enum RUN_TYPE {
  RUN_TYPE_UNKNOW,
  RUN_TYPE_ASYNC,
  RUN_TYPE_SYNC,
} RUN_TYPE;

} // namespace itat

#endif // STATE_HPP
