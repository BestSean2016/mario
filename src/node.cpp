#include "node.hpp"
#include "djangoapi.hpp"
#include "pipeline.hpp"
#include "saltapi.hpp"
#include "saltman.hpp"

#define SERVER_IP "10.10.10.19"
#define SERVER_PORT 8000

namespace itat {

iNode::iNode(Pipeline *g) : g_(g) {
  // sm_ = new dfNodeStateMachine(g_, this);
}

iNode::~iNode() {
  // if (sm_) delete sm_;
  if (plnode_)
    delete plnode_;
}

int iNode::check() {
  assert(g_ != nullptr && gsm_ != nullptr && nsm_ != nullptr &&
         plnode_ != nullptr);

  return nsm_->do_trans(state_, &iNode::do_check_front_, this, nullptr);
}

int iNode::run() {
  assert(g_ != nullptr && gsm_ != nullptr && nsm_ != nullptr &&
         plnode_ != nullptr);
  return nsm_->do_trans(state_, &iNode::do_run_front_, this, nullptr);
}

int iNode::user_confirm() {
  assert(g_ != nullptr && gsm_ != nullptr && nsm_ != nullptr &&
         plnode_ != nullptr);
  return nsm_->do_trans(state_, &iNode::do_user_confirm_front_, this, nullptr);
}

//
// .........................................................................
//
void iNode::setup_state_machine_() {
  // check
  nsm_->add_state_trans(ST_initial, &iNode::do_check_front_, ST_checking,
                        &iNode::do_check_back_);
  nsm_->add_state_trans(ST_checked_serr, &iNode::do_check_front_, ST_checking,
                        &iNode::do_check_back_);
  nsm_->add_state_trans(ST_checked_herr, &iNode::do_check_front_, ST_checking,
                        &iNode::do_check_back_);
  nsm_->add_state_trans(ST_checked_ok, &iNode::do_check_front_, ST_checking,
                        &iNode::do_check_back_);
  nsm_->add_state_trans(ST_error, &iNode::do_check_front_, ST_checking,
                        &iNode::do_check_back_);
  nsm_->add_state_trans(ST_succeed, &iNode::do_check_front_, ST_checking,
                        &iNode::do_check_back_);
  nsm_->add_state_trans(ST_timeout, &iNode::do_check_front_, ST_checking,
                        &iNode::do_check_back_);

  // run
  // nsm_->add_state_trans(ST_initial, &iNode::do_run_front_, ST_checking,
  //                       &iNode::do_run_back_);
  nsm_->add_state_trans(ST_checked_ok, &iNode::do_run_front_, ST_running,
                        &iNode::do_run_back_);

  // user_confirm
  nsm_->add_state_trans(ST_waiting_for_confirm, &iNode::do_user_confirm_front_,
                        ST_running, &iNode::do_user_confirm_back_);

  //..................... internal trans ........................
  //////////////////////////////////////////////////////////////////

  nsm_->add_state_trans(ST_running, &iNode::on_run_error_front_, ST_error,
                        &iNode::on_run_error_back_);
  nsm_->add_state_trans(ST_running, &iNode::on_run_ok_front_, ST_error,
                        &iNode::on_run_ok_back_);
  nsm_->add_state_trans(ST_running, &iNode::on_run_timeout_front_, ST_error,
                        &iNode::on_run_error_back_);
}

int iNode::do_check_front_(FUN_PARAM) {
  assert(g_ != nullptr);
  state_ = ST_checking;
  dj_.send_graph_status(g_->get_pl_exe_id(), g_->get_plid(), id_, state_,
                        state_);
  if (test_param_)
    std::this_thread::sleep_for(
        std::chrono::milliseconds(test_param_->sleep_interval));
  return 0;
}

void iNode::simu_check__() {
  switch (test_param_->check_type) {
  case SIMULATE_RESULT_TYPE_OK:
    if (test_param_->check_err_id == id_) {
      state_ = (rand() % 2) ? ST_checked_serr : ST_checked_herr;
    } else {
      state_ = ST_checked_ok;
    }
    break;
  case SIMULATE_RESULT_TYPE_ERR:
    state_ = (rand() % 2) ? ST_checked_serr : ST_checked_herr;
    break;
  case SIMULATE_RESULT_TYPE_RONDOM:
    // chk_state_ = (rand() % 2) ? ST_checked_ok : ST_checked_err;
    state_ = (rand() % 2) ? ST_checked_ok : ST_checked_err;
    if (state_ != ST_checked_ok)
      state_ = (rand() % 2) ? ST_checked_serr : ST_checked_herr;
    break;
  default:
    break;
  }
}

int iNode::do_check_back_(FUN_PARAM) {
  assert(plnode_ != nullptr);
  if (plnode_->ref_type != 1) {
    state_ = ST_checked_ok;
  } else {
    if (test_param_ && test_param_->check_type > SIMULATE_RESULT_TYPE_UNKNOW) {
      simu_check__();
    } else {
      state_ = saltman_->check_node(this);
    }
  }
  dj_.send_graph_status(g_->get_pl_exe_id(), g_->get_plid(), id_, state_,
                        state_);

  return (state_ == ST_checked_ok) ? 0 : state_;
}

int iNode::init(int64_t i, MARIO_NODE *node, iGraphStateMachine *gsm,
                iNodeStateMachine *nsm, saltman *sm) {
  gen_pl_node(i, node);
  gsm_ = gsm, nsm_ = nsm;

  assert(sm != nullptr);
  saltman_ = sm;

  id_ = i;
  setup_state_machine_();
  return 0;
}

int iNode::do_run_front_(FUN_PARAM) {
  assert(g_ != nullptr);
  state_ = ST_running;

  if (plnode_->ref_type != 1) {
    switch (plnode_->ref_type) {
    case 2: // waiting for confirms
    case 4: // start
    case 5: // end
      state_ = ST_running;
      dj_.send_graph_status(g_->get_pl_exe_id(), g_->get_plid(), id_, state_,
                            state_);
      break;
    // case 6: // poweroff
    //   break;
    // case 7: // restart
    //   break;
    // case 8: // logoff
    //   break;
    default:
      break;
    }
  }

  if (test_param_)
    std::this_thread::sleep_for(
        std::chrono::milliseconds(test_param_->sleep_interval));
  return 0;
}

void iNode::simu_run__() {
  switch (test_param_->run_type) {
  case SIMULATE_RESULT_TYPE_OK: {
    if (test_param_->run_err_id == id_)
      state_ = ST_error;
    else if (test_param_->timeout_id == id_)
      state_ = ST_timeout;
    else
      state_ = ST_succeed;
  } break;
  case SIMULATE_RESULT_TYPE_ERR:
    state_ = ST_error;
    break;
  case SIMULATE_RESULT_TYPE_RONDOM:
    state_ = (STATE_TYPE)((rand() % 3) + (int)ST_error);
    break;
  default:
    break;
  }
}

int iNode::do_run_back_(FUN_PARAM) {
  if (plnode_->ref_type != 1) {
    switch (plnode_->ref_type) {
    case 2:
      state_ = ST_waiting_for_confirm;
      break;
    case 4: // start
    case 5: // end
      state_ = ST_succeed;
      break;
    // case 6: // poweroff
    //   break;
    // case 7: // restart
    //   break;
    // case 8: // logoff
    //   break;
    default:
      state_ = ST_succeed;
      break;
    }
    dj_.send_graph_status(g_->get_pl_exe_id(), g_->get_plid(), id_, state_,
                          state_);
    return state_;
  } else {
    if (test_param_ && test_param_->run_type > SIMULATE_RESULT_TYPE_UNKNOW) {
      simu_run__();
    } else {
      /* run one node
      char minion[64] = {"old080027789636"};
      // char minion[64] = {"new080027BB4DAF"};
      itat::SALT_JOB_RET ret;
      HTTP_API_PARAM param("10.10.10.19", 8000, nullptr, &ret, minion);
      salt_api_cmd_runall(&param, minion,
      "C:\\\\hongt\\\\Client\\\\ExecClient.exe abcd");
      if (ret.retcode == 0)
          state_ = ST_succeed;
      else
          state_ = ST_error;
      */

      state_ = saltman_->run_node(this);
    }

    switch (state_) {
    case ST_running:
      dj_.send_graph_status(g_->get_pl_exe_id(), g_->get_plid(), id_, state_,
                            state_);
      break;
    case ST_error:
      on_run_error(nullptr);
      break;
    case ST_timeout:
      on_run_timeout(nullptr);
      break;
    case ST_succeed:
      on_run_ok(nullptr);
      break;
    default:
      on_run_error(nullptr);
      break;
    }
  }
  return state_;
}

int iNode::do_user_confirm_front_(FUN_PARAM confirmd) {
  UNUSE(confirmd);
  state_ = ST_succeed;
  dj_.send_graph_status(g_->get_pl_exe_id(), g_->get_plid(), id_, state_,
                        state_);
  return 0;
}
int iNode::do_user_confirm_back_(FUN_PARAM confirmd) { UNUSE(confirmd); return 0; }

//////////////////////////////////////////////////////////////////

int iNode::on_run_error(FUN_PARAM) {
#ifdef _DEBUG_
  printf("iNode -> on_run_error %d\n", inodeid_2_ignodeid[this->id_]);
#endif //_DEBUG_
  return nsm_->do_trans(state_, &iNode::on_run_error_front_, this, nullptr);
}

int iNode::on_run_error_front_(FUN_PARAM) {
#ifdef _DEBUG_
  printf("iNode -> on_run_error_front_ %d\n", inodeid_2_ignodeid[this->id_]);
#endif //_DEBUG_
  state_ = ST_error;
  dj_.send_graph_status(g_->get_pl_exe_id(), g_->get_plid(), id_, state_,
                        state_);
  return 0;
}
int iNode::on_run_error_back_(FUN_PARAM) {
#ifdef _DEBUG_
  printf("iNode -> on_run_error_back_ %d\n", inodeid_2_ignodeid[this->id_]);
#endif //_DEBUG_
  g_->on_run_error(this);
  return state_;
}

int iNode::on_run_timeout(FUN_PARAM) {
  return nsm_->do_trans(state_, &iNode::on_run_timeout_front_, this, nullptr);
}

int iNode::on_run_timeout_front_(FUN_PARAM) {
  state_ = ST_timeout;
  dj_.send_graph_status(g_->get_pl_exe_id(), g_->get_plid(), id_, state_,
                        state_);
  return 0;
}
int iNode::on_run_timeout_back_(FUN_PARAM) { return state_; }

int iNode::on_run_ok(FUN_PARAM) {
#ifdef _DEBUG_
  printf("iNode -> on_run_ok %d\n", inodeid_2_ignodeid[this->id_]);
#endif //_DEBUG_
  return nsm_->do_trans(state_, &iNode::on_run_ok_front_, this, nullptr);
}
int iNode::on_run_ok_front_(FUN_PARAM) {
  state_ = ST_succeed;
#ifdef _DEBUG_
  printf("iNode -> on_run_ok_front_ %d state %d\n",
         inodeid_2_ignodeid[this->id_], state_);
#endif //_DEBUG_
  dj_.send_graph_status(g_->get_pl_exe_id(), g_->get_plid(), id_, state_,
                        state_);
  return 0;
}

int iNode::on_run_ok_back_(FUN_PARAM) {
#ifdef _DEBUG_
  printf("iNode -> on_run_ok_back_ %d state %d\n",
         inodeid_2_ignodeid[this->id_], state_);
#endif //_DEBUG_
  g_->on_run_ok(this);
  return state_;
}

} // namespace dfgraph
