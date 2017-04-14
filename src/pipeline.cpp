#include "pipeline.hpp"
#include "djagoapi.hpp"
#include "node.hpp"
#include "saltapi.hpp"

#define USERNAME "salt-test"
#define PASSWORD "hongt@8a51"
#define SALTAPI_SERVER_IP "10.10.10.19"
#define SALTAPI_SERVER_PORT 8000

std::mutex g_run_mutex;
#define ENTER_MULTEX  auto guard = new std::lock_guard<std::mutex>(g_run_mutex);
#define EXIT_MULTEX delete guard;



bool vec_find(std::vector<int>& vec, int a) {
    for(auto&p : vec)
        if (p == a) return true;
    return false;
}

void vec_insert(std::vector<int>& vec, int a) {
    if (!vec_find(vec, a))
        vec.emplace_back(a);
}

void vec_erase(std::vector<int>& vec, int a) {
    for(auto it = vec.begin(); it != vec.end(); it++) {
        if (*it == a) {
            vec.erase(it);
            break;
        }

    }
}

namespace itat {

Pipeline::Pipeline(int64_t plid)
    : plid_(plid){
  memset(&ig_, 0, sizeof(igraph_t));
  gsm_ = new iGraphStateMachine();
  nsm_ = new iNodeStateMachine();
}

Pipeline::~Pipeline() {
  if (tsim_.joinable()) {
      state_ = ST_stoped;
      tsim_.join();
  }

  if (tevent_.joinable()) {
      state_ = ST_stoped;
      tevent_.join();
  }

  if (thpool_) {
      threadpool_destroy(thpool_, 0);
      thpool_ = 0;
  }
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
int Pipeline::diamod_simulator(int node_num, int branch_num, SIMULATE_RESULT_TYPE type) {
  simret_type_= type;
  if (simret_type_ > SIMULATE_RESULT_TYPE_UNKNOW) srand(time(0));
  gen_diamod_graph_(node_num, branch_num);
  gen_node_(type);

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

int Pipeline::gen_node_(SIMULATE_RESULT_TYPE type) {
  for (int i = 0; i < ig_.n; ++i) {
    auto node = new iNode(this);
    node->init(i, gsm_, nsm_);
    node->set_simulate(type);
    node_.emplace_back(node);
  }
  return (0);
}

int Pipeline::gen_piple_graph() {
  int ret = load_pipe_line_from_db_();
  if (!ret)
    ret = gen_piple_graph_();
  if (!ret)
    ret = gen_node_(SIMULATE_RESULT_TYPE_UNKNOW);

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

int Pipeline::init(bool real_run) {
  setup_state_machine_();
  if (real_run)
      load_pipe_line_from_db_();

  HTTP_API_PARAM param(SALTAPI_SERVER_IP, SALTAPI_SERVER_PORT, parse_token_fn,
                       nullptr, nullptr);
  return salt_api_login(&param, USERNAME, PASSWORD);
}

int Pipeline::check() {
  return gsm_->do_trans(state_, &Pipeline::do_check_, this, nullptr);
}

int Pipeline::run(int start_id) {
    return gsm_->do_trans(state_, &Pipeline::do_run_, this, (void*)start_id);
}

int Pipeline::run_node(int node_id) { return 0; }

int Pipeline::pause() { return 0; }

int Pipeline::go_on() { return 0; }

int Pipeline::stop() { return 0; }

int Pipeline::redo(int node_id) { return 0; }

int Pipeline::on_run_ok_(FUN_PARAM node) {
  djagno_api_send_graph_status(plid_, NO_NODE, state_, chk_state_);
  return gsm_->do_trans(state_, &Pipeline::on_run_ok_event_, this, node);
}


int Pipeline::on_run_ok_event_(FUN_PARAM node) {
  iNode* n = (iNode*)node;
  djagno_api_send_graph_status(plid_, NO_NODE, state_, chk_state_);
  return 0;
}


int Pipeline::on_run_after_ok_(FUN_PARAM node) {
    int id = ((iNode*)node)->get_id();
    cout << "on_run_after_ok_\n";
    ENTER_MULTEX
    //remove from run_set_
    vec_erase(run_set_, id);

    // find new node to run
    if (!run_set_.empty())
        find_node_to_run_(id, true);
    EXIT_MULTEX
    djagno_api_send_graph_status(plid_, NO_NODE, state_, chk_state_);
    return 0;
}

int Pipeline::on_run_error_(FUN_PARAM node) {
  iNode* n = (iNode*)node;
  cout << "on_run_error\n";
  return 0;
}

int Pipeline::on_run_timeout_(FUN_PARAM node) {
    iNode* n = (iNode*)node;
    cout << "on_run_timeout\n";
    return 0;
}

int Pipeline::on_run_allok_(FUN_PARAM) {
    djagno_api_send_graph_status(plid_, NO_NODE, state_, chk_state_);
    return gsm_->do_trans(state_, &Pipeline::on_run_allok_event_, this, nullptr);
}

int Pipeline::on_run_allok_event_(FUN_PARAM) {
    return 0;
}

int Pipeline::on_run_after_allok_(FUN_PARAM) {
    state_ = ST_successed;
    djagno_api_send_graph_status(plid_, NO_NODE, state_, chk_state_);
    return 0;
}

int Pipeline::on_pause() {
  state_ = ST_paused;
  djagno_api_send_graph_status(plid_, NO_NODE, state_, chk_state_);
  return 0;
}

int Pipeline::on_continue() {
  state_ = ST_running;
  djagno_api_send_graph_status(plid_, NO_NODE, state_, chk_state_);
  return 0;
}

int Pipeline::on_stop() {
  state_ = ST_stoped;
  djagno_api_send_graph_status(plid_, NO_NODE, state_, chk_state_);
  return 0;
}

int Pipeline::on_wait_for_user(int64_t node_id) {
  state_ = ST_waiting_for_input;
  djagno_api_send_graph_status(plid_, NO_NODE, state_, chk_state_);
  return 0;
}

int Pipeline::on_user_confirmed(int64_t node_id) {
  state_ = ST_running;
  djagno_api_send_graph_status(plid_, NO_NODE, state_, chk_state_);
  return 0;
}

int Pipeline::on_wait_ror_run() {
  state_ = ST_waiting_for_run;
  djagno_api_send_graph_status(plid_, NO_NODE, state_, chk_state_);
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

/*
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
*/

int Pipeline::setup_state_machine_() {
  // check
  gsm_->add_state_trans(ST_initial,           DOCHECK, ST_checking, DOCHECKING);
  gsm_->add_state_trans(ST_checked_err,       DOCHECK, ST_checking, DOCHECKING);
  gsm_->add_state_trans(ST_checked_ok,        DOCHECK, ST_checking, DOCHECKING);
  gsm_->add_state_trans(ST_error,             DOCHECK, ST_checking, DOCHECKING);
  gsm_->add_state_trans(ST_timeout,           DOCHECK, ST_checking, DOCHECKING);
  gsm_->add_state_trans(ST_successed,         DOCHECK, ST_checking, DOCHECKING);
  gsm_->add_state_trans(ST_waiting_for_input, DOCHECK, ST_checking, DOCHECKING);
  gsm_->add_state_trans(ST_stoped,            DOCHECK, ST_checking, DOCHECKING);
  gsm_->add_state_trans(ST_paused,            DOCHECK, ST_checking, DOCHECKING);

  // run
  gsm_->add_state_trans(ST_initial,           DORUN,   ST_running,  DORUNNING);
  gsm_->add_state_trans(ST_checked_ok,        DORUN,   ST_running,  DORUNNING);

  // pause
  // gsm_->add_state_trans(ST_running,           DORUN,   ST_running,  DOCHECKING);
  // gsm_->add_state_trans(ST_checked_ok,        DORUN,   ST_running,  DOCHECKING);


  // internal action
  gsm_->add_state_trans(ST_running, &Pipeline::on_run_ok_event_,      ST_running, &Pipeline::on_run_after_ok_);
  gsm_->add_state_trans(ST_running, &Pipeline::on_run_error_,   ST_error, nullptr);
  gsm_->add_state_trans(ST_running, &Pipeline::on_run_timeout_, ST_timeout, nullptr);
  gsm_->add_state_trans(ST_running, &Pipeline::on_run_allok_event_, ST_successed, &Pipeline::on_run_after_allok_);

  return 0;
}


//Check Action
int Pipeline::do_check_(FUN_PARAM) {
  chk_state_ = ST_checking;
  djagno_api_send_graph_status(plid_, NO_NODE, state_, chk_state_);
#ifdef _DEBUG_
  cout << "igraph " << plid_ << ": do_check_" << endl;
#endif //_DEBUG_


  return 0;
}

int Pipeline::do_node_check_cb_(const igraph_t *graph, igraph_integer_t vid,
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
             nullptr, nullptr, nullptr, nullptr, &Pipeline::do_node_check_cb_, this);
  igraph_vector_destroy(&order);

  if (chk_state_ == ST_checking)
    chk_state_ = ST_checked_ok;
  djagno_api_send_graph_status(plid_, NO_NODE, state_, chk_state_);

  return 0;
}


//Run Action
int Pipeline::do_run_(FUN_PARAM node_id) {
    STATE_TYPE tmp_state = state_;
    int ret = gsm_->do_trans(state_, DOCHECK, this, nullptr);
    if (!ret) {
        int start_node = PTR2INT(node_id);
        if (start_node < 0 || start_node >= ig_.n)
            ret = ERROR_INVAILD_NODE_ID;
    }

    if (ST_checked_ok != chk_state_)
        ret = ERROR_WRONG_STATE_TO_ACTION;

    if (ret) {
        state_ = ST_checked_err;
        djagno_api_send_graph_status(plid_, NO_NODE, state_, chk_state_);
        return ret;
    }

    if (!ret) {
        state_ = ST_running;
        if (thpool_) {
            threadpool_destroy(thpool_, 0);
            delete thpool_;
        }

        thpool_ = threadpool_create(5, ig_.n * 3, 0);
        if (!thpool_) ret = ERROR_CANNOT_CREATE_THREAD_POOL;
    }

    djagno_api_send_graph_status(plid_, NO_NODE, state_, chk_state_);

    if (ret) state_ = ST_error;
    return ret;
}

#define INTVEC(y, i) ((int)(((y).stor_begin)[(i)]))

void Pipeline::find_node_to_run_(int node_id, bool after) {
  if (after) {
      //get all child node of node_id put into prepare_to_run_
      igraph_vector_t y;
      igraph_vector_init(&y, 0);
      igraph_neighbors(&ig_, &y, node_id, IGRAPH_OUT);
      for (int i = 0; i < igraph_vector_size(&y); ++i)
        vec_insert(prepare_to_run_, INTVEC(y, i));


      igraph_vector_t order;
      igraph_vector_init(&order, 0);
      igraph_bfs(&ig_, 0, &y, IGRAPH_OUT, false, nullptr, &order,
                 nullptr, nullptr, nullptr, nullptr,  nullptr, nullptr, nullptr);

      for (int j = 0; j < igraph_vector_size(&order); ++j) {
        if (INTVEC(order, j) >= 0) vec_insert(run_set_
                                              , INTVEC(order, j));
      }

      igraph_vector_destroy(&order);

      igraph_vector_destroy(&y);
  } else {
     vec_insert (prepare_to_run_, node_id);
     vec_insert(run_set_, node_id);
  }
}


int Pipeline::do_running_(FUN_PARAM node_id) {
    assert(ig_.n > 0);

    int ret = 0;
    if (!ret){
        ENTER_MULTEX
        prepare_to_run_.clear();
        running_set_.clear();
        run_set_.clear();

        find_node_to_run_(PTR2INT(node_id), (node_id) ? false : true);

        state_ = ST_running;
        if (simret_type_ >= SIMULATE_RESULT_TYPE_OK) {
          //do simulator
          tsim_ = std::thread{&Pipeline::thread_simulator_, this};
        } else {
          //add into threadpool to run
          for(auto& node_id : prepare_to_run_)
              threadpool_add(thpool_, &Pipeline::do_thread_run_node_, get_node_by_id(node_id), 0);
          prepare_to_run_.clear();
        }

        EXIT_MULTEX
    }

    djagno_api_send_graph_status(plid_, NO_NODE, state_, chk_state_);
    return 0;
}

void Pipeline::do_thread_run_node_(FUN_PARAM node_ptr) {
    iNode* node = (iNode*)node_ptr;
    Pipeline* pl = node->get_pipeline();

    ENTER_MULTEX
    vec_insert(pl->running_set_, node->get_id());
    EXIT_MULTEX

    //real run;
    if (node->run()) {
        //error or timeout
        pl->state_ = node->get_state();
        if (pl->state_ == ST_error)
            pl->gsm_->do_trans(pl->state_, &Pipeline::on_run_error_, pl, node);
        else if (pl->state_ == ST_timeout)
            pl->gsm_->do_trans(pl->state_, &Pipeline::on_run_timeout_, pl, node);
    } else {
        //async call
        if (node->get_run_type() == RUN_TYPE_ASYNC) {
        } else if (node->get_run_type() == RUN_TYPE_SYNC) {
          //sync call
          ENTER_MULTEX
          //remove from running set
          vec_erase(pl->running_set_, node->get_id());
          //remove from run set
          vec_erase(pl->run_set_, node->get_id());

          //all done.
          if (pl->run_set_.empty())
              pl->gsm_->do_trans(pl->state_, &Pipeline::on_run_allok_, pl, node);
          EXIT_MULTEX
        }
    }
}

//the running callback function for igraph's visitor
int Pipeline::do_node_run_cb_(const igraph_t *graph, igraph_integer_t vid,
                        igraph_integer_t, igraph_integer_t,
                        igraph_integer_t, igraph_integer_t, void *extra) {
    return 0;
}

int Pipeline::thread_simulator_(Pipeline* pl) {
    iNode* node = nullptr;
    while (pl->state_ == ST_running) {
      node = nullptr;
      ENTER_MULTEX
      if (!pl->prepare_to_run_.empty()) {
        node = pl->get_node_by_id(*(pl->prepare_to_run_.begin()));
        pl->prepare_to_run_.erase(pl->prepare_to_run_.begin());
      }
      EXIT_MULTEX
      if (node != nullptr) {
          STATE_TYPE state = (STATE_TYPE)(node->run());
          switch (state) {
          case ST_successed:
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
      //std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    cout << "hahaha\n";

    return 0;
}

bool Pipeline::is_all_done_() {
    return run_set_.empty();
}

} // namespace itat
