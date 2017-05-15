#ifndef SALT_API_HPP
#define SALT_API_HPP

#include "itat_global.h"
#include "itat.h"

#include "httpapi.hpp"
#include "state.hpp"

namespace itat {


typedef enum RETURN_TYPE {
    RETURN_TYPE_OBJECT,
    RETURN_TYPE_BOOL,
    RETURN_TYPE_STRING,
} RETURN_TYPE;

typedef struct salt_job_ret {
  int64_t ple_id = 0;            ///PIPELINE EXECUTIVE ID
  RETURN_TYPE rettype = RETURN_TYPE_OBJECT;
  std::string tag       = "";
  std::string stamp     = "";
  time_t      stamp_sec = 0;
  uint32_t    stamp_usec= 0;
  uint32_t    pid = 0;
  int         retcode = 0;
  std::string stderr = "";
  std::string stdout = "";
  bool        success = false;
  std::string cmd = "";
  std::string jid = "";
  std::string fun = "";
  std::string minion_id = "";
} SALT_JOB_RET;



typedef enum SALT_JOB_EVENT_TYPE {
    SALT_JOB_TYPE_IGNORE,
    SALT_JOB_TYPE_NEW,
    SALT_JOB_TYPE_RET,
} SALT_JOB_EVENT_TYPE;


typedef struct salt_job {
  int64_t     ple_id      = 0;            ///PIPELINE EXECUTIVE ID
  int64_t     node_id     = -1;
  void*       inode       = 0;
  time_t      stamp_sec   = time(0);
  uint32_t    timerout    = 60;
  size_t      retnum      = 0;
  size_t      success_num = 0;
  STATE_TYPE  status      = ST_running;
  std::string tag        = "";
  std::string tgt_type   = "";
  std::string jid        = "";
  std::string tgt        = "";
  std::string stamp      = "";
  std::string user       = "";
  std::string fun        = "";
  std::vector<std::string> arg;
  std::vector<std::string> minions;

  MapStr2Ptr<SALT_JOB_RET> minion_ret;

  salt_job() {}
  salt_job(int64_t pleid, int64_t nodeid) :
    ple_id  (pleid ),
    node_id (nodeid)
  {}

  ~salt_job() {
    for(auto& p : minion_ret) {
      delete (p.second);
    }
    minion_ret.clear();
  }
} SALT_JOB;


typedef MapStr2Ptr<SALT_JOB> MAP_SALT_JOB;


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


extern ITAT_API void thread_check_timer_out(MAP_SALT_JOB* jobmap);

extern ITAT_API int salt_api_login(HTTP_API_PARAM* param, const char* user, const char* pass);
extern ITAT_API int salt_api_testping(HTTP_API_PARAM* param, const char* target);
extern ITAT_API int salt_api_events(HTTP_API_PARAM *param);
extern ITAT_API int salt_api_async_cmd_runall(HTTP_API_PARAM* param, const char* target, const char* script);
extern ITAT_API int salt_api_cmd_runall(HTTP_API_PARAM* param, const char* target, const char* script);
extern ITAT_API int salt_api_cp_getfile(HTTP_API_PARAM* param, const char* target, const char* src_file, const char* des_file);
extern ITAT_API int salt_api_file_exists(HTTP_API_PARAM* param, const char* target, const char* script);

extern ITAT_API int parse_token_fn(const char *data, size_t len, void* param1, void* param2);

//param1 is MAP_SALT_JOB type
extern ITAT_API int parse_job(const char *json, size_t len, void* param1, void* param2);
extern ITAT_API int parse_salt_runall_ret(const char *json, size_t len, void* param1, void* param2);
extern ITAT_API int parse_salt_testping_ret(const char *json_data, size_t len, void* param1, void* param2);
extern ITAT_API int parse_salt_myjob_jobmap(const char *json_data, size_t len,
                                            PARAM p1, PARAM p2);
#define TOKEN_LEN 128
extern ITAT_API char g_token[TOKEN_LEN];
extern ITAT_API int g_run;

const int64_t testping_pid = -1;
const int64_t testping_nid = -1;


} //namespace itat


extern ITAT_API std::ostream& operator<< (std::ostream& out, itat::SALT_JOB_RET& ret);
extern ITAT_API std::ostream& operator<< (std::ostream& out, itat::SALT_JOB& job);

#endif // SALT_API_HPP
