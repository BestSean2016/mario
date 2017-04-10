#include "node.hpp"
#include "pipeline.hpp"
#include "saltapi.hpp"
#include "djagoapi.hpp"

#include <iostream>
#include <vector>

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

  return nsm_->do_trans(state_, &iNode::do_check_, this, nullptr);
}

int iNode::run() { return 0; }

int iNode::pause() { return 0; }

int iNode::goon() { return 0; }

int iNode::stop() { return 0; }

int iNode::on_check_start() { return 0; }

int iNode::on_check_error() { return 0; }

int iNode::on_check_ok() { return 0; }

int iNode::on_run_start() { return 0; }

int iNode::on_run_error() { return 0; }

int iNode::on_run_ok() { return 0; }

int iNode::on_pause() { return 0; }

int iNode::on_continue() { return 0; }

int iNode::on_stop() { return 0; }

int iNode::on_wait_for_user() { return 0; }

int iNode::on_wait_ror_run() { return 0; }

void iNode::setup_state_machine_() {
  // check
  nsm_->add_state_trans(ST_initial, &iNode::do_check_, ST_checking,
                        &iNode::do_checking_);
  nsm_->add_state_trans(ST_checked_err, &iNode::do_check_, ST_checking,
                        &iNode::do_checking_);
  nsm_->add_state_trans(ST_checked_ok, &iNode::do_check_, ST_checking,
                        &iNode::do_checking_);
  nsm_->add_state_trans(ST_paused, &iNode::do_check_, ST_checking,
                        &iNode::do_checking_);
  nsm_->add_state_trans(ST_error, &iNode::do_check_, ST_checking,
                        &iNode::do_checking_);
  nsm_->add_state_trans(ST_successed, &iNode::do_check_, ST_checking,
                        &iNode::do_checking_);
  nsm_->add_state_trans(ST_timeout, &iNode::do_check_, ST_checking,
                        &iNode::do_checking_);
  nsm_->add_state_trans(ST_stoped, &iNode::do_check_, ST_checking,
                        &iNode::do_checking_);
}

int iNode::do_check_(FUN_PARAM) {
    assert(g_ != nullptr);
    chk_state_ = ST_checking;
    djagno_api_send_graph_status(g_->get_graph_id(),
                           id_,
                           state_,
                           chk_state_);
    return 0;
}


#define _TEST_RANDOM_CHECKING_RESULT


int iNode::do_checking_(FUN_PARAM) {
    int ret = 0;
#ifdef _TEST_RANDOM_CHECKING_RESULT
    //ret = id_ > 9;
    if (ret) {
        chk_state_ = ST_checked_err;
    } else {
        chk_state_ = ST_checked_ok;
        cout << "OK\n";
    }
#endif //_TEST_RANDOM_CHECKING_RESULT

    djagno_api_send_graph_status(g_->get_graph_id(),
                           id_,
                           state_,
                           chk_state_);

    return ret;
}

int iNode::init(int64_t i, iGraphStateMachine *gsm, iNodeStateMachine *nsm) {
  gen_pl_node(i);
  gsm_ = gsm, nsm_ = nsm;

  id_ = i;
  setup_state_machine_();
  return 0;
}

} // namespace dfgraph
