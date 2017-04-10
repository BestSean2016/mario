#include <iostream>
#include <gtest/gtest.h>
#include <iostream>

#include "node.hpp"
#include "pipeline.hpp"
#include "mario.hpp"
#include "state.hpp"


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

  return RUN_ALL_TESTS();
}

TEST(state_machine, empty_graph) {
  auto pgraph = new itat::Pipeline();
  auto pnode = new itat::iNode(pgraph);

  delete pnode;
  delete pgraph;
}

TEST(state_machine, diamond_graph) {
  auto graph = new itat::Pipeline();
  graph->diamod_simulator(12, 2);

  ASSERT_EQ(itat::ST_initial, graph->get_node_by_id(0)->get_state());

  delete graph;
}


TEST(plumber, init) {
  auto m = new itat::Mario(123);
  ASSERT_EQ(0, m->simulator_pipeline(1000,3));
  delete m;
}



TEST(mario_state, graph_check) {
  auto graph = new itat::Pipeline();
  int node_num = 20;
  graph->diamod_simulator(node_num, 2);

  graph->init();

  ASSERT_EQ(itat::ST_initial, graph->get_state());
  ASSERT_EQ(itat::ST_initial, graph->get_node_by_id(0)->get_state());

  graph->check();

  iGraphStateTrans st(ST_initial, &Pipeline::test_1, ST_checking, &Pipeline::test_2);
  iNode* node = new iNode(graph);
  ASSERT_EQ(2, st.do_back_action(graph, node));
  ASSERT_EQ(1, st.do_front_action(graph, node));


  iGraphStateMachine gsm;
  gsm.add_state_trans(ST_initial, &Pipeline::test_1, ST_checking, &Pipeline::test_2);
  ASSERT_EQ(2, gsm.do_trans(ST_initial, &Pipeline::test_1, graph, node));
  ASSERT_EQ(-10000, gsm.do_trans(ST_checked_ok, &Pipeline::test_1, graph, node));

  delete node;
  delete graph;
}

