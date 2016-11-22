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
  char desc[255];  ///描述
} MR_HOST;

typedef struct mr_script {
  int64_t id;       ///脚本ID
  char name[50];    ///脚本名称
  char *script;     ///脚本
  char desc[255];   ///说明
  time_t create_tm; ///创建时间
  time_t modify_tm; ///修改时间
} MR_SCRIPT;

typedef struct mr_scpt_host {
  int64_t scpt_host_id;
  int64_t host_id;
  int64_t scpt_id;
  MR_HOST *host;
  MR_SCRIPT *script;
} MR_SCPT_HOST;

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

typedef struct mr_pl_node {
  int64_t node_id;         /// ID
  NODE_TYPE node_type;     ///类型：0, 脚本; 1，流程
  int64_t pl_id;           ///所属流程ID
  MR_PIPELINE *pipeline;   ///所属流程
  int64_t ref_id;          ///引用ID
  int64_t src_id;          ///引用源ID
  int64_t trg_id;          ///引用目标ID
  void *ref_node_ptr;      ///引用节点
  void *src_node_ptr;      ///引用源节点
  void *trg_node_ptr;      ///引用目标节点
  NODE_TYPE src_type;      ///类型：0, 脚本; 1，流程
  NODE_TYPE trg_type;      ///类型：0, 脚本; 1，流程
  char node_desc[255];     ///说明
  time_t create_tm;        ///创建时间
  time_t modify_tm;        ///修改时间
} MR_PIPELINE_NODE;

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
  int64_t pln_id;          ///节点执行结果ID
  int64_t pe_id;           ///流程执行ID
  MR_PIPELINE_EXEC *ple;   ///所属流程
  int64_t pl_node;         ///节点ID
  MR_PIPELINE_NODE *node;  ///节点
  time_t pln_stm;          ///开始时间
  time_t pln_etm;          ///结束时间
  char pln_desc[255];      ///说明
  int pln_ret_code;        ///执行结果代码
  char pln_ret_state[255]; ///执行结果或状态
} MR_PIPLIELINE_NODE_EXEC;

#endif // MARIO_TYPES_H
