#ifndef SALT_API_HPP
#define SALT_API_HPP

#include "itat_global.h"
#include "itat.h"

#include "httpapi.hpp"
#include "state.hpp"
#include "saltman.hpp"

namespace itat {



#define ALL_TASK_FINISHED -10000000


typedef enum SALT_API_TYPE {
  SALT_API_TYPE_LOGIN,
  SALT_API_TYPE_ASYNC_TESTPING,
  SALT_API_TYPE_ASYNC_RUNALL,
  SALT_API_TYPE_RUNALL,
  SALT_API_TYPE_EVENTS,
  SALT_API_TYPE_CP_GETFILE,
  SALT_API_TYPE_TESTPING,
  SALT_API_TYPE_FILE_EXISTS,
} SALT_API_TYPE;


extern ITAT_API void thread_check_timer_out(itat::saltman *sm, MAP_SALT_JOB* jobmap);

extern ITAT_API int salt_api_login(saltman *sm, HTTP_API_PARAM* param, const char* user, const char* pass);
extern ITAT_API int salt_api_testping(saltman *sm, HTTP_API_PARAM* param, const char* target);
extern ITAT_API int salt_api_events(saltman *sm, HTTP_API_PARAM *param);
extern ITAT_API int salt_api_async_cmd_runall(saltman *sm, HTTP_API_PARAM* param, const char* target, const char* script);
extern ITAT_API int salt_api_cmd_runall(saltman *sm, HTTP_API_PARAM* param, const char* target, const char* script);
extern ITAT_API int salt_api_cp_getfile(saltman *sm, HTTP_API_PARAM* param, const char* target, const char* src_file, const char* des_file);
extern ITAT_API int salt_api_file_exists(saltman *sm, HTTP_API_PARAM* param, const char* target, const char* script);

extern ITAT_API int parse_token_fn(saltman *sm, const char *data, size_t len, void* param1, void* param2);

//param1 is MAP_SALT_JOB type
extern ITAT_API int parse_job(saltman *sm, const char *json, size_t len, void* param1, void* param2);
extern ITAT_API int parse_salt_runall_ret(saltman *sm, const char *json, size_t len, void* param1, void* param2);
extern ITAT_API int parse_salt_testping_ret(saltman *sm, const char *json_data, size_t len, void* param1, void* param2);
extern ITAT_API int parse_salt_myjob_jobmap(saltman *sm, const char *json_data, size_t len,
                                            PARAM p1, PARAM p2);

const int64_t testping_pid = -1;
const int64_t testping_nid = -1;


} //namespace itat


extern ITAT_API std::ostream& operator<< (std::ostream& out, itat::SALT_JOB_RET& ret);
extern ITAT_API std::ostream& operator<< (std::ostream& out, itat::SALT_JOB& job);

#endif // SALT_API_HPP
