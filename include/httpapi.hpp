#ifndef HTTP_API_HPP
#define HTTP_API_HPP

#include "itat_global.h"
#include "itat.h"
#include "str.h"
#include "saltman.hpp"




namespace itat {

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
extern ITAT_API int itat_httpc(saltman *sm, HTTP_API_PARAM *param, HTTPBUF buf, const char *cmd);
extern ITAT_API int event_sender();

} //namespace itat
#endif // HTTP_API_HPP
