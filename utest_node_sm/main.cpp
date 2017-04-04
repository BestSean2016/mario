#include <iostream>
#include <gtest/gtest.h>
#include <iostream>

#include "state.hpp"
#include "node.hpp"
#include "graph.hpp"
#include "plumber.hpp"

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
  auto pgraph = new itat::igraph();
  auto pnode = new itat::inode(pgraph);

  auto nsm = pnode->get_state_machine();
  nsm->test();

  delete pnode;
  delete pgraph;
}

TEST(state_machine, diamond_graph) {
  auto graph = new itat::igraph();
  graph->diamod_simulator(12, 2);

  graph->get_node_by_id(0)->get_state_machine()->test();

  delete graph;
}


TEST(plumber, init) {
  auto m = new itat::mario(123);
  m->simulator(100,3);
  delete m;
}
