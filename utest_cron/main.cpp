#include <iostream>
#include <gtest/gtest.h>

#include "cronie_common.h"
#include "funcs.h"
#include "globals.h"
#include "pathnames.h"

using namespace std;

int main(int argc, char *argv[])
{
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

extern "C" {
  // extern void find_jobs(int, cron_db *, int, int, long);
  // extern void run_reboot_jobs(cron_db * db);
  // extern int virtualTime;
  // extern long GMToff;
  extern int cron_main_main();
}


TEST(libcron, init) {
    // cron_db database;
    // database.head = NULL;
    // database.tail = NULL;
    // database.mtime = (time_t) 0;
    //
    // load_database(&database);
    //
    // run_reboot_jobs(&database);
    // for (;;) {
    //     check_orphans(&database);
    //     load_database(&database);
    //
    //     find_jobs(virtualTime, &database, TRUE, TRUE, GMToff);
    //
    //     job_runqueue();
    //     //find_jobs(timeRunning, &database, TRUE, FALSE, GMToff);
    //
    //     sleep(60);
    // }
     cron_main_main();
}
