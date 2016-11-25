#include <iostream>
#include "curl_helper.h"
#include "mario_data.h"
#include "salt_api.h"

using namespace std;


int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  curl_salt_event();
  return 0;
}
