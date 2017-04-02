#ifndef SALT_API_HPP
#define SALT_API_HPP

#include "itat_global.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <time.h>

#include <unistd.h>

#include <vector>
#include <map>
#include <set>
#include <string.h>
#include "httpapi.hpp"

namespace itat {

template<typename T>
class MapStr2Ptr : public std::map<std::string, T*> {
public:
   MapStr2Ptr() {}
   ~MapStr2Ptr() {
     for (auto p = this->begin(); p != this->end(); ++p)
       delete (*p).second;
   }
};

typedef enum RETURN_TYPE {
    RETURN_TYPE_OBJECT,
    RETURN_TYPE_BOOL,
    RETURN_TYPE_STRING,
} RETURN_TYPE;

typedef struct salt_job_ret {
  int64_t ple_id;            ///PIPELINE EXECUTIVE ID
  RETURN_TYPE rettype;
  std::string tag;
  std::string stamp;
  time_t      stamp_sec;
  uint32_t    stamp_usec;
  uint32_t    pid;
  int         retcode;
  std::string stderr;
  std::string stdout;
  bool        success;
  std::string cmd;
  std::string jid;
  std::string fun;
  std::string minion_id;
} SALT_JOB_RET;


typedef enum SALT_JOB_EVENT_TYPE {
    SALT_JOB_TYPE_IGNORE,
    SALT_JOB_TYPE_NEW,
    SALT_JOB_TYPE_RET,
} SALT_JOB_EVENT_TYPE;

typedef enum JOB_STATUS_TYPE {
  JOB_STATUS_TYPE_NOSTART,
  JOB_STATUS_TYPE_RUNNING,
  JOB_STATUS_TYPE_FAILED,
  JOB_STATUS_TYPE_TIMEOUT,
  JOB_STATUS_TYPE_SUCCESSED,
} JOB_STATUS_TYPE;


typedef struct salt_job {
  int64_t     ple_id;            ///PIPELINE EXECUTIVE ID
  int64_t     node_id;
  time_t      stamp_sec;
  uint32_t    timerout;
  size_t      retnum;
  size_t      success_num;
  JOB_STATUS_TYPE status;
  std::string tag;
  std::string tgt_type;
  std::string jid;
  std::string tgt;
  std::string stamp;
  std::string user;
  std::vector<std::string> arg;
  std::string fun;
  std::vector<std::string> minions;

  MapStr2Ptr<SALT_JOB_RET> minion_ret;

  salt_job() {
    ple_id = 0;
    node_id = -1;
    stamp_sec = time(0);
    timerout = 60;
    success_num = 0;
    retnum = 0;
    status = JOB_STATUS_TYPE_RUNNING;
  }

  salt_job(int64_t pleid, int64_t nodeid) {
    ple_id = pleid;
    node_id = nodeid;
    stamp_sec = time(0);
    timerout = 60;
    success_num = 0;
    retnum = 0;
    status = JOB_STATUS_TYPE_RUNNING;
  }

  ~salt_job() {
    for(auto& p : minion_ret) {
      delete (p.second);
    }
    minion_ret.clear();
  }
} SALT_JOB;


typedef MapStr2Ptr<SALT_JOB> MAP_SALT_JOB;
typedef MAP_SALT_JOB::iterator SaltIter;


typedef struct htrd_job {
  int nJobId;
  std::string strJobId;
  JOB_STATUS_TYPE status;
} HTRD_JOB;

#define ALL_TASK_FINISHED -10000000


typedef enum SALT_API_TYPE {
  SALT_API_TYPE_LOGIN,
  SALT_API_TYPE_TESTPING,
  SALT_API_TYPE_ASYNC_RUNALL,
  SALT_API_TYPE_RUNALL,
  SALT_API_TYPE_EVENTS,
  SALT_API_TYPE_CP_GETFILE,

} SALT_API_TYPE;

typedef struct salt_callback {
  response_function parse_token_cb;
  response_function parse_job_cb;
  response_function parase_my_job_cb;
  response_function process_event_cb;
} SALT_CALLBACK;


extern ITAT_API void set_default_callback();

extern ITAT_API int salt_api_login(HTTP_API_PARAM& param, const char* user, const char* pass);
extern ITAT_API int salt_api_testping(HTTP_API_PARAM& param, const char* target);
extern ITAT_API int salt_api_events(HTTP_API_PARAM *param);
extern ITAT_API int salt_api_async_cmd_runall(HTTP_API_PARAM& param, const char* target, const char* script);
extern ITAT_API int salt_api_cmd_runall(HTTP_API_PARAM& param, const char* target, const char* script);
extern ITAT_API int salt_api_cp_getfile(HTTP_API_PARAM& param, const char* target, const char* src_file, const char* des_file);


extern ITAT_API int parse_token_fn(const char *data, size_t len, void* param1, void* param2);

//param1 is MAP_SALT_JOB type
extern ITAT_API int parse_salt_job(const char *json, size_t len, void* param1, void* param2);

#define TOKEN_LEN 128
extern ITAT_API char g_token[TOKEN_LEN];
extern ITAT_API int g_run;

const int64_t testping_pid = -1;
const int64_t testping_nid = -1;

} //namespace itat

#endif // SALT_API_HPP
