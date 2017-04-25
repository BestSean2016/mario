#include "node.hpp"
#include "djangoapi.hpp"
#include "pipeline.hpp"
#include "saltapi.hpp"

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

void iNode::set_pipline_node(mr_pl_node *plnode) {
  assert(plnode != nullptr);
  plnode_ = plnode;
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
  nsm_->add_state_trans(ST_initial, &iNode::do_check_front_, ST_checking, &iNode::do_check_back_);
  nsm_->add_state_trans(ST_checked_serr, &iNode::do_check_front_, ST_checking, &iNode::do_check_back_);
  nsm_->add_state_trans(ST_checked_herr, &iNode::do_check_front_, ST_checking, &iNode::do_check_back_);
  nsm_->add_state_trans(ST_checked_ok, &iNode::do_check_front_, ST_checking, &iNode::do_check_back_);
  nsm_->add_state_trans(ST_error, &iNode::do_check_front_, ST_checking, &iNode::do_check_back_);
  nsm_->add_state_trans(ST_succeed, &iNode::do_check_front_, ST_checking, &iNode::do_check_back_);
  nsm_->add_state_trans(ST_timeout, &iNode::do_check_front_, ST_checking, &iNode::do_check_back_);

  // run
  nsm_->add_state_trans(ST_checked_ok, &iNode::do_run_front_, ST_running, &iNode::do_run_back_);

  // user_confirm
  nsm_->add_state_trans(ST_waiting_for_confirm, &iNode::do_user_confirm_front_, ST_running, &iNode::do_user_confirm_back_);


  //..................... internal trans ........................
  //////////////////////////////////////////////////////////////////

  nsm_->add_state_trans(ST_running, &iNode::on_run_error_front_, ST_error, &iNode::on_run_error_back_);
  nsm_->add_state_trans(ST_running, &iNode::on_run_ok_front_, ST_error, &iNode::on_run_ok_back_);
  nsm_->add_state_trans(ST_running, &iNode::on_run_timeout_front_, ST_error, &iNode::on_run_error_back_);
}

int iNode::do_check_front_(FUN_PARAM) {
  assert(g_ != nullptr);
  state_ = ST_checking;
  dj_.send_graph_status(g_->get_pl_exe_id(), g_->get_plid(), id_, state_, state_);
  if (test_param_) std::this_thread::sleep_for(std::chrono::milliseconds(test_param_->sleep_interval));
  return 0;
}

void iNode::simu_check__() {
  switch (test_param_->check_type) {
  case SIMULATE_RESULT_TYPE_OK:
    if (test_param_->check_err_id == id_) {
        state_ = (rand() % 2) ? ST_checked_serr :ST_checked_herr ;
    } else {
        state_ = ST_checked_ok;
    }
    break;
  case SIMULATE_RESULT_TYPE_ERR:
    state_ = (rand() % 2) ? ST_checked_serr :ST_checked_herr ;
    break;
  case SIMULATE_RESULT_TYPE_RONDOM:
    //chk_state_ = (rand() % 2) ? ST_checked_ok : ST_checked_err;
    state_ = (rand() % 2) ? ST_checked_ok : ST_checked_err;
    if (state_ != ST_checked_ok)
        state_ = (rand() % 2) ? ST_checked_serr :ST_checked_herr ;
    break;
  default:
    break;
  }
}

int iNode::do_check_back_(FUN_PARAM) {
  if (id_ == 0 || id_ == g_->get_amount_node() - 1) {
    state_ = ST_checked_ok;
  } else {
    if (test_param_ && test_param_->check_type > SIMULATE_RESULT_TYPE_UNKNOW) {
        simu_check__();
    } else {
        //do check
        //ret = check_node ... ;
    }
  }
  dj_.send_graph_status(g_->get_pl_exe_id(), g_->get_plid(), id_, state_, state_);

  return (state_ == ST_checked_ok) ? 0 : state_;
}

int iNode::init(int64_t i, iGraphStateMachine *gsm, iNodeStateMachine *nsm) {
  gen_pl_node(i);
  gsm_ = gsm, nsm_ = nsm;

  id_ = i;
  setup_state_machine_();
  return 0;
}


int iNode::do_run_front_(FUN_PARAM) {
  assert(g_ != nullptr);
  state_ = ST_running;
  dj_.send_graph_status(g_->get_pl_exe_id(), g_->get_plid(), id_, state_, state_);
  if (test_param_) std::this_thread::sleep_for(std::chrono::milliseconds(test_param_->sleep_interval));
  return 0;
}

void iNode::simu_run__() {
  switch (test_param_->run_type) {
  case SIMULATE_RESULT_TYPE_OK:
    {
      if (test_param_->run_err_id == id_)
          state_ = ST_run_one_err;
      else if (test_param_->timeout_id == id_)
          state_ = ST_timeout;
      else
          state_ = ST_succeed;
    }
    break;
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
  if (id_ == 0 || id_ == g_->get_amount_node() - 1) {
    state_ = ST_succeed;
  } else {
      if (test_param_ && test_param_->run_type > SIMULATE_RESULT_TYPE_UNKNOW) {
          simu_run__();
      } else {
          //do real run
          //ret = run_node ... ;
      }
  }

  dj_.send_graph_status(g_->get_pl_exe_id(), g_->get_plid(), id_, state_, state_);

  switch(state_) {
  case ST_error:
      on_run_error_(nullptr);
      break;
  case ST_timeout:
      on_run_timeout_(nullptr);
      break;
  case ST_succeed:
      on_run_ok_(nullptr);
      break;
  default:
      on_run_error_(nullptr);
      break;
  }

  return state_;
}

int iNode::do_user_confirm_front_(FUN_PARAM confirmd) {
    if (confirmd) state_ = ST_succeed;
    else state_ = ST_confirm_refused;
    dj_.send_graph_status(g_->get_pl_exe_id(), g_->get_plid(), id_, state_, state_);
    return 0;
}
int iNode::do_user_confirm_back_(FUN_PARAM confirmd) { return 0; }

//////////////////////////////////////////////////////////////////

int iNode::on_run_error_(FUN_PARAM) {
    return nsm_->do_trans(state_, &iNode::on_run_error_front_, this, nullptr);
}

int iNode::on_run_error_front_(FUN_PARAM) {
    state_ = ST_error;
    dj_.send_graph_status(g_->get_pl_exe_id(), g_->get_plid(), id_, state_, state_);
    return 0;
}
int iNode::on_run_error_back_(FUN_PARAM) { return state_; }

int iNode::on_run_timeout_(FUN_PARAM) {
    return nsm_->do_trans(state_, &iNode::on_run_timeout_front_, this, nullptr);
}

int iNode::on_run_timeout_front_(FUN_PARAM) {
    state_ = ST_timeout;
    dj_.send_graph_status(g_->get_pl_exe_id(), g_->get_plid(), id_, state_, state_);
    return 0;
}
int iNode::on_run_timeout_back_(FUN_PARAM) { return state_; }

int iNode::on_run_ok_(FUN_PARAM) {
    return nsm_->do_trans(state_, &iNode::on_run_ok_front_, this, nullptr);
}
int iNode::on_run_ok_front_(FUN_PARAM) {
    state_ = ST_succeed;
    dj_.send_graph_status(g_->get_pl_exe_id(), g_->get_plid(), id_, state_, state_);
    return 0; }
int iNode::on_run_ok_back_(FUN_PARAM) { return state_; }



} // namespace dfgraph
