#include "djangoapi.hpp"

std::string python_filename = {"bill_message"};

namespace itat {

DjangoAPI dj_;

DjangoAPI::DjangoAPI() {}

int DjangoAPI::init(const char* py_message_path) {
    if (inited_) return 0;

    inited_ = true;
    Py_Initialize();

    int ret = 0;
    static const char* class_name = {"Message"};

    // Py_Initialize();
    if (!ret) ret = (nullptr == (pName = PyString_FromString(py_message_path)));
    if (!ret) ret = (nullptr == (pModule = PyImport_Import(pName)));
    if (!ret) ret = (nullptr == (pDict = PyModule_GetDict(pModule)));

    // Build the name of a callable class
    if (!ret) ret = (nullptr == (pClass = PyDict_GetItemString(pDict, class_name)));

    // Create an instance of the class
    if (!ret) {
        if (PyCallable_Check(pClass))
        {
            ret = (nullptr == (pInstance = PyObject_CallObject(pClass, NULL)));
        } else {
            ret = -1;
        }
    }

    return ret;
}

DjangoAPI::~DjangoAPI() {

    // Clean up
    // Py_DECREF(pInstance);
    // Py_DECREF(pClass);
    if (pDict) Py_DECREF(pDict);
    if (pModule) Py_DECREF(pModule);
    if (pName) Py_DECREF(pName);


    // Py_Finalize();
}


int DjangoAPI::send_graph_status(int pl_ex_id,
                     int graph_id,
                     int node_id,
                     STATE_TYPE run_state,
                     STATE_TYPE check_state,
                     int code,
                     const char* strout,
                     const char* strerr) {
    static const char* function_name = {"accept"};

    int ret = 0;
    if (pInstance) {
        //node_id, node_status, pl_id, pl_status, pl_ex_id, code, strout, strerr
        ret = (nullptr == (pValue = PyObject_CallMethod(pInstance,
                                                        (char*)function_name,
                                                        "(iiiiiiss)",
                                                        pl_ex_id,
                                                        graph_id,
                                                        node_id,
                                                        run_state,
                                                        check_state,
                                                        code,
                                                        strout,
                                                        strerr)));
        if (!ret) {
            printf("Return of call : %ld\n", PyInt_AsLong(pValue));
            Py_DECREF(pValue);
        } else {
            PyErr_Print();
        }
    } else {
        ret = -1;
    }


    //assert(bm_ != 0);
    // cout << "bm_ " << (uint64_t)bm_ << endl;
    //
    // if (bm_)
    //     return bm_(
    //             pl_ex_id,
    //             graph_id,
    //             node_id,
    //             run_state,
    //             check_state,
    //             code,
    //             strout,
    //             strerr);
    // cout << "bm_ 2\n";
    // return -1;
}

} // namespace itat

//
// #ifdef __cplusplus
// extern "C" {
// #endif //_cpp
//
//
// bill_message bm_ = nullptr;
//
//
//
// int set_bill_message(bill_message bm) {
//     if (bm == nullptr) return -1;
//     bm_ = bm;
//     return 0;
// }
//
// #ifdef __cplusplus
// }
// #endif //_cpp
//
