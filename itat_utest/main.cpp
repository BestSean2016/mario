#include <gtest/gtest.h>
#include <iostream>
#include "itat.h"

using namespace std;

int main(int argc, char **argv) {

#ifdef _WINDOWS
#ifdef _DEBUG_
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
  _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
  _CrtDumpMemoryLeaks();
#endif //_DEBUG_
#endif //_WINDOWS

  // if (argc != 3) {
  //   printf("Usage testing <json config file> <json config file2>\n");
  //   return (0);
  // }

  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}



TEST(itat, login) {
  set_default_callback();
  EXPECT_EQ(0, salt_api_login("10.10.10.19", 8000, "sean", "hongt@8a51"));
}

TEST(itat, testping) {
  set_default_callback();
  EXPECT_EQ(0, salt_api_login("10.10.10.19", 8000, "sean", "hongt@8a51"));
  EXPECT_EQ(0, salt_api_testping("10.10.10.19", 8000, "old*", nullptr, nullptr));
}

