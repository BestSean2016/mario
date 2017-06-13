#ifndef DJIAGO_API_HPP
#define DJIAGO_API_HPP

#include "itat.h"
#include "itat_global.h"
#include "state.hpp"
#include "httpapi.hpp"
#include "mario_sql.h"
#include <threadpool.h>

#include <Python.h>

//
// #ifdef __cplusplus
// extern "C" {
// #endif //_cpp
//
// typedef int (*bill_message) (int pl_ex_id,
//                              int graph_id,
//                              int node_id,
//                              int run_state,
//                              int check_state,
//                              int code,
//                              const char *strout,
//                              const char *strerr);
//
//
//
//
// extern bill_message bm_;
//
// int set_bill_message(bill_message bm);
//
//
// #ifdef __cplusplus
// }
// #endif //_cpp
//

namespace itat {

//int django_api_send_graph_status(int graph_id, int node_id, STATE_TYPE run_state, STATE_TYPE check_state,
//                                 int code = 0, const std::string &stdout = "",
//                                 const std::string &stderr = "");


typedef struct buffers {
    char * cmd;
    char * buf;
    int ret;
} buffers;



class DjangoAPI {
public:
    DjangoAPI();
    ~DjangoAPI();

    void set_run(NODEMAPS* maps, DBHANDLE h_db) { maps_ = maps, g_h_db_ = h_db; }
    void set_user(int userid) { global_userid_ = userid; }
    int send_graph_status(int pl_ex_id,
                         int graph_id,
                         int node_id,
                         STATE_TYPE run_state,
                         STATE_TYPE check_state,
                         int code = 0,
                         const char* strout = {""},
                         const char* strerr = {""},
                         const char* why = nullptr);


private:
    char buf_[BUFSIZ * 8];   //8192 * 8
    char cmd_[BUFSIZ * 8];   //8192 * 8

private:
    void make_send_msg();
    NODEMAPS* maps_ = nullptr;
    DBHANDLE g_h_db_ = nullptr;
    int global_userid_ = 0;
    threadpool_t* g_thpool;
};



} // namespace itat

#endif // DJIAGO_API_HPP
