#ifndef DJIAGO_API_HPP
#define DJIAGO_API_HPP

#include "itat.h"
#include "itat_global.h"
#include "state.hpp"


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



class DjangoAPI {
public:
    DjangoAPI();
    ~DjangoAPI();

    int init(const char *py_message_path);

    int send_graph_status(int pl_ex_id,
                         int graph_id,
                         int node_id,
                         STATE_TYPE run_state,
                         STATE_TYPE check_state,
                         int code = 0,
                         const char *strout = "",
                         const char *strerr = "");


private:
    PyObject *pName = nullptr;
    PyObject *pModule = nullptr;
    PyObject *pDict = nullptr;
    PyObject *pClass = nullptr;
    PyObject *pInstance = nullptr;
    PyObject *pValue = nullptr;

private:
    bool inited_ = false;
};



extern DjangoAPI dj_;

} // namespace itat

#endif // DJIAGO_API_HPP
