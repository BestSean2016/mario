#ifndef SALTMAN_HPP
#define SALTMAN_HPP

#define _USE_VECTOR_AS_SET_

#include <igraph/igraph.h>
#include "str.h"
#include "state.hpp"


#ifdef BUFSIZE
#undef BUFSIZE
#endif //BUFSIZE
#define BUFSIZE 32768

namespace itat {

typedef char HTTPBUF[BUFSIZE];

typedef std::map<std::string, std::string> HttpReqKeyValue;
typedef std::map<std::string, std::string> ApiUriSet;
typedef int (*action_fun) (const HttpReqKeyValue& requests);
typedef std::map<std::string, action_fun> ActionSet;
typedef std::pair<std::string, action_fun> AsPair;
typedef std::pair<std::string, std::string> KVPair;

typedef struct HttpServerParam {
  ActionSet actions;
  ApiUriSet uris;
  std::string port;
} HttpServerParam;

typedef enum HTTP_REQUEST_METHOD_TYPE {
  HTTP_REQUEST_METHOD_POST,
  HTTP_REQUEST_METHOD_GET,
}HTTP_REQUEST_METHOD_TYPE;

typedef struct HttpRequest {
  HTTP_REQUEST_METHOD_TYPE method;
  std::string uri;
  std::string content;
  int content_len;
  HttpReqKeyValue request;
} HttpRequest;


typedef std::set<std::string> SetUri;
typedef std::map<std::string, SetUri> URI_REQUEST;

typedef struct HTTP_CLIENT_PARAM {
  int socket;
  struct sockaddr_in cli_addr;
  URI_REQUEST& uris;

  HTTP_CLIENT_PARAM(URI_REQUEST& ureq) : uris(ureq) {}
} HTTP_CLIENT_PARAM;

class saltman;

typedef void* PARAM;
typedef int (*response_function) (saltman* sm, const char* data, size_t len, PARAM param1, PARAM param2);
typedef int (*api_function) (const char *hostname, int port, const char* target, PARAM param1, PARAM param2);

typedef struct HTTP_API_PARAM {
    char hostname[32] = "";
    int port = 0;
    int sockfd = 0;
    response_function rf = nullptr;
    PARAM param1 = nullptr;
    PARAM param2 = nullptr;

    HTTP_API_PARAM() {}

    HTTP_API_PARAM (const char* host, const int portno, response_function fun, PARAM p1, PARAM p2)
      : port(portno), rf(fun), param1(p1), param2(p2) {
      strcpy_s(hostname, 31, host);
    }

    void set_param(const char* host, const int portno, response_function fun, PARAM p1, PARAM p2) {
      port = portno, rf = fun, param1 = p1, param2 = p2;
      strcpy_s(hostname, 31, host);
    }
} HTTP_API_PARAM;


typedef enum RETURN_TYPE {
    RETURN_TYPE_OBJECT,
    RETURN_TYPE_BOOL,
    RETURN_TYPE_STRING,
} RETURN_TYPE;

typedef struct salt_job_ret {
  int64_t ple_id = 0;            ///PIPELINE EXECUTIVE ID
  RETURN_TYPE rettype = RETURN_TYPE_OBJECT;
  std::string tag       = "";
  std::string stamp     = "";
  time_t      stamp_sec = 0;
  uint32_t    stamp_usec= 0;
  uint32_t    pid = 0;
  int         retcode = 0;
  std::string stderr = "";
  std::string stdout = "";
  bool        success = false;
  std::string cmd = "";
  std::string jid = "";
  std::string fun = "";
  std::string minion_id = "";
} SALT_JOB_RET;



typedef enum SALT_JOB_EVENT_TYPE {
    SALT_JOB_TYPE_IGNORE,
    SALT_JOB_TYPE_NEW,
    SALT_JOB_TYPE_RET,
} SALT_JOB_EVENT_TYPE;


typedef struct salt_job {
  int64_t     ple_id      = 0;            ///PIPELINE EXECUTIVE ID
  int64_t     node_id     = -1;
  void*       inode       = 0;
  time_t      stamp_sec   = time(0);
  uint32_t    timerout    = 60;
  size_t      retnum      = 0;
  size_t      success_num = 0;
  STATE_TYPE  status      = ST_running;
  std::string tag        = "";
  std::string tgt_type   = "";
  std::string jid        = "";
  std::string tgt        = "";
  std::string stamp      = "";
  std::string user       = "";
  std::string fun        = "";
  std::vector<std::string> arg;
  std::vector<std::string> minions;

  MapStr2Ptr<SALT_JOB_RET> minion_ret;

  salt_job() {}
  salt_job(int64_t pleid, int64_t nodeid) :
    ple_id  (pleid ),
    node_id (nodeid)
  {}

  ~salt_job() {
    for(auto& p : minion_ret) {
      delete (p.second);
    }
    minion_ret.clear();
  }
} SALT_JOB;


typedef MapStr2Ptr<SALT_JOB> MAP_SALT_JOB;


typedef struct NODESET {
#ifdef _USE_VECTOR_AS_SET_
  std::vector<int> prepare_to_run_;
  // std::vector<int> running_set_;
  std::vector<int> run_set_;
  //std::vector<int> done_set_;
  std::vector<int> issues_;
#else //#define _USE_VECTOR_AS_SET_
  std::set<int> prepare_to_run_;
  std::set<int> running_set_;
  std::set<int> run_set_;
  std::set<int> done_set_;
#endif //#define _USE_VECTOR_AS_SET_
}NODESET;


class Pipeline;
class iNode;

#define TOKEN_LEN 128


typedef struct SaltAPiParam {
    char g_token[TOKEN_LEN] = {0};
    std::mutex g_job_mutex;
    bool g_run_check_timer_out = false;
    int g_run;
} SaltAPiParam;

class saltman {
public:
    saltman();
    ~saltman();

    void init(Pipeline* pl);
    int start();
    void stop();

    void dump_jobmap();
    void check_time_out();

    STATE_TYPE check_node(iNode* node);
    STATE_TYPE run_node(iNode *node);

    SaltAPiParam sap_;

private:
    std::thread thread_event_;
    std::thread thread_timeout_;

    HTTP_API_PARAM param_check_;
    HTTP_API_PARAM param_run_;
    HTTP_API_PARAM param_event_;

    MAP_SALT_JOB map_job_;
    Pipeline* pl_ = nullptr;
    igraph_t* g_ = nullptr;

    NODESET* nodeset_ = nullptr;
};


extern bool vec_find(std::vector<int> &vec, int a);
extern bool vec_insert(std::vector<int> &vec, int a);
extern bool vec_erase(std::vector<int> &vec, int a);

}



#endif // SALTMAN_HPP
