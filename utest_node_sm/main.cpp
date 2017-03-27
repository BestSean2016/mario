#include <iostream>
#include <gtest/gtest.h>
#include <iostream>

#include "state.hpp"

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

TEST(state_machine, node) {
  itat::dfnode_state_mechine nsm;
  nsm.test();
}
