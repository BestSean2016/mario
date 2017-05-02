#include "pipeline.hpp"
#include "djangoapi.hpp"
#include "node.hpp"
#include "saltapi.hpp"

#define USERNAME "salt-test"
#define PASSWORD "salt-test"
#define SALTAPI_SERVER_IP "10.10.10.19"
#define SALTAPI_SERVER_PORT 8000

std::mutex g_run_mutex;
#define ENTER_MULTEX auto guard = new std::lock_guard<std::mutex>(g_run_mutex);
#define EXIT_MULTEX delete guard;

bool vec_find(std::vector<int> &vec, int a) {
  for (auto &p : vec)
    if (p == a)
      return true;
  return false;
}

void vec_insert(std::vector<int> &vec, int a) {
  if (!vec_find(vec, a))
    vec.emplace_back(a);
}

void vec_erase(std::vector<int> &vec, int a) {
  for (auto it = vec.begin(); it != vec.end(); it++) {
    if (*it == a) {
      vec.erase(it);
      break;
    }
  }
}

namespace itat {

Pipeline::Pipeline(int plid) : plid_(plid) {
  memset(&ig_, 0, sizeof(igraph_t));
  gsm_ = new iGraphStateMachine();
  nsm_ = new iNodeStateMachine();
}

Pipeline::~Pipeline() {
  // if (tsim_.joinable()) {
  //     state_ = ST_stoped;
  //     tsim_.join();
  // }
  //
  // if (tevent_.joinable()) {
  //     state_ = ST_stoped;
  //     tevent_.join();
  // }

  SafeDeletePtr(gsm_);
  SafeDeletePtr(nsm_);

  jid_2_node_.clear();

  for (auto &n : node_) {
    delete n;
  }
  node_.clear();
  if (ig_.n > 0)
    igraph_destroy(&ig_);

  SafeDeletePtr(test_param_);
}

/**
 * @brief diamod_simulator generate a graph
 * @param n number of nodes
 * @param b number of branches
 * @return 0 for good
 */
int Pipeline::diamod_simulator_(int node_num, int branch_num) {
  gen_diamod_graph_(node_num, branch_num);
  gen_node_(nullptr);

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
  for (int i = 0; i < igraph_vector_size(&edge); ++i)
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
  for (int i = 0; i < igraph_vector_size(&edge); ++i) {
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

int Pipeline::gen_node_(vector<MR_BILL_PIPELINE_NODE>* nodes) {
  for (int i = 0; i < ig_.n; ++i) {
    auto node = new iNode(this);
    node->init(i, (nodes) ? nullptr : &(nodes->at(i)), gsm_, nsm_);
    if (test_param_)
      node->set_test_param(test_param_);
    node_.emplace_back(node);
  }
  return (0);
}

int Pipeline::gen_piple_graph() {
  vector<MR_BILL_PIPELINE_NODE> pl_node;
  vector<MR_BILL_PIPELINE_EDGE> pl_edge;
  int ret = load_pipe_line_from_db_(pl_node, pl_edge);
  if (!ret)
    ret = gen_node_(&pl_node);

  return ret;
}

#define MYSQL_DB_HOST "10.10.10.16"
#define MYSQL_DB_PORT 3306
#define MYSQL_DB_NAME "mysite"
#define MYSQL_DB_USER "bill"
#define MYSQL_DB_PASS "hongt@8a51"

int Pipeline::load_pipe_line_from_db_(vector<MR_BILL_PIPELINE_NODE> &pl_node,
                                      vector<MR_BILL_PIPELINE_EDGE> &pl_edge) {
  // to do: load piple line from db by piplline id
  int ret = 0;
  DBHANDLE h_db = connect_db(MYSQL_DB_HOST, MYSQL_DB_PORT, MYSQL_DB_NAME,
                             MYSQL_DB_USER, MYSQL_DB_PASS);
  if (!h_db)
    return -1;
  ret = create_graph(&ig_, pl_node, pl_edge, h_db, plid_);
  disconnect_db(h_db);
  return ret;
}

iNode *Pipeline::get_node_by_jid(std::string &jid) {
  auto iter = jid_2_node_.find(jid);
  return (iter == jid_2_node_.end()) ? nullptr : iter->second;
}

int Pipeline::initial(int real_run, int node_num, int branch_num) {
  setup_state_machine_();
  if (real_run)
    gen_piple_graph();
  else
    diamod_simulator_(node_num, branch_num);

  HTTP_API_PARAM param(SALTAPI_SERVER_IP, SALTAPI_SERVER_PORT, parse_token_fn,
                       nullptr, nullptr);
  return salt_api_login(&param, USERNAME, PASSWORD);
}

int Pipeline::check() {
  return gsm_->do_trans(state_, &Pipeline::do_check_front_, this, nullptr);
}

static void thread_test(void) { // bill_message bm) {
  for (int i = 0; i < 10; i++) {
    cout << "thred_test " << i << endl;
    cout << "thred_test end " << i << endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(400));
  }
}

int Pipeline::run(int start_id) {
  iNode *node = get_node_by_id((int)((uint64_t)start_id));
  if (nullptr == node)
    return ERROR_INVAILD_NODE_ID;
  else
    return gsm_->do_trans(state_, &Pipeline::do_run_front_, this,
                          (void *)start_id);
}

int Pipeline::run_node(int node_id) {
  iNode *node = get_node_by_id((int)((uint64_t)node_id));
  if (nullptr == node)
    return ERROR_INVAILD_NODE_ID;
  else
    return gsm_->do_trans(state_, &Pipeline::do_run_one_front_, this, node);
}

int Pipeline::pause() {
  return gsm_->do_trans(state_, &Pipeline::do_pause_front_, this, nullptr);
}

int Pipeline::go_on() {
  return gsm_->do_trans(state_, &Pipeline::do_go_on_front_, this, nullptr);
}

int Pipeline::stop() {
  return gsm_->do_trans(state_, &Pipeline::do_stop_front_, this, nullptr);
}

int Pipeline::user_confirm(int ok) {
  return gsm_->do_trans(state_, &Pipeline::do_user_confirm_front_, this,
                        (FUN_PARAM)ok);
}

int Pipeline::on_run_ok_(FUN_PARAM node) {
  // dj_.send_graph_status(pleid_, plid_, NO_NODE, state_, chk_state_);
  return gsm_->do_trans(state_, &Pipeline::on_run_ok_front_, this, node);
}

int Pipeline::on_run_ok_front_(FUN_PARAM node) {
  iNode *n = (iNode *)node;
  dj_.send_graph_status(pleid_, plid_, NO_NODE, state_, chk_state_);
  return 0;
}

int Pipeline::on_run_ok_back_(FUN_PARAM node) {
  int id = ((iNode *)node)->get_id();
  cout << "on_run_after_ok_\n";
  ENTER_MULTEX
  // remove from run_set_
  vec_erase(run_set_, id);

  // find new node to run
  if (!run_set_.empty())
    find_node_to_run_(id, true);
  EXIT_MULTEX
  return 0;
}

// ........ on run error ..........................
int Pipeline::on_run_error_(FUN_PARAM node) {
  iNode *n = (iNode *)node;
  cout << "on_run_error\n";
  return gsm_->do_trans(state_, &Pipeline::on_run_error_front_, this, n);
}

int Pipeline::on_run_error_front_(FUN_PARAM node) {
  state_ = ST_error;
  dj_.send_graph_status(pleid_, plid_, NO_NODE, state_, chk_state_);
  return 0;
}

int Pipeline::on_run_error_back_(FUN_PARAM node) { return 0; }
// ........ on run error ..........................

// ........ on run timeout ..........................
int Pipeline::on_run_timeout_(FUN_PARAM node) {
  iNode *n = (iNode *)node;
  cout << "on_run_timeout\n";
  return gsm_->do_trans(state_, &Pipeline::on_run_timeout_front_, this, n);
}
int Pipeline::on_run_timeout_front_(FUN_PARAM node) {
  state_ = ST_timeout;
  dj_.send_graph_status(pleid_, plid_, NO_NODE, state_, chk_state_);
  return 0;
}
int Pipeline::on_run_timeout_back_(FUN_PARAM node) { return 0; }
// ........ on run timeout ..........................

int Pipeline::on_run_allok_(FUN_PARAM) {
  // dj_.send_graph_status(pleid_, plid_, NO_NODE, state_, chk_state_);
  return gsm_->do_trans(state_, &Pipeline::on_run_allok_front_, this, nullptr);
}

int Pipeline::on_run_allok_front_(FUN_PARAM) {
  state_ = ST_succeed;
  dj_.send_graph_status(pleid_, plid_, NO_NODE, state_, chk_state_);
  return 0;
}

int Pipeline::on_run_allok_back_(FUN_PARAM) { return 0; }

int Pipeline::on_run_one_error_(FUN_PARAM node) {
  return gsm_->do_trans(state_, &Pipeline::on_run_one_error_front_, this, node);
}

int Pipeline::on_run_one_error_front_(FUN_PARAM node) {
  return gsm_->do_trans(state_, &Pipeline::on_run_one_error_back_, this, node);
}
int Pipeline::on_run_one_error_back_(FUN_PARAM node) { return 0; }

int Pipeline::on_run_one_timeout_(FUN_PARAM node) {
  return gsm_->do_trans(state_, &Pipeline::on_run_one_timeout_front_, this,
                        node);
}

int Pipeline::on_run_one_timeout_front_(FUN_PARAM node) { return 0; }
int Pipeline::on_run_one_timeout_back_(FUN_PARAM node) { return 0; }

int Pipeline::on_run_one_ok_(FUN_PARAM node) {
  return gsm_->do_trans(state_, &Pipeline::on_run_one_ok_front_, this, node);
}

int Pipeline::on_run_one_ok_front_(FUN_PARAM node) { return 0; }
int Pipeline::on_run_one_ok_back_(FUN_PARAM node) { return 0; }

int Pipeline::on_paused_(FUN_PARAM) {
  return gsm_->do_trans(state_, &Pipeline::on_paused_front_, this, nullptr);
}

int Pipeline::on_paused_front_(FUN_PARAM) {
  // check if all node are paused
  state_ = ST_paused;
  dj_.send_graph_status(pleid_, plid_, NO_NODE, state_, chk_state_);
  return 0;
}
int Pipeline::on_paused_back_(FUN_PARAM) { return 0; }

int Pipeline::on_stoped_(FUN_PARAM) {
  return gsm_->do_trans(state_, &Pipeline::on_stoped_front_, this, nullptr);
}

int Pipeline::on_stoped_front_(FUN_PARAM) {
  // check if all node are stoped
  state_ = ST_stoped;
  dj_.send_graph_status(pleid_, plid_, NO_NODE, state_, chk_state_);
  return 0;
}
int Pipeline::on_stoped_back_(FUN_PARAM) { return 0; }

int Pipeline::on_waitin_confirm_(FUN_PARAM node) {
  return gsm_->do_trans(state_, &Pipeline::on_waitin_confirm_front_, this,
                        node);
}

int Pipeline::on_waitin_confirm_front_(FUN_PARAM node) {
  state_ = ST_waiting_for_confirm;
  dj_.send_graph_status(pleid_, plid_, NO_NODE, state_, chk_state_);
  return 0;
}

int Pipeline::on_waitin_confirm_back_(FUN_PARAM node) { return 0; }

int Pipeline::test_1(FUN_PARAM) {
  cout << "test 1 " << (uint64_t) this << endl;
  return 1;
}

int Pipeline::test_2(FUN_PARAM) {
  cout << "test 2 " << (uint64_t) this << endl;
  return 2;
}

int Pipeline::setup_state_machine_() {
  // check
  gsm_->add_state_trans(ST_initial, &Pipeline::do_check_front_, ST_checking,
                        &Pipeline::do_check_back_);
  gsm_->add_state_trans(ST_checked_err, &Pipeline::do_check_front_, ST_checking,
                        &Pipeline::do_check_back_);
  gsm_->add_state_trans(ST_checked_ok, &Pipeline::do_check_front_, ST_checking,
                        &Pipeline::do_check_back_);
  gsm_->add_state_trans(ST_error, &Pipeline::do_check_front_, ST_checking,
                        &Pipeline::do_check_back_);
  gsm_->add_state_trans(ST_timeout, &Pipeline::do_check_front_, ST_checking,
                        &Pipeline::do_check_back_);
  gsm_->add_state_trans(ST_succeed, &Pipeline::do_check_front_, ST_checking,
                        &Pipeline::do_check_back_);
  gsm_->add_state_trans(ST_waiting_for_confirm, &Pipeline::do_check_front_,
                        ST_checking, &Pipeline::do_check_back_);
  gsm_->add_state_trans(ST_stoped, &Pipeline::do_check_front_, ST_checking,
                        &Pipeline::do_check_back_);
  gsm_->add_state_trans(ST_paused, &Pipeline::do_check_front_, ST_checking,
                        &Pipeline::do_check_back_);

  // ////////////////////////////////////////////////////////////////////////////////////////////
  // run, user action run(node)
  gsm_->add_state_trans(ST_initial, &Pipeline::do_run_front_, ST_running,
                        &Pipeline::do_run_back_);
  gsm_->add_state_trans(ST_checked_ok, &Pipeline::do_run_front_, ST_running,
                        &Pipeline::do_run_back_);

  // internal action for run
  gsm_->add_state_trans(ST_running, &Pipeline::on_run_ok_front_, ST_running,
                        &Pipeline::on_run_ok_back_);
  gsm_->add_state_trans(ST_running, &Pipeline::on_run_error_front_, ST_error,
                        &Pipeline::on_run_error_back_);
  gsm_->add_state_trans(ST_running, &Pipeline::on_run_timeout_front_,
                        ST_timeout, &Pipeline::on_run_timeout_back_);
  gsm_->add_state_trans(ST_running, &Pipeline::on_run_allok_front_, ST_succeed,
                        &Pipeline::on_run_allok_back_);
  // ////////////////////////////////////////////////////////////////////////////////////////////

  //.....................................................................................
  // run_one
  gsm_->add_state_trans(ST_initial, &Pipeline::do_run_one_front_, ST_running,
                        &Pipeline::do_run_one_back_);
  gsm_->add_state_trans(ST_checked_ok, &Pipeline::do_run_one_front_, ST_running,
                        &Pipeline::do_run_one_back_);
  gsm_->add_state_trans(ST_error, &Pipeline::do_run_one_front_, ST_running,
                        &Pipeline::do_run_one_back_);
  gsm_->add_state_trans(ST_timeout, &Pipeline::do_run_one_front_, ST_running,
                        &Pipeline::do_run_one_back_);
  gsm_->add_state_trans(ST_paused, &Pipeline::do_run_one_front_, ST_running,
                        &Pipeline::do_run_one_back_);

  // internal action for run_one
  gsm_->add_state_trans(ST_running, &Pipeline::on_run_one_ok_front_, ST_running,
                        &Pipeline::on_run_ok_back_);
  gsm_->add_state_trans(ST_running, &Pipeline::on_run_one_error_front_,
                        ST_error, &Pipeline::on_run_error_back_);
  gsm_->add_state_trans(ST_running, &Pipeline::on_run_one_timeout_front_,
                        ST_timeout, &Pipeline::on_run_timeout_back_);
  //.....................................................................................

  //.....................................................................................
  // pause and pausing
  gsm_->add_state_trans(ST_running, &Pipeline::do_pause_front_, ST_pausing,
                        &Pipeline::do_pause_back_);
  gsm_->add_state_trans(ST_pausing, &Pipeline::on_paused_front_, ST_paused,
                        &Pipeline::on_paused_back_);
  // paused ..............................

  //.....................................................................................
  // go_on
  gsm_->add_state_trans(ST_paused, &Pipeline::do_go_on_front_, ST_running,
                        &Pipeline::do_go_on_back_);
  gsm_->add_state_trans(ST_error, &Pipeline::do_go_on_front_, ST_running,
                        &Pipeline::do_go_on_back_);
  gsm_->add_state_trans(ST_timeout, &Pipeline::do_go_on_front_, ST_running,
                        &Pipeline::do_go_on_back_);
  gsm_->add_state_trans(ST_waiting_for_confirm, &Pipeline::do_go_on_front_,
                        ST_running, &Pipeline::do_go_on_back_);

  //.....................................................................................
  // stop and stoping
  gsm_->add_state_trans(ST_running, &Pipeline::do_stop_front_, ST_stoping,
                        &Pipeline::do_stop_back_);
  gsm_->add_state_trans(ST_paused, &Pipeline::do_stop_front_, ST_stoping,
                        &Pipeline::do_stop_back_);
  gsm_->add_state_trans(ST_error, &Pipeline::do_stop_front_, ST_stoping,
                        &Pipeline::do_stop_back_);
  gsm_->add_state_trans(ST_timeout, &Pipeline::do_stop_front_, ST_stoping,
                        &Pipeline::do_stop_back_);
  gsm_->add_state_trans(ST_waiting_for_confirm, &Pipeline::do_stop_front_,
                        ST_stoping, &Pipeline::do_stop_back_);
  gsm_->add_state_trans(ST_stoping, &Pipeline::on_stoped_front_, ST_stoped,
                        &Pipeline::do_stop_back_);

  // confirm ..............................
  gsm_->add_state_trans(ST_waiting_for_confirm,
                        &Pipeline::do_user_confirm_front_, ST_running,
                        &Pipeline::do_user_confirm_back_);
  gsm_->add_state_trans(ST_running, &Pipeline::on_waitin_confirm_front_,
                        ST_waiting_for_confirm,
                        &Pipeline::on_waitin_confirm_back_);

  return 0;
}

// Check Action
int Pipeline::do_check_front_(FUN_PARAM) {
  chk_state_ = ST_checking;
  dj_.send_graph_status(pleid_, plid_, NO_NODE, state_, chk_state_);
#ifdef _DEBUG_
  cout << "igraph " << plid_ << ": do_check_" << endl;
#endif //_DEBUG_

  return 0;
}

int Pipeline::do_node_check_cb_(const igraph_t *graph, igraph_integer_t vid,
                                igraph_integer_t /*pred*/,
                                igraph_integer_t /*succ*/,
                                igraph_integer_t /*rank*/,
                                igraph_integer_t /*dist*/, void *extra) {
  Pipeline *g = (Pipeline *)extra;
  assert(graph == &g->ig_);

  iNode *node = g->get_node_by_id(vid);
  assert(node != nullptr);

  if (node->check()) {
    g->chk_state_ = ST_checked_err;
  }

  if (g->test_param_) {
    std::this_thread::sleep_for(
        std::chrono::milliseconds(g->test_param_->sleep_interval));
  }
  return 0;
}

int Pipeline::do_check_back_(FUN_PARAM) {
  assert(ig_.n > 0);

  igraph_vector_t order;
  igraph_vector_init(&order, igraph_vcount(&ig_));
  igraph_bfs(&ig_, 0, nullptr, IGRAPH_OUT, true, nullptr, &order, nullptr,
             nullptr, nullptr, nullptr, nullptr, &Pipeline::do_node_check_cb_,
             this);
  igraph_vector_destroy(&order);

  if (chk_state_ == ST_checking)
    chk_state_ = ST_checked_ok;
  dj_.send_graph_status(pleid_, plid_, NO_NODE, state_, chk_state_);

  return 0;
}

// Run Action
int Pipeline::do_run_front_(FUN_PARAM node_id) {
  // first thing first, do check~~

  int ret = 0;

  gsm_->do_trans(state_, &Pipeline::do_check_front_, this, (void *)node_id);
  if (!ret) {
    int start_node = PTR2INT(node_id);
    if (start_node < 0 || start_node >= ig_.n)
      ret = ERROR_INVAILD_NODE_ID;
  }

  if (ST_checked_ok != chk_state_)
    ret = ERROR_WRONG_STATE_TO_ACTION;

  if (ret) {
    state_ = ST_checked_err;
    dj_.send_graph_status(pleid_, plid_, NO_NODE, state_, chk_state_);
    return ret;
  }

  if (!ret) {
    state_ = ST_running;
    //start event thread
  }

  dj_.send_graph_status(pleid_, plid_, NO_NODE, state_, chk_state_);

  if (ret)
    state_ = ST_error;
  return ret;
}

#define INTVEC(y, i) ((int)(((y).stor_begin)[(i)]))

void Pipeline::find_node_to_run_(int node_id, bool after) {
  if (after) {
    // get all child node of node_id put into prepare_to_run_
    igraph_vector_t y;
    igraph_vector_init(&y, 0);
    igraph_neighbors(&ig_, &y, node_id, IGRAPH_OUT);
    for (int i = 0; i < igraph_vector_size(&y); ++i)
      vec_insert(prepare_to_run_, INTVEC(y, i));

    igraph_vector_t order;
    igraph_vector_init(&order, 0);
    igraph_bfs(&ig_, 0, &y, IGRAPH_OUT, false, nullptr, &order, nullptr,
               nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);

    for (int j = 0; j < igraph_vector_size(&order); ++j) {
      if (INTVEC(order, j) >= 0)
        vec_insert(run_set_, INTVEC(order, j));
    }

    igraph_vector_destroy(&order);

    igraph_vector_destroy(&y);
  } else {
    vec_insert(prepare_to_run_, node_id);
    vec_insert(run_set_, node_id);
  }
}

int Pipeline::do_run_back_(FUN_PARAM node_id) {
  assert(ig_.n > 0);

  int ret = 0;
  if (!ret) {
    ENTER_MULTEX
    prepare_to_run_.clear();
    running_set_.clear();
    run_set_.clear();

    find_node_to_run_(PTR2INT(node_id), (node_id) ? false : true);

    EXIT_MULTEX

    if (true) {
      // do simulator
      // cout << "do simulator state is " << state_ <<endl;
      // tsim_ = std::thread{&Pipeline::thread_simulator_, this};
      // tsim_ = std::thread{thread_test, bm_};
      tsim_ = std::thread{ &Pipeline::thread_simulator_ex_, this };
      tsim_.join();
    } else {
      // add into threadpool to run
      for (auto &node_id : prepare_to_run_)
        do_run_node_http_request_(get_node_by_id(node_id));
      prepare_to_run_.clear();
    }
  }

  // dj_.send_graph_status(pleid_, plid_, NO_NODE, state_, chk_state_);
  return 0;
}

void Pipeline::do_run_node_http_request_(iNode* node) {
  ENTER_MULTEX
  vec_insert(running_set_, node->get_id());
  EXIT_MULTEX

  // real run;
  if (node->run()) {
    // error or timeout
    state_ = node->get_state();
    if (state_ == ST_error)
      gsm_->do_trans(state_, &Pipeline::on_run_error_, this, node);
    else if (state_ == ST_timeout)
      gsm_->do_trans(state_, &Pipeline::on_run_timeout_, this, node);
  } else {
    // async call
    if (node->get_run_type() == RUN_TYPE_ASYNC) {
    } else if (node->get_run_type() == RUN_TYPE_SYNC) {
      // sync call
      ENTER_MULTEX
      // remove from running set
      vec_erase(running_set_, node->get_id());
      // remove from run set
      vec_erase(run_set_, node->get_id());

      // all done.
      if (run_set_.empty())
        gsm_->do_trans(state_, &Pipeline::on_run_allok_, this, node);
      EXIT_MULTEX
    }
  }
}

// the running callback function for igraph's visitor
int Pipeline::do_node_run_cb_(const igraph_t *graph, igraph_integer_t vid,
                              igraph_integer_t, igraph_integer_t,
                              igraph_integer_t, igraph_integer_t, void *extra) {
  return 0;
}

int Pipeline::thread_simulator_(Pipeline *pl) {
  cout << "Pipeline thread simulator pl->state_ is " << pl->state_ << endl;
  iNode *node = nullptr;
  while (pl->state_ == ST_running) {
    node = nullptr;
    ENTER_MULTEX
    if (!pl->prepare_to_run_.empty()) {
      node = pl->get_node_by_id(*(pl->prepare_to_run_.begin()));
      cout << node->get_id() << " <--- node id "
           << "4\n";

      pl->prepare_to_run_.erase(pl->prepare_to_run_.begin());
    }
    EXIT_MULTEX
    if (node != nullptr) {
      STATE_TYPE state = (STATE_TYPE)(node->run());
      switch (state) {
      case ST_succeed:
        pl->on_run_ok_(node);
        if (pl->is_all_done_())
          pl->on_run_allok_(nullptr);
        break;
      case ST_timeout:
        pl->on_run_timeout_(node);
        break;
      case ST_error:
      default:
        pl->on_run_error_(node);
        break;
      }
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }

  // if (pl->state_ == ST_stoping)
  //   pl->on_stoped_(nullptr);

  cout << "thread simultor exit, hahaha\n";

  return 0;
}

bool Pipeline::is_all_done_() { return run_set_.empty(); }

// run one node
int Pipeline::do_run_one_front_(FUN_PARAM node) {
  assert(node != nullptr);
  return 0;
}

int Pipeline::do_run_one_back_(FUN_PARAM node) {
  assert(node != nullptr);
  return ((iNode *)node)->run();
}

// pause
int Pipeline::do_pause_front_(FUN_PARAM) {
  state_ = ST_pausing;
  dj_.send_graph_status(pleid_, plid_, NO_NODE, state_, chk_state_);
  return 0;
}
int Pipeline::do_pause_back_(FUN_PARAM) { return 0; }

// go_on
int Pipeline::do_go_on_front_(FUN_PARAM) {
  state_ = ST_running;
  dj_.send_graph_status(pleid_, plid_, NO_NODE, state_, chk_state_);
  return 0;
}
int Pipeline::do_go_on_back_(FUN_PARAM) { return 0; }

// stop
int Pipeline::do_stop_front_(FUN_PARAM) {
  state_ = ST_stoping;
  dj_.send_graph_status(pleid_, plid_, NO_NODE, state_, chk_state_);
  return 0;
}
int Pipeline::do_stop_back_(FUN_PARAM) { return 0; }

// user confirm
int Pipeline::do_user_confirm_front_(FUN_PARAM ok) {
  if (ok)
    state_ = ST_running;
  else
    state_ = ST_confirm_refused;
  dj_.send_graph_status(pleid_, plid_, NO_NODE, state_, chk_state_);
  return 0;
}
int Pipeline::do_user_confirm_back_(FUN_PARAM node) { return 0; }

void Pipeline::test_setup(SIMULATE_RESULT_TYPE check, SIMULATE_RESULT_TYPE run,
                          int check_err_id, int run_err_id, int timeout_id,
                          int pause_id, int stop_id, int confirm_id,
                          int sleep_interval) {
  SafeDeletePtr(test_param_);
  test_param_ =
      new TEST_PARAM{ check,         run,
                      check_err_id, run_err_id, timeout_id,    pause_id,
                      stop_id,      confirm_id, sleep_interval };

}

int Pipeline::thread_simulator_ex_(Pipeline *pl) {
  iNode *node = nullptr;
  while (true) {
    if (pl->state_ == ST_running) {
      // simulate run a node
      node = nullptr;
      ENTER_MULTEX
      // get all nodes will have to run
      if (!pl->prepare_to_run_.empty()) {
        node = pl->get_node_by_id(*(pl->prepare_to_run_.begin()));
        pl->prepare_to_run_.erase(pl->prepare_to_run_.begin());
      }
      EXIT_MULTEX
      if (node != nullptr) {
        // run node
        STATE_TYPE node_state = (STATE_TYPE)(node->run());
        // check the node state
        switch (node_state) {
        case ST_succeed:
          pl->on_run_ok_(node);
          if (pl->is_all_done_())
            pl->on_run_allok_(nullptr);
          break;
        case ST_timeout:
          pl->on_run_timeout_(node);
          break;
        case ST_waiting_for_confirm:
          pl->on_waitin_confirm_(node);
          break;
        case ST_error:
        default:
          pl->on_run_error_(node);
          break;
        }
      }
    } else if (pl->state_ == ST_stoping) {
      pl->on_stoped_(nullptr);
    } else if (pl->state_ == ST_pausing) {
      pl->on_paused_(nullptr);
    } else if (pl->state_ == ST_succeed || pl->state_ == ST_stoped) {
      break;
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  cout << "thread ex exited hahaha\n";

  return 0;
}

} // namespace itat
