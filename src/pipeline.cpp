#include "saltapi.hpp"
#include "pipeline.hpp"
#include "djangoapi.hpp"
#include "node.hpp"
#include "saltman.hpp"
#include "mario_sql.h"

#define MYSQL_DB_HOST "10.10.10.16"
#define MYSQL_DB_PORT 3306
#define MYSQL_DB_NAME "mysite"
#define MYSQL_DB_USER "bill"
#define MYSQL_DB_PASS "hongt@8a51"




#define ENTER_MULTEX auto guard = new std::lock_guard<std::mutex>(g_job_mutex);
#define EXIT_MULTEX delete guard;
#define SEND_STATUS dj_->send_graph_status(pleid_, plid_, NO_NODE, state_, chk_state_);

namespace itat {
extern std::mutex g_job_mutex;

Pipeline::Pipeline(int plid) : plid_(plid) {
  srand(time(0));
  nodemaps_ = init_nodemaps();
  saltman_ = new saltman;
  memset(&ig_, 0, sizeof(igraph_t));
  gsm_ = new iGraphStateMachine();
  nsm_ = new iNodeStateMachine();
}

Pipeline::~Pipeline() {
  // if (tsim_.joinable()) {
  //     state_ = ST_stoped;
  // tsim_.join();
  // }
  //
  saltman_->stop();

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

  delete saltman_;


  if (dj_) delete dj_;
  dj_ = nullptr;

  disconnect_db(g_h_db_);
  g_h_db_ = nullptr;

  destroy_nodemaps(nodemaps_);
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

int Pipeline::gen_node_(vector<MARIO_NODE> *nodes) {
  assert(nodes != nullptr);
  for (int i = 0; i < ig_.n; ++i) {
    auto node = new iNode(this);
    MARIO_NODE &mrnode = nodes->at(i);
    node->init(i, &mrnode, gsm_, nsm_, saltman_);
    if (test_param_)
      node->set_test_param(test_param_);
    node_.emplace_back(node);
  }
  return (0);
}

int Pipeline::gen_piple_graph() {
  vector<MARIO_NODE> pl_node;
  vector<MARIO_EDGE> pl_edge;
  int ret = load_pipe_line_from_db_(pl_node, pl_edge);
  if (!ret)
    ret = gen_node_(&pl_node);

  return ret;
}

int Pipeline::load_pipe_line_from_db_(vector<MARIO_NODE> &pl_node,
                                      vector<MARIO_EDGE> &pl_edge) {
  // to do: load piple line from db by piplline id
  int ret = 0;
  if (!g_h_db_)
    return -1;
  ret = create_graph(&ig_, pl_node, pl_edge, g_h_db_, plid_, nodemaps_);
  return ret;
}

iNode *Pipeline::get_node_by_jid(std::string &jid) {
  auto iter = jid_2_node_.find(jid);
  return (iter == jid_2_node_.end()) ? nullptr : iter->second;
}

int Pipeline::initial(int real_run, int node_num, int branch_num) {
  if (nullptr == (g_h_db_ = connect_db(MYSQL_DB_HOST, MYSQL_DB_PORT, MYSQL_DB_NAME,
                 MYSQL_DB_USER, MYSQL_DB_PASS)))
    return -1;

  dj_ = new DjangoAPI();
  setup_state_machine_();
  if (real_run)
    gen_piple_graph();
  else
    diamod_simulator_(node_num, branch_num);

  saltman_->init(this);
  saltman_->start();

  dj_->set_run(nodemaps_, g_h_db_);
  return 0;
}

int Pipeline::check() {
  return gsm_->do_trans(state_, &Pipeline::do_check_front_, this, nullptr);
}

// static void thread_test(void) { // bill_message bm) {
//   for (int i = 0; i < 10; i++) {
//     cout << "thred_test " << i << endl;
//     cout << "thred_test end " << i << endl;
//     std::this_thread::sleep_for(std::chrono::milliseconds(400));
//   }
// }

int Pipeline::run(int start_id, int pleid) {
  pleid_ = pleid;
  iNode *node = get_node_by_id((int)((uint64_t)start_id));
  if (nullptr == node)
    return ERROR_INVAILD_NODE_ID;
  else {
    // return gsm_->do_trans(state_, &Pipeline::do_run_front_, this,
    //                       (void *)((uint64_t)start_id));
    tsim_ = std::thread{ &Pipeline::run_run_run__, this, start_id };
    tsim_.detach();
  }

  return 0;
}

int Pipeline::run_node(int node_id) {
  iNode *node = get_node_by_id((int)((uint64_t)node_id));
  if (nullptr == node)
    return ERROR_INVAILD_NODE_ID;
  else
    return gsm_->do_trans(state_, &Pipeline::do_run_one_front_, this, node);
}

int Pipeline::pause() {
#ifdef _DEBUG_
  std::cout << "Pipeline pause()\n";
#endif //_DEBUG_
  return gsm_->do_trans(state_, &Pipeline::do_pause_front_, this, nullptr);
}

int Pipeline::go_on() {
  return gsm_->do_trans(state_, &Pipeline::do_go_on_front_, this, nullptr);
}

int Pipeline::stop() {
  return gsm_->do_trans(state_, &Pipeline::do_stop_front_, this, nullptr);
}

int Pipeline::user_confirm(int node_id) {
  return gsm_->do_trans(state_, &Pipeline::do_user_confirm_front_, this,
                        (FUN_PARAM)((int64_t)node_id));
}

int Pipeline::on_run_ok(FUN_PARAM node) {
  // SEND_STATUS
  return gsm_->do_trans(state_, &Pipeline::on_run_ok_front_, this, node);
}

int Pipeline::on_run_ok_front_(FUN_PARAM node) {
  UNUSE(node);
  // async run
  // iNode *n = (iNode *)node;
  // SEND_STATUS
  return 0;
}

int Pipeline::on_run_ok_back_(FUN_PARAM node) {
  assert(node != nullptr);
  int id = nodemaps_->inodeid_2_ignodeid[((iNode *)node)->get_id()];

#ifdef _DEBUG_
  cout << "Pipeline on_run_after_ok_ state" << id << ", "
       << ((iNode *)node)->get_state() << "\n";
#endif //_DEBUG_

  ENTER_MULTEX
// remove from nodeset_.run_set_
#ifdef _USE_VECTOR_AS_SET_
  vec_erase(nodeset_.run_set_, id);
  if (vec_erase(nodeset_.issues_, id)) {
     if (nodeset_.issues_.empty())
       state_ = ST_running;
  }

#else  //_USE_VECTOR_AS_SET_
  nodeset_.run_set_.erase(id);
  nodeset_.done_set_.insert(id);
#endif //_USE_VECTOR_AS_SET_

  // find new node to run
  if (!nodeset_.run_set_.empty())
    find_node_to_run_(id, false);
  EXIT_MULTEX

#ifdef _DEBUG_
  cout << "Pipeline exit on_run_after_ok_ " << id << "\n";
#endif //_DEBUG_
  return 0;
}

// ........ on run error ..........................
int Pipeline::on_run_error(FUN_PARAM node) {
  iNode *n = (iNode *)node;
  cout << "Pipeline on_run_error inodeid is " << n->get_id() << ", ig node id"
       << nodemaps_->inodeid_2_ignodeid[n->get_id()] << " \n";
  return gsm_->do_trans(state_, &Pipeline::on_run_error_front_, this, n);
}

int Pipeline::on_run_error_front_(FUN_PARAM node) {
  UNUSE(node);
  state_ = ST_error;
  SEND_STATUS
  return 0;
}

int Pipeline::on_run_error_back_(FUN_PARAM node) {
    iNode* n = (iNode*)node;
    assert(n != nullptr);
    ENTER_MULTEX
    vec_insert(nodeset_.issues_, nodemaps_->inodeid_2_ignodeid[n->get_id()]);
    EXIT_MULTEX
    return 0;
}
// ........ on run error ..........................

// ........ on run timeout ..........................
int Pipeline::on_run_timeout(FUN_PARAM node) {
  iNode *n = (iNode *)node;
  cout << "on_run_timeout\n";
  return gsm_->do_trans(state_, &Pipeline::on_run_timeout_front_, this, n);
}
int Pipeline::on_run_timeout_front_(FUN_PARAM node) {
    UNUSE(node);
  state_ = ST_timeout;
  SEND_STATUS
  return 0;
}
int Pipeline::on_run_timeout_back_(FUN_PARAM node) {
  iNode* n = (iNode*)node;
  assert(n != nullptr);
  ENTER_MULTEX
  vec_insert(nodeset_.issues_, nodemaps_->inodeid_2_ignodeid[n->get_id()]);
  EXIT_MULTEX
  return 0;
}
// ........ on run timeout ..........................

int Pipeline::on_run_allok_(FUN_PARAM) {
  // SEND_STATUS
  return gsm_->do_trans(state_, &Pipeline::on_run_allok_front_, this, nullptr);
}

int Pipeline::on_run_allok_front_(FUN_PARAM) {
  state_ = ST_succeed;
  SEND_STATUS
  return 0;
}

int Pipeline::on_run_allok_back_(FUN_PARAM) { return 0; }

int Pipeline::on_run_one_error_(FUN_PARAM node) {
  return gsm_->do_trans(state_, &Pipeline::on_run_one_error_front_, this, node);
}

int Pipeline::on_run_one_error_front_(FUN_PARAM node) {
  return gsm_->do_trans(state_, &Pipeline::on_run_one_error_back_, this, node);
}
int Pipeline::on_run_one_error_back_(FUN_PARAM node) { UNUSE(node); return 0; }

int Pipeline::on_run_one_timeout_(FUN_PARAM node) {
  return gsm_->do_trans(state_, &Pipeline::on_run_one_timeout_front_, this,
                        node);
}

int Pipeline::on_run_one_timeout_front_(FUN_PARAM node) { UNUSE(node); return 0; }
int Pipeline::on_run_one_timeout_back_(FUN_PARAM node) { UNUSE(node); return 0; }

int Pipeline::on_run_one_ok_(FUN_PARAM node) {
  return gsm_->do_trans(state_, &Pipeline::on_run_one_ok_front_, this, node);
}

int Pipeline::on_run_one_ok_front_(FUN_PARAM node) { UNUSE(node); return 0; }
int Pipeline::on_run_one_ok_back_(FUN_PARAM node) { UNUSE(node); return 0; }

int Pipeline::on_paused_(FUN_PARAM) {
  return gsm_->do_trans(state_, &Pipeline::on_paused_front_, this, nullptr);
}

int Pipeline::on_paused_front_(FUN_PARAM) {
  // check if all node are paused
  state_ = ST_paused;
  SEND_STATUS
  return 0;
}
int Pipeline::on_paused_back_(FUN_PARAM) { return 0; }

int Pipeline::on_stoped_(FUN_PARAM) {
  return gsm_->do_trans(state_, &Pipeline::on_stoped_front_, this, nullptr);
}

int Pipeline::on_stoped_front_(FUN_PARAM) {
  // check if all node are stoped
  state_ = ST_stoped;
  SEND_STATUS
  return 0;
}
int Pipeline::on_stoped_back_(FUN_PARAM) { return 0; }

int Pipeline::on_waitin_confirm_(FUN_PARAM node) {
  return gsm_->do_trans(state_, &Pipeline::on_waitin_confirm_front_, this,
                        node);
}

int Pipeline::on_waitin_confirm_front_(FUN_PARAM node) {
  UNUSE(node);
  state_ = ST_waiting_for_confirm;
  SEND_STATUS
  return 0;
}

int Pipeline::on_waitin_confirm_back_(FUN_PARAM node) { UNUSE(node); return 0; }

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
  gsm_->add_state_trans(ST_initial, &Pipeline::do_run_front_, ST_checking,
                        &Pipeline::do_run_back_);
  gsm_->add_state_trans(ST_checked_ok, &Pipeline::do_run_front_, ST_running,
                        &Pipeline::do_run_back_);

  // internal action for run
  gsm_->add_state_trans(ST_running, &Pipeline::on_run_ok_front_, ST_running,
                        &Pipeline::on_run_ok_back_);
  gsm_->add_state_trans(ST_pausing, &Pipeline::on_run_ok_front_, ST_running,
                        &Pipeline::on_run_ok_back_);
  gsm_->add_state_trans(ST_paused, &Pipeline::on_run_ok_front_, ST_running,
                        &Pipeline::on_run_ok_back_);
  gsm_->add_state_trans(ST_timeout, &Pipeline::on_run_ok_front_, ST_running,
                        &Pipeline::on_run_ok_back_);
  gsm_->add_state_trans(ST_error, &Pipeline::on_run_ok_front_, ST_running,
                        &Pipeline::on_run_ok_back_);
  gsm_->add_state_trans(ST_waiting_for_confirm, &Pipeline::on_run_ok_front_, ST_running,
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
  SEND_STATUS
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
#ifdef _DEBUG_
  std::cout << "checking " << vid << " -> " << nodemaps_->ignodeid_2_inodeid[vid] << std::endl;
#endif //_DEBUG_
  iNode *node = g->get_node_by_id(g->nodemaps_->ignodeid_2_inodeid[vid]);
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

  tchk_ = std::thread{&Pipeline::chk_chek_chk__, this};
  tchk_.detach();

  return 0;
}

// Run Action
int Pipeline::do_run_front_(FUN_PARAM node_id) {
  UNUSE(node_id);
  // first thing first, do check~~
  int ret = 0;
  //
  // gsm_->do_trans(state_, &Pipeline::do_check_front_, this, (void *)node_id);
  if (!ret) {
    int start_node = PTR2INT(node_id);
    if (start_node < 0 || start_node >= ig_.n)
      ret = ERROR_INVAILD_NODE_ID;
  }

  if (ST_checked_ok != chk_state_)
     ret = ERROR_WRONG_STATE_TO_ACTION;

  if (ret) {
    state_ = ST_checked_err;
    SEND_STATUS
    return ret;
  }

  if (!ret) {
    state_ = ST_running;
    SEND_STATUS
    // start event thread
  }

  if (ret)
    state_ = ST_error;
  return ret;
}

#define INTVEC(y, i) ((int)(((y).stor_begin)[(i)]))

void Pipeline::find_node_to_run_(int ig_node_id, bool igno_error) {
  // get all child node of node_id put into nodeset_.prepare_to_run_
  igraph_vector_t children, fathers;
  igraph_vector_init(&children, 0);
  igraph_neighbors(&ig_, &children, ig_node_id, IGRAPH_OUT);

#ifdef _DEBUG_
  std::cout << ig_node_id << " Child: "
            << get_node_by_id(nodemaps_->ignodeid_2_inodeid[ig_node_id])->get_state()
            << std::endl;
  for (int i = 0; i < igraph_vector_size(&children); ++i)
    std::cout << INTVEC(children, i) << ", ";
  std::cout << std::endl;

  for (int i = 0; i < igraph_vector_size(&children); ++i) {
    std::cout << INTVEC(children, i) << " Father: " << std::endl;
    igraph_vector_init(&fathers, 0);
    igraph_neighbors(&ig_, &fathers, INTVEC(children, i), IGRAPH_IN);
    for (int j = 0; j < igraph_vector_size(&fathers); ++j)
      std::cout << INTVEC(fathers, j) << "->"
                << get_node_by_id(nodemaps_->ignodeid_2_inodeid[INTVEC(fathers, j)])
                       ->get_state() << ", ";
    std::cout << std::endl;
    igraph_vector_destroy(&fathers);
  }
#endif //_DEBUG_

  // add this child to prepare vector
  for (int i = 0; i < igraph_vector_size(&children); ++i) {
    bool all_father_done = true;
    igraph_vector_init(&fathers, 0);
    if (ig_node_id > 0) {
      igraph_neighbors(&ig_, &fathers, INTVEC(children, i), IGRAPH_IN);
      for (int j = 0; j < igraph_vector_size(&fathers); ++j) {
        if (igno_error) {
          STATE_TYPE st = get_node_by_id(nodemaps_->ignodeid_2_inodeid[INTVEC(fathers, j)])
                  ->get_state() ;
          if (st != ST_succeed && st != ST_error && st != ST_timeout) {
            all_father_done = false;
            break;
          }
        } else {
          if (get_node_by_id(nodemaps_->ignodeid_2_inodeid[INTVEC(fathers, j)])
                  ->get_state() != ST_succeed) {
            all_father_done = false;
            break;
          }
        }
      }
    }
    igraph_vector_destroy(&fathers);

    if (all_father_done) {
#ifdef _DEBUG_
      printf("find add %d -> %d\n", ig_node_id, INTVEC(children, i));
#endif //_DEBUG_

#ifdef _USE_VECTOR_AS_SET_
      vec_insert(nodeset_.prepare_to_run_, INTVEC(children, i));
#else //_USE_VECTOR_AS_SET_
      nodeset_.prepare_to_run_.insert(INTVEC(y, i));
#endif //_USE_VECTOR_AS_SET_
    }
  }

  igraph_vector_destroy(&children);
}

int Pipeline::do_run_back_(FUN_PARAM node_id) {
  assert(ig_.n > 0);

  int ret = 0;
  if (!ret) {
    ENTER_MULTEX
    nodeset_.prepare_to_run_.clear();
    // nodeset_.running_set_.clear();
    nodeset_.run_set_.clear();
    // nodeset_.done_set_.clear();
    nodeset_.issues_.clear();

#ifdef _USE_VECTOR_AS_SET_
    vec_insert(nodeset_.prepare_to_run_, (int)(int64_t)node_id);
#else  //_USE_VECTOR_AS_SET_
    nodeset_.prepare_to_run_.insert((int)(int64_t)node_id);
#endif //_USE_VECTOR_AS_SET_

    igraph_vector_t order;
    igraph_vector_init(&order, 0);
    igraph_bfs(&ig_, (int)(int64_t)node_id, nullptr, IGRAPH_OUT, false, nullptr,
               &order, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
               nullptr);

    // get all nodes that needs to run
    for (int j = 0; j < igraph_vector_size(&order); ++j) {
      if (INTVEC(order, j) >= 0) {
#ifdef _USE_VECTOR_AS_SET_
        vec_insert(nodeset_.run_set_, INTVEC(order, j));
#else  //_USE_VECTOR_AS_SET_
        nodeset_.run_set_.insert(INTVEC(order, j));
#endif //_USE_VECTOR_AS_SET_
      }
    }

    igraph_vector_destroy(&order);
    EXIT_MULTEX

    if (true) {
      // do simulator
      // cout << "do simulator state is " << state_ <<endl;
      // tsim_ = std::thread{&Pipeline::thread_simulator_, this};
      // tsim_ = std::thread{thread_test, bm_};
      // tsim_ = std::thread{ &Pipeline::thread_simulator_ex_, this };
      // tsim_.detach();
      // tsim_.join();
      return thread_simulator_ex_ex_();
    }
  }

// SEND_STATUS
#ifdef _DEBUG_
  printf("exit do_run_back\n");
#endif //_DEBUG_
  return 0;
}

void Pipeline::do_run_node_http_request_(iNode *node) {
  ENTER_MULTEX
#ifdef _USE_VECTOR_AS_SET_
  // vec_insert(nodeset_.running_set_, node->get_id());
#else  //_USE_VECTOR_AS_SET_
  nodeset_.running_set_.insert(node->get_id());
#endif //_USE_VECTOR_AS_SET_
  EXIT_MULTEX

  // real run;
  if (node->run()) {
    // error or timeout
    state_ = node->get_state();
    if (state_ == ST_error)
      gsm_->do_trans(state_, &Pipeline::on_run_error, this, node);
    else if (state_ == ST_timeout)
      gsm_->do_trans(state_, &Pipeline::on_run_timeout, this, node);
  } else {
    // async call

    ENTER_MULTEX

#ifdef _USE_VECTOR_AS_SET_
    // remove from running set
    // vec_erase(nodeset_.running_set_, node->get_id());
    // remove from run set
    vec_erase(nodeset_.run_set_, node->get_id());
#else  //_USE_VECTOR_AS_SET_
    nodeset_.running_set_.erase(node->get_id());
    nodeset_.run_set_.erase(node->get_id());
#endif //_USE_VECTOR_AS_SET_

    // all done.
    // if (nodeset_.run_set_.empty())
    if (nodeset_.run_set_.empty())
      gsm_->do_trans(state_, &Pipeline::on_run_allok_, this, node);
    EXIT_MULTEX
  }
}

int Pipeline::thread_simulator_(Pipeline *pl) {
#ifdef _DEBUG_
  cout << "Pipeline thread simulator pl->state_ is " << pl->state_ << endl;
#endif //_DEBUG_

  iNode *node = nullptr;
  while (pl->state_ == ST_running) {
    node = nullptr;
    ENTER_MULTEX
    if (!pl->nodeset_.prepare_to_run_.empty()) {
      node = pl->get_node_by_id(*(pl->nodeset_.prepare_to_run_.begin()));
      cout << node->get_id() << " <--- node id " << "\n";

      pl->nodeset_.prepare_to_run_.erase(pl->nodeset_.prepare_to_run_.begin());
    }
    EXIT_MULTEX
    if (node != nullptr) {
      STATE_TYPE state = (STATE_TYPE)(node->run());
      switch (state) {
      case ST_succeed:
        pl->on_run_ok(node);
        if (pl->is_all_done_())
          pl->on_run_allok_(nullptr);
        break;
      case ST_timeout:
        pl->on_run_timeout(node);
        break;
      case ST_error:
      default:
        pl->on_run_error(node);
        break;
      }
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }

  // if (pl->state_ == ST_stoping)
  //   pl->on_stoped_(nullptr);

#ifdef _DEBUG_
  cout << "thread simultor exit, h\n";
#endif //_DEBUG_

  return 0;
}

bool Pipeline::is_all_done_() {
    ENTER_MULTEX
    bool is_done = nodeset_.run_set_.empty();
    EXIT_MULTEX
    return is_done;
}

// run one node
int Pipeline::do_run_one_front_(FUN_PARAM node) {
  assert(node != nullptr);
  iNode* n = (iNode*)node;
  int ret = 0;
  ENTER_MULTEX
  if (!vec_find(nodeset_.issues_, nodemaps_->inodeid_2_ignodeid[n->get_id()]))
    ret = 1;
  EXIT_MULTEX
  return (ret) ? ERROR_WRONG_STATE_TO_ACTION : 0;
}

int Pipeline::do_run_one_back_(FUN_PARAM node) {
  assert(node != nullptr);
  STATE_TYPE state = (STATE_TYPE)(((iNode *)node)->run());
  bool status_changed = false;
  int ret = 0;
  if (state == ST_succeed) {
      ENTER_MULTEX
      iNode* n = (iNode*)node;
      if (!vec_erase(nodeset_.issues_, nodemaps_->inodeid_2_ignodeid[n->get_id()]))
        ret = 1;
      if (nodeset_.issues_.empty())
          status_changed = true;
      EXIT_MULTEX
  }
  if (status_changed) {
     state_ = ST_paused;
     SEND_STATUS
  }

  return (ret) ? ERROR_WRONG_STATE_TO_ACTION : 0;
}

// pause
int Pipeline::do_pause_front_(FUN_PARAM) {
#ifdef _DEBUG_
  std::cout << "Pipeline do_pause_front_()\n";
#endif //_DEBUG_
  state_ = ST_pausing;
  SEND_STATUS
#ifdef _DEBUG_
  std::cout << "Pipeline do_pause_front_() return\n";
#endif //_DEBUG_
  return 0;
}

int Pipeline::do_pause_back_(FUN_PARAM) {
#ifdef _DEBUG_
  std::cout << "Pipeline do_pause_back_()\n";
#endif //_DEBUG_
    return 0;
}

// go_on
int Pipeline::do_go_on_front_(FUN_PARAM) {
    ENTER_MULTEX
      for (auto& igid : nodeset_.issues_) {
        vec_erase(nodeset_.run_set_, igid);

        // find new node to run
        if (!nodeset_.run_set_.empty())
          find_node_to_run_(igid, true);
      }
    EXIT_MULTEX
    return 0;
}
int Pipeline::do_go_on_back_(FUN_PARAM) {
    state_ = ST_running;
    SEND_STATUS
    return 0;
}

// stop
int Pipeline::do_stop_front_(FUN_PARAM) {
  state_ = ST_stoping;
  SEND_STATUS
  return 0;
}
int Pipeline::do_stop_back_(FUN_PARAM) { return 0; }

// user confirm
int Pipeline::do_user_confirm_front_(FUN_PARAM node_id) {
  UNUSE(node_id);
  return 0;
}

int Pipeline::do_user_confirm_back_(FUN_PARAM node_id) {
    iNode* n = get_node_by_id(nodemaps_->mysql_node_map[(int)((uint64_t)node_id)]);

    // std::cout << "n is null ?" << (n == nullptr) << std::endl;

    if (!n) return ERROR_INVAILD_NODE_ID;


    n->user_confirm();

    on_run_ok(n);


    state_ = ST_running;
    SEND_STATUS
    return 0;
}

void Pipeline::test_setup(SIMULATE_RESULT_TYPE check, SIMULATE_RESULT_TYPE run,
                          int check_err_id, int run_err_id, int timeout_id,
                          int pause_id, int stop_id, int confirm_id,
                          int sleep_interval) {
  SafeDeletePtr(test_param_);
  test_param_ = new TEST_PARAM{ check,      run,        check_err_id,
                                run_err_id, timeout_id, pause_id,
                                stop_id,    confirm_id, sleep_interval };
}


int Pipeline::thread_simulator_ex_ex_() {
#ifdef _DEBUG_
  std::cout << "thread_simulator_ex_\n";
#endif //_DEBUG_
  assert(state_ == ST_running);
  assert(chk_state_ = ST_checked_ok);
  // SEND_STATUS

  // dj_.save_thread();
  iNode *node = nullptr;
  while (true) {
    if (state_ == ST_running) {
      // simulate run a node
      node = nullptr;
      ENTER_MULTEX
      // get all nodes will have to run
      if (!nodeset_.prepare_to_run_.empty()) {
        int ig_node_id = *(nodeset_.prepare_to_run_.begin());
        node = get_node_by_id(nodemaps_->ignodeid_2_inodeid[ig_node_id]);
        nodeset_.prepare_to_run_.erase(
            nodeset_.prepare_to_run_.begin());
      }
      EXIT_MULTEX
      if (node != nullptr) {
        // run node
        STATE_TYPE node_state = (STATE_TYPE)(node->run());
        // check the node state
        switch (node_state) {
        case ST_running:
          break;
        case ST_succeed:
          on_run_ok(node);
          if (is_all_done_()) {
            if (nodeset_.issues_.empty())
              on_run_allok_(nullptr);
            else {
              on_run_with_error();
              goto error_exit;
            }
          }
          break;
        case ST_timeout:
          on_run_timeout(node);
          break;
        case ST_waiting_for_confirm:
          on_waitin_confirm_(node);
          break;
        case ST_error:
          on_run_error(node);
          break;
        default:
// error
#ifdef _DEBUG_
          cout << "UNKONW status in pipeline is " << node_state << std::endl;
#endif //_DEBUG_
          break;
        }
      }
    } else if (state_ == ST_stoping) {
      on_stoped_(nullptr);
    } else if (state_ == ST_pausing) {
      on_paused_(nullptr);
    } else if (state_ == ST_succeed || state_ == ST_stoped) {
      break;
    }
    if (nodeset_.prepare_to_run_.empty()
        || state_ == ST_pausing
        || state_ == ST_paused)
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    else
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

error_exit:
#ifdef _DEBUG_
  cout << "thread ex exited hahaha\n";
#endif //_DEBUG_

  // dj_.restory_thread();
  return 0;
}

 int Pipeline::thread_simulator_ex_(Pipeline *pl) {
#ifdef _DEBUG_
  std::cout << "thread_simulator_ex_\n";
#endif //_DEBUG_
  pl->state_ = ST_running;
  pl->chk_state_ = ST_checked_ok;
  pl->dj_->send_graph_status(pl->pleid_, pl->plid_, NO_NODE, pl->state_, pl->chk_state_);

  // dj_.save_thread();
  iNode *node = nullptr;
  while (true) {
    if (pl->state_ == ST_running) {
      // simulate run a node
      node = nullptr;
      ENTER_MULTEX
      // get all nodes will have to run
      if (!pl->nodeset_.prepare_to_run_.empty()) {
        int ig_node_id = *(pl->nodeset_.prepare_to_run_.begin());
        node = pl->get_node_by_id(pl->nodemaps_->ignodeid_2_inodeid[ig_node_id]);
        pl->nodeset_.prepare_to_run_.erase(
            pl->nodeset_.prepare_to_run_.begin());
      }
      EXIT_MULTEX
      if (node != nullptr) {
        // run node
        STATE_TYPE node_state = (STATE_TYPE)(node->run());
        // check the node state
        switch (node_state) {
        case ST_running:
          break;
        case ST_succeed:
          pl->on_run_ok(node);
          if (pl->is_all_done_())
            pl->on_run_allok_(nullptr);
          break;
        case ST_timeout:
          pl->on_run_timeout(node);
          break;
        case ST_waiting_for_confirm:
          pl->on_waitin_confirm_(node);
          break;
        case ST_error:
          pl->on_run_error(node);
          break;
        default:
// error
#ifdef _DEBUG_
          cout << "UNKONW status in pipeline is " << node_state << std::endl;
#endif //_DEBUG_
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
    if (pl->nodeset_.prepare_to_run_.empty()
        || pl->state_ == ST_pausing
        || pl->state_ == ST_paused)
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    else
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
#ifdef _DEBUG_
  cout << "thread ex exited hahaha\n";
#endif //_DEBUG_

  // dj_.restory_thread();
  return 0;
}

int Pipeline::get_start_node_id() {
  for (int i = 0; i < (int)node_.size(); i++) {
    if (node_[i]->get_mario_node()->ref_type == 4) // start
      return i;
  }
  return -1;
}

int Pipeline::run_run_run__(Pipeline* pl, int node_id) {
  pl->do_check_front_(nullptr);
  pl->real_do_check__();
  if (pl->chk_state_ != ST_checked_ok) {
      pl->state_ = ST_error;
      pl->dj_->send_graph_status(pl->pleid_, pl->plid_, NO_NODE, pl->state_, pl->chk_state_);
      return ST_error;
  }
  pl->do_run_front_((void *)((uint64_t)node_id));
  if (pl->state_ == ST_running) {
    pl->do_run_back_ ((void *)((uint64_t)node_id));
  }

  return 0;
}



int Pipeline::real_do_check__() {
  igraph_vector_t order;
  igraph_vector_init(&order, igraph_vcount(&ig_));
  igraph_bfs(&ig_, 0, nullptr, IGRAPH_OUT, true, nullptr, &order, nullptr,
             nullptr, nullptr, nullptr, nullptr, &Pipeline::do_node_check_cb_,
             this);
  igraph_vector_destroy(&order);

  if (chk_state_ == ST_checking)
    chk_state_ = ST_checked_ok;

  SEND_STATUS


  return 0;
}

int Pipeline::chk_chek_chk__(Pipeline* pl) {
  pl->real_do_check__();
  return 0;
}

int Pipeline::on_run_with_error() {
    state_ = ST_done_but_error;
    SEND_STATUS;
    return 0;
}

} // namespace itat
