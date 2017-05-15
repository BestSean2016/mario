#include "mario_mysql.h"
#include "mario_data.h"
#include "utility.h"

const char *select_host_from_db = {"select * from mr_host"};
const char *select_pipeline_from_db = {
    "SELECT"
    "  mr_pipeline.pl_id,"
    "  mr_pipeline.pl_oldid,"
    "  mr_pipeline.pl_name,"
    "  mr_pipeline.pl_desc,"
    "  UNIX_TIMESTAMP(mr_pipeline.pl_create_tm) as create_tm,"
    "  UNIX_TIMESTAMP(mr_pipeline.pl_modify_tm) as modify_tm,"
    "  mr_pipeline.pl_schedule "
    "FROM"
    "  mr_pipeline"};
const char *select_script_from_db = {
    "SELECT"
    "  mr_script.scpt_id,"
    "  mr_script.scpt_name,"
    "  mr_script.host_id,"
    "  mr_script.scpt_script,"
    "  mr_script.scpt_type,"
    "  mr_script.scpt_timeout,"
    "  mr_script.scpt_desc,"
    "  UNIX_TIMESTAMP(mr_script.scpt_create_tm) as create_tm,"
    "  UNIX_TIMESTAMP(mr_script.scpt_modify_tm) as modify_tm "
    "FROM"
    "  mr_script"};

const char *select_edge_from_db = {
    "SELECT                                                 "
    "    mr_pl_edge.edge_id,                                "
    "    mr_pl_edge.pl_id,                                  "
    "    mr_pl_edge.src_id,                                 "
    "    mr_pl_edge.trg_id,                                 "
    "    mr_pl_edge.src_type,                               "
    "    mr_pl_edge.trg_type,                               "
    "    mr_pl_edge.src_min_id,                             "
    "    mr_pl_edge.trg_min_id,                             "
    "    mr_pl_edge.src_timeout,                            "
    "    mr_pl_edge.trg_timeout,                            "
    "    mr_pl_edge.err_prss_type,                          "
    "    mr_pl_edge.to_prss_type,                           "
    "    mr_pl_edge.src_oldid,                              "
    "    mr_pl_edge.trg_oldid,                              "
    "    mr_pl_edge.edge_desc,                              "
    "    UNIX_TIMESTAMP(mr_pl_edge.create_tm) as create_tm, "
    "    UNIX_TIMESTAMP(mr_pl_edge.modify_tm) as modify_tm  "
    "FROM                                                   "
    "    mr_pl_edge                                         "};

const char *select_plexec_from_db = {"select * from mr_pl_exec"};

const char *select_plen_from_db = {"SELECT "
    "    mr_pln_exec.plne_id, "
    "    mr_pln_exec.pe_id, "
    "    mr_pln_exec.edge_id, "
    "    mr_pln_exec.salt_jid, "
    "    mr_pln_exec.pid, "
    "    mr_pln_exec.ret_code, "
    "    UNIX_TIMESTAMP(mr_pln_exec.pln_stm) AS start_tm, "
    "    UNIX_TIMESTAMP(mr_pln_exec.pln_etm) AS end_tm, "
    "    mr_pln_exec.ret_stdout, "
    "    mr_pln_exec.ret_stdout, "
    "    mr_pln_exec.minion_id, "
    "    mr_pln_exec.arg, "
    "    mr_pln_exec.fun  "
    "FROM  "
    "    mr_pln_exec"};


const char *select_host_status_from_db = {
    "SELECT "
    "  mr_host_status.pe_host_id, "
    "  mr_host_status.pe_id,      "
    "  mr_host_status.host_id,    "
    "  mr_host_status.host_status,"
    "  UNIX_TIMESTAMP(mr_host_status.updatetime) AS updatetime "
    "FROM "
    "    mr_host_status"};

const char* select_real_node = {"SELECT * FROM mr_ple_real_node"};
const char* select_real_edge = {"SELECT * FROM mr_ple_real_edge"};

#ifdef __USING_MYSQL__

DBHANDLE connect_db(const char *host, int port, const char *db,
                    const char *user, const char *passwd) {
  // printf("connect_db ..... \n");

  MYSQL *mysql = mysql_init(NULL);

  if (mysql == 0) {
    fprintf(stdout, "%s\n", mysql_error(mysql));
    return nullptr;
  }

  if (mysql_real_connect(mysql, host, user, passwd, db, port, NULL, 0) ==
      NULL) {
    fprintf(stdout, "%s\n", mysql_error(mysql));
    mysql_close(mysql);
    return nullptr;
  }

  mysql_query(mysql, "SET NAMES utf8");

  // printf("connect mysql sucess!\n");
  return mysql;
}

void disconnect_db(DBHANDLE dbh) {
  if (dbh) {
    mysql_close((MYSQL *)dbh);
    mysql_server_end();
    // printf("disconnected_db\n");
  }
}

/*
 * CREATE TABLE `mr_host` (
  `host_id` bigint(20) NOT NULL COMMENT '主机ID',
  `host` varchar(128) NOT NULL COMMENT '主机',
  `name` varchar(128) NOT NULL COMMENT '名称',
  `ip` varchar(15) NOT NULL COMMENT 'IP地址',
  `minion_id` varchar(32) NOT NULL COMMENT 'MINION ID',
  `desc` varchar(255) DEFAULT 'NULL' COMMENT '描述',
  PRIMARY KEY (`host_id`),
  UNIQUE KEY `unique_minion` (`minion_id`),
  KEY `ip` (`ip`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
*/
void get_hosts(void *host_ptr, MYSQL_ROW &mysql_row) {
  MR_HOST *host = reinterpret_cast<MR_HOST *>(host_ptr);
  host->id = mysql_atoll(mysql_row[0]);
  mysql_scpy(host->host, HOST_NAME_LENGTH, mysql_row[1]);
  mysql_scpy(host->name, HOST_NAME_LENGTH, mysql_row[2]);
  mysql_scpy(host->ip, IP_ADDRESS_LENGTH, mysql_row[3]);
  mysql_scpy(host->minion_id, HOST_MINION_ID_LENGTH, mysql_row[4]);
  mysql_scpy(host->desc, SHORT_TEXT_LENGTH, mysql_row[5]);
}

/*
CREATE TABLE `mr_pipeline` (
  `pl_id` bigint(20) NOT NULL COMMENT '流程ID',
  `pl_oldid` varchar(64) DEFAULT NULL COMMENT '原系统ID',
  `pl_name` varchar(50) NOT NULL COMMENT '名称',
  `pl_desc` varchar(255) NOT NULL COMMENT '说明',
  `pl_create_tm` datetime NOT NULL COMMENT '创建时间',
  `pl_modify_tm` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE
CURRENT_TIMESTAMP COMMENT '更新时间',
  PRIMARY KEY (`pl_id`),
  UNIQUE KEY `pl_name` (`pl_name`),
  UNIQUE KEY `pl_oldid` (`pl_oldid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
*/
void get_pipeline(void *pipeline_ptr, MYSQL_ROW &mysql_row) {
  MR_PIPELINE *pl = reinterpret_cast<MR_PIPELINE *>(pipeline_ptr);
  pl->id = mysql_atoll(mysql_row[0]);
  mysql_scpy(pl->pl_oldid, OLD_SYS_ID_LENGTH, mysql_row[1]);
  mysql_scpy(pl->pl_name, NORMAL_NAME_LENGTH, mysql_row[2]);
  mysql_scpy(pl->pl_desc, SHORT_TEXT_LENGTH, mysql_row[3]);
  pl->pl_create_tm = mysql_atoll(mysql_row[4]);
  pl->pl_modify_tm = mysql_atoll(mysql_row[5]);
  mysql_scpy(pl->pl_schedule, SCHEDULE_LENGTH, mysql_row[6]);
}
/*
CREATE TABLE `mr_script` (
  `scpt_id` bigint(20) NOT NULL AUTO_INCREMENT COMMENT '脚本ID',
  `scpt_name` varchar(50) NOT NULL COMMENT '脚本名称',
  `host_id` bigint(20) DEFAULT NULL COMMENT '脚本对应的主机ID',
  `scpt_script` text NOT NULL COMMENT '脚本',
  `scpt_type` enum('1','0') NOT NULL DEFAULT '0' COMMENT
'0,调用执行器;1,salt命令',
  `scpt_timeout` int(11) NOT NULL DEFAULT '300' COMMENT '执行超时时间',
  `scpt_desc` varchar(255) NOT NULL COMMENT '说明',
  `scpt_create_tm` datetime NOT NULL COMMENT '创建时间',
  `scpt_modify_tm` timestamp NULL DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP
COMMENT '修改时间',
  PRIMARY KEY (`scpt_id`),
  UNIQUE KEY `script` (`scpt_name`,`host_id`),
  FULLTEXT KEY `scpt_name` (`scpt_name`),
  FULLTEXT KEY `scpt_script` (`scpt_script`)
) ENGINE=MyISAM AUTO_INCREMENT=10 DEFAULT CHARSET=utf8;
*/

void get_script(void *script_ptr, MYSQL_ROW &mysql_row) {
  MR_SCRIPT *script = reinterpret_cast<MR_SCRIPT *>(script_ptr);
  script->id = mysql_atoll(mysql_row[0]);
  mysql_scpy(script->scpt_name, NORMAL_NAME_LENGTH, mysql_row[1]);
  script->host_id = mysql_atoll(mysql_row[2]);
  //script->script = mysql_sdup(mysql_row[3]);
  // if (mysql_row[3]) {
  //   script->script = new char[strlen(mysql_row[3]) + 1];
  //   strcpy(script->script, mysql_row[3]);
  // } else
  //   script->script = nullptr;
  mysql_scpy(script->script, SHORT_TEXT_LENGTH, mysql_row[3]);
  script->scpt_type = (SCRIPT_TYPE)(mysql_row[4][0] - '0');
  script->scpt_timeout = mysql_atoi(mysql_row[5]);
  mysql_scpy(script->desc, SHORT_TEXT_LENGTH, mysql_row[6]);
  script->create_tm = mysql_atoll(mysql_row[7]);
  script->modify_tm = mysql_atoll(mysql_row[8]);
}

/*
CREATE TABLE `mr_pl_edge` (
  `edge_id` bigint(20) NOT NULL AUTO_INCREMENT COMMENT '边ID',
  `pl_id` bigint(20) NOT NULL COMMENT '所属流程ID',
  `src_id` bigint(20) NOT NULL COMMENT '引用源ID',
  `trg_id` bigint(20) NOT NULL COMMENT '引用目标ID',
  `src_type` enum('1','0') NOT NULL DEFAULT '0' COMMENT '类型：0, 脚本;
1，流程',
  `trg_type` enum('1','0') NOT NULL DEFAULT '0' COMMENT '类型：0, 脚本;
1，流程',
  `src_min_id` varchar(32) NOT NULL COMMENT '源主机MINION_ID',
  `trg_min_id` varchar(32) NOT NULL COMMENT '目标主机MINION_ID',
  `src_timeout` int(11) DEFAULT '0' COMMENT '源节点超时时间',
  `trg_timeout` int(11) DEFAULT '0' COMMENT '目的节点超时时间',
  `err_prss_type` enum('1','0') DEFAULT '0' COMMENT
'错误处理类型:0,遇到错误停止;1,遇到错误继续',
  `to_prss_type` enum('1','0') DEFAULT '0' COMMENT
'超时处理类型;0,超时停止;1超时继续',
  `src_oldid` varchar(64) DEFAULT NULL COMMENT '原系统中的源ID',
  `trg_oldid` varchar(64) DEFAULT NULL COMMENT '原系统中的目标ID',
  `edge_desc` varchar(255) DEFAULT NULL COMMENT '说明',
  `create_tm` datetime NOT NULL COMMENT '创建时间',
  `modify_tm` timestamp NULL DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP COMMENT
'修改时间',
  PRIMARY KEY (`edge_id`),
  KEY `pl_id` (`pl_id`),
  KEY `src_min_id` (`src_min_id`),
  KEY `trg_mini_id` (`trg_mini_id`),
  CONSTRAINT `mr_pl_edge_ibfk_1` FOREIGN KEY (`pl_id`) REFERENCES `mr_pipeline`
(`pl_id`),
  CONSTRAINT `mr_pl_edge_ibfk_2` FOREIGN KEY (`src_min_id`) REFERENCES `mr_host`
(`minion_id`),
  CONSTRAINT `mr_pl_edge_ibfk_3` FOREIGN KEY (`trg_mini_id`) REFERENCES
`mr_host` (`minion_id`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;
*/
void get_edge(void *edge_ptr, MYSQL_ROW &mysql_row) {
  MR_PIPELINE_EDGE *edge = reinterpret_cast<MR_PIPELINE_EDGE *>(edge_ptr);
  edge->id = mysql_atoll(mysql_row[0]);
  edge->pl_id = mysql_atoll(mysql_row[1]);
  edge->src_id = mysql_atoll(mysql_row[2]);
  edge->trg_id = mysql_atoll(mysql_row[3]);
  edge->src_type = (NODE_TYPE)(mysql_row[4][0] - '0');
  edge->trg_type = (NODE_TYPE)(mysql_row[5][0] - '0');
  mysql_scpy(edge->src_min_id, HOST_MINION_ID_LENGTH, mysql_row[6]);
  mysql_scpy(edge->trg_min_id, HOST_MINION_ID_LENGTH, mysql_row[7]);
  edge->src_timeout = mysql_atoi(mysql_row[8]);
  edge->trg_timeout = mysql_atoi(mysql_row[9]);
  edge->err_prss_type = (ERROR_PROCESS_TYPE)(mysql_row[10][0] - '0');
  edge->to_prss_type = (TIMEOUT_PROCESS_TYPE)(mysql_row[11][0] - '0');
  mysql_scpy(edge->src_oldid, OLD_SYS_ID_LENGTH, mysql_row[12]);
  mysql_scpy(edge->trg_oldid, OLD_SYS_ID_LENGTH, mysql_row[13]);
  mysql_scpy(edge->edge_desc, SHORT_TEXT_LENGTH, mysql_row[14]);
  edge->create_tm = mysql_atoll(mysql_row[15]);
  edge->modify_tm = mysql_atoll(mysql_row[16]);
}

/*
CREATE TABLE `mr_pl_exec` (
  `pe_id` bigint(20) NOT NULL AUTO_INCREMENT COMMENT '流程执行结果ID',
  `pl_id` bigint(20) NOT NULL COMMENT '流程ID',
  `pe_stm` datetime NOT NULL COMMENT '开始时间',
  `pe_etm` datetime DEFAULT NULL COMMENT '结束时间',
  `pe_desc` varchar(255) DEFAULT NULL COMMENT '说明',
  `pe_ret_code` int(11) NOT NULL COMMENT '返回值',
  `pe_ret_state` varchar(255) DEFAULT NULL COMMENT '执行结果或状态',
  `pe_type` enum('1','0') DEFAULT '0' COMMENT '执行类型：0,自动执行;1,手动执行',
  PRIMARY KEY (`pe_id`),
  KEY `pl_id` (`pl_id`),
  CONSTRAINT `mr_pl_exec_ibfk_1` FOREIGN KEY (`pl_id`) REFERENCES `mr_pipeline`
(`pl_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
*/

void get_pipeline_exec(void *ptr, MYSQL_ROW &row) {
  MR_PIPELINE_EXEC *ple = reinterpret_cast<MR_PIPELINE_EXEC *>(ptr);
  ple->id = mysql_atoll(row[0]);
  ple->pl_id = mysql_atoll(row[1]);
  ple->pe_stm = mysql_atoll(row[2]);
  ple->pe_etm = mysql_atoll(row[3]);
  mysql_scpy(ple->pe_desc, SHORT_TEXT_LENGTH, row[4]);
  ple->pe_ret_code = atol(row[5]);
  mysql_scpy(ple->pe_ret_state, SHORT_TEXT_LENGTH, row[6]);
  ple->pe_type = (EXEC_TYPE)(row[7][0] - '0');
}

/*CREATE TABLE `mr_pln_exec` (
  `plne_id` bigint(20) NOT NULL AUTO_INCREMENT COMMENT '节点执行结果ID',
  `pe_id` bigint(20) NOT NULL COMMENT '流程执行ID',
  `edge_id` bigint(20) NOT NULL COMMENT '节点ID',
  `salt_jid` varchar(32) DEFAULT NULL COMMENT 'Salt Job ID',
  `pid` int(11) DEFAULT NULL COMMENT '进程ID',
  `ret_code` int(11) DEFAULT NULL COMMENT '执行结果代码',
  `pln_stm` timestamp NULL DEFAULT NULL COMMENT '开始时间',
  `pln_etm` timestamp NULL DEFAULT NULL COMMENT '结束时间',
  `ret_stdout` varchar(255) DEFAULT NULL COMMENT '标准错误输出',
  `ret_stdout` varchar(255) DEFAULT NULL COMMENT '标准输出',
  `minion_id` varchar(32) DEFAULT NULL COMMENT '主机MinionID',
  `arg` varchar(255) DEFAULT NULL COMMENT '运行参数',
  `fun` varchar(32) DEFAULT NULL COMMENT '运行函数',
  PRIMARY KEY (`plne_id`),
  KEY `mr_pln_exec_ibfk_3` (`edge_id`),
  KEY `mr_pln_exec_ibfk_4` (`pe_id`) USING BTREE,
  CONSTRAINT `mr_pln_exec_ibfk_4` FOREIGN KEY (`pe_id`) REFERENCES `mr_pl_exec`
(`pe_id`),
  CONSTRAINT `mr_pln_exec_ibfk_3` FOREIGN KEY (`edge_id`) REFERENCES
`mr_pl_edge` (`edge_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;
*/

void get_plen_exec(void *ptr, MYSQL_ROW &row) {
  MR_PIPELINE_NODE_EXEC *ne = reinterpret_cast<MR_PIPELINE_NODE_EXEC *>(ptr);
  ne->id = mysql_atoll(row[0]);
  ne->pe_id = mysql_atoll(row[1]);
  ne->edge_id = mysql_atoll(row[2]);
  mysql_scpy(ne->salt_jid, SALT_JOB_ID_LENGTH, row[3]);
  ne->pid = atol(row[4]);
  ne->ret_code = atol(row[5]);
  ne->pln_stm = mysql_atoll(row[6]);
  ne->pln_etm = mysql_atoll(row[7]);
  mysql_scpy(ne->ret_stdout, SHORT_TEXT_LENGTH, row[8]);
  mysql_scpy(ne->ret_stdout, SHORT_TEXT_LENGTH, row[9]);
  mysql_scpy(ne->minion_id, HOST_MINION_ID_LENGTH, row[10]);
  mysql_scpy(ne->arg, SHORT_TEXT_LENGTH, row[11]);
  mysql_scpy(ne->fun, SALT_FUNCTION_LENGTH, row[12]);
}

/*
CREATE TABLE `mr_host_status` (
  `pe_host_id` bigint(20) NOT NULL COMMENT '执行时主机ID',
  `pe_id` bigint(20) NOT NULL COMMENT '管道执行ID',
  `host_id` bigint(20) NOT NULL COMMENT '主机id',
  `host_status` enum('9','8','7','6','5','4','3','2','1','0') NOT NULL DEFAULT
'0' COMMENT '主机状态,0未知,1正常2无反应',
  `updatetime` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE
CURRENT_TIMESTAMP COMMENT '状态更新时间',
  PRIMARY KEY (`pe_host_id`),
  KEY `pe_id` (`pe_id`),
  KEY `host_id` (`host_id`),
  CONSTRAINT `mr_host_status_ibfk_2` FOREIGN KEY (`host_id`) REFERENCES
`mr_host` (`host_id`),
  CONSTRAINT `mr_host_status_ibfk_1` FOREIGN KEY (`pe_id`) REFERENCES
`mr_pl_exec` (`pe_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
*/
void get_host_status(void *ptr, MYSQL_ROW &row) {
  MR_HOST_STATUS *hs = reinterpret_cast<MR_HOST_STATUS *>(ptr);
  hs->id = mysql_atoll(row[0]);
  hs->pe_id = mysql_atoll(row[2]);
  hs->host_id = mysql_atoll(row[3]);
  hs->host_status = (HOST_STATUS_TYPE)(row[4][0] - '0');
  hs->updatetime = mysql_atoll(row[4]);
}

/*
CREATE TABLE `mr_pl_real_edge` (
  `e_id` bigint(20) NOT NULL,
  `pl_id` bigint(20) NOT NULL,
  `node_src_id` bigint(20) NOT NULL,
  `node_trg_id` bigint(20) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
*/
void get_real_edge(void* ptr, MYSQL_ROW &row) {
  MR_REAL_EDGE *re = reinterpret_cast<MR_REAL_EDGE*>(ptr);
  re->id = mysql_atoll(row[0]);
  re->ple_id = mysql_atoll(row[1]);
  re->node_src_id = mysql_atoll(row[2]);
  re->node_trg_id = mysql_atoll(row[3]);
}

/*
CREATE TABLE `mr_pl_real_node` (
  `node_id` bigint(20) NOT NULL,
  `pl_id` bigint(20) NOT NULL,
  `script_id` bigint(20) NOT NULL,
  `host_id` bigint(20) NOT NULL DEFAULT '0' COMMENT '????;0:',
  PRIMARY KEY (`node_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
*/
void get_real_node(void* ptr, MYSQL_ROW &row) {
  MR_REAL_NODE *rn = reinterpret_cast<MR_REAL_NODE*>(ptr);
  rn->id = mysql_atoll(row[0]);
  rn->ple_id = mysql_atoll(row[1]);
  rn->script_id = mysql_atoll(row[2]);
  rn->host_id = mysql_atoll(row[3]);
  rn->timerout = mysql_atol(row[4]);
}

/*
int insertPolicyalarm(const char *message, int deviceid, const char *firstoccur,
                      const char *kpiid, int alarmLevelId, int icount,
                      int isclose, int isopen) {
  char szSql[512] = {0};
  int nDeviceID = getLocalIP();
  sprintf(szSql, "insert into policyalarm(message, icount,\
            deviceid, firstoccur,\
            kpiid, isclose, isopen, alarmLevelId) \
            values('%s', %d, %d, str_to_date('%s', '%%Y-%%m-%%d %%H:%%i:%%s'),
%d, %d,%d,%d)",
          message, icount, nDeviceID, firstoccur, kpiid, isclose, isopen,
          alarmLevelId);

  printf("[policyalarm]:%s\n", szSql);
  if (0 != mysql_query(mysql, szSql)) {
    const char *szError = mysql_error(mysql);
    printf("[error]  %s\n", szError);
    return -1;
  }
  return 0;
}

int insertPolicyalarm(const char *message, int deviceid, const char *firstoccur,
                      const char *kpiid, int alarmLevelId, int icount,
                      int isclose, int isopen) {
  char szSql[512] = {0};
  int nDeviceID = getLocalIP();
  sprintf(szSql, "insert into policyalarm(message, icount,\
            deviceid, firstoccur,\
            kpiid, isclose, isopen, alarmLevelId) \
            values('%s', %d, %d, str_to_date('%s', '%%Y-%%m-%%d %%H:%%i:%%s'),
%d, %d,%d,%d)",
          message, icount, nDeviceID, firstoccur, kpiid, isclose, isopen,
          alarmLevelId);

  printf("[policyalarm]:%s\n", szSql);
  if (0 != mysql_query(mysql, szSql)) {
    const char *szError = mysql_error(mysql);
    printf("[error]  %s\n", szError);
    return -1;
  }
  return 0;
}
*/
#endif //__USING_MYSQL__
