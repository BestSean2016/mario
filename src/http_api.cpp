#include "http_api.hpp"
#include <thread>
#include "mylog.h"

const char* http_err_500 =
        "HTTP/1.0 500 Server Internal Error\r\n"
        "Content-Length: 0\r\n\r\n";

const char* http_err_501 =
        "HTTP/1.0 501 Not Implemented\r\n"
        "Content-Length: 0\r\n\r\n";

static const char *_content_len_ = {"Content-Length: "};
// "PUT /api/v1/foo HTTP/1.1\r\n"
// "Host: localhost:8000\r\n"
// "User-Agent: curl/7.51.0\r\n"
// "Accept: */*\r\n"
// "Content-Length: 7\r\n"
// "Content-Type: application/x-www-form-urlencoded\r\n"
// "\r\n"
// "aaa=bbb"


static char* get_content(const char* buf, size_t buflen, size_t* content_len) {
  char *tmp = (char*)buf;
  char *line = tmp;
  int has_content = 0;
  *content_len = 0;

  while (tmp - buf < (int)buflen) {
    if (*tmp != '\r')
      ++tmp;
    else {
      // this is a new line
      if (!(strncmp(line, _content_len_, strlen(_content_len_)) == 0)) {
        has_content = 1;
        *content_len = strtoull(line + strlen(_content_len_), 0, 10);
      }

      if (has_content && line == tmp) {
        tmp += 2;
        break;
      }

      //next line
      tmp += 2;
      line = tmp;
    }
  }

  if (!has_content) tmp = nullptr;
  return tmp;
}

#include <string>
#include <sstream>
#include <vector>

static void split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

static int parse_http_request_(HttpRequest& request, const char* reqstr, size_t reqlen) {
  std::vector<std::string> elems = split(std::string(reqstr, reqlen), '&');
  for (auto& str : elems) {
    std::vector<std::string> e = split(str, '=');
    if (e.size() == 2)
      request.insert(std::pair<std::string, std::string>(e[0], e[1]));
  }
  return  0;
}

int parse_http_request(HttpRequest& params, struct mg_str* key) {
  size_t reqlen = 0;
  char* request = get_content(key->p, key->len, &reqlen);
  params.clear();

  if (!request || !reqlen) return -1;

  return parse_http_request_(params, request, reqlen);
}

static int has_prefix(const struct mg_str *uri, const struct mg_str *prefix) {
  return uri->len > prefix->len && memcmp(uri->p, prefix->p, prefix->len) == 0;
}

static int is_equal(const struct mg_str *s1, const struct mg_str *s2) {
  return s1->len == s2->len && memcmp(s1->p, s2->p, s2->len) == 0;
}

static action_fun get_action(ActionSet* actions, std::string& action) {
  auto iter = actions->find(action);
  return (iter != actions->end()) ? iter->second : nullptr;
}


static const struct mg_str s_get_method = MG_MK_STR("GET");
static const struct mg_str s_post_method = MG_MK_STR("POST");

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
  static const struct mg_str api_prefix = MG_MK_STR("/api/pyaxa");
  struct http_message *hm = (struct http_message *) ev_data;
  int ret = 0;

  switch (ev) {
    case MG_EV_HTTP_REQUEST:
      if (has_prefix(&hm->uri, &api_prefix)) {
        if (is_equal(&hm->method, &s_get_method)
            || is_equal(&hm->method, &s_post_method)) {
          HttpRequest params;
          if (0 != (ret = parse_http_request_(params, hm->body.p, hm->body.len)))
            ret = 500;
          else {
            action_fun fun = get_action((ActionSet*)(nc->mgr->user_data), params["action"]);
            if (fun) {
              ret = (fun(nc, hm, &hm->body, params)) ? 500 : 200;
            } else {
              ret = 501;
            }
          }
        } else
          ret = 500;
      } else {
        ret = 500;
      }
      break;
    default:
      break;
  }

  switch(ret) {
    case 500:
     mg_printf(nc, "%s", http_err_500);
    break;
    case 501:
      mg_printf(nc, "%s", http_err_501);
      break;
    default:
      break;
  }
}


int itat_http_api_server(const ActionSet *actions, const char* s_http_port) {
  struct mg_mgr mgr;
  struct mg_connection *nc;
  int ret = 0;

#ifdef _UNIX
  pthread_t pid = pthread_self();
#else
  HANDLE pid = 0;
#endif //_UNIX
  logmsg(L_INFO, "Start http_api_thread %lx\n", pid);

  /* Open listening socket */
  mg_mgr_init(&mgr, NULL);
  mgr.user_data = (void*)actions;
  if (nullptr == (nc = mg_bind(&mgr, s_http_port, ev_handler))) {
    ret = -1;
    goto error_exit;
  }

  mg_set_protocol_http_websocket(nc);

  /* Run event loop until signal is received */
  logmsg(L_INFO, "Starting RESTful server on port %s\n", s_http_port);
  while (g_run) {
    mg_mgr_poll(&mgr, 1000);
  }

error_exit:
  /* Cleanup */
  mg_mgr_free(&mgr);

  printf("Exiting http_api_thread %lx\n", pid);
  return ret;
}

