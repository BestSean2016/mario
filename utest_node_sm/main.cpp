#include <iostream>
#include <gtest/gtest.h>
#include <iostream>

#include "node.hpp"
#include "pipeline.hpp"
#include "mario.hpp"
#include "state.hpp"
#include "djangoapi.hpp"

using namespace itat;
using itat::Pipeline;
using itat::iNode;

using namespace std;

int main(int argc, char **argv) {
#ifdef _WINDOWS
#ifdef _DEBUG_
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
  _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
  _CrtDumpMemoryLeaks();
#endif //_DEBUG_
#endif //_WINDOWS

  testing::InitGoogleTest(&argc, argv);
  Py_Initialize();

  dj_.init("bill_message");
  int ret = RUN_ALL_TESTS();

  Py_Finalize();
  return ret;
}

TEST(state_machine, empty_graph) {
  auto pgraph = new itat::Pipeline(0);
  auto pnode = new itat::iNode(pgraph);

  delete pnode;
  delete pgraph;
}

TEST(state_machine, diamond_graph) {
  auto graph = new itat::Pipeline(0);
  graph->test_setup(12, 2);

  ASSERT_EQ(itat::ST_initial, graph->get_node_by_id(0)->get_state());

  delete graph;
}

TEST(plumber, init) {
  auto m = new itat::Mario(123);
  m->initial(0, "bill_message");
  m->test_setup(1000, 3);
  delete m;
}

class Machine {
public:
  Machine() {}
  int do_check_(FUN_PARAM) { return 0; }
  int do_checking_(FUN_PARAM) { return 0; }
  int do_run_(FUN_PARAM) { return 0; }
  int do_running_(FUN_PARAM) { return 0; }
};

typedef int (Machine::*machine_action)(FUN_PARAM);

#define M_DOCHECK &Machine::do_check_
#define M_DOCHECKING &Machine::do_checking_
#define M_DORUN &Machine::do_run_
#define M_DORUNNING &Machine::do_running_

TEST(mario_state, state_test) {
  auto gsm_ = new SateMachine<machine_action, Machine>();
  // check
  gsm_->add_state_trans(ST_initial, M_DOCHECK, ST_checking, M_DOCHECKING);
  gsm_->add_state_trans(ST_checked_err, M_DOCHECK, ST_checking, M_DOCHECKING);
  gsm_->add_state_trans(ST_checked_ok, M_DOCHECK, ST_checking, M_DOCHECKING);
  gsm_->add_state_trans(ST_error, M_DOCHECK, ST_checking, M_DOCHECKING);
  gsm_->add_state_trans(ST_timeout, M_DOCHECK, ST_checking, M_DOCHECKING);
  gsm_->add_state_trans(ST_succeed, M_DOCHECK, ST_checking, M_DOCHECKING);
  gsm_->add_state_trans(ST_waiting_for_confirm, M_DOCHECK, ST_checking,
                        M_DOCHECKING);
  gsm_->add_state_trans(ST_stoped, M_DOCHECK, ST_checking, M_DOCHECKING);
  gsm_->add_state_trans(ST_paused, M_DOCHECK, ST_checking, M_DOCHECKING);

  // run
  gsm_->add_state_trans(ST_initial, M_DORUN, ST_running, M_DORUNNING);
  gsm_->add_state_trans(ST_checked_ok, M_DORUN, ST_running, M_DORUNNING);

  ASSERT_EQ(true, gsm_->find(ST_initial, M_DOCHECK));
  ASSERT_EQ(true, gsm_->find(ST_checked_err, M_DOCHECK));
  ASSERT_EQ(true, gsm_->find(ST_checked_ok, M_DOCHECK));
  ASSERT_EQ(true, gsm_->find(ST_error, M_DOCHECK));
  ASSERT_EQ(true, gsm_->find(ST_timeout, M_DOCHECK));
  ASSERT_EQ(true, gsm_->find(ST_succeed, M_DOCHECK));
  ASSERT_EQ(true, gsm_->find(ST_waiting_for_confirm, M_DOCHECK));
  ASSERT_EQ(true, gsm_->find(ST_stoped, M_DOCHECK));
  ASSERT_EQ(true, gsm_->find(ST_paused, M_DOCHECK));
  ASSERT_EQ(true, gsm_->find(ST_initial, M_DORUN));
  ASSERT_EQ(true, gsm_->find(ST_checked_ok, M_DORUN));

  delete gsm_;
}

TEST(mario_state, state_machine) {
  auto graph = new itat::Pipeline(0);
  graph->test_setup(20, 2);

  iGraphStateTrans st(ST_initial, &Pipeline::test_1, ST_checking,
                      &Pipeline::test_2);
  iNode *node = new iNode(graph);
  ASSERT_EQ(2, st.do_back_action(graph, node));
  ASSERT_EQ(1, st.do_front_action(graph, node));

  iGraphStateMachine gsm;
  gsm.add_state_trans(ST_initial, &Pipeline::test_1, ST_checking,
                      &Pipeline::test_2);
  ASSERT_EQ(1, gsm.do_trans(ST_initial, &Pipeline::test_1, graph, node));
  ASSERT_EQ(-10000,
            gsm.do_trans(ST_checked_ok, &Pipeline::test_1, graph, node));

  delete node;
  delete graph;
}

TEST(mario_state, graph_check_ok) {
  auto graph = new itat::Pipeline(0);
  int node_num = 20;

  graph->initial(false);
  graph->test_setup(node_num, 2);

  ASSERT_EQ(itat::ST_initial, graph->get_state());
  for (int i = 0; i < node_num; ++i)
    ASSERT_EQ(itat::ST_initial, graph->get_node_by_id(i)->get_state());

  graph->check();
  ASSERT_EQ(itat::ST_initial, graph->get_state());
  ASSERT_EQ(itat::ST_checked_ok, graph->get_chk_state());
  for (int i = 0; i < node_num; ++i) {
    ASSERT_EQ(itat::ST_checked_ok, graph->get_node_by_id(i)->get_state());
  }

  delete graph;
}

TEST(mario_state, graph_check_err) {
  auto graph = new itat::Pipeline(0);
  int node_num = 20;

  graph->initial(false);
  graph->test_setup(node_num, 2, SIMULATE_RESULT_TYPE_ERR);

  ASSERT_EQ(itat::ST_initial, graph->get_state());
  for (int i = 0; i < node_num; ++i)
    ASSERT_EQ(itat::ST_initial, graph->get_node_by_id(i)->get_state());

  graph->check();
  ASSERT_EQ(itat::ST_initial, graph->get_state());
  ASSERT_EQ(itat::ST_checked_err, graph->get_chk_state());
  for (int i = 1; i < node_num - 1; ++i) {
    ASSERT_EQ(
        true,
        (itat::ST_checked_herr == graph->get_node_by_id(i)->get_state()) ||
            (itat::ST_checked_serr == graph->get_node_by_id(i)->get_state()));
  }

  delete graph;
}

TEST(mario_state, graph_check_rnd) {
  auto graph = new itat::Pipeline(0);
  int node_num = 20;

  graph->initial(false);
  graph->test_setup(node_num, 2, SIMULATE_RESULT_TYPE_RONDOM);

  ASSERT_EQ(itat::ST_initial, graph->get_state());
  for (int i = 0; i < node_num; ++i)
    ASSERT_EQ(itat::ST_initial, graph->get_node_by_id(i)->get_state());

  graph->check();
  ASSERT_EQ(itat::ST_initial, graph->get_state());
  ASSERT_EQ(itat::ST_checked_err, graph->get_chk_state());
  // We do not konw what was happed
  // for (int i = 1; i < node_num; ++i) {
  //   ASSERT_EQ(itat::ST_checked_err, graph->get_node_by_id(i)->get_state());
  // }

  delete graph;
}

TEST(mario_state, graph_do_run_rnd) {
  auto graph = new itat::Pipeline(0);
  int node_num = 10;

  graph->initial(false);
  graph->test_setup(node_num, 2, SIMULATE_RESULT_TYPE_RONDOM,
                    SIMULATE_RESULT_TYPE_RONDOM);

  ASSERT_EQ(ERROR_WRONG_STATE_TO_ACTION, graph->run(0));

  delete graph;
}

void run_mario(itat::Mario *mario) {
  int node_num = 10;
  mario->initial(0, "bill_message");
  mario->test_setup(node_num, 2);

  ASSERT_EQ(0, mario->check());

  ASSERT_EQ(0, mario->run(0));
}

TEST(mario_state, mario_stop) {
  auto mario = new itat::Mario(0);

  std::thread t(run_mario, mario);
  std::this_thread::sleep_for(std::chrono::seconds(50));

  mario->stop();

  t.join();
  delete mario;
}

TEST(mario_state, graph_do_run_ok) {
  auto graph = new itat::Pipeline(0);
  int node_num = 20;

  graph->initial(false);
  graph->test_setup();

  ASSERT_EQ(0, graph->check());
  ASSERT_EQ(itat::ST_initial, graph->get_state());
  ASSERT_EQ(itat::ST_checked_ok, graph->get_chk_state());

  ASSERT_EQ(0, graph->run(0));

  // successed
  ASSERT_EQ(itat::ST_succeed, graph->get_state());
  ASSERT_EQ(itat::ST_checked_ok, graph->get_chk_state());

  for (int i = 1; i < node_num - 1; ++i) {
    ASSERT_EQ(itat::ST_succeed, graph->get_state());
  }
  delete graph;
}

TEST(Django_python_cpp, mario_bill) {
  const char *strout{ "hahahah" };
  const char *strerr{ "hahahah" };

  itat::dj_.init("bill_message");
  for (int i = 0; i < 5; i++) {

    int ret = itat::dj_.send_graph_status(
        1, 2, 3, (itat::STATE_TYPE)4, (itat::STATE_TYPE)5, 0, strout, strerr);
    ASSERT_EQ(ret, 0);
  }
}
