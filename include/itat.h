#ifndef ITAT_H
#define ITAT_H

#include "itat_global.h"

#include <ctype.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <iostream>
#include <map>
#include <queue>
#include <vector>
#include <set>

#define HOST_NAME_LENGTH 128
#define HOST_MINION_ID_LENGTH 32
#define SALT_JOB_ID_LENGTH 32
#define SALT_FUNCTION_LENGTH 32
#define SHORT_TEXT_LENGTH 255
#define IP_ADDRESS_LENGTH 15
#define OLD_SYS_ID_LENGTH 64
#define NORMAL_NAME_LENGTH 50
#define SCHEDULE_LENGTH    128

//#define new_obj(Obj, Type) Obj = (#Type*)new #Type; \ memset(Obj, 0, sizeof(#Type));

typedef enum SALT_API_TYPE {
  SALT_API_TYPE_LOGIN,
  SALT_API_TYPE_TESTPING,
  SALT_API_TYPE_TEST_CMDRUN,
  SALT_API_TYPE_EVENTS,
  SALT_API_TYPE_RUNALL,
} SALT_API_TYPE;

typedef struct mr_host {
  int64_t id;                            ///主机ID
  char host[HOST_NAME_LENGTH];           ///主机
  char name[HOST_NAME_LENGTH];           ///名称
  char ip[IP_ADDRESS_LENGTH];            /// IP地址
  char minion_id[HOST_MINION_ID_LENGTH]; /// MINION ID
  char desc[SHORT_TEXT_LENGTH];          ///描述
} MR_HOST;

typedef enum SCRIPT_TYPE {
  SCRIPT_TYPE_RUNNER, /// 0,调用执行器
  SCRIPT_TYPE_SALT,   /// 1,salt命令
} SCRIPT_TYPE;

typedef struct mr_script {
  int64_t id;                         ///脚本ID
  int64_t host_id;                    ///脚本对应的主机ID
  MR_HOST* host;
  char script[SHORT_TEXT_LENGTH];     ///脚本
  SCRIPT_TYPE scpt_type;              ///脚本type
  int scpt_timeout;                   /// DEFAULT '300 '执行超时时间
  time_t create_tm;                   ///创建时间
  time_t modify_tm;                   ///修改时间
  char scpt_name[NORMAL_NAME_LENGTH]; ///脚本名称
  char desc[SHORT_TEXT_LENGTH];       ///说明
} MR_SCRIPT;

typedef enum JOB_STATUS_TYPE {
  JOB_STATUS_TYPE_NOSTART,
  JOB_STATUS_TYPE_RUNNING,
  JOB_STATUS_TYPE_FAILED,
  JOB_STATUS_TYPE_TIMEOUT,
  JOB_STATUS_TYPE_SUCCESSED,
} JOB_STATUS_TYPE;

extern const char* job_status(JOB_STATUS_TYPE status);

typedef struct mr_real_node {
  int64_t id;
  int64_t ple_id;
  int64_t script_id;
  int64_t host_id;
  int     timerout;
  JOB_STATUS_TYPE status;
} MR_REAL_NODE;

typedef struct mr_real_edge {
  int64_t id;
  int64_t ple_id;
  int64_t node_src_id;
  int64_t node_trg_id;
} MR_REAL_EDGE;

typedef enum HOST_STATUS_TYPE {
  HOST_STATUS_TYPE_UNKONW,
  HOST_STATUS_TYPE_NORMAL,
  HOST_STATUS_TYPE_NO_RESPONSE,
} HOST_STATUS_TYPE;

typedef struct mr_pipeline {
  int64_t id;                       ///流程ID
  time_t pl_create_tm;              ///创建时间
  time_t pl_modify_tm;              ///更新时间
  char pl_oldid[OLD_SYS_ID_LENGTH]; ///原系统ID
  char pl_name[NORMAL_NAME_LENGTH]; ///名称
  char pl_desc[SHORT_TEXT_LENGTH];  ///说明
  char pl_schedule[SCHEDULE_LENGTH];  ///schaduling
} MR_PIPELINE;

typedef enum NODE_TYPE {
  NODE_TYPE_SCRIPT,
  NODE_TYPE_PIPELINE,
} NODE_TYPE;

///错误处理类型:0,遇到错误停止;1,遇到错误继续
typedef enum ERROR_PROCESS_TYPE {
  ERROR_PROCESS_TYPE_STOP,
  ERROR_PROCESS_TYPE_CONTINUE,
} ERROR_PROCESS_TYPE;

///超时处理类型;0,超时停止;1超时继续
typedef enum TIMEOUT_PROCESS_TYPE {
  TIMEOUT_PROCESS_TYPE_STOP,
  TIMEOUT_PROCESS_TYPE_CONTINUE,
} TIMEOUT_PROCESS_TYPE;

typedef struct mr_pl_edge {
  int64_t id;                             /// ID
  int64_t pl_id;                          ///所属流程ID
  MR_PIPELINE *pipeline;                  ///所属流程
  void *src_node_ptr;                     ///引用源节点
  void *trg_node_ptr;                     ///引用目标节点
  MR_HOST* src_host;                      ///source host
  MR_HOST* trg_host;                      ///target host
  int64_t src_id;                         ///引用源ID
  int64_t trg_id;                         ///引用目标ID
  NODE_TYPE src_type;                     ///类型：0, 脚本; 1，流程
  NODE_TYPE trg_type;                     ///类型：0, 脚本; 1，流程
  int src_timeout;                        ///
  int trg_timeout;                        ///
  ERROR_PROCESS_TYPE
      err_prss_type;                      ///错误处理类型:0,遇到错误停止;1,遇到错误继续
  TIMEOUT_PROCESS_TYPE to_prss_type;      ///超时处理类型;0,超时停止;1超时继续
  time_t create_tm;                       ///创建时间
  time_t modify_tm;                       ///修改时间
  char src_min_id[HOST_MINION_ID_LENGTH]; ///源主机MINION_ID
  char trg_min_id[HOST_MINION_ID_LENGTH]; ///目标主机MINION_ID
  char src_oldid[OLD_SYS_ID_LENGTH];      ///原系统中的源ID
  char trg_oldid[OLD_SYS_ID_LENGTH];      ///原系统中的目标ID
  char edge_desc[SHORT_TEXT_LENGTH];      ///说明
} MR_PIPELINE_EDGE;

typedef enum EXEC_TYPE {
  EXEC_TYPE_AUTO,
  EXEC_TYPE_MANUAL,
} EXEC_TYPE;

typedef struct mr_pl_exec {
  int64_t id;                           ///流程执行结果ID
  int64_t pl_id;                        ///流程ID
  MR_PIPELINE *pipeline;                ///所属流程
  time_t pe_stm;                        ///开始时间
  time_t pe_etm;                        ///结束时间
  int pe_ret_code;                      ///返回值
  char pe_desc[SHORT_TEXT_LENGTH];      ///说明
  char pe_ret_state[SHORT_TEXT_LENGTH]; ///执行结果或状态
  EXEC_TYPE pe_type;                    ///执行类型：0,自动执行;1,手动执行
} MR_PIPELINE_EXEC;

typedef struct mr_pln_exec {
  int64_t id;                            ///节点执行结果ID
  int64_t pe_id;                         ///流程执行ID
  int64_t edge_id;                       ///节点ID
  MR_PIPELINE_EXEC *ple;                 ///所属流程
  MR_PIPELINE_EDGE *edge;                /// edge
  MR_HOST* host;                         ///主机Object
  int pid;                               ///进程ID
  int ret_code;                          ///执行结果代码
  time_t pln_stm;                        ///开始时间
  time_t pln_etm;                        ///结束时间
  char salt_jid[SALT_JOB_ID_LENGTH];     /// Salt Job ID
  char ret_stderr[SHORT_TEXT_LENGTH];    ///标准错误输出
  char ret_stdout[SHORT_TEXT_LENGTH];    ///标准输出
  char minion_id[HOST_MINION_ID_LENGTH]; ///主机MinionID
  char arg[SHORT_TEXT_LENGTH];           ///运行参数
  char fun[SALT_FUNCTION_LENGTH];        ///运行函数
} MR_PIPELINE_NODE_EXEC;

typedef struct mr_host_status {
  int64_t id;                   ///执行时主机ID
  int64_t pe_id;                ///管道执行ID
  int64_t host_id;              ///主机id
  MR_PIPELINE_EXEC *pe;         ///管道执行Object
  MR_HOST *host;                ///主机Object
  HOST_STATUS_TYPE host_status; ///主机状态,0未知,1正常2无反应
  time_t updatetime;            ///状态更新时间
} MR_HOST_STATUS;


template<typename T>
struct DataSet {
  size_t size;
  T* data;

  DataSet<T> () :size(0), data(nullptr) {
  }

  DataSet<T> (size_t s) {
    size = s;
    data = new T[s];
    memset(data, 0, sizeof(T) * s);
  }

  void init(size_t s) {
    free_data_set();
    size = s;
    data = new T[s];
    memset(data, 0, sizeof(T) * s);
  }

  ~DataSet<T> () {
    free_data_set();
  }

  void free_data_set() {
    if (data) delete [] data;
    size = 0;
    data = nullptr;
  }

  T& operator[] (size_t d) {
    return data[d];
  }

  int64_t rand_id() {
    if (!size) return -1;
    srand(time(0));
    return (int64_t)(rand() % size);
  }
};


template<typename T>
bool operator==(T& a, T&b) {
  return (0 == (memcmp(&a, &b, sizeof(T))));
}

typedef std::map<int64_t, void*> MapId2Ptr;
typedef MapId2Ptr::iterator id2ptr_iter;
typedef std::map<std::string, void*>MapStr2Ptr;

template<typename T>
void show_data_set(struct DataSet<T>& set, std::ostream& out) {
  for (size_t i = 0; i < set.size; ++i)
    out << set.data[i];
}


// extern void free_script_set(struct DataSet<MR_SCRIPT>& set);

template<typename T>
void insert_int2ptr_map(MapId2Ptr& map, struct DataSet<T>& set) {
    for (size_t i = 0; i < set.size; ++i)
      map[set.data[i].id] = set.data + i;
}

template<typename T>
void insert_str2_ptr_map(MapStr2Ptr& map, struct DataSet<T>& set, size_t offset) {
  for (size_t i = 0; i < set.size; ++i)
    map[(char*)(set.data + i) + offset] = set.data + i;
}

template<typename T>
void set_key_to_ptr(MapId2Ptr& map, struct DataSet<T>& set, size_t id_offset, size_t ptr_offset) {
    for (size_t i = 0; i < set.size; ++i) {
      *(char**)((char*)(set.data + i) + ptr_offset) = (char*)(map[*(int64_t*)((char*)(set.data + i) + id_offset)]);
    }
}

extern std::ostream& operator<<(std::ostream& out, MR_HOST& host);
extern std::ostream& operator<<(std::ostream& out, MR_PIPELINE& pl);
extern std::ostream& operator<<(std::ostream& out, MR_SCRIPT& script);
extern std::ostream& operator<<(std::ostream& out, MR_PIPELINE_EDGE edge);
extern std::ostream& operator<<(std::ostream& out, MR_PIPELINE_EXEC& ple);
extern std::ostream& operator<<(std::ostream& out, MR_PIPELINE_NODE_EXEC& plen);
extern std::ostream& operator<<(std::ostream& out, MR_HOST_STATUS& hs);


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
} SALT_JOB;

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

typedef void* SALT_JOB_PTR;
typedef enum SALT_JOB_TYPE {
    SALT_JOB_TYPE_IGNORE,
    SALT_JOB_TYPE_NEW,
    SALT_JOB_TYPE_RET,
} SALT_JOB_TYPE;



typedef struct salt_job_data {
    SALT_JOB_TYPE type;
    SALT_JOB_PTR  ptr;
} SALT_JOB_DATA;


#define ALL_TASK_FINISHED -10000000

typedef std::string JOBID;
typedef std::string MINION;
typedef std::map<MINION, SALT_JOB_RET*> MapMinionRet;   //job return
typedef std::map<JOBID, SALT_JOB*> MapJid2Job;      //job new
typedef std::map<JOBID, MapMinionRet*> MapJid2Minions;
typedef MapJid2Minions::iterator MJ2M_Iterator;

typedef struct SaltJob {
  MapJid2Job     jobs;
  MapJid2Minions minions;
} JOBMAP;

extern SALT_JOB g_salt_job;
typedef void* PARAM;
typedef int (*parse_response) (const char* data, size_t len, PARAM param1, PARAM param2);
typedef int (*api_function) (const char *hostname, int port, const char* target, PARAM param1, PARAM param2);

typedef struct salt_callback {
  parse_response parse_token_cb;
  parse_response parse_job_cb;
  parse_response parase_my_job_cb;
  parse_response process_event_cb;
} SALT_CALLBACK;

extern ITAT_API void set_default_callback();

extern ITAT_API int salt_api_login(const char *hostname, int port, const char* user, const char* pass);
extern ITAT_API int salt_api_testping(const char *hostname, int port, const char* target, PARAM param1, PARAM param2);
extern ITAT_API int salt_api_test_cmdrun(const char* hostname, int port, PARAM param1, PARAM param2);
extern ITAT_API int salt_api_events(const char* hostname, int port, PARAM param1, PARAM param2);
extern ITAT_API int salt_api_cmd_runall(const char* hostname, int port, const char* target, const char* script, PARAM param1, PARAM param2);
extern ITAT_API int salt_api_cmd(const char* hostname, int port, const char* minion, const char* cmd);


#define TOKEN_LEN 128
extern ITAT_API char g_token[TOKEN_LEN];
extern ITAT_API int g_run;

const int64_t testping_pid = -1;
const int64_t testping_nid = -1;

#endif // ITAT_H
