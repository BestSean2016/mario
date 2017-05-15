#include "mario_sql.h"
#include <memory>   // For std::unique_ptr
#include <stdarg.h> // For va_start, etc.
#include <assert.h>

using namespace std;

DBHANDLE g_h_db = nullptr;

const char *query_sql[] = {
  "select id,`name` from auth_group",
  "select id,group_id,permission_id from auth_group_permissions",
  "select id,`name`,content_type_id,codename from auth_permission",
  "select "
  "id,password,last_login,is_superuser,username,first_name,last_name,email,is_"
  "staff,is_active,date_joined from auth_user",
  "select id,user_id,group_id from auth_user_groups",
  "select id,user_id,permission_id from auth_user_user_permissions",
  "select `key`,created,user_id from authtoken_token",
  "select "
  "id,level,ip_address,title,result_info,started_at,pipeline_id,node_id,"
  "operator_id,is_exec,ex_pl_id from bill_alarm",
  "select id,status,alarm_id,user_id from bill_alarm_user",
  "select "
  "id,host,script_name,started_at,ended_at,status,result_info,ck_pl_id,node_id "
  "from bill_checked_node",
  "select id,type,started_at,ended_at,status,result_info,pipeline_id,user_id "
  "from bill_checked_pipeline",
  "select id,status,result_info,created_at,ex_pl_id,node_id,operator_id from "
  "bill_exec_log",
  "select "
  "id,host,script_name,started_at,ended_at,result_info,warning_level,ex_pl_id,"
  "node_id from bill_exec_node",
  "select "
  "id,started_at,ended_at,type,result_info,result_status,pipeline_id,user_id "
  "from bill_exec_pipeline",
  "select id,`name`,user_id,`desc`,lock_on,status from bill_exec_pipeline_view",
  "select id,`name`,ip_address,minion_id,`desc` from bill_host",
  "select id,`name`,icon,`desc` from bill_node_type",
  "select "
  "id,old_id,`name`,`desc`,lock_on,status,created_at,updated_at,reviewed_at,"
  "deleted_at,creator_id,deletor_id,group_id,modifier_id,reviewer_id from "
  "bill_pipeline",
  "select "
  "id,ref_id,old_id,ref_type,`name`,minion_id,timeout,argv,`desc`,created_at,"
  "updated_at,creator_id,modifier_id,pipeline_id,ip_address,position_x,"
  "position_y,command,script_name,script_content from "
  "bill_pipeline_all_node_view",
  "select "
  "id,`desc`,created_at,updated_at,creator_id,modifier_id,pipeline_id,src_id,"
  "trg_id from bill_pipeline_edge",
  "select id,`name`,parent_id,old_id from bill_pipeline_group",
  "select id,`key`,value,pipeline_id from bill_pipeline_info",
  "select id,json_str,pipeline_id from bill_pipeline_json",
  "select "
  "id,old_id,ref_id,ref_type,`name`,minion_id,timeout,argv,`desc`,created_at,"
  "updated_at,creator_id,modifier_id,pipeline_id from bill_pipeline_node",
  "select id,position_x,position_y,width,height,node_id from "
  "bill_pipeline_node_info",
  "select "
  "id,ref_id,ref_type,`name`,minion_id,timeout,argv,`desc`,created_at,updated_"
  "at,creator_id,modifier_id,pipeline_id,ip_address,script_name from "
  "bill_pipeline_node_view",
  "select "
  "id,`name`,type,content,timeout,`desc`,file_path,target_path,argc,argv,md5,"
  "screen_path,log_path,created_at,updated_at,host_id from bill_script",
  "select "
  "id,action_time,object_id,object_repr,action_flag,change_message,content_"
  "type_id,user_id from django_admin_log",
  "select id,app_label,model from django_content_type",
  "select id,app,`name`,applied from django_migrations",
  "select session_key,session_data,expire_date from django_session"
};

int get_auth_group(void *r, MYSQL_ROW &row) {
  MR_AUTH_GROUP &mr_record = *(MR_AUTH_GROUP *)r;
  mr_record.id = mysql_atoi(row[0]);
  mr_record.name = mysql_str(row[1]);
  return 0;
}

int get_auth_group_permissions(void *r, MYSQL_ROW &row) {
  MR_AUTH_GROUP_PERMISSIONS &mr_record = *(MR_AUTH_GROUP_PERMISSIONS *)r;
  mr_record.id = mysql_atoi(row[0]);
  mr_record.group_id = mysql_atoi(row[1]);
  mr_record.permission_id = mysql_atoi(row[2]);
  return 0;
}

int get_auth_permission(void *r, MYSQL_ROW &row) {
  MR_AUTH_PERMISSION &mr_record = *(MR_AUTH_PERMISSION *)r;
  mr_record.id = mysql_atoi(row[0]);
  mr_record.name = mysql_str(row[1]);
  mr_record.content_type_id = mysql_atoi(row[2]);
  mr_record.codename = mysql_str(row[3]);
  return 0;
}

int get_auth_user(void *r, MYSQL_ROW &row) {
  MR_AUTH_USER &mr_record = *(MR_AUTH_USER *)r;
  mr_record.id = mysql_atoi(row[0]);
  mr_record.password = mysql_str(row[1]);
  mr_record.last_login = mysql_atoi(row[2]);
  mr_record.is_superuser = mysql_atoi(row[3]);
  mr_record.username = mysql_str(row[4]);
  mr_record.first_name = mysql_str(row[5]);
  mr_record.last_name = mysql_str(row[6]);
  mr_record.email = mysql_str(row[7]);
  mr_record.is_staff = mysql_atoi(row[8]);
  mr_record.is_active = mysql_atoi(row[9]);
  mr_record.date_joined = mysql_atoi(row[10]);
  return 0;
}

int get_auth_user_groups(void *r, MYSQL_ROW &row) {
  MR_AUTH_USER_GROUPS &mr_record = *(MR_AUTH_USER_GROUPS *)r;
  mr_record.id = mysql_atoi(row[0]);
  mr_record.user_id = mysql_atoi(row[1]);
  mr_record.group_id = mysql_atoi(row[2]);
  return 0;
}

int get_auth_user_user_permissions(void *r, MYSQL_ROW &row) {
  MR_AUTH_USER_USER_PERMISSIONS &mr_record =
      *(MR_AUTH_USER_USER_PERMISSIONS *)r;
  mr_record.id = mysql_atoi(row[0]);
  mr_record.user_id = mysql_atoi(row[1]);
  mr_record.permission_id = mysql_atoi(row[2]);
  return 0;
}

int get_authtoken_token(void *r, MYSQL_ROW &row) {
  MR_AUTHTOKEN_TOKEN &mr_record = *(MR_AUTHTOKEN_TOKEN *)r;
  mr_record.key = mysql_str(row[0]);
  mr_record.created = mysql_atoi(row[1]);
  mr_record.user_id = mysql_atoi(row[2]);
  return 0;
}

int get_bill_alarm(void *r, MYSQL_ROW &row) {
  MR_BILL_ALARM &mr_record = *(MR_BILL_ALARM *)r;
  mr_record.id = mysql_atoi(row[0]);
  mr_record.level = mysql_atoi(row[1]);
  mr_record.ip_address = mysql_str(row[2]);
  mr_record.title = mysql_str(row[3]);
  mr_record.result_info = mysql_str(row[4]);
  mr_record.started_at = mysql_atoi(row[5]);
  mr_record.pipeline_id = mysql_atoi(row[6]);
  mr_record.node_id = mysql_atoi(row[7]);
  mr_record.operator_id = mysql_atoi(row[8]);
  mr_record.is_exec = mysql_atoi(row[9]);
  mr_record.ex_pl_id = mysql_atoi(row[10]);
  return 0;
}

int get_bill_alarm_user(void *r, MYSQL_ROW &row) {
  MR_BILL_ALARM_USER &mr_record = *(MR_BILL_ALARM_USER *)r;
  mr_record.id = mysql_atoi(row[0]);
  mr_record.status = mysql_str(row[1]);
  mr_record.alarm_id = mysql_atoi(row[2]);
  mr_record.user_id = mysql_atoi(row[3]);
  return 0;
}

int get_bill_checked_node(void *r, MYSQL_ROW &row) {
  MR_BILL_CHECKED_NODE &mr_record = *(MR_BILL_CHECKED_NODE *)r;
  mr_record.id = mysql_atoi(row[0]);
  mr_record.host = mysql_str(row[1]);
  mr_record.script_name = mysql_str(row[2]);
  mr_record.started_at = mysql_atoi(row[3]);
  mr_record.ended_at = mysql_atoi(row[4]);
  mr_record.status = mysql_atoi(row[5]);
  mr_record.result_info = mysql_str(row[6]);
  mr_record.ck_pl_id = mysql_atoi(row[7]);
  mr_record.node_id = mysql_atoi(row[8]);
  return 0;
}

int get_bill_checked_pipeline(void *r, MYSQL_ROW &row) {
  MR_BILL_CHECKED_PIPELINE &mr_record = *(MR_BILL_CHECKED_PIPELINE *)r;
  mr_record.id = mysql_atoi(row[0]);
  mr_record.type = mysql_str(row[1]);
  mr_record.started_at = mysql_atoi(row[2]);
  mr_record.ended_at = mysql_atoi(row[3]);
  mr_record.status = mysql_atoi(row[4]);
  mr_record.result_info = mysql_str(row[5]);
  mr_record.pipeline_id = mysql_atoi(row[6]);
  mr_record.user_id = mysql_atoi(row[7]);
  return 0;
}

int get_bill_exec_log(void *r, MYSQL_ROW &row) {
  MR_BILL_EXEC_LOG &mr_record = *(MR_BILL_EXEC_LOG *)r;
  mr_record.id = mysql_atoi(row[0]);
  mr_record.status = mysql_atoi(row[1]);
  mr_record.result_info = mysql_str(row[2]);
  mr_record.created_at = mysql_atoi(row[3]);
  mr_record.ex_pl_id = mysql_atoi(row[4]);
  mr_record.node_id = mysql_atoi(row[5]);
  mr_record.operator_id = mysql_atoi(row[6]);
  return 0;
}

int get_bill_exec_node(void *r, MYSQL_ROW &row) {
  MR_BILL_EXEC_NODE &mr_record = *(MR_BILL_EXEC_NODE *)r;
  mr_record.id = mysql_atoi(row[0]);
  mr_record.host = mysql_str(row[1]);
  mr_record.script_name = mysql_str(row[2]);
  mr_record.started_at = mysql_atoi(row[3]);
  mr_record.ended_at = mysql_atoi(row[4]);
  mr_record.result_info = mysql_str(row[5]);
  mr_record.warning_level = mysql_atoi(row[6]);
  mr_record.ex_pl_id = mysql_atoi(row[7]);
  mr_record.node_id = mysql_atoi(row[8]);
  return 0;
}

int get_bill_exec_pipeline(void *r, MYSQL_ROW &row) {
  MR_BILL_EXEC_PIPELINE &mr_record = *(MR_BILL_EXEC_PIPELINE *)r;
  mr_record.id = mysql_atoi(row[0]);
  mr_record.started_at = mysql_atoi(row[1]);
  mr_record.ended_at = mysql_atoi(row[2]);
  mr_record.type = mysql_atoi(row[3]);
  mr_record.result_info = mysql_str(row[4]);
  mr_record.result_status = mysql_atoi(row[5]);
  mr_record.pipeline_id = mysql_atoi(row[6]);
  mr_record.user_id = mysql_atoi(row[7]);
  return 0;
}

int get_bill_exec_pipeline_view(void *r, MYSQL_ROW &row) {
  MR_BILL_EXEC_PIPELINE_VIEW &mr_record = *(MR_BILL_EXEC_PIPELINE_VIEW *)r;
  mr_record.id = mysql_atoi(row[0]);
  mr_record.name = mysql_str(row[1]);
  mr_record.user_id = mysql_atoi(row[2]);
  mr_record.desc = mysql_str(row[3]);
  mr_record.lock_on = mysql_atoi(row[4]);
  mr_record.status = mysql_atoi(row[5]);
  return 0;
}

int get_bill_host(void *r, MYSQL_ROW &row) {
  MR_BILL_HOST &mr_record = *(MR_BILL_HOST *)r;
  mr_record.id = mysql_atoi(row[0]);
  mr_record.name = mysql_str(row[1]);
  mr_record.ip_address = mysql_str(row[2]);
  mr_record.minion_id = mysql_str(row[3]);
  mr_record.desc = mysql_str(row[4]);
  return 0;
}

int get_bill_node_type(void *r, MYSQL_ROW &row) {
  MR_BILL_NODE_TYPE &mr_record = *(MR_BILL_NODE_TYPE *)r;
  mr_record.id = mysql_atoi(row[0]);
  mr_record.name = mysql_str(row[1]);
  mr_record.icon = mysql_str(row[2]);
  mr_record.desc = mysql_str(row[3]);
  return 0;
}

int get_bill_pipeline(void *r, MYSQL_ROW &row) {
  MR_BILL_PIPELINE &mr_record = *(MR_BILL_PIPELINE *)r;
  mr_record.id = mysql_atoi(row[0]);
  mr_record.old_id = mysql_str(row[1]);
  mr_record.name = mysql_str(row[2]);
  mr_record.desc = mysql_str(row[3]);
  mr_record.lock_on = mysql_atoi(row[4]);
  mr_record.status = mysql_atoi(row[5]);
  mr_record.created_at = mysql_atoi(row[6]);
  mr_record.updated_at = mysql_atoi(row[7]);
  mr_record.reviewed_at = mysql_atoi(row[8]);
  mr_record.deleted_at = mysql_atoi(row[9]);
  mr_record.creator_id = mysql_atoi(row[10]);
  mr_record.deletor_id = mysql_atoi(row[11]);
  mr_record.group_id = mysql_atoi(row[12]);
  mr_record.modifier_id = mysql_atoi(row[13]);
  mr_record.reviewer_id = mysql_atoi(row[14]);
  return 0;
}

int get_bill_pipeline_all_node_view(void *r, MYSQL_ROW &row) {
  MR_BILL_PIPELINE_ALL_NODE_VIEW &mr_record =
      *(MR_BILL_PIPELINE_ALL_NODE_VIEW *)r;
  mr_record.id = mysql_atoi(row[0]);
  mr_record.ref_id = mysql_atoi(row[1]);
  mr_record.old_id = mysql_str(row[2]);
  mr_record.ref_type = mysql_atoi(row[3]);
  mr_record.name = mysql_str(row[4]);
  mr_record.minion_id = mysql_str(row[5]);
  mr_record.timeout = mysql_atoi(row[6]);
  mr_record.argv = mysql_str(row[7]);
  mr_record.desc = mysql_str(row[8]);
  mr_record.created_at = mysql_atoi(row[9]);
  mr_record.updated_at = mysql_atoi(row[10]);
  mr_record.creator_id = mysql_atoi(row[11]);
  mr_record.modifier_id = mysql_atoi(row[12]);
  mr_record.pipeline_id = mysql_atoi(row[13]);
  mr_record.ip_address = mysql_str(row[14]);
  mr_record.position_x = mysql_str(row[15]);
  mr_record.position_y = mysql_str(row[16]);
  mr_record.command = mysql_str(row[17]);
  mr_record.script_name = mysql_str(row[18]);
  mr_record.script_content = mysql_str(row[19]);
  return 0;
}

int get_bill_pipeline_edge(void *r, MYSQL_ROW &row) {
  MR_BILL_PIPELINE_EDGE &mr_record = *(MR_BILL_PIPELINE_EDGE *)r;
  mr_record.id = mysql_atoi(row[0]);
  mr_record.desc = mysql_str(row[1]);
  mr_record.created_at = mysql_atoi(row[2]);
  mr_record.updated_at = mysql_atoi(row[3]);
  mr_record.creator_id = mysql_atoi(row[4]);
  mr_record.modifier_id = mysql_atoi(row[5]);
  mr_record.pipeline_id = mysql_atoi(row[6]);
  mr_record.src_id = mysql_atoi(row[7]);
  mr_record.trg_id = mysql_atoi(row[8]);
  return 0;
}

int get_bill_pipeline_group(void *r, MYSQL_ROW &row) {
  MR_BILL_PIPELINE_GROUP &mr_record = *(MR_BILL_PIPELINE_GROUP *)r;
  mr_record.id = mysql_atoi(row[0]);
  mr_record.name = mysql_str(row[1]);
  mr_record.parent_id = mysql_atoi(row[2]);
  mr_record.old_id = mysql_str(row[3]);
  return 0;
}

int get_bill_pipeline_info(void *r, MYSQL_ROW &row) {
  MR_BILL_PIPELINE_INFO &mr_record = *(MR_BILL_PIPELINE_INFO *)r;
  mr_record.id = mysql_atoi(row[0]);
  mr_record.key = mysql_str(row[1]);
  mr_record.value = mysql_str(row[2]);
  mr_record.pipeline_id = mysql_atoi(row[3]);
  return 0;
}

int get_bill_pipeline_json(void *r, MYSQL_ROW &row) {
  MR_BILL_PIPELINE_JSON &mr_record = *(MR_BILL_PIPELINE_JSON *)r;
  mr_record.id = mysql_atoi(row[0]);
  mr_record.json_str = mysql_str(row[1]);
  mr_record.pipeline_id = mysql_atoi(row[2]);
  return 0;
}

int get_bill_pipeline_node(void *r, MYSQL_ROW &row) {
  MR_BILL_PIPELINE_NODE &mr_record = *(MR_BILL_PIPELINE_NODE *)r;
  mr_record.id = mysql_atoi(row[0]);
  mr_record.old_id = mysql_str(row[1]);
  mr_record.ref_id = mysql_atoi(row[2]);
  mr_record.ref_type = mysql_atoi(row[3]);
  mr_record.name = mysql_str(row[4]);
  mr_record.minion_id = mysql_str(row[5]);
  mr_record.timeout = mysql_atoi(row[6]);
  mr_record.argv = mysql_str(row[7]);
  mr_record.desc = mysql_str(row[8]);
  mr_record.created_at = mysql_atoi(row[9]);
  mr_record.updated_at = mysql_atoi(row[10]);
  mr_record.creator_id = mysql_atoi(row[11]);
  mr_record.modifier_id = mysql_atoi(row[12]);
  mr_record.pipeline_id = mysql_atoi(row[13]);
  return 0;
}

int get_bill_pipeline_node_info(void *r, MYSQL_ROW &row) {
  MR_BILL_PIPELINE_NODE_INFO &mr_record = *(MR_BILL_PIPELINE_NODE_INFO *)r;
  mr_record.id = mysql_atoi(row[0]);
  mr_record.position_x = mysql_str(row[1]);
  mr_record.position_y = mysql_str(row[2]);
  mr_record.width = mysql_atoi(row[3]);
  mr_record.height = mysql_atoi(row[4]);
  mr_record.node_id = mysql_atoi(row[5]);
  return 0;
}

int get_bill_pipeline_node_view(void *r, MYSQL_ROW &row) {
  MR_BILL_PIPELINE_NODE_VIEW &mr_record = *(MR_BILL_PIPELINE_NODE_VIEW *)r;
  mr_record.id = mysql_atoi(row[0]);
  mr_record.ref_id = mysql_atoi(row[1]);
  mr_record.ref_type = mysql_atoi(row[2]);
  mr_record.name = mysql_str(row[3]);
  mr_record.minion_id = mysql_str(row[4]);
  mr_record.timeout = mysql_atoi(row[5]);
  mr_record.argv = mysql_str(row[6]);
  mr_record.desc = mysql_str(row[7]);
  mr_record.created_at = mysql_atoi(row[8]);
  mr_record.updated_at = mysql_atoi(row[9]);
  mr_record.creator_id = mysql_atoi(row[10]);
  mr_record.modifier_id = mysql_atoi(row[11]);
  mr_record.pipeline_id = mysql_atoi(row[12]);
  mr_record.ip_address = mysql_str(row[13]);
  mr_record.script_name = mysql_str(row[14]);
  return 0;
}

int get_bill_script(void *r, MYSQL_ROW &row) {
  MR_BILL_SCRIPT &mr_record = *(MR_BILL_SCRIPT *)r;
  mr_record.id = mysql_atoi(row[0]);
  mr_record.name = mysql_str(row[1]);
  mr_record.type = mysql_atoi(row[2]);
  mr_record.content = mysql_str(row[3]);
  mr_record.timeout = mysql_atoi(row[4]);
  mr_record.desc = mysql_str(row[5]);
  mr_record.file_path = mysql_str(row[6]);
  mr_record.target_path = mysql_str(row[7]);
  mr_record.argc = mysql_atoi(row[8]);
  mr_record.argv = mysql_str(row[9]);
  mr_record.md5 = mysql_str(row[10]);
  mr_record.screen_path = mysql_str(row[11]);
  mr_record.log_path = mysql_str(row[12]);
  mr_record.created_at = mysql_atoi(row[13]);
  mr_record.updated_at = mysql_atoi(row[14]);
  mr_record.host_id = mysql_atoi(row[15]);
  return 0;
}

int get_django_admin_log(void *r, MYSQL_ROW &row) {
  MR_DJANGO_ADMIN_LOG &mr_record = *(MR_DJANGO_ADMIN_LOG *)r;
  mr_record.id = mysql_atoi(row[0]);
  mr_record.action_time = mysql_atoi(row[1]);
  mr_record.object_id = mysql_str(row[2]);
  mr_record.object_repr = mysql_str(row[3]);
  mr_record.action_flag = mysql_atoi(row[4]);
  mr_record.change_message = mysql_str(row[5]);
  mr_record.content_type_id = mysql_atoi(row[6]);
  mr_record.user_id = mysql_atoi(row[7]);
  return 0;
}

int get_django_content_type(void *r, MYSQL_ROW &row) {
  MR_DJANGO_CONTENT_TYPE &mr_record = *(MR_DJANGO_CONTENT_TYPE *)r;
  mr_record.id = mysql_atoi(row[0]);
  mr_record.app_label = mysql_str(row[1]);
  mr_record.model = mysql_str(row[2]);
  return 0;
}

int get_django_migrations(void *r, MYSQL_ROW &row) {
  MR_DJANGO_MIGRATIONS &mr_record = *(MR_DJANGO_MIGRATIONS *)r;
  mr_record.id = mysql_atoi(row[0]);
  mr_record.app = mysql_str(row[1]);
  mr_record.name = mysql_str(row[2]);
  mr_record.applied = mysql_atoi(row[3]);
  return 0;
}

int get_django_session(void *r, MYSQL_ROW &row) {
  MR_DJANGO_SESSION &mr_record = *(MR_DJANGO_SESSION *)r;
  mr_record.session_key = mysql_str(row[0]);
  mr_record.session_data = mysql_str(row[1]);
  mr_record.expire_date = mysql_atoi(row[2]);
  return 0;
}

int get_auth_group_to_insert_sql(void *t, string &str) {
  MR_AUTH_GROUP &mr_record = *(MR_AUTH_GROUP *)t;
  str = stringFormat("insert into auth_group (id,`name`) values(%d,'%s')",
                     mr_record.id, mr_record.name.c_str());
  return 0;
}

int get_auth_group_permissions_to_insert_sql(void *t, string &str) {
  MR_AUTH_GROUP_PERMISSIONS &mr_record = *(MR_AUTH_GROUP_PERMISSIONS *)t;
  str = stringFormat("insert into auth_group_permissions "
                     "(id,group_id,permission_id) values(%d,%d,%d)",
                     mr_record.id, mr_record.group_id, mr_record.permission_id);
  return 0;
}

int get_auth_permission_to_insert_sql(void *t, string &str) {
  MR_AUTH_PERMISSION &mr_record = *(MR_AUTH_PERMISSION *)t;
  str = stringFormat("insert into auth_permission "
                     "(id,`name`,content_type_id,codename) "
                     "values(%d,'%s',%d,'%s')",
                     mr_record.id, mr_record.name.c_str(),
                     mr_record.content_type_id, mr_record.codename.c_str());
  return 0;
}

int get_auth_user_to_insert_sql(void *t, string &str) {
  MR_AUTH_USER &mr_record = *(MR_AUTH_USER *)t;
  str = stringFormat("insert into auth_user "
                     "(id,password,last_login,is_superuser,username,first_name,"
                     "last_name,email,is_staff,is_active,date_joined) "
                     "values(%d,'%s',%d,%d,'%s','%s','%s','%s',%d,%d,%d)",
                     mr_record.id, mr_record.password.c_str(),
                     mr_record.last_login, mr_record.is_superuser,
                     mr_record.username.c_str(), mr_record.first_name.c_str(),
                     mr_record.last_name.c_str(), mr_record.email.c_str(),
                     mr_record.is_staff, mr_record.is_active,
                     mr_record.date_joined);
  return 0;
}

int get_auth_user_groups_to_insert_sql(void *t, string &str) {
  MR_AUTH_USER_GROUPS &mr_record = *(MR_AUTH_USER_GROUPS *)t;
  str = stringFormat(
      "insert into auth_user_groups (id,user_id,group_id) values(%d,%d,%d)",
      mr_record.id, mr_record.user_id, mr_record.group_id);
  return 0;
}

int get_auth_user_user_permissions_to_insert_sql(void *t, string &str) {
  MR_AUTH_USER_USER_PERMISSIONS &mr_record =
      *(MR_AUTH_USER_USER_PERMISSIONS *)t;
  str = stringFormat("insert into auth_user_user_permissions "
                     "(id,user_id,permission_id) values(%d,%d,%d)",
                     mr_record.id, mr_record.user_id, mr_record.permission_id);
  return 0;
}

int get_authtoken_token_to_insert_sql(void *t, string &str) {
  MR_AUTHTOKEN_TOKEN &mr_record = *(MR_AUTHTOKEN_TOKEN *)t;
  str = stringFormat(
      "insert into authtoken_token (`key`,created,user_id) values('%s',%d,%d)",
      mr_record.key.c_str(), mr_record.created, mr_record.user_id);
  return 0;
}

int get_bill_alarm_to_insert_sql(void *t, string &str) {
  MR_BILL_ALARM &mr_record = *(MR_BILL_ALARM *)t;
  str = stringFormat(
      "insert into bill_alarm "
      "(id,level,ip_address,title,result_info,started_at,pipeline_id,node_id,"
      "operator_id,is_exec,ex_pl_id) "
      "values(%d,%d,'%s','%s','%s',%d,%d,%d,%d,%d,%d)",
      mr_record.id, mr_record.level, mr_record.ip_address.c_str(),
      mr_record.title.c_str(), mr_record.result_info.c_str(),
      mr_record.started_at, mr_record.pipeline_id, mr_record.node_id,
      mr_record.operator_id, mr_record.is_exec, mr_record.ex_pl_id);
  return 0;
}

int get_bill_alarm_user_to_insert_sql(void *t, string &str) {
  MR_BILL_ALARM_USER &mr_record = *(MR_BILL_ALARM_USER *)t;
  str = stringFormat("insert into bill_alarm_user (id,status,alarm_id,user_id) "
                     "values(%d,'%s',%d,%d)",
                     mr_record.id, mr_record.status.c_str(), mr_record.alarm_id,
                     mr_record.user_id);
  return 0;
}

int get_bill_checked_node_to_insert_sql(void *t, string &str) {
  MR_BILL_CHECKED_NODE &mr_record = *(MR_BILL_CHECKED_NODE *)t;
  str = stringFormat(
      "insert into bill_checked_node "
      "(id,host,script_name,started_at,ended_at,status,result_info,ck_pl_id,"
      "node_id) values(%d,'%s','%s',%d,%d,%d,'%s',%d,%d)",
      mr_record.id, mr_record.host.c_str(), mr_record.script_name.c_str(),
      mr_record.started_at, mr_record.ended_at, mr_record.status,
      mr_record.result_info.c_str(), mr_record.ck_pl_id, mr_record.node_id);
  return 0;
}

int get_bill_checked_pipeline_to_insert_sql(void *t, string &str) {
  MR_BILL_CHECKED_PIPELINE &mr_record = *(MR_BILL_CHECKED_PIPELINE *)t;
  str = stringFormat("insert into bill_checked_pipeline "
                     "(id,type,started_at,ended_at,status,result_info,pipeline_"
                     "id,user_id) values(%d,'%s',%d,%d,%d,'%s',%d,%d)",
                     mr_record.id, mr_record.type.c_str(), mr_record.started_at,
                     mr_record.ended_at, mr_record.status,
                     mr_record.result_info.c_str(), mr_record.pipeline_id,
                     mr_record.user_id);
  return 0;
}

int get_bill_exec_log_to_insert_sql(void *t, string &str) {
  MR_BILL_EXEC_LOG &mr_record = *(MR_BILL_EXEC_LOG *)t;
  str = stringFormat("insert into bill_exec_log "
                     "(id,status,result_info,created_at,ex_pl_id,node_id,"
                     "operator_id) values(%d,%d,'%s',%d,%d,%d,%d)",
                     mr_record.id, mr_record.status,
                     mr_record.result_info.c_str(), mr_record.created_at,
                     mr_record.ex_pl_id, mr_record.node_id,
                     mr_record.operator_id);
  return 0;
}

int get_bill_exec_node_to_insert_sql(void *t, string &str) {
  MR_BILL_EXEC_NODE &mr_record = *(MR_BILL_EXEC_NODE *)t;
  str = stringFormat(
      "insert into bill_exec_node "
      "(id,host,script_name,started_at,ended_at,result_info,warning_level,ex_"
      "pl_id,node_id) values(%d,'%s','%s',%d,%d,'%s',%d,%d,%d)",
      mr_record.id, mr_record.host.c_str(), mr_record.script_name.c_str(),
      mr_record.started_at, mr_record.ended_at, mr_record.result_info.c_str(),
      mr_record.warning_level, mr_record.ex_pl_id, mr_record.node_id);
  return 0;
}

int get_bill_exec_pipeline_to_insert_sql(void *t, string &str) {
  MR_BILL_EXEC_PIPELINE &mr_record = *(MR_BILL_EXEC_PIPELINE *)t;
  str = stringFormat("insert into bill_exec_pipeline "
                     "(id,started_at,ended_at,type,result_info,result_status,"
                     "pipeline_id,user_id) values(%d,%d,%d,%d,'%s',%d,%d,%d)",
                     mr_record.id, mr_record.started_at, mr_record.ended_at,
                     mr_record.type, mr_record.result_info.c_str(),
                     mr_record.result_status, mr_record.pipeline_id,
                     mr_record.user_id);
  return 0;
}

int get_bill_exec_pipeline_view_to_insert_sql(void *t, string &str) {
  MR_BILL_EXEC_PIPELINE_VIEW &mr_record = *(MR_BILL_EXEC_PIPELINE_VIEW *)t;
  str = stringFormat(
      "insert into bill_exec_pipeline_view "
      "(id,`name`,user_id,`desc`,lock_on,status) values(%d,'%s',%d,'%s',%d,%d)",
      mr_record.id, mr_record.name.c_str(), mr_record.user_id,
      mr_record.desc.c_str(), mr_record.lock_on, mr_record.status);
  return 0;
}

int get_bill_host_to_insert_sql(void *t, string &str) {
  MR_BILL_HOST &mr_record = *(MR_BILL_HOST *)t;
  str = stringFormat(
      "insert into bill_host (id,`name`,ip_address,minion_id,`desc`) "
      "values(%d,'%s','%s','%s','%s')",
      mr_record.id, mr_record.name.c_str(), mr_record.ip_address.c_str(),
      mr_record.minion_id.c_str(), mr_record.desc.c_str());
  return 0;
}

int get_bill_node_type_to_insert_sql(void *t, string &str) {
  MR_BILL_NODE_TYPE &mr_record = *(MR_BILL_NODE_TYPE *)t;
  str = stringFormat("insert into bill_node_type (id,`name`,icon,`desc`) "
                     "values(%d,'%s','%s','%s')",
                     mr_record.id, mr_record.name.c_str(),
                     mr_record.icon.c_str(), mr_record.desc.c_str());
  return 0;
}

int get_bill_pipeline_to_insert_sql(void *t, string &str) {
  MR_BILL_PIPELINE &mr_record = *(MR_BILL_PIPELINE *)t;
  str = stringFormat(
      "insert into bill_pipeline "
      "(id,old_id,`name`,`desc`,lock_on,status,created_at,updated_at,reviewed_"
      "at,deleted_at,creator_id,deletor_id,group_id,modifier_id,reviewer_id) "
      "values(%d,'%s','%s','%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)",
      mr_record.id, mr_record.old_id.c_str(), mr_record.name.c_str(),
      mr_record.desc.c_str(), mr_record.lock_on, mr_record.status,
      mr_record.created_at, mr_record.updated_at, mr_record.reviewed_at,
      mr_record.deleted_at, mr_record.creator_id, mr_record.deletor_id,
      mr_record.group_id, mr_record.modifier_id, mr_record.reviewer_id);
  return 0;
}

int get_bill_pipeline_all_node_view_to_insert_sql(void *t, string &str) {
  MR_BILL_PIPELINE_ALL_NODE_VIEW &mr_record =
      *(MR_BILL_PIPELINE_ALL_NODE_VIEW *)t;
  str = stringFormat(
      "insert into bill_pipeline_all_node_view "
      "(id,ref_id,old_id,ref_type,`name`,minion_id,timeout,argv,`desc`,created_"
      "at,updated_at,creator_id,modifier_id,pipeline_id,ip_address,position_x,"
      "position_y,command,script_name,script_content) "
      "values(%d,%d,'%s',%d,'%s','%s',%d,'%s','%s',%d,%d,%d,%d,%d,'%s','%s','%"
      "s','%s','%s','%s')",
      mr_record.id, mr_record.ref_id, mr_record.old_id.c_str(),
      mr_record.ref_type, mr_record.name.c_str(), mr_record.minion_id.c_str(),
      mr_record.timeout, mr_record.argv.c_str(), mr_record.desc.c_str(),
      mr_record.created_at, mr_record.updated_at, mr_record.creator_id,
      mr_record.modifier_id, mr_record.pipeline_id,
      mr_record.ip_address.c_str(), mr_record.position_x.c_str(),
      mr_record.position_y.c_str(), mr_record.command.c_str(),
      mr_record.script_name.c_str(), mr_record.script_content.c_str());
  return 0;
}

int get_bill_pipeline_edge_to_insert_sql(void *t, string &str) {
  MR_BILL_PIPELINE_EDGE &mr_record = *(MR_BILL_PIPELINE_EDGE *)t;
  str = stringFormat(
      "insert into bill_pipeline_edge "
      "(id,`desc`,created_at,updated_at,creator_id,modifier_id,pipeline_id,src_"
      "id,trg_id) values(%d,'%s',%d,%d,%d,%d,%d,%d,%d)",
      mr_record.id, mr_record.desc.c_str(), mr_record.created_at,
      mr_record.updated_at, mr_record.creator_id, mr_record.modifier_id,
      mr_record.pipeline_id, mr_record.src_id, mr_record.trg_id);
  return 0;
}

int get_bill_pipeline_group_to_insert_sql(void *t, string &str) {
  MR_BILL_PIPELINE_GROUP &mr_record = *(MR_BILL_PIPELINE_GROUP *)t;
  str = stringFormat("insert into bill_pipeline_group "
                     "(id,`name`,parent_id,old_id) values(%d,'%s',%d,'%s')",
                     mr_record.id, mr_record.name.c_str(), mr_record.parent_id,
                     mr_record.old_id.c_str());
  return 0;
}

int get_bill_pipeline_info_to_insert_sql(void *t, string &str) {
  MR_BILL_PIPELINE_INFO &mr_record = *(MR_BILL_PIPELINE_INFO *)t;
  str = stringFormat("insert into bill_pipeline_info "
                     "(id,`key`,value,pipeline_id) values(%d,'%s','%s',%d)",
                     mr_record.id, mr_record.key.c_str(),
                     mr_record.value.c_str(), mr_record.pipeline_id);
  return 0;
}

int get_bill_pipeline_json_to_insert_sql(void *t, string &str) {
  MR_BILL_PIPELINE_JSON &mr_record = *(MR_BILL_PIPELINE_JSON *)t;
  str = stringFormat("insert into bill_pipeline_json (id,json_str,pipeline_id) "
                     "values(%d,'%s',%d)",
                     mr_record.id, mr_record.json_str.c_str(),
                     mr_record.pipeline_id);
  return 0;
}

int get_bill_pipeline_node_to_insert_sql(void *t, string &str) {
  MR_BILL_PIPELINE_NODE &mr_record = *(MR_BILL_PIPELINE_NODE *)t;
  str = stringFormat(
      "insert into bill_pipeline_node "
      "(id,old_id,ref_id,ref_type,`name`,minion_id,timeout,argv,`desc`,created_"
      "at,updated_at,creator_id,modifier_id,pipeline_id) "
      "values(%d,'%s',%d,%d,'%s','%s',%d,'%s','%s',%d,%d,%d,%d,%d)",
      mr_record.id, mr_record.old_id.c_str(), mr_record.ref_id,
      mr_record.ref_type, mr_record.name.c_str(), mr_record.minion_id.c_str(),
      mr_record.timeout, mr_record.argv.c_str(), mr_record.desc.c_str(),
      mr_record.created_at, mr_record.updated_at, mr_record.creator_id,
      mr_record.modifier_id, mr_record.pipeline_id);
  return 0;
}

int get_bill_pipeline_node_info_to_insert_sql(void *t, string &str) {
  MR_BILL_PIPELINE_NODE_INFO &mr_record = *(MR_BILL_PIPELINE_NODE_INFO *)t;
  str = stringFormat("insert into bill_pipeline_node_info "
                     "(id,position_x,position_y,width,height,node_id) "
                     "values(%d,'%s','%s',%d,%d,%d)",
                     mr_record.id, mr_record.position_x.c_str(),
                     mr_record.position_y.c_str(), mr_record.width,
                     mr_record.height, mr_record.node_id);
  return 0;
}

int get_bill_pipeline_node_view_to_insert_sql(void *t, string &str) {
  MR_BILL_PIPELINE_NODE_VIEW &mr_record = *(MR_BILL_PIPELINE_NODE_VIEW *)t;
  str = stringFormat(
      "insert into bill_pipeline_node_view "
      "(id,ref_id,ref_type,`name`,minion_id,timeout,argv,`desc`,created_at,"
      "updated_at,creator_id,modifier_id,pipeline_id,ip_address,script_name) "
      "values(%d,%d,%d,'%s','%s',%d,'%s','%s',%d,%d,%d,%d,%d,'%s','%s')",
      mr_record.id, mr_record.ref_id, mr_record.ref_type,
      mr_record.name.c_str(), mr_record.minion_id.c_str(), mr_record.timeout,
      mr_record.argv.c_str(), mr_record.desc.c_str(), mr_record.created_at,
      mr_record.updated_at, mr_record.creator_id, mr_record.modifier_id,
      mr_record.pipeline_id, mr_record.ip_address.c_str(),
      mr_record.script_name.c_str());
  return 0;
}

int get_bill_script_to_insert_sql(void *t, string &str) {
  MR_BILL_SCRIPT &mr_record = *(MR_BILL_SCRIPT *)t;
  str = stringFormat(
      "insert into bill_script "
      "(id,`name`,type,content,timeout,`desc`,file_path,target_path,argc,argv,"
      "md5,screen_path,log_path,created_at,updated_at,host_id) "
      "values(%d,'%s',%d,'%s',%d,'%s','%s','%s',%d,'%s','%s','%s','%s',%d,%d,%"
      "d)",
      mr_record.id, mr_record.name.c_str(), mr_record.type,
      mr_record.content.c_str(), mr_record.timeout, mr_record.desc.c_str(),
      mr_record.file_path.c_str(), mr_record.target_path.c_str(),
      mr_record.argc, mr_record.argv.c_str(), mr_record.md5.c_str(),
      mr_record.screen_path.c_str(), mr_record.log_path.c_str(),
      mr_record.created_at, mr_record.updated_at, mr_record.host_id);
  return 0;
}

int get_django_admin_log_to_insert_sql(void *t, string &str) {
  MR_DJANGO_ADMIN_LOG &mr_record = *(MR_DJANGO_ADMIN_LOG *)t;
  str = stringFormat("insert into django_admin_log "
                     "(id,action_time,object_id,object_repr,action_flag,change_"
                     "message,content_type_id,user_id) "
                     "values(%d,%d,'%s','%s',%d,'%s',%d,%d)",
                     mr_record.id, mr_record.action_time,
                     mr_record.object_id.c_str(), mr_record.object_repr.c_str(),
                     mr_record.action_flag, mr_record.change_message.c_str(),
                     mr_record.content_type_id, mr_record.user_id);
  return 0;
}

int get_django_content_type_to_insert_sql(void *t, string &str) {
  MR_DJANGO_CONTENT_TYPE &mr_record = *(MR_DJANGO_CONTENT_TYPE *)t;
  str = stringFormat("insert into django_content_type (id,app_label,model) "
                     "values(%d,'%s','%s')",
                     mr_record.id, mr_record.app_label.c_str(),
                     mr_record.model.c_str());
  return 0;
}

int get_django_migrations_to_insert_sql(void *t, string &str) {
  MR_DJANGO_MIGRATIONS &mr_record = *(MR_DJANGO_MIGRATIONS *)t;
  str = stringFormat("insert into django_migrations (id,app,`name`,applied) "
                     "values(%d,'%s','%s',%d)",
                     mr_record.id, mr_record.app.c_str(),
                     mr_record.name.c_str(), mr_record.applied);
  return 0;
}

int get_django_session_to_insert_sql(void *t, string &str) {
  MR_DJANGO_SESSION &mr_record = *(MR_DJANGO_SESSION *)t;
  str = stringFormat("insert into django_session "
                     "(session_key,session_data,expire_date) "
                     "values('%s','%s',%d)",
                     mr_record.session_key.c_str(),
                     mr_record.session_data.c_str(), mr_record.expire_date);
  return 0;
}

int get_auth_group_to_update_sql(void *t, string &str) {
  MR_AUTH_GROUP &mr_record = *(MR_AUTH_GROUP *)t;
  str = stringFormat("update auth_group set id = %d,`name` = '%s'",
                     mr_record.id, mr_record.name.c_str());
  return 0;
}

int get_auth_group_permissions_to_update_sql(void *t, string &str) {
  MR_AUTH_GROUP_PERMISSIONS &mr_record = *(MR_AUTH_GROUP_PERMISSIONS *)t;
  str = stringFormat("update auth_group_permissions set id = %d,group_id = "
                     "%d,permission_id = %d",
                     mr_record.id, mr_record.group_id, mr_record.permission_id);
  return 0;
}

int get_auth_permission_to_update_sql(void *t, string &str) {
  MR_AUTH_PERMISSION &mr_record = *(MR_AUTH_PERMISSION *)t;
  str = stringFormat("update auth_permission set id = %d,`name` = "
                     "'%s',content_type_id = %d,codename = '%s'",
                     mr_record.id, mr_record.name.c_str(),
                     mr_record.content_type_id, mr_record.codename.c_str());
  return 0;
}

int get_auth_user_to_update_sql(void *t, string &str) {
  MR_AUTH_USER &mr_record = *(MR_AUTH_USER *)t;
  str = stringFormat(
      "update auth_user set id = %d,password = '%s',last_login = "
      "%d,is_superuser = %d,username = '%s',first_name = '%s',last_name = "
      "'%s',email = '%s',is_staff = %d,is_active = %d,date_joined = %d",
      mr_record.id, mr_record.password.c_str(), mr_record.last_login,
      mr_record.is_superuser, mr_record.username.c_str(),
      mr_record.first_name.c_str(), mr_record.last_name.c_str(),
      mr_record.email.c_str(), mr_record.is_staff, mr_record.is_active,
      mr_record.date_joined);
  return 0;
}

int get_auth_user_groups_to_update_sql(void *t, string &str) {
  MR_AUTH_USER_GROUPS &mr_record = *(MR_AUTH_USER_GROUPS *)t;
  str = stringFormat(
      "update auth_user_groups set id = %d,user_id = %d,group_id = %d",
      mr_record.id, mr_record.user_id, mr_record.group_id);
  return 0;
}

int get_auth_user_user_permissions_to_update_sql(void *t, string &str) {
  MR_AUTH_USER_USER_PERMISSIONS &mr_record =
      *(MR_AUTH_USER_USER_PERMISSIONS *)t;
  str = stringFormat("update auth_user_user_permissions set id = %d,user_id = "
                     "%d,permission_id = %d",
                     mr_record.id, mr_record.user_id, mr_record.permission_id);
  return 0;
}

int get_authtoken_token_to_update_sql(void *t, string &str) {
  MR_AUTHTOKEN_TOKEN &mr_record = *(MR_AUTHTOKEN_TOKEN *)t;
  str = stringFormat(
      "update authtoken_token set `key` = '%s',created = %d,user_id = %d",
      mr_record.key.c_str(), mr_record.created, mr_record.user_id);
  return 0;
}

int get_bill_alarm_to_update_sql(void *t, string &str) {
  MR_BILL_ALARM &mr_record = *(MR_BILL_ALARM *)t;
  str = stringFormat(
      "update bill_alarm set id = %d,level = %d,ip_address = '%s',title = "
      "'%s',result_info = '%s',started_at,pipeline_id = %d,node_id = "
      "%d,operator_id = %d,is_exec = %d,ex_pl_id = %d",
      mr_record.id, mr_record.level, mr_record.ip_address.c_str(),
      mr_record.title.c_str(), mr_record.result_info.c_str(),
      mr_record.started_at, mr_record.pipeline_id, mr_record.node_id,
      mr_record.operator_id, mr_record.is_exec, mr_record.ex_pl_id);
  return 0;
}

int get_bill_alarm_user_to_update_sql(void *t, string &str) {
  MR_BILL_ALARM_USER &mr_record = *(MR_BILL_ALARM_USER *)t;
  str = stringFormat("update bill_alarm_user set id = %d,status = "
                     "'%s',alarm_id = %d,user_id = %d",
                     mr_record.id, mr_record.status.c_str(), mr_record.alarm_id,
                     mr_record.user_id);
  return 0;
}

int get_bill_checked_node_to_update_sql(void *t, string &str) {
  MR_BILL_CHECKED_NODE &mr_record = *(MR_BILL_CHECKED_NODE *)t;
  str = stringFormat(
      "update bill_checked_node set id = %d,host = '%s',script_name = "
      "'%s',started_at = %d,ended_at = %d,status = %d,result_info = "
      "'%s',ck_pl_id = %d,node_id = %d",
      mr_record.id, mr_record.host.c_str(), mr_record.script_name.c_str(),
      mr_record.started_at, mr_record.ended_at, mr_record.status,
      mr_record.result_info.c_str(), mr_record.ck_pl_id, mr_record.node_id);
  return 0;
}

int get_bill_checked_pipeline_to_update_sql(void *t, string &str) {
  MR_BILL_CHECKED_PIPELINE &mr_record = *(MR_BILL_CHECKED_PIPELINE *)t;
  str = stringFormat("update bill_checked_pipeline set id = %d,type = "
                     "'%s',started_at = %d,ended_at = %d,status = "
                     "%d,result_info = '%s',pipeline_id = %d,user_id = %d",
                     mr_record.id, mr_record.type.c_str(), mr_record.started_at,
                     mr_record.ended_at, mr_record.status,
                     mr_record.result_info.c_str(), mr_record.pipeline_id,
                     mr_record.user_id);
  return 0;
}

int get_bill_exec_log_to_update_sql(void *t, string &str) {
  MR_BILL_EXEC_LOG &mr_record = *(MR_BILL_EXEC_LOG *)t;
  str = stringFormat(
      "update bill_exec_log set id = %d,status = %d,result_info = "
      "'%s',created_at=%d,ex_pl_id = %d,node_id = %d,operator_id = %d",
      mr_record.id, mr_record.status, mr_record.result_info.c_str(),
      mr_record.created_at, mr_record.ex_pl_id, mr_record.node_id,
      mr_record.operator_id);
  return 0;
}

int get_bill_exec_node_to_update_sql(void *t, string &str) {
  MR_BILL_EXEC_NODE &mr_record = *(MR_BILL_EXEC_NODE *)t;
  str = stringFormat(
      "update bill_exec_node set id = %d,host = '%s',script_name = "
      "'%s',started_at = %d,ended_at = %d,result_info = '%s',warning_level = "
      "%d,ex_pl_id = %d,node_id = %d",
      mr_record.id, mr_record.host.c_str(), mr_record.script_name.c_str(),
      mr_record.started_at, mr_record.ended_at, mr_record.result_info.c_str(),
      mr_record.warning_level, mr_record.ex_pl_id, mr_record.node_id);
  return 0;
}

int get_bill_exec_pipeline_to_update_sql(void *t, string &str) {
  MR_BILL_EXEC_PIPELINE &mr_record = *(MR_BILL_EXEC_PIPELINE *)t;
  str = stringFormat("update bill_exec_pipeline set id = %d,started_at = "
                     "%d,ended_at = %d,type = %d,result_info = "
                     "'%s',result_status = %d,pipeline_id = %d,user_id = %d",
                     mr_record.id, mr_record.started_at, mr_record.ended_at,
                     mr_record.type, mr_record.result_info.c_str(),
                     mr_record.result_status, mr_record.pipeline_id,
                     mr_record.user_id);
  return 0;
}

int get_bill_exec_pipeline_view_to_update_sql(void *t, string &str) {
  MR_BILL_EXEC_PIPELINE_VIEW &mr_record = *(MR_BILL_EXEC_PIPELINE_VIEW *)t;
  str =
      stringFormat("update bill_exec_pipeline_view set id = %d,`name` = "
                   "'%s',user_id = %d,`desc` = '%s',lock_on = %d,status = %d",
                   mr_record.id, mr_record.name.c_str(), mr_record.user_id,
                   mr_record.desc.c_str(), mr_record.lock_on, mr_record.status);
  return 0;
}

int get_bill_host_to_update_sql(void *t, string &str) {
  MR_BILL_HOST &mr_record = *(MR_BILL_HOST *)t;
  str = stringFormat("update bill_host set id = %d,`name` = '%s',ip_address = "
                     "'%s',minion_id = '%s',`desc` = '%s'",
                     mr_record.id, mr_record.name.c_str(),
                     mr_record.ip_address.c_str(), mr_record.minion_id.c_str(),
                     mr_record.desc.c_str());
  return 0;
}

int get_bill_node_type_to_update_sql(void *t, string &str) {
  MR_BILL_NODE_TYPE &mr_record = *(MR_BILL_NODE_TYPE *)t;
  str = stringFormat("update bill_node_type set id = %d,`name` = '%s',icon = "
                     "'%s',`desc` = '%s'",
                     mr_record.id, mr_record.name.c_str(),
                     mr_record.icon.c_str(), mr_record.desc.c_str());
  return 0;
}

int get_bill_pipeline_to_update_sql(void *t, string &str) {
  MR_BILL_PIPELINE &mr_record = *(MR_BILL_PIPELINE *)t;
  str = stringFormat(
      "update bill_pipeline set id = %d,old_id = '%s',`name` = '%s',`desc` = "
      "'%s',lock_on = %d,status = %d,created_at = %d,updated_at = "
      "%d,reviewed_at = %d,deleted_at = %d,creator_id = %d,deletor_id = "
      "%d,group_id = %d,modifier_id = %d,reviewer_id = %d",
      mr_record.id, mr_record.old_id.c_str(), mr_record.name.c_str(),
      mr_record.desc.c_str(), mr_record.lock_on, mr_record.status,
      mr_record.created_at, mr_record.updated_at, mr_record.reviewed_at,
      mr_record.deleted_at, mr_record.creator_id, mr_record.deletor_id,
      mr_record.group_id, mr_record.modifier_id, mr_record.reviewer_id);
  return 0;
}

int get_bill_pipeline_all_node_view_to_update_sql(void *t, string &str) {
  MR_BILL_PIPELINE_ALL_NODE_VIEW &mr_record =
      *(MR_BILL_PIPELINE_ALL_NODE_VIEW *)t;
  str = stringFormat(
      "update bill_pipeline_all_node_view set id = %d,ref_id = %d,old_id = "
      "'%s',ref_type = %d,`name` = '%s',minion_id = '%s',timeout = %d,argv = "
      "'%s',`desc` = '%s',created_at = %d,updated_at = %d,creator_id = "
      "%d,modifier_id = %d,pipeline_id = %d,ip_address = '%s',position_x = "
      "'%s',position_y = '%s',command = '%s',script_name = '%s',script_content "
      "= '%s'",
      mr_record.id, mr_record.ref_id, mr_record.old_id.c_str(),
      mr_record.ref_type, mr_record.name.c_str(), mr_record.minion_id.c_str(),
      mr_record.timeout, mr_record.argv.c_str(), mr_record.desc.c_str(),
      mr_record.created_at, mr_record.updated_at, mr_record.creator_id,
      mr_record.modifier_id, mr_record.pipeline_id,
      mr_record.ip_address.c_str(), mr_record.position_x.c_str(),
      mr_record.position_y.c_str(), mr_record.command.c_str(),
      mr_record.script_name.c_str(), mr_record.script_content.c_str());
  return 0;
}

int get_bill_pipeline_edge_to_update_sql(void *t, string &str) {
  MR_BILL_PIPELINE_EDGE &mr_record = *(MR_BILL_PIPELINE_EDGE *)t;
  str = stringFormat(
      "update bill_pipeline_edge set id = %d,`desc` = '%s',created_at = "
      "%d,updated_at = %d,creator_id = %d,modifier_id = %d,pipeline_id = "
      "%d,src_id = %d,trg_id = %d",
      mr_record.id, mr_record.desc.c_str(), mr_record.created_at,
      mr_record.updated_at, mr_record.creator_id, mr_record.modifier_id,
      mr_record.pipeline_id, mr_record.src_id, mr_record.trg_id);
  return 0;
}

int get_bill_pipeline_group_to_update_sql(void *t, string &str) {
  MR_BILL_PIPELINE_GROUP &mr_record = *(MR_BILL_PIPELINE_GROUP *)t;
  str = stringFormat("update bill_pipeline_group set id = %d,`name` = "
                     "'%s',parent_id = %d,old_id = '%s'",
                     mr_record.id, mr_record.name.c_str(), mr_record.parent_id,
                     mr_record.old_id.c_str());
  return 0;
}

int get_bill_pipeline_info_to_update_sql(void *t, string &str) {
  MR_BILL_PIPELINE_INFO &mr_record = *(MR_BILL_PIPELINE_INFO *)t;
  str = stringFormat("update bill_pipeline_info set id = %d,`key` = '%s',value "
                     "= '%s',pipeline_id = %d",
                     mr_record.id, mr_record.key.c_str(),
                     mr_record.value.c_str(), mr_record.pipeline_id);
  return 0;
}

int get_bill_pipeline_json_to_update_sql(void *t, string &str) {
  MR_BILL_PIPELINE_JSON &mr_record = *(MR_BILL_PIPELINE_JSON *)t;
  str = stringFormat(
      "update bill_pipeline_json set id = %d,json_str = '%s',pipeline_id = %d",
      mr_record.id, mr_record.json_str.c_str(), mr_record.pipeline_id);
  return 0;
}

int get_bill_pipeline_node_to_update_sql(void *t, string &str) {
  MR_BILL_PIPELINE_NODE &mr_record = *(MR_BILL_PIPELINE_NODE *)t;
  str = stringFormat(
      "update bill_pipeline_node set id = %d,old_id = '%s',ref_id = "
      "%d,ref_type = %d,`name` = '%s',minion_id = '%s',timeout = %d,argv = "
      "'%s',`desc` = '%s',created_at = %d,updated_at = %d,creator_id = "
      "%d,modifier_id = %d,pipeline_id = %d",
      mr_record.id, mr_record.old_id.c_str(), mr_record.ref_id,
      mr_record.ref_type, mr_record.name.c_str(), mr_record.minion_id.c_str(),
      mr_record.timeout, mr_record.argv.c_str(), mr_record.desc.c_str(),
      mr_record.created_at, mr_record.updated_at, mr_record.creator_id,
      mr_record.modifier_id, mr_record.pipeline_id);
  return 0;
}

int get_bill_pipeline_node_info_to_update_sql(void *t, string &str) {
  MR_BILL_PIPELINE_NODE_INFO &mr_record = *(MR_BILL_PIPELINE_NODE_INFO *)t;
  str = stringFormat(
      "update bill_pipeline_node_info set id = %d,position_x = '%s',position_y "
      "= '%s',width = %d,height = %d,node_id = %d",
      mr_record.id, mr_record.position_x.c_str(), mr_record.position_y.c_str(),
      mr_record.width, mr_record.height, mr_record.node_id);
  return 0;
}

int get_bill_pipeline_node_view_to_update_sql(void *t, string &str) {
  MR_BILL_PIPELINE_NODE_VIEW &mr_record = *(MR_BILL_PIPELINE_NODE_VIEW *)t;
  str = stringFormat(
      "update bill_pipeline_node_view set id = %d,ref_id = %d,ref_type = "
      "%d,`name` = '%s',minion_id = '%s',timeout = %d,argv = '%s',`desc` = "
      "'%s',created_at = %d,updated_at = %d,creator_id = %d,modifier_id = "
      "%d,pipeline_id = %d,ip_address = '%s',script_name = '%s'",
      mr_record.id, mr_record.ref_id, mr_record.ref_type,
      mr_record.name.c_str(), mr_record.minion_id.c_str(), mr_record.timeout,
      mr_record.argv.c_str(), mr_record.desc.c_str(), mr_record.created_at,
      mr_record.updated_at, mr_record.creator_id, mr_record.modifier_id,
      mr_record.pipeline_id, mr_record.ip_address.c_str(),
      mr_record.script_name.c_str());
  return 0;
}

int get_bill_script_to_update_sql(void *t, string &str) {
  MR_BILL_SCRIPT &mr_record = *(MR_BILL_SCRIPT *)t;
  str = stringFormat(
      "update bill_script set id = %d,`name` = '%s',type = %d,content = "
      "'%s',timeout = %d,`desc` = '%s',file_path = '%s',target_path = "
      "'%s',argc = %d,argv = '%s',md5 = '%s',screen_path = '%s',log_path = "
      "'%s',created_at = %d,updated_at = %d,host_id = %d",
      mr_record.id, mr_record.name.c_str(), mr_record.type,
      mr_record.content.c_str(), mr_record.timeout, mr_record.desc.c_str(),
      mr_record.file_path.c_str(), mr_record.target_path.c_str(),
      mr_record.argc, mr_record.argv.c_str(), mr_record.md5.c_str(),
      mr_record.screen_path.c_str(), mr_record.log_path.c_str(),
      mr_record.created_at, mr_record.updated_at, mr_record.host_id);
  return 0;
}

int get_django_admin_log_to_update_sql(void *t, string &str) {
  MR_DJANGO_ADMIN_LOG &mr_record = *(MR_DJANGO_ADMIN_LOG *)t;
  str =
      stringFormat("update django_admin_log set id = %d,action_time = "
                   "%d,object_id = '%s',object_repr = '%s',action_flag = "
                   "%d,change_message = '%s',content_type_id = %d,user_id = %d",
                   mr_record.id, mr_record.action_time,
                   mr_record.object_id.c_str(), mr_record.object_repr.c_str(),
                   mr_record.action_flag, mr_record.change_message.c_str(),
                   mr_record.content_type_id, mr_record.user_id);
  return 0;
}

int get_django_content_type_to_update_sql(void *t, string &str) {
  MR_DJANGO_CONTENT_TYPE &mr_record = *(MR_DJANGO_CONTENT_TYPE *)t;
  str = stringFormat(
      "update django_content_type set id = %d,app_label = '%s',model = '%s'",
      mr_record.id, mr_record.app_label.c_str(), mr_record.model.c_str());
  return 0;
}

int get_django_migrations_to_update_sql(void *t, string &str) {
  MR_DJANGO_MIGRATIONS &mr_record = *(MR_DJANGO_MIGRATIONS *)t;
  str = stringFormat("update django_migrations set id = %d,app = '%s',`name` = "
                     "'%s',applied = %d",
                     mr_record.id, mr_record.app.c_str(),
                     mr_record.name.c_str(), mr_record.applied);
  return 0;
}

int get_django_session_to_update_sql(void *t, string &str) {
  MR_DJANGO_SESSION &mr_record = *(MR_DJANGO_SESSION *)t;
  str = stringFormat("update django_session set session_key = "
                     "'%s',session_data = '%s',expire_date = %d",
                     mr_record.session_key.c_str(),
                     mr_record.session_data.c_str(), mr_record.expire_date);
  return 0;
}

std::string stringFormat(const std::string fmt_str, ...) {
  int final_n,
      n = ((int)fmt_str.size()) * 2; /* Reserve two times as much as
                                              the length of the fmt_str */
  std::unique_ptr<char[]> formatted;
  va_list ap;
  while (1) {
    formatted.reset(
        new char[n]); /* Wrap the plain char array into the unique_ptr */
    strcpy(&formatted[0], fmt_str.c_str());
    va_start(ap, fmt_str);
    final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
    va_end(ap);
    if (final_n < 0 || final_n >= n)
      n += abs(final_n - n + 1);
    else
      break;
  }
  return std::string(formatted.get());
}

DBHANDLE connect_db(const char *host, int port, const char *db,
                    const char *user, const char *passwd) {
#ifdef _DEBUG_
  printf("connect_db ..... \n");
#endif

  MYSQL *mysql = mysql_init(NULL);
  if (mysql == 0) {
    fprintf(stdout, "%s", mysql_error(mysql));
    return nullptr;
  }
  if (mysql_real_connect(mysql, host, user, passwd, db, port, NULL, 0) ==
      NULL) {
    fprintf(stdout, "%s", mysql_error(mysql));
    mysql_close(mysql);
    return nullptr;
  }
  mysql_query(mysql, "SET NAMES utf8");

#ifdef _DEBUG_
  printf("connect mysql sucess!\n");
#endif
  return mysql;
}

void disconnect_db(DBHANDLE dbh) {
  if (dbh) {
    mysql_close((MYSQL *)dbh);
    mysql_server_end();
  }
}

int exec_db(DBHANDLE db, const char *sql) {
  int ret = (db == nullptr);
  if (!ret)
    ret = mysql_query((MYSQL *)db, sql);
  return (ret);
}

#include <stdio.h>
#define GUID_LEN 64

int returninid(DBHANDLE db) {
  int x = mysql_insert_id((MYSQL *)db);
  return x;
}

map<int, int> node_mysql_map;
map<int, int> mysql_node_map;
map<int, int> ignodeid_2_inodeid;
map<int, int> inodeid_2_ignodeid;

// igraph index, node id in mysql
// node id in mysql, igraph index
void create_node_mysql_map(
    const std::vector<MR_BILL_PIPELINE_ALL_NODE_VIEW> &pl_node,
    map<int, int> &node_mysql_map, map<int, int> &mysql_node_map) {
  for (size_t i = 0; i < pl_node.size(); i++) {
    MR_BILL_PIPELINE_NODE *node = (MR_BILL_PIPELINE_NODE *)&pl_node.at(i);
    node_mysql_map.insert(std::make_pair(i, node->id));
    mysql_node_map.insert(std::make_pair(node->id, i));
  }
}

// get value by key from map
int get_value_by_key(int key, const std::map<int, int> &my_map) {
  auto it = my_map.find(key);
  if (it == mysql_node_map.end()) {
    return -1;
  }
  return it->second;
}

// get all nodes`s edged by node pipeline_id
int
get_edges_by_nodes(const std::vector<MR_BILL_PIPELINE_ALL_NODE_VIEW> &pl_node,
                   std::vector<MR_BILL_PIPELINE_EDGE> &pl_edge,
                   std::map<int, int> &parentnodeid_firstnodeid,
                   std::map<int, int> &parentnodeid_lastchildnodeid,
                   DBHANDLE h_db) {
  if (pl_node.size() == 0) {
    return -1;
  }

  string where = " pipeline_id in (";
  for (size_t i = 0; i < pl_node.size(); i++) {
    MR_BILL_PIPELINE_ALL_NODE_VIEW n = pl_node.at(i);
    if (i != 0) {
      where.append(",");
    }
    char number[10] = { 0 };
    sprintf(number, "%d", n.pipeline_id);
    where.append(number);
  }
  where.append(")");

  int ret = query_data(pl_edge, h_db, query_sql[19], get_bill_pipeline_edge,
                       where.c_str());
  if (ret < 0) {
    return -1;
  }

  // deal first child edge(point to child start node)
  auto it = parentnodeid_firstnodeid.begin();
  while (it != parentnodeid_firstnodeid.end()) {
    int parent_id = it->first;
    int first_child_id = it->second;

    for (size_t i = 0; i < pl_edge.size(); i++) {
      MR_BILL_PIPELINE_EDGE *p = &pl_edge.at(i);

      if (p->trg_id == parent_id) {
        p->trg_id = first_child_id;
      }
    }
    it++;
  }

  // deal last child edge(child end node point to parent`s next node)
  it = parentnodeid_lastchildnodeid.begin();
  while (it != parentnodeid_lastchildnodeid.end()) {
    int parent_id = it->first;
    int last_child_id = it->second;

    for (size_t i = 0; i < pl_edge.size(); i++) {
      MR_BILL_PIPELINE_EDGE *p = &pl_edge.at(i);
      if (p->src_id == parent_id) {
        p->src_id = last_child_id;
      }
    }
    it++;
  }

  return 0;
}

// get id from table bill_pipeline_node by pipeline_id and ref_type
int get_id_by_pipelineid_and_reftype(DBHANDLE h_db, int pipeline_id,
                                     int ref_type) {
  std::vector<MR_BILL_PIPELINE_NODE> pipeline_node_vector;
  char tmp_where[1024] = { 0 };

  // get start node
  sprintf(tmp_where, "pipeline_id = %d and ref_type = %d", pipeline_id,
          ref_type);
  if (query_data(pipeline_node_vector, h_db, query_sql[23],
                 get_bill_pipeline_node, tmp_where) < 0) {
    return -1;
  }

  if (pipeline_node_vector.size() != 1) {
    return -1;
  }

  return pipeline_node_vector.at(0).id;
}

int
get_nodes_by_pipelineid(std::vector<MR_BILL_PIPELINE_ALL_NODE_VIEW> &pl_node,
                        std::map<int, int> &parentnodeid_firstnodeid,
                        std::map<int, int> &parentnodeid_lastchildnodeid,
                        DBHANDLE h_db, int pl_id) {
  std::vector<MR_BILL_PIPELINE_ALL_NODE_VIEW> nodes;
  string where = stringFormat("pipeline_id = %d", pl_id);

  // query nodes from view bill_pipeline_all_node_view
  if (query_data(nodes, h_db, query_sql[18], get_bill_pipeline_all_node_view,
                 where.c_str()) < 0) {
    return -1;
  }

  for (size_t i = 0; i < nodes.size(); i++) {
    MR_BILL_PIPELINE_ALL_NODE_VIEW node = nodes.at(i);
    if (node.ref_type == 3) {
      // save parent node id and parent`s first child id(start of sub node) and
      // last child id(end of sub node),
      // to change the edge later (replace edge`s parent id to it`s fist child
      // id and last child id)
      int id = get_id_by_pipelineid_and_reftype(h_db, node.ref_id, 4);
      if (id == -1) {
        return -1;
      }
      parentnodeid_firstnodeid.insert(make_pair(node.id, id));

      // get end node
      id = get_id_by_pipelineid_and_reftype(h_db, node.ref_id, 5);
      if (id == -1) {
        return -1;
      }
      parentnodeid_lastchildnodeid.insert(make_pair(node.id, id));

      // recursion get nodes
      get_nodes_by_pipelineid(pl_node, parentnodeid_firstnodeid,
                              parentnodeid_lastchildnodeid, h_db, node.ref_id);
    } else {
      // node
      pl_node.emplace_back(node);
    }
  }

  return 0;
}

// create igraph
int create_graph(igraph_t *g,
                 std::vector<MR_BILL_PIPELINE_ALL_NODE_VIEW> &pl_node,
                 std::vector<MR_BILL_PIPELINE_EDGE> &pl_edge, DBHANDLE h_db,
                 int pl_id) {
  assert(h_db != nullptr);
  assert(g_h_db != nullptr);
  assert(h_db == g_h_db);

  std::map<int, int> parentnodeid_firstnodeid;
  std::map<int, int> parentnodeid_lastnodeid;
  pl_edge.clear();
  pl_node.clear();

  node_mysql_map.clear();
  mysql_node_map.clear();
  ignodeid_2_inodeid.clear();
  inodeid_2_ignodeid.clear();

  // get nodes
  if (get_nodes_by_pipelineid(pl_node, parentnodeid_firstnodeid,
                              parentnodeid_lastnodeid, h_db, pl_id) < 0) {
    return -1;
  }

  // get edges and develop subnodes
  if (get_edges_by_nodes(pl_node, pl_edge, parentnodeid_firstnodeid,
                         parentnodeid_lastnodeid, h_db) < 0) {
    return -1;
  }

  // create index of igraph id and mysql node id
  create_node_mysql_map(pl_node, node_mysql_map, mysql_node_map);

  igraph_t graph;
  igraph_empty(&graph, pl_node.size(), IGRAPH_DIRECTED);

  for (size_t i = 0; i < pl_edge.size(); i++) {
    MR_BILL_PIPELINE_EDGE &e = pl_edge.at(i);
    int src_index = get_value_by_key(e.src_id, mysql_node_map);
    int trg_index = get_value_by_key(e.trg_id, mysql_node_map);
#ifdef _DEBUG_
    // printf("(%s->%s)\n", pl_node.at(src_index).ip_address.c_str(),
    // pl_node.at(trg_index).ip_address.c_str());
    printf("aa %d->%d (%d->%d),", e.src_id, e.trg_id, src_index, trg_index);
#endif
    // add egde to igraph
    igraph_add_edge(&graph, src_index, trg_index);
  }

  int first = get_value_by_key(pl_edge[0].src_id, mysql_node_map);
#ifdef _DEBUG_
  printf("first is %d\n", first);
#endif

  igraph_vector_t order;
  igraph_vector_init(&order, pl_node.size());
  igraph_bfs(&graph, first, nullptr, IGRAPH_OUT, false, nullptr, &order, nullptr,
             nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
#ifndef INTVEC
#define INTVEC(y, i) ((int)(((y).stor_begin)[(i)]))
#endif // INTVEC

  // get all nodes that needs to run

  map<int, int> mapNode;
  for (int j = 0; j < igraph_vector_size(&order); ++j) {
    if (INTVEC(order, j) >= 0) {
#ifdef _DEBUG_
      printf("og %d -> %d, ", j, INTVEC(order, j));
#endif //_DEBUG_
      ignodeid_2_inodeid.insert(std::make_pair(j, INTVEC(order, j)));
      inodeid_2_ignodeid.insert(std::make_pair(INTVEC(order, j), j));
      mapNode.insert(std::make_pair(INTVEC(order, j), j));
    }
  }
#ifdef _DEBUG_
  printf("\n");
#endif //_DEBUG_

  igraph_vector_t edge;
  // get the out style tree's all edges, the put them to vector e
  igraph_vector_init(&edge, pl_edge.size() * 2);

  for (size_t i = 0; i < pl_edge.size(); i++) {
    MR_BILL_PIPELINE_EDGE &e = pl_edge.at(i);
    // VECTOR(edge)[i * 2] = mapNode[mysql_node_map[e.src_id]];
    // VECTOR(edge)[i * 2 + 1] = mapNode[mysql_node_map[e.trg_id]];
    int src_id = mapNode[mysql_node_map[e.src_id]];
    int trg_id = mapNode[mysql_node_map[e.trg_id]];
    VECTOR(edge)[i * 2] = src_id;
    VECTOR(edge)[i * 2 + 1] = trg_id;

#ifdef _DEBUG_
    fprintf(stdout, "New Graph Eage: %d -> %d, %d -> %d, %d -> %d\n",
            // VECTOR(edge)[i * 2], VECTOR(edge)[i * 2 + 1],
            // ignodeid_2_inodeid[VECTOR(edge)[i * 2]],
            // ignodeid_2_inodeid[VECTOR(edge)[i * 2 + 1]],
            // node_mysql_map[ignodeid_2_inodeid[VECTOR(edge)[i * 2]]],
            // node_mysql_map[ignodeid_2_inodeid[VECTOR(edge)[i * 2 + 1]]]);
            src_id, trg_id, ignodeid_2_inodeid[src_id],
            ignodeid_2_inodeid[trg_id],
            node_mysql_map[ignodeid_2_inodeid[src_id]],
            node_mysql_map[ignodeid_2_inodeid[trg_id]]);
    fflush(stdout);
#endif //_DEBUG_
  }

// #ifdef _DEBUG_
//   for (int i = 0; i < igraph_vector_size(&edge); i += 2)
//     printf("kkkkk  %d -> %d, %d -> %d\n", INTVEC(edge, i), INTVEC(edge, i + 1),
//            ignodeid_2_inodeid[INTVEC(edge, i)],
//            ignodeid_2_inodeid[INTVEC(edge, i + 1)]);
// #endif //_DEBUG_

  igraph_create(g, &edge, graph.n, 1);
  igraph_destroy(&graph);
  igraph_vector_destroy(&order);
  igraph_vector_destroy(&edge);
  return 0;
}



int update_bill_exec_node(int pl_ex_id, int /*graph_id*/, int node_id,
                          itat::STATE_TYPE run_state, itat::STATE_TYPE check_state,
                          int /*code*/, const char */*strout*/,
                          const char */*strerr*/){
    if (run_state != check_state || node_id == 0){
#ifdef _DEBUG_
        cout << run_state << " " << check_state << " " << node_id << endl;
#endif
        return 0;
    }

    assert(g_h_db != nullptr);
    DBHANDLE h_db = g_h_db;// connect_db(g_mysql_db.host.c_str(), g_mysql_db.port, g_mysql_db.db_name.c_str(),
                           //     g_mysql_db.user_name.c_str(), g_mysql_db.user_pass.c_str());
    if (!h_db)
      return -1;

    string sql = "";
    if(run_state == (itat::STATE_TYPE)6){
        sql = stringFormat("insert into bill_exec_node(started_at, ex_pl_id, node_id, status) value(now(), %d, %d, %d)",
                            pl_ex_id, node_id, run_state);
    }else{
        string result_info = "";
        if(run_state == itat::STATE_TYPE(7)){
            result_info = "失败";
        } else if(run_state == itat::STATE_TYPE(8)){
            result_info = "超时";
        } else if(run_state == itat::STATE_TYPE(9)){
            result_info = "成功";
        } else if(run_state == itat::STATE_TYPE(10)){
            result_info = "等待用户确认";
        }
        else{
            // disconnect_db(h_db);
            return 0;
        }
        sql = stringFormat("update bill_exec_node set ended_at=now(),result_info=\'%s\',status=%d where ex_pl_id=%d and node_id=%d",
                     result_info.c_str(), run_state, pl_ex_id, node_id);
    }

    if (sql.length() == 0){
        // disconnect_db(h_db);
        return 0;
    }

#ifdef _DEBUG_
    cout << sql <<endl;
#endif
    int res = mysql_query(reinterpret_cast<MYSQL *>(h_db), sql.c_str());
    if (res) {
        fprintf(stdout, "\ninsert or update error: %s by %s\n",
                mysql_error(reinterpret_cast<MYSQL *>(h_db)), sql.c_str());
        // disconnect_db(h_db);
        return -4;
    }

    // disconnect_db(h_db);
    return 0;
}

int update_bill_exec_pipeline(int pl_ex_id, int /*graph_id*/, int node_id,
                          itat::STATE_TYPE run_state, itat::STATE_TYPE check_state,
                          int /*code*/, const char */*strout*/,
                          const char */*strerr*/){
    if (node_id != 0 || check_state != (itat::STATE_TYPE)5){
#ifdef _DEBUG_
        cout << check_state << " " << node_id << endl;
#endif
        return 0;
    }

    assert(g_h_db != nullptr);
    DBHANDLE h_db = g_h_db;  //connect_db(g_mysql_db.host.c_str(), g_mysql_db.port, g_mysql_db.db_name.c_str(),
                             //    g_mysql_db.user_name.c_str(), g_mysql_db.user_pass.c_str());
    if (!h_db)
      return -2;

    string sql = "";
    if(run_state == (itat::STATE_TYPE)7
            || run_state == (itat::STATE_TYPE)8){
        sql = stringFormat("update bill_exec_pipeline set result_status=%d where id=%d",
                            run_state, pl_ex_id);
    }else if(run_state == (itat::STATE_TYPE)9){
        sql = stringFormat("update bill_exec_pipeline set result_status=%d,result_info=\'执行成功\',ended_at=now() where id=%d",
                     run_state, pl_ex_id);
    }

    if (sql.length() == 0){
        // disconnect_db(h_db);
        return 0;
    }

#ifdef _DEBUG_
    cout << sql <<endl;
#endif
    int res = mysql_query(reinterpret_cast<MYSQL *>(h_db), sql.c_str());
    if (res) {
        fprintf(stdout, "\nupdate error: %s by %s\n",
                mysql_error(reinterpret_cast<MYSQL *>(h_db)), sql.c_str());
        // disconnect_db(h_db);
        return -4;
    }

    // disconnect_db(h_db);
    return 0;
}

int update_bill_checked_node(int /*pl_ex_id*/, int graph_id, int node_id,
                          itat::STATE_TYPE run_state, itat::STATE_TYPE check_state,
                          int /*code*/, const char */*strout*/,
                          const char */*strerr*/){
    if (node_id == 0 || check_state != run_state){
#ifdef _DEBUG_
        cout << check_state << " " << run_state << " " << node_id << endl;
#endif
        return 0;
    }

    assert(g_h_db != nullptr);
    DBHANDLE h_db = g_h_db; // connect_db(g_mysql_db.host.c_str(), g_mysql_db.port, g_mysql_db.db_name.c_str(),
                            //    g_mysql_db.user_name.c_str(), g_mysql_db.user_pass.c_str());
    if (!h_db)
      return -2;

    string sql = "";
    if(run_state == (itat::STATE_TYPE)1){
        sql = stringFormat("insert into bill_checked_node(started_at, status, ck_pl_id, node_id) value(now(),%d,\
(select id from bill_checked_pipeline where pipeline_id=%d and status = 1 order by id desc limit 1),%d)",
                            run_state, graph_id, node_id);
    }else{
        string strResultInfo = "";
        if(run_state == (itat::STATE_TYPE)3){
            strResultInfo = "脚本错误";
        }else if(run_state == (itat::STATE_TYPE)4){
            strResultInfo = "机器未联通";
        }else if(run_state == (itat::STATE_TYPE)5){
            strResultInfo = "检测成功";
        }else{
            // disconnect_db(h_db);
            return 0;
        }
        sql = stringFormat("update bill_checked_node set status=%d,result_info=\'%s\',ended_at=now() \
where ck_pl_id=(select id from bill_checked_pipeline where pipeline_id=%d and status=1 order by id desc limit 1) and node_id=%d",
                     run_state, strResultInfo.c_str(), graph_id, node_id);
    }

    if (sql.length() == 0){
        // disconnect_db(h_db);
        return 0;
    }

#ifdef _DEBUG_
    cout << sql <<endl;
#endif
    int res = mysql_query(reinterpret_cast<MYSQL *>(h_db), sql.c_str());
    if (res) {
        fprintf(stdout, "\ninsert or update error: %s by %s\n",
                mysql_error(reinterpret_cast<MYSQL *>(h_db)), sql.c_str());
        // disconnect_db(h_db);
        return -5;
    }

    // disconnect_db(h_db);
    return 0;
}

int update_bill_checked_pipeline(int pl_ex_id, int graph_id, int node_id,
                          itat::STATE_TYPE run_state, itat::STATE_TYPE check_state,
                          int /*code*/, const char */*strout*/,
                          const char */*strerr*/, int global_userid_){
    if (node_id != 0 || (itat::STATE_TYPE)0 != run_state){
#ifdef _DEBUG_
        cout << check_state << " " << run_state << " " << node_id << endl;
#endif
        return 0;
    }

    assert(g_h_db != nullptr);
    DBHANDLE h_db = g_h_db; // connect_db(g_mysql_db.host.c_str(), g_mysql_db.port, g_mysql_db.db_name.c_str(),
                            //            g_mysql_db.user_name.c_str(), g_mysql_db.user_pass.c_str());
    if (!h_db)
      return -2;

    string sql = "";
    if(check_state == (itat::STATE_TYPE)1){
        char type[10] = {0};
        if(pl_ex_id == 0){
            strcpy(type, "EC");
        }else{
            strcpy(type, "SC");
        }
        sql = stringFormat("insert into bill_checked_pipeline(started_at,type, status, pipeline_id, user_id) value(now(),\'%s\',%d,%d,%d)",
                            type, check_state, graph_id, global_userid_);
    }else{
        string strResultInfo = "";
        if(check_state == (itat::STATE_TYPE)2){
            strResultInfo = "检测异常";
        }else if(check_state == (itat::STATE_TYPE)5){
            strResultInfo = "检测成功";
        }else{
            // disconnect_db(h_db);
            return 0;
        }
        sql = stringFormat("update bill_checked_pipeline set status=%d,result_info=\'%s\',ended_at=now() \
where pipeline_id=%d and status = 1",
                     check_state, strResultInfo.c_str(), graph_id);
    }

    if (sql.length() == 0){
        // disconnect_db(h_db);
        return 0;
    }

#ifdef _DEBUG_
    cout << sql <<endl;
#endif
    int res = mysql_query(reinterpret_cast<MYSQL *>(h_db), sql.c_str());
    if (res) {
        fprintf(stdout, "\ninsert or update error: %s by %s\n",
                mysql_error(reinterpret_cast<MYSQL *>(h_db)), sql.c_str());
        // disconnect_db(h_db);
        return -5;
    }

    // disconnect_db(h_db);
    return 0;
}


