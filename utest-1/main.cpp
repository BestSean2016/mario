#include <gtest/gtest.h>
#include <iostream>


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

#include "chinese.h"

TEST(convert, utf8_to_ucs2) {
  char hello[100] = "abc 123 大家好！";
  char temp[100];
  memset(temp, 0, 100);
  EXPECT_EQ(0, convert(hello, temp, strlen(hello), 100, "UTF-8", "UCS-2"));

  // dumpbuf("hello", hello, 100);
  // dumpbuf("temp", temp, 100);
  unsigned char t1[] = {0x61, 0x00, 0x62, 0x00, 0x63, 0x00, 0x20, 0x00, 0x31,
                        0x00, 0x32, 0x00, 0x33, 0x00, 0x20, 0x00, 0x27,
                        0x59, 0xB6, 0x5B, 0x7D, 0x59, 0x01, 0xFF, 0x00, 0x00};
  EXPECT_EQ(memcmp(temp, t1, sizeof(t1)), 0);
}

TEST(convert, utf8_to_ucs4) {
  char hello[100] = "abc 123 大家好！";
  char temp[200];
  memset(temp, 0, 200);
  EXPECT_EQ(0, convert(hello, temp, strlen(hello), 200, "UTF-8", "UCS-4"));

  // dumpbuf("hello", hello, 100);
  // dumpbuf("temp", temp, 100);

  unsigned char t2[] = {0x00, 0x00, 0x00, 0x61, 0x00, 0x00, 0x00, 0x62, 0x00,
                        0x00, 0x00, 0x63, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00,
                        0x00, 0x31, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00,
                        0x33, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x59, 0x27,
                        0x00, 0x00, 0x5B, 0xB6, 0x00, 0x00, 0x59, 0x7D, 0x00,
                        0x00, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00};

  // printf("sizeof t2 %lu\n", sizeof(t2));
  EXPECT_EQ(memcmp(temp, t2, sizeof(t2)), 0);

  // for (int k = 1; k <= 100; k++)
  //     for (int j = 1; j <= 100; j++) {
  //         int ret = convert(hello, temp, k, j, "UTF-8", "UCS-4");
  //         if (ret == 0) printf("%d %d %d\n", k, j, ret);
  //     }
}


TEST(convert, utf8_to_webucs2) {
  char hello[100] = "abc 123 大家好！";
  char tmp[200];
  memset(tmp, 0, 200);
  EXPECT_EQ(0, utf8_2_webucs2(hello, tmp, strlen(hello), 200, "&#", ";"));
  EXPECT_EQ(strcmp(tmp, "abc 123 &#22823;&#23478;&#22909;&#65281;"), 0);
}


TEST(convert, gbk_to_webucs2) {
  char hello[100] = "abc 123 大家好！";
  char tmp[200];
  char gbk[200];

  memset(gbk, 0, 200);
  EXPECT_EQ(0, convert(hello, gbk, strlen(hello), 200, "UTF-8", "GBK"));

  memset(tmp, 0, 200);
  EXPECT_EQ(0, gbk_2_webucs2(gbk, tmp, strlen(gbk), 200, "&#", ";"));
  EXPECT_EQ(strcmp(tmp, "abc 123 &#22823;&#23478;&#22909;&#65281;"), 0);
}

TEST(convert, webucs_2_gbk) {
  char hello[100] = "abc 123 大家好！";
  char gbk[200];
  char webucs_2[] = "abc 123 &#22823;&#23478;&#22909;&#65281;";

  char tmp[100];
  memset(tmp, 0, 100);
  EXPECT_EQ(0, convert(hello, tmp, strlen(hello), 100, "UTF-8", "GBK"));

  memset(gbk, 0, 200);
  EXPECT_EQ(0, webucs_2_gbk(webucs_2, gbk, strlen(webucs_2) + 1, 200, "&#", ";"));
  EXPECT_EQ(strcmp(gbk, tmp), 0);
}

TEST(convert, webucs_2_utf8) {
  char hello[100] = "abc 123 大家好！";
  char utf8[200];
  char webucs_2[] = "abc 123 &#22823;&#23478;&#22909;&#65281;";

  memset(utf8, 0, 200);
  EXPECT_EQ(0, webucs_2_utf8(webucs_2, utf8, strlen(webucs_2) + 1, 200, "&#", ";"));
  EXPECT_EQ(strcmp(utf8, hello), 0);
}





TEST(convert, webucs16_2_utf8) {
  char hello[100] = "abc 123 大家好！";
  char utf8[200];
  char webucs_2[] = "abc 123 &#x5927;&#x5bb6;&#x597d;&#xff01;";

  memset(utf8, 0, 200);
  EXPECT_EQ(0, webucs16_2_utf8(webucs_2, utf8, strlen(webucs_2) + 1, 200, "&#x", ";"));
  EXPECT_EQ(strcmp(utf8, hello), 0);
}

TEST(convert, webucs16_2_gbk) {
  char hello[100] = "abc 123 大家好！";
  char gbk[200];
  char webucs_2[] = "abc 123 &#x5927;&#x5bb6;&#x597d;&#xff01;";

  char tmp[100];
  memset(tmp, 0, 100);
  EXPECT_EQ(0, convert(hello, tmp, strlen(hello), 100, "UTF-8", "GBK"));

  memset(gbk, 0, 200);
  EXPECT_EQ(0, webucs16_2_utf8(webucs_2, gbk, strlen(webucs_2) + 1, 200, "&#x", ";"));
  EXPECT_EQ(strcmp(gbk, hello), 0);
}



#include "mario_types.h"
