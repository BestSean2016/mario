#ifndef HTTP_API_HPP
#define HTTP_API_HPP

#include "itat_global.h"
#include <string.h>
#include "str.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <set>


#include <netdb.h>

#ifdef BUFSIZE
#undef BUFSIZE
#endif //BUFSIZE

#define BUFSIZE 32768

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

typedef void* PARAM;

typedef int (*response_function) (const char* data, size_t len, PARAM param1, PARAM param2);
typedef int (*api_function) (const char *hostname, int port, const char* target, PARAM param1, PARAM param2);



typedef struct HTTP_API_PARAM {
    char hostname[32];
    int port;
    response_function rf;
    PARAM param1;
    PARAM param2;

    HTTP_API_PARAM (const char* host, const int portno, response_function fun, PARAM p1, PARAM p2)
      : port(portno), rf(fun), param1(p1), param2(p2) {
      strcpy_s(hostname, 31, host);
    }
} HTTP_API_PARAM;


extern ITAT_API void show_cstring(const char *cstring, size_t len);
extern ITAT_API std::vector<std::string> split(const std::string &s, char delim);

template <typename T>
ITAT_API void output_vector(std::vector<T> vec) {
  std::cout << "[";
  for (auto &p : vec)
    std::cout << p << ", ";
  std::cout << "]\n";
}

template <typename T1, typename T2>
ITAT_API void output_map(std::map<T1, T2> m) {
  std::cout << "[";
  for (auto &p : m)
    std::cout << p.first << " -> " << p.second << ", ";
  std::cout << "]\n";
}


extern ITAT_API int g_run;
extern ITAT_API int itat_httpd(short int portno, URI_REQUEST *uris);
extern ITAT_API int itat_httpc(HTTP_API_PARAM &param, HTTPBUF buf, const char *cmd);
extern ITAT_API int event_sender();

#endif // HTTP_API_HPP
