#ifndef HTTP_API_HPP
#define HTTP_API_HPP

#include <map>
#include <string>
#include <vector>

typedef std::map<std::string, std::string> HttpRequest;
typedef int (*action_fun) (struct mg_connection *nc, const struct http_message *hm,
                           const struct mg_str *key, const HttpRequest& requests);
typedef std::map<std::string, action_fun> ActionSet;
typedef std::pair<std::string, action_fun> AsPair;
#include "itat_global.h"
#include "itat.h"
#include "mongoose.h"


extern ITAT_API int parse_http_request(HttpRequest& params, mg_str *key);
extern ITAT_API int itat_http_api_server(const ActionSet* actions, const char *s_http_port);
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

#endif // HTTP_API_HPP
