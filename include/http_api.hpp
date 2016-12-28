#ifndef HTTP_API_HPP
#define HTTP_API_HPP

#include "itat_global.h"
#include "itat.h"

#include <map>
#include <string>
#include <vector>

#ifdef BUFSIZE
#undef BUFSIZE
#endif //BUFSIZE

#define BUFSIZE 32768

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

extern ITAT_API int itat_httpd(short int portno);
extern ITAT_API int itat_httpc(const char *hostname, int portno, char *buf, int buflen, const char *cmd);
extern ITAT_API int event_sender();

#endif // HTTP_API_HPP
