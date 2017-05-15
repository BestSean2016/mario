#ifndef MARIO_MYSQL_H
#define MARIO_MYSQL_H

#include <functional>
#include <iostream>
#include <string.h>
#include <time.h>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <sstream>
#include <mysql/mysql.h>
#include <igraph/igraph.h>
#include "state.hpp"

using namespace std;

#define mysql_str(dptr) ((dptr) ? dptr : "")
#define mysql_atoll(dptr) ((dptr) ? atoll(dptr) : 0)
#define mysql_atol(dptr) ((dptr) ? atol(dptr) : 0)
#define mysql_atoi(dptr) ((dptr) ? atoi(dptr) : 0)

typedef void *DBHANDLE;

template <typename T>
int query_data(std::vector<T> &vecDataSet, DBHANDLE dbh, const char *sql,
               std::function<int(void*, MYSQL_ROW &)> get_field,
               const char *where = 0) {
    if (!dbh)
        return -1;

    int nCount = 0;

    char *query = 0;
    if (where) {
        query = new char[strlen(sql) + strlen(where) + strlen(" WHERE ") + 1];
        snprintf(query, strlen(sql) + strlen(where) + strlen(" WHERE ") + 1,
                 "%s WHERE %s", sql, where);
    } else
        query = (char *)sql;
    //    保证结构体是干净的
    vecDataSet.clear();

    int res = mysql_query(reinterpret_cast<MYSQL *>(dbh), query);
    if (res) {
        fprintf(stdout, "SELECT error: %s by %s", mysql_error(reinterpret_cast<MYSQL *>(dbh)), query);
        if (where)
            delete query;
        return -2;
    } else {
        MYSQL_RES *res_ptr = mysql_store_result(reinterpret_cast<MYSQL *>(dbh));

        if (res_ptr) {
            nCount = mysql_num_rows(res_ptr);
            if (nCount <= 0) {
                mysql_free_result(res_ptr);
                vecDataSet.clear();
                return 0;
            }

            MYSQL_ROW mysql_row;
            while ((mysql_row = mysql_fetch_row(res_ptr))) {
                T t;
                get_field(&t, mysql_row);
                vecDataSet.emplace_back(t);
            }

            if (mysql_errno(reinterpret_cast<MYSQL *>(dbh))) {
                fprintf(stdout, "Retrive an error: %s", mysql_error(reinterpret_cast<MYSQL *>(dbh)));
                nCount = -3;
            }
            mysql_free_result(res_ptr);
        }
    }
    if (where)
        delete[] query;
    return nCount;
}

template <typename T>
int insert_data(const T *t, DBHANDLE dbh,
                std::function<int(void*, string &)> get_field_string){
    string sql_insert = "";
    get_field_string((void*)t, sql_insert);
    printf("%s", sql_insert.c_str());

    int res = mysql_query(reinterpret_cast<MYSQL *>(dbh), sql_insert.c_str());
    if (res) {
        fprintf(stdout, "\ninsert error: %s by %s\n",
                mysql_error(reinterpret_cast<MYSQL *>(dbh)), sql_insert.c_str());
        return -2;
    }
    return 0;
}

template <typename T>
int update_data(const T *t, DBHANDLE dbh,
                std::function<int(void*, string &)> get_field_string, const char *where){
    string sql_update = "";
    get_field_string((void*)t, sql_update);
    //printf("%s", sql_update.c_str());

    sql_update.append(" where ");
    sql_update.append(where);
    int res = mysql_query(reinterpret_cast<MYSQL *>(dbh), sql_update.c_str());
    if (res) {
        fprintf(stdout, "\ninsert error: %s by %s\n",
                mysql_error(reinterpret_cast<MYSQL *>(dbh)), sql_update.c_str());
        return -2;
    }
    return 0;
}

extern std::string stringFormat(const std::string fmt_str, ...);
extern DBHANDLE connect_db(const char *host, int port, const char *db,const char *user, const char *passwd);
extern void disconnect_db(DBHANDLE dbh);
extern int exec_db(DBHANDLE db, const char*sql);
extern string getLocalTime();
extern int returninid(DBHANDLE db);

/*tabel:auth_group*/
typedef struct MR_AUTH_GROUP{
    int id;
    string name;
}MR_AUTH_GROUP;

/*tabel:auth_group_permissions*/
typedef struct MR_AUTH_GROUP_PERMISSIONS{
    int id;
    int group_id;
    int permission_id;
}MR_AUTH_GROUP_PERMISSIONS;

/*tabel:auth_permission*/
typedef struct MR_AUTH_PERMISSION{
    int id;
    string name;
    int content_type_id;
    string codename;
}MR_AUTH_PERMISSION;

/*tabel:auth_user*/
typedef struct MR_AUTH_USER{
    int id;
    string password;
    time_t last_login;
    int is_superuser;
    string username;
    string first_name;
    string last_name;
    string email;
    int is_staff;
    int is_active;
    time_t date_joined;
}MR_AUTH_USER;

/*tabel:auth_user_groups*/
typedef struct MR_AUTH_USER_GROUPS{
    int id;
    int user_id;
    int group_id;
}MR_AUTH_USER_GROUPS;

/*tabel:auth_user_user_permissions*/
typedef struct MR_AUTH_USER_USER_PERMISSIONS{
    int id;
    int user_id;
    int permission_id;
}MR_AUTH_USER_USER_PERMISSIONS;

/*tabel:authtoken_token*/
typedef struct MR_AUTHTOKEN_TOKEN{
    string key;
    time_t created;
    int user_id;
}MR_AUTHTOKEN_TOKEN;

/*tabel:bill_alarm*/
typedef struct MR_BILL_ALARM{
    int id;
    int level;
    string ip_address;
    string title;
    string result_info;
    time_t started_at;
    int pipeline_id;
    int node_id;
    int operator_id;
    int is_exec;
    int ex_pl_id;
}MR_BILL_ALARM;

/*tabel:bill_alarm_user*/
typedef struct MR_BILL_ALARM_USER{
    int id;
    string status;
    int alarm_id;
    int user_id;
}MR_BILL_ALARM_USER;

/*tabel:bill_checked_node*/
typedef struct MR_BILL_CHECKED_NODE{
    int id;
    string host;
    string script_name;
    time_t started_at;
    time_t ended_at;
    int status;
    string result_info;
    int ck_pl_id;
    int node_id;
}MR_BILL_CHECKED_NODE;

/*tabel:bill_checked_pipeline*/
typedef struct MR_BILL_CHECKED_PIPELINE{
    int id;
    string type;
    time_t started_at;
    time_t ended_at;
    int status;
    string result_info;
    int pipeline_id;
    int user_id;
}MR_BILL_CHECKED_PIPELINE;

/*tabel:bill_exec_log*/
typedef struct MR_BILL_EXEC_LOG{
    int id;
    int status;
    string result_info;
    time_t created_at;
    int ex_pl_id;
    int node_id;
    int operator_id;
}MR_BILL_EXEC_LOG;

/*tabel:bill_exec_node*/
typedef struct MR_BILL_EXEC_NODE{
    int id;
    string host;
    string script_name;
    time_t started_at;
    time_t ended_at;
    string result_info;
    int warning_level;
    int ex_pl_id;
    int node_id;
}MR_BILL_EXEC_NODE;

/*tabel:bill_exec_pipeline*/
typedef struct MR_BILL_EXEC_PIPELINE{
    int id;
    time_t started_at;
    time_t ended_at;
    int type;
    string result_info;
    int result_status;
    int pipeline_id;
    int user_id;
}MR_BILL_EXEC_PIPELINE;

/*tabel:bill_exec_pipeline_view*/
typedef struct MR_BILL_EXEC_PIPELINE_VIEW{
    int id;
    string name;
    int user_id;
    string desc;
    int lock_on;
    int status;
}MR_BILL_EXEC_PIPELINE_VIEW;

/*tabel:bill_host*/
typedef struct MR_BILL_HOST{
    int id;
    string name;
    string ip_address;
    string minion_id;
    string desc;
}MR_BILL_HOST;

/*tabel:bill_node_type*/
typedef struct MR_BILL_NODE_TYPE{
    int id;
    string name;
    string icon;
    string desc;
}MR_BILL_NODE_TYPE;

/*tabel:bill_pipeline*/
typedef struct MR_BILL_PIPELINE{
    int id;
    string old_id;
    string name;
    string desc;
    int lock_on;
    int status;
    time_t created_at;
    time_t updated_at;
    time_t reviewed_at;
    time_t deleted_at;
    int creator_id;
    int deletor_id;
    int group_id;
    int modifier_id;
    int reviewer_id;
}MR_BILL_PIPELINE;

/*tabel:bill_pipeline_all_node_view*/
typedef struct MR_BILL_PIPELINE_ALL_NODE_VIEW{
    int id;
    int ref_id;
    string old_id;
    int ref_type;
    string name;
    string minion_id;
    int timeout;
    string argv;
    string desc;
    time_t created_at;
    time_t updated_at;
    int creator_id;
    int modifier_id;
    int pipeline_id;
    string ip_address;
    string position_x;
    string position_y;
    string command;
    string script_name;
    string script_content;
    int script_type;
    string script_file_path;
    string script_argv;
    int script_argc;
    int status;

    static MR_BILL_PIPELINE_ALL_NODE_VIEW* clone(MR_BILL_PIPELINE_ALL_NODE_VIEW* n) {
            auto nn = new MR_BILL_PIPELINE_ALL_NODE_VIEW;
            nn->id = n->id;
            nn->ref_id = n->ref_id;
            nn->old_id = n->old_id;
            nn->ref_type = n->ref_type;
            nn->name = n->name;
            nn->minion_id = n->minion_id;
            nn->timeout = n->timeout;
            nn->argv = n->argv;
            nn->desc = n->desc;
            nn->created_at = n->created_at;
            nn->updated_at = n->updated_at;
            nn->creator_id = n->creator_id;
            nn->modifier_id = n->modifier_id;
            nn->pipeline_id = n->pipeline_id;
            nn->ip_address = n->ip_address;
            nn->position_x = n->position_x;
            nn->position_y = n->position_y;
            nn->command = n->command;
            nn->script_name = n->script_name;
            nn->script_content = n->script_content;
            nn->script_type = n->script_type;
            nn->script_file_path = n->script_file_path;
            nn->script_argv = n->script_argv;
            nn->script_argc = n->script_argc;
            nn->status = n->status;

            return nn;
        }
}MR_BILL_PIPELINE_ALL_NODE_VIEW;
typedef MR_BILL_PIPELINE_ALL_NODE_VIEW MARIO_NODE;

/*tabel:bill_pipeline_edge*/
typedef struct MR_BILL_PIPELINE_EDGE{
    int id;
    string desc;
    time_t created_at;
    time_t updated_at;
    int creator_id;
    int modifier_id;
    int pipeline_id;
    int src_id;
    int trg_id;
}MR_BILL_PIPELINE_EDGE;
typedef MR_BILL_PIPELINE_EDGE MARIO_EDGE;


/*tabel:bill_pipeline_group*/
typedef struct MR_BILL_PIPELINE_GROUP{
    int id;
    string name;
    int parent_id;
    string old_id;
}MR_BILL_PIPELINE_GROUP;

/*tabel:bill_pipeline_info*/
typedef struct MR_BILL_PIPELINE_INFO{
    int id;
    string key;
    string value;
    int pipeline_id;
}MR_BILL_PIPELINE_INFO;

/*tabel:bill_pipeline_json*/
typedef struct MR_BILL_PIPELINE_JSON{
    int id;
    string json_str;
    int pipeline_id;
}MR_BILL_PIPELINE_JSON;

/*tabel:bill_pipeline_node*/
typedef struct MR_BILL_PIPELINE_NODE{
    int id;
    string old_id;
    int ref_id;
    int ref_type;
    string name;
    string minion_id;
    int timeout;
    string argv;
    string desc;
    time_t created_at;
    time_t updated_at;
    int creator_id;
    int modifier_id;
    int pipeline_id;
}MR_BILL_PIPELINE_NODE;

/*tabel:bill_pipeline_node_info*/
typedef struct MR_BILL_PIPELINE_NODE_INFO{
    int id;
    string position_x;
    string position_y;
    int width;
    int height;
    int node_id;
}MR_BILL_PIPELINE_NODE_INFO;

/*tabel:bill_pipeline_node_view*/
typedef struct MR_BILL_PIPELINE_NODE_VIEW{
    int id;
    int ref_id;
    int ref_type;
    string name;
    string minion_id;
    int timeout;
    string argv;
    string desc;
    time_t created_at;
    time_t updated_at;
    int creator_id;
    int modifier_id;
    int pipeline_id;
    string ip_address;
    string script_name;
}MR_BILL_PIPELINE_NODE_VIEW;

/*tabel:bill_script*/
typedef struct MR_BILL_SCRIPT{
    int id;
    string name;
    int type;
    string content;
    int timeout;
    string desc;
    string file_path;
    string target_path;
    int argc;
    string argv;
    string md5;
    string screen_path;
    string log_path;
    time_t created_at;
    time_t updated_at;
    int host_id;
}MR_BILL_SCRIPT;

/*tabel:django_admin_log*/
typedef struct MR_DJANGO_ADMIN_LOG{
    int id;
    time_t action_time;
    string object_id;
    string object_repr;
    int action_flag;
    string change_message;
    int content_type_id;
    int user_id;
}MR_DJANGO_ADMIN_LOG;

/*tabel:django_content_type*/
typedef struct MR_DJANGO_CONTENT_TYPE{
    int id;
    string app_label;
    string model;
}MR_DJANGO_CONTENT_TYPE;

/*tabel:django_migrations*/
typedef struct MR_DJANGO_MIGRATIONS{
    int id;
    string app;
    string name;
    time_t applied;
}MR_DJANGO_MIGRATIONS;

/*tabel:django_session*/
typedef struct MR_DJANGO_SESSION{
    string session_key;
    string session_data;
    time_t expire_date;
}MR_DJANGO_SESSION;


extern const char *query_sql[];

extern int get_auth_group(void *r, MYSQL_ROW& row);
extern int get_auth_group_to_insert_sql(void *r, string & str);
extern int get_auth_group_to_update_sql(void *r, string & str);
extern int get_auth_group_permissions(void *r, MYSQL_ROW& row);
extern int get_auth_group_permissions_to_insert_sql(void *r, string & str);
extern int get_auth_group_permissions_to_update_sql(void *r, string & str);
extern int get_auth_permission(void *r, MYSQL_ROW& row);
extern int get_auth_permission_to_insert_sql(void *r, string & str);
extern int get_auth_permission_to_update_sql(void *r, string & str);
extern int get_auth_user(void *r, MYSQL_ROW& row);
extern int get_auth_user_to_insert_sql(void *r, string & str);
extern int get_auth_user_to_update_sql(void *r, string & str);
extern int get_auth_user_groups(void *r, MYSQL_ROW& row);
extern int get_auth_user_groups_to_insert_sql(void *r, string & str);
extern int get_auth_user_groups_to_update_sql(void *r, string & str);
extern int get_auth_user_user_permissions(void *r, MYSQL_ROW& row);
extern int get_auth_user_user_permissions_to_insert_sql(void *r, string & str);
extern int get_auth_user_user_permissions_to_update_sql(void *r, string & str);
extern int get_authtoken_token(void *r, MYSQL_ROW& row);
extern int get_authtoken_token_to_insert_sql(void *r, string & str);
extern int get_authtoken_token_to_update_sql(void *r, string & str);
extern int get_bill_alarm(void *r, MYSQL_ROW& row);
extern int get_bill_alarm_to_insert_sql(void *r, string & str);
extern int get_bill_alarm_to_update_sql(void *r, string & str);
extern int get_bill_alarm_user(void *r, MYSQL_ROW& row);
extern int get_bill_alarm_user_to_insert_sql(void *r, string & str);
extern int get_bill_alarm_user_to_update_sql(void *r, string & str);
extern int get_bill_checked_node(void *r, MYSQL_ROW& row);
extern int get_bill_checked_node_to_insert_sql(void *r, string & str);
extern int get_bill_checked_node_to_update_sql(void *r, string & str);
extern int get_bill_checked_pipeline(void *r, MYSQL_ROW& row);
extern int get_bill_checked_pipeline_to_insert_sql(void *r, string & str);
extern int get_bill_checked_pipeline_to_update_sql(void *r, string & str);
extern int get_bill_exec_log(void *r, MYSQL_ROW& row);
extern int get_bill_exec_log_to_insert_sql(void *r, string & str);
extern int get_bill_exec_log_to_update_sql(void *r, string & str);
extern int get_bill_exec_node(void *r, MYSQL_ROW& row);
extern int get_bill_exec_node_to_insert_sql(void *r, string & str);
extern int get_bill_exec_node_to_update_sql(void *r, string & str);
extern int get_bill_exec_pipeline(void *r, MYSQL_ROW& row);
extern int get_bill_exec_pipeline_to_insert_sql(void *r, string & str);
extern int get_bill_exec_pipeline_to_update_sql(void *r, string & str);
extern int get_bill_exec_pipeline_view(void *r, MYSQL_ROW& row);
extern int get_bill_exec_pipeline_view_to_insert_sql(void *r, string & str);
extern int get_bill_exec_pipeline_view_to_update_sql(void *r, string & str);
extern int get_bill_host(void *r, MYSQL_ROW& row);
extern int get_bill_host_to_insert_sql(void *r, string & str);
extern int get_bill_host_to_update_sql(void *r, string & str);
extern int get_bill_node_type(void *r, MYSQL_ROW& row);
extern int get_bill_node_type_to_insert_sql(void *r, string & str);
extern int get_bill_node_type_to_update_sql(void *r, string & str);
extern int get_bill_pipeline(void *r, MYSQL_ROW& row);
extern int get_bill_pipeline_to_insert_sql(void *r, string & str);
extern int get_bill_pipeline_to_update_sql(void *r, string & str);
extern int get_bill_pipeline_all_node_view(void *r, MYSQL_ROW& row);
extern int get_bill_pipeline_all_node_view_to_insert_sql(void *r, string & str);
extern int get_bill_pipeline_all_node_view_to_update_sql(void *r, string & str);
extern int get_bill_pipeline_edge(void *r, MYSQL_ROW& row);
extern int get_bill_pipeline_edge_to_insert_sql(void *r, string & str);
extern int get_bill_pipeline_edge_to_update_sql(void *r, string & str);
extern int get_bill_pipeline_group(void *r, MYSQL_ROW& row);
extern int get_bill_pipeline_group_to_insert_sql(void *r, string & str);
extern int get_bill_pipeline_group_to_update_sql(void *r, string & str);
extern int get_bill_pipeline_info(void *r, MYSQL_ROW& row);
extern int get_bill_pipeline_info_to_insert_sql(void *r, string & str);
extern int get_bill_pipeline_info_to_update_sql(void *r, string & str);
extern int get_bill_pipeline_json(void *r, MYSQL_ROW& row);
extern int get_bill_pipeline_json_to_insert_sql(void *r, string & str);
extern int get_bill_pipeline_json_to_update_sql(void *r, string & str);
extern int get_bill_pipeline_node(void *r, MYSQL_ROW& row);
extern int get_bill_pipeline_node_to_insert_sql(void *r, string & str);
extern int get_bill_pipeline_node_to_update_sql(void *r, string & str);
extern int get_bill_pipeline_node_info(void *r, MYSQL_ROW& row);
extern int get_bill_pipeline_node_info_to_insert_sql(void *r, string & str);
extern int get_bill_pipeline_node_info_to_update_sql(void *r, string & str);
extern int get_bill_pipeline_node_view(void *r, MYSQL_ROW& row);
extern int get_bill_pipeline_node_view_to_insert_sql(void *r, string & str);
extern int get_bill_pipeline_node_view_to_update_sql(void *r, string & str);
extern int get_bill_script(void *r, MYSQL_ROW& row);
extern int get_bill_script_to_insert_sql(void *r, string & str);
extern int get_bill_script_to_update_sql(void *r, string & str);
extern int get_django_admin_log(void *r, MYSQL_ROW& row);
extern int get_django_admin_log_to_insert_sql(void *r, string & str);
extern int get_django_admin_log_to_update_sql(void *r, string & str);
extern int get_django_content_type(void *r, MYSQL_ROW& row);
extern int get_django_content_type_to_insert_sql(void *r, string & str);
extern int get_django_content_type_to_update_sql(void *r, string & str);
extern int get_django_migrations(void *r, MYSQL_ROW& row);
extern int get_django_migrations_to_insert_sql(void *r, string & str);
extern int get_django_migrations_to_update_sql(void *r, string & str);
extern int get_django_session(void *r, MYSQL_ROW& row);
extern int get_django_session_to_insert_sql(void *r, string & str);
extern int get_django_session_to_update_sql(void *r, string & str);


extern int create_graph(igraph_t *g, std::vector<MR_BILL_PIPELINE_ALL_NODE_VIEW> &pl_node,
                        std::vector<MR_BILL_PIPELINE_EDGE> &pl_edge, DBHANDLE h_db,
                        int pl_id);


extern map<int, int> node_mysql_map;
extern map<int, int> mysql_node_map;
extern map<int, int> ignodeid_2_inodeid;
extern map<int, int> inodeid_2_ignodeid;

extern int get_value_by_key(int key, const std::map<int, int> &my_map);




#endif // MARIO_MYSQL_H
