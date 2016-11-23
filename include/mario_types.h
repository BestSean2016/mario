#ifndef MARIO_TYPES_H
#define MARIO_TYPES_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct mr_host {
  int64_t host_id; ///主机ID
  char host[128];  ///主机
  char name[128];  ///名称
  char ip[15];     /// IP地址
  char minion_id[32]; ///MINION ID
  char desc[255];  ///描述
} MR_HOST;



typedef enum SCRIPT_TYPE {
   SCRIPT_TYPE_RUNNER, ///0,调用执行器
   SCRIPT_TYPE_SALT,   ///1,salt命令
}SCRIPT_TYPE;

typedef struct mr_script {
  int64_t scpt_id;      ///脚本ID
  char scpt_name[50];   ///脚本名称
  int64_t host_id;      ///脚本对应的主机ID
  char *script;         ///脚本
  SCRIPT_TYPE scpt_type;///脚本type
  int scpt_timeout;     ///DEFAULT '300 '执行超时时间
  char desc[255];       ///说明
  time_t create_tm;     ///创建时间
  time_t modify_tm;     ///修改时间
} MR_SCRIPT;


typedef enum HOST_STATUS_TYPE {
    HOST_STATUS_TYPE_UNKONW,
    HOST_STATUS_TYPE_NORMAL,
    HOST_STATUS_TYPE_NO_RESPONSE,
} HOST_STATUS_TYPE;


typedef struct mr_pipeline {
  int64_t pl_id;        ///流程ID
  char pl_oldid[64];    ///原系统ID
  char pl_name[50];     ///名称
  char pl_desc[255];    ///说明
  time_t pl_create_tm;  ///创建时间
  time_t pl_modify_tm;  ///更新时间
} MR_PIPELINE;


typedef enum NODE_TYPE {
  NODE_TYPE_SCRIPT,
  NODE_TYPE_PIPELINE,
} NODE_TYPE;


///错误处理类型:0,遇到错误停止;1,遇到错误继续
typedef enum ERROR_PROCESS_TYPE {
    ERROR_PROCESS_TYPE_STOP,
    ERROR_PROCESS_TYPE_CONTINUE,
} ERROR_PROCESS_TYPE ;

///超时处理类型;0,超时停止;1超时继续
typedef enum TIMEOUT_PROCESS_TYPE {
    TIMEOUT_PROCESS_TYPE_STOP,
    TIMEOUT_PROCESS_TYPE_CONTINUE,
} TIMEOUT_PROCESS_TYPE;

typedef struct mr_pl_edge {
  int64_t node_id;         /// ID
  int64_t pl_id;           ///所属流程ID
  MR_PIPELINE *pipeline;   ///所属流程
  int64_t src_id;          ///引用源ID
  int64_t trg_id;          ///引用目标ID
  void *src_node_ptr;      ///引用源节点
  void *trg_node_ptr;      ///引用目标节点
  NODE_TYPE src_type;      ///类型：0, 脚本; 1，流程
  NODE_TYPE trg_type;      ///类型：0, 脚本; 1，流程
  char src_min_id [32];    ///源主机MINION_ID
  char trg_mini_id[32];    ///目标主机MINION_ID
  ERROR_PROCESS_TYPE err_prss_type; ///错误处理类型:0,遇到错误停止;1,遇到错误继续
  TIMEOUT_PROCESS_TYPE to_prss_type; ///超时处理类型;0,超时停止;1超时继续
  char src_oldid[64];      ///原系统中的源ID
  char trg_oldid[64];      ///原系统中的目标ID
  char edge_desc[255];     ///说明
  time_t create_tm;        ///创建时间
  time_t modify_tm;        ///修改时间
} MR_PIPELINE_EDGE;

typedef enum EXEC_TYPE {
  EXEC_TYPE_AUTO,
  EXEC_TYPE_MANUAL,
} EXEC_TYPE;


typedef struct mr_pl_exec {
  int64_t pe_id;          ///流程执行结果ID
  int64_t pl_id;          ///流程ID
  MR_PIPELINE *pipeline;  ///所属流程
  time_t pe_stm;          ///开始时间
  time_t pe_etm;          ///结束时间
  char pe_desc[255];      ///说明
  int pe_ret_code;        ///返回值
  char pe_ret_state[255]; ///执行结果或状态
  EXEC_TYPE pe_type;      ///执行类型：0,自动执行;1,手动执行
} MR_PIPELINE_EXEC;


typedef struct mr_pln_exec {
  int64_t plne_id;          ///节点执行结果ID
  int64_t pe_id;           ///流程执行ID
  MR_PIPELINE_EXEC *ple;   ///所属流程
  int64_t edge_id;         ///节点ID
  MR_PIPELINE_EDGE *edge;  ///edge
  char salt_jid[32];       ///Salt Job ID
  int pid;                 ///进程ID
  int ret_code;            ///执行结果代码
  time_t pln_stm;          ///开始时间
  time_t pln_etm;          ///结束时间
  char ret_stderr[255];    ///标准错误输出
  char ret_stdout[255];    ///标准输出
  char minion_id [32 ];    ///主机MinionID
  char arg[255];           ///运行参数
  char fun[32] ;           ///运行函数
} MR_PIPLIELINE_NODE_EXEC;


typedef struct mr_host_status {
  int64_t pe_host_id;           ///执行时主机ID
  int64_t pe_id;                ///管道执行ID
  int64_t host_id;              ///主机id
  MR_PIPELINE_EXEC* pe;         ///管道执行Object
  MR_HOST* host;                ///主机Object
  HOST_STATUS_TYPE host_status; ///主机状态,0未知,1正常2无反应
  time_t updatetime;            ///状态更新时间
} MR_HOST_STATUS;



#endif // MARIO_TYPES_H
