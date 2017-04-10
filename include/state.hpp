#ifndef STATE_HPP
#define STATE_HPP
#include "itat.h"
#include "itat_global.h"

#define ERROR_WRONG_STATE_TO_ACTION -10000
#define ERROR_DONT_HAVE_FRONT_ACTION -10001
#define NO_NODE -1

namespace itat {

static const char *mario_state_name[] = {
    "initial          ", "checking         ", "checked_err      ",
    "checked_ok       ", "waiting_for_run  ", "running          ",
    "error            ", "timeout          ", "successed        ",
    "waiting_for_input", "stoped           ", "paused           ",
    "running_one      ", "run_one_ok       ", "run_one_err      ",
    "not_doing        "
};

typedef enum MARIO_STATE_TYPE {
  ST_unknow = -1,
  ST_initial,
  ST_checking,
  ST_checked_err,
  ST_checked_ok,
  ST_waiting_for_run,
  ST_running,
  ST_error,
  ST_timeout,
  ST_successed,
  ST_waiting_for_input,
  ST_stoped,
  ST_paused,
  ST_running_one,
  ST_run_one_ok,
  ST_run_one_err,
  ST_not_doing, //NOT (ST_checking or ST_running or ST_running_one)
} MARIO_STATE_TYPE;

class Pipeline;
class iNode;

typedef void *FUN_PARAM;

template <typename Action, typename Object> class STATE_TRANS {
public:
  MARIO_STATE_TYPE source;
  Action front_action;
  MARIO_STATE_TYPE target;
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
  }

  STATE_TRANS(MARIO_STATE_TYPE s, Action f, MARIO_STATE_TYPE t, Action b)
      : source(s), front_action(f), target(t), back_action(b) {}
};

template <typename Action, typename Object> class STATE_TRANS_SOURCE {
public:
  MARIO_STATE_TYPE source;
  Action front_action;

  STATE_TRANS_SOURCE(MARIO_STATE_TYPE s, Action f)
      : source(s), front_action(f) {}

  bool operator<(const STATE_TRANS_SOURCE<Action, Object> &sts) const {
    if (source < sts.source)
      return true;
    if ((uint64_t)((void *)front_action) < (uint64_t)((void *)sts.front_action))
      return true;
    else
      return false;
  }
};

template <typename Action, typename Object> class SateMachine {
public:
  SateMachine() {}
  virtual ~SateMachine() {}

  void add_state_trans(MARIO_STATE_TYPE s, Action f, MARIO_STATE_TYPE t,
                       Action b) {
    STATE_TRANS<Action, Object> st(s, f, t, b);
    STATE_TRANS_SOURCE<Action, Object> sts(s, f);
    mapStateMachine.insert(std::make_pair(sts, st));
  }

  int do_trans(MARIO_STATE_TYPE s, Action f, Object *o, FUN_PARAM p) {
    STATE_TRANS_SOURCE<Action, Object> sts(s, f);
    auto iter = mapStateMachine.find(sts);
    if (iter == mapStateMachine.end())
      return ERROR_WRONG_STATE_TO_ACTION;
    STATE_TRANS<Action, Object> st = iter->second;
    st.do_front_action(o, p);
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

} // namespace itat

#endif // STATE_HPP
