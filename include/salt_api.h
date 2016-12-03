#ifndef SALT_API_H
#define SALT_API_H

#include <vector>
#include <map>
#include "mario_data.h"

extern int parse_job(const char* json_data, size_t len, void *param1, void *param2);
extern int parse_my_job(const char* json_data, size_t len, void *param1, void *param2);


extern std::ostream& operator << (std::ostream& out, SALT_JOB& jobnew);
extern std::ostream& operator << (std::ostream& out, SALT_JOB_RET& jobret);
extern void show_job(SALT_JOB_DATA* job);

extern void thread_check_timer_out(int *run);
extern void thread_run_pipeline();

extern void jobmap_cleanup(JOBMAP *jm);

#endif // SALT_API_H

