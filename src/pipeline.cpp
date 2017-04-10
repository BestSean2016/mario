#include "pipeline.hpp"
#include "djagoapi.hpp"
#include "node.hpp"
#include "saltapi.hpp"

#include <memory.h>
#include <vector>

#define USERNAME "salt-test"
#define PASSWORD "hongt@8a51"
#define SALTAPI_SERVER_IP "10.10.10.19"
#define SALTAPI_SERVER_PORT 8000

namespace itat {

Pipeline::Pipeline() {
  memset(&ig_, 0, sizeof(igraph_t));
  gsm_ = new iGraphStateMachine();
  nsm_ = new iNodeStateMachine();
}

Pipeline::~Pipeline() {
  SafeDeletePtr(gsm_);
  SafeDeletePtr(nsm_);

  jid_2_node_.clear();

  for (auto &n : node_) {
    delete n;
  }
  node_.clear();
  if (ig_.n > 0)
    igraph_destroy(&ig_);
}

/**
 * @brief diamod_simulator generate a graph
 * @param n number of nodes
 * @param b number of branches
 * @return 0 for good
 */
int Pipeline::diamod_simulator(int node_num, int branch_num) {
  gen_diamod_graph_(node_num, branch_num);
  gen_node_();

  return (0);
}

int Pipeline::gen_diamod_graph_(int node_num, int branch_num) {
  igraph_t gIn, gOut;
  igraph_vector_t edge;
  std::vector<int> e;
  int half = node_num / 2;

  // generate 2 tree type graphs with in and out style
  igraph_tree(&gIn, half, branch_num, IGRAPH_TREE_IN);
  igraph_tree(&gOut, half, branch_num, IGRAPH_TREE_OUT);

  // get the out style tree's all edges, the put them to vector e
  igraph_vector_init(&edge, 0);
  igraph_get_edgelist(&gOut, &edge, false);
  for (int64_t i = 0; i < igraph_vector_size(&edge); ++i)
    e.push_back(VECTOR(edge)[i]);

  { // connect out-tree to in-tree
    igraph_vector_t neinode;
    igraph_vector_init(&neinode, 0);
    for (int i = 0; i < half; ++i) {
      igraph_neighbors(&gOut, &neinode, i, IGRAPH_OUT);
      // if this node have no neighbors, then connect to the next part of graph
      if (0 == igraph_vector_size(&neinode)) {
        e.push_back(i);
        e.push_back(i + ((half - i) - 1) * 2 + 1);
      }
    }
    igraph_vector_destroy(&neinode);
  }

  // get all edges of in style graph
  igraph_get_edgelist(&gIn, &edge, false);
  for (int64_t i = 0; i < igraph_vector_size(&edge); ++i) {
    int vid = VECTOR(edge)[i];
    e.push_back(vid + ((half - vid) - 1) * 2 + 1);
  }

  igraph_vector_destroy(&edge);
  igraph_destroy(&gOut);
  igraph_destroy(&gIn);

  //
  // generate new graph with diamod style
  //
  // get all edges from vector e
  igraph_vector_init(&edge, e.size());
  for (int i = 0; i < (int)e.size(); ++i)
    VECTOR(edge)[i] = e[i];

  igraph_i_set_attribute_table(&igraph_cattribute_table);
  igraph_create(&ig_, &edge, half * 2, 1);
  igraph_vector_destroy(&edge);

  return (0);
}

int Pipeline::gen_node_() {
  for (int i = 0; i < ig_.n; ++i) {
    auto node = new iNode(this);
    node->init(i, gsm_, nsm_);
    node_.emplace_back(node);
  }
  return (0);
}

int Pipeline::gen_piple_graph() {
  int ret = load_pipe_line_from_db_();
  if (!ret)
    ret = gen_piple_graph_();
  if (!ret)
    ret = gen_node_();

  return ret;
}

int Pipeline::load_pipe_line_from_db_() {
  // to do: load piple line from db by piplline id
  return 0;
}

int Pipeline::gen_piple_graph_() { return 0; }

iNode *Pipeline::get_node_by_jid(std::string &jid) {
  auto iter = jid_2_node_.find(jid);
  return (iter == jid_2_node_.end()) ? nullptr : iter->second;
}

int Pipeline::init() {
  setup_state_machine_();
  HTTP_API_PARAM param(SALTAPI_SERVER_IP, SALTAPI_SERVER_PORT, parse_token_fn,
                       nullptr, nullptr);
  return salt_api_login(&param, USERNAME, PASSWORD);
}

int Pipeline::check() {
  // do_check_
  return gsm_->do_trans(ST_initial, &Pipeline::do_check_, this, nullptr);
}

int Pipeline::run(int start_id) { return 0; }

int Pipeline::run_node(int node_id) { return 0; }

int Pipeline::pause() { return 0; }

int Pipeline::goon() { return 0; }

int Pipeline::stop() { return 0; }

int Pipeline::skip() { return 0; }

int Pipeline::redo() { return 0; }

int Pipeline::on_check_error(int64_t node_id, bool hosterr, std::string &errmsg) {
  state_ = ST_checked_err;
  djagno_api_send_graph_status(graph_id_, NO_NODE, state_, chk_state_);
  // set node's state to check error
  return 0;
}

int Pipeline::on_check_ok(int64_t node_id, bool hosterr, std::string &errmsg) {
  return 0;
}

int Pipeline::on_check_allok() {
  state_ = ST_checked_ok;
  djagno_api_send_graph_status(graph_id_, NO_NODE, state_, chk_state_);
  return 0;
}

int Pipeline::on_run_start() {
  state_ = ST_running;
  djagno_api_send_graph_status(graph_id_, NO_NODE, state_, chk_state_);
  return 0;
}

int Pipeline::on_run_error(int64_t node_id, int code, std::string &stdout,
                         std::string &stderr) {
  state_ = ST_error;
  djagno_api_send_graph_status(graph_id_, node_id, state_, chk_state_, code,
                               stdout, stderr);
  return 0;
}

int Pipeline::on_run_ok(int64_t node_id, int code, std::string &stdout,
                      std::string &stderr) {
  return 0;
}

int Pipeline::on_run_allok() {
  state_ = ST_successed;
  djagno_api_send_graph_status(graph_id_, NO_NODE, state_, chk_state_);
  return 0;
}

int Pipeline::on_pause() {
  state_ = ST_paused;
  djagno_api_send_graph_status(graph_id_, NO_NODE, state_, chk_state_);
  return 0;
}

int Pipeline::on_continue() {
  state_ = ST_running;
  djagno_api_send_graph_status(graph_id_, NO_NODE, state_, chk_state_);
  return 0;
}

int Pipeline::on_stop() {
  state_ = ST_stoped;
  djagno_api_send_graph_status(graph_id_, NO_NODE, state_, chk_state_);
  return 0;
}

int Pipeline::on_wait_for_user(int64_t node_id) {
  state_ = ST_waiting_for_input;
  djagno_api_send_graph_status(graph_id_, NO_NODE, state_, chk_state_);
  return 0;
}

int Pipeline::on_user_confirmed(int64_t node_id) {
  state_ = ST_running;
  djagno_api_send_graph_status(graph_id_, NO_NODE, state_, chk_state_);
  return 0;
}

int Pipeline::on_wait_ror_run() {
  state_ = ST_waiting_for_run;
  djagno_api_send_graph_status(graph_id_, NO_NODE, state_, chk_state_);
  return 0;
}

int Pipeline::test_check_error(int node_id, bool hosterr, std::string &errmsg) {
  on_check_error(node_id, hosterr, errmsg);
  return 0;
}

int Pipeline::test_check_ok(int node_id, bool hosterr, std::string &errmsg) {
  return 0;
}

int Pipeline::test_run_error(int node_id, int code, std::string &stdout,
                           std::string &stderr) {
  on_run_error(node_id, code, stdout, stderr);
  return 0;
}

int Pipeline::test_run_ok(int node_id, int code, std::string &stdout,
                        std::string &stderr) {
  return 0;
}

int Pipeline::test_user_confirmed(int node_id) {
  on_wait_for_user(node_id);
  return 0;
}

int Pipeline::test_1(FUN_PARAM) {
  cout << "test 1 " << (uint64_t)this << endl;
  return 1;
}

int Pipeline::test_2(FUN_PARAM) {
  cout << "test 2 " << (uint64_t)this << endl;
  return 2;
}

#define DOCHECK &Pipeline::do_check_
#define DOCHECKING &Pipeline::do_checking_
#define DORUN &Pipeline::do_run_
#define DORUNNING &Pipeline::do_running_

int Pipeline::setup_state_machine_() {
  // check
  gsm_->add_state_trans(ST_initial, DOCHECK, ST_checking, DOCHECKING);
  gsm_->add_state_trans(ST_paused, DOCHECK, ST_checking, DOCHECKING);
  gsm_->add_state_trans(ST_error, DOCHECK, ST_checking, DOCHECKING);
  gsm_->add_state_trans(ST_successed, DOCHECK, ST_checking, DOCHECKING);
  gsm_->add_state_trans(ST_timeout, DOCHECK, ST_checking, DOCHECKING);
  gsm_->add_state_trans(ST_stoped, DOCHECK, ST_checking, DOCHECKING);

  // run
  gsm_->add_state_trans(ST_initial, DORUN, ST_running, DORUNNING);

  return 0;
}


//Check Action
int Pipeline::do_check_(FUN_PARAM) {
  chk_state_ = ST_checking;
  djagno_api_send_graph_status(graph_id_, NO_NODE, state_, chk_state_);
#ifdef _DEBUG_
  cout << "igraph " << graph_id_ << ": do_check_" << endl;
#endif //_DEBUG_

  return 0;
}

int Pipeline::do_node_check_(const igraph_t *graph, igraph_integer_t vid,
                           igraph_integer_t /*pred*/, igraph_integer_t /*succ*/,
                           igraph_integer_t /*rank*/, igraph_integer_t /*dist*/,
                           void *extra) {
  Pipeline *g = (Pipeline *)extra;
  assert(graph == &g->ig_);

  iNode *node = g->get_node_by_id(vid);
  assert(node != nullptr);

  if (node->check()) {
    g->chk_state_ = ST_checked_err;
  }

  return 0;
}

int Pipeline::do_checking_(FUN_PARAM) {
  assert(ig_.n > 0);

  igraph_vector_t order;
  igraph_vector_init(&order, igraph_vcount(&ig_));
  igraph_bfs(&ig_, 0, nullptr, IGRAPH_OUT, true, nullptr, &order, nullptr,
             nullptr, nullptr, nullptr, nullptr, &Pipeline::do_node_check_, this);
  igraph_vector_destroy(&order);

  if (chk_state_ == ST_checking)
    chk_state_ = ST_checked_ok;
  djagno_api_send_graph_status(graph_id_, NO_NODE, state_, chk_state_);

  return 0;
}


//Run Action
int Pipeline::do_run_(FUN_PARAM node_id) {
    return 0;
}

int Pipeline::do_running_(FUN_PARAM node_id) {
    return 0;
}

//the running callback function for igraph's visitor
int Pipeline::do_node_run_(const igraph_t *graph, igraph_integer_t vid,
                        igraph_integer_t, igraph_integer_t,
                        igraph_integer_t, igraph_integer_t, void *extra) {
    return 0;
}

} // namespace itat
