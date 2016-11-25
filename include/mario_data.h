#ifndef MARIO_TYPES_H
#define MARIO_TYPES_H

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

#define HOST_NAME_LENGTH 128
#define HOST_MINION_ID_LENGTH 32
#define SALT_JOB_ID_LENGTH 32
#define SALT_FUNCTION_LENGTH 32
#define SHORT_TEXT_LENGTH 255
#define IP_ADDRESS_LENGTH 15
#define OLD_SYS_ID_LENGTH 64
#define NORMAL_NAME_LENGTH 50

//#define new_obj(Obj, Type) Obj = (#Type*)new #Type; \ memset(Obj, 0, sizeof(#Type));

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
  char *script;                       ///脚本
  SCRIPT_TYPE scpt_type;              ///脚本type
  int scpt_timeout;                   /// DEFAULT '300 '执行超时时间
  time_t create_tm;                   ///创建时间
  time_t modify_tm;                   ///修改时间
  char scpt_name[NORMAL_NAME_LENGTH]; ///脚本名称
  char desc[SHORT_TEXT_LENGTH];       ///说明
} MR_SCRIPT;

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

typedef std::vector<MR_HOST*> HostArray;
typedef std::map<int64_t, void*> MapId2Ptr;
typedef MapId2Ptr::iterator id2ptr_iter;
typedef std::map<std::string, void*>MapStr2Ptr;

typedef std::vector<MR_PIPELINE*> PipeLineArray;

template<typename T>
void show_ptr_array(T& t, std::ostream& out) {
  for (auto &p : t)
    out << *p;
}

template<typename T>
void free_ptr_array(T& t) {
  for (auto& p : t)
    delete p;

  t.clear();
}

template<typename T>
void insert_int2ptr_map(MapId2Ptr& map, std::vector<T*>& t) {
  for (auto& p : t)
    map[p->id] = p;
}

template<typename T>
void insert_str2_ptr_map(MapStr2Ptr& map, std::vector<T*>& t, size_t offset) {
  for (auto& p : t) {
    map[(char*)(p) + offset] = p;
  }
}

template<typename T>
void set_key_to_ptr(MapId2Ptr& map, std::vector<T*>& t, size_t id_offset, size_t ptr_offset) {
    for (auto& p : t) {
      *(char**)((char*)p + ptr_offset) = (char*)(map[*(int64_t*)((char*)(p) + id_offset)]);
    }
}

extern std::ostream& operator<<(std::ostream& out, MR_HOST& host);
extern std::ostream& operator<<(std::ostream& out, MR_PIPELINE& pl);
extern std::ostream& operator<<(std::ostream& out, MR_SCRIPT& script);
extern std::ostream& operator<<(std::ostream& out, MR_PIPELINE_EDGE edge);
extern std::ostream& operator<<(std::ostream& out, MR_PIPELINE_EXEC& ple);
extern std::ostream& operator<<(std::ostream& out, MR_PIPELINE_NODE_EXEC& plen);
extern std::ostream& operator<<(std::ostream& out, MR_HOST_STATUS& hs);

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

extern void free_script_array(std::vector<MR_SCRIPT*> array);
extern void set_host_status_map(std::vector<MR_HOST_STATUS *> &status,
                                MapId2Ptr &ple, MapId2Ptr &mapHost);

extern void set_plne_map(std::vector<MR_PIPELINE_NODE_EXEC*>& array,
                          MapId2Ptr& mapPle, MapId2Ptr& mapEdge, MapStr2Ptr& mapHost);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // MARIO_TYPES_H
