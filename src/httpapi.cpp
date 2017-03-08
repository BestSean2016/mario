#include "httpapi.hpp"
#include "mylog.h"
#include "threadpool.h"
#include <assert.h>
#include <mutex>
#include <netdb.h>
#include <signal.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

int g_run = 1;

static std::mutex g_mutex_event;

static std::set<HTTP_CLIENT_PARAM *> g_event_clients;

static const char *http_ret_200 = "HTTP/1.0 200 OK\r\n";

static const char *http_ret_400 = "HTTP/1.0 400 Bad Request\r\n";

static const char *http_chuncked = "HTTP/1.1 200 OK\r\n"
                            "Access-Control-Expose-Headers: GET, POST\r\n"
                            "Cache-Control: no-cache\r\n"
                            "Vary: Accept-Encoding\r\n"
                            "Server: CherryPy/3.2.2\r\n"
                            "Connection: keep-alive\r\n"
                            "Allow: GET, HEAD\r\n"
                            "Access-Control-Allow-Credentials: true\r\n"
                            "Date: Fri, 16 Dec 2016 07:50:12 GMT\r\n"
                            "Access-Control-Allow-Origin: *\r\n"
                            "Content-Type: text/event-stream;charset=utf-8\r\n"
                            "Transfer-Encoding: chunked\r\n\r\n"
                            "a\r\nretry: 400\r\n\r\n";

static const char *_http_header_ = {"HTTP/1.1 "};
static const char *_content_len_ = {"Content-Length: "};



// "PUT /api/v1/foo HTTP/1.1\r\n"
// "Host: localhost:8000\r\n"
// "User-Agent: curl/7.51.0\r\n"
// "Accept: */*\r\n"
// "Content-Length: 7\r\n"
// "Content-Type: application/x-www-form-urlencoded\r\n"
// "\r\n"
// "aaa=bbb"

// **************************************************************************************
// Server Api
// **************************************************************************************
void split(const std::string &s, char delim, std::vector<std::string> &elems) {
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


static char *get_line(char *line, int *len) {
  assert(line != nullptr);
  assert(len != nullptr);

  *len = 0;
  char *ptr = line;
  while (ptr && *ptr) {
    if (*ptr == '\r') {
      *len = ptr - line;
      return line;
    }
    ++ptr;
  }

  return nullptr;
}

static int get_uri(const char *line, HttpRequest *hr) {
  char *get_pos = 0;
  char *uri = 0;
  char spliter = 0;
  if (hr->method == HTTP_REQUEST_METHOD_GET) {
    uri = (char *)line + 4;
    spliter = '?';
  } else {
    uri = (char *)line + 5;
    spliter = ' ';
  }

  get_pos = strchr(uri, spliter);
  if (!get_pos)
    return -1;
  *get_pos = 0;
  hr->uri = uri;
  *get_pos = spliter;

  if (spliter == '?') {
    char *content = strchr(get_pos + 1, ' ');
    *content = 0;
    hr->content = get_pos + 1;
    *content = ' ';
    hr->content_len = (int)(hr->content.size());
  }
  return 0;
}

static void get_content(const char *line, HttpRequest *hr) {
  hr->content_len = atoi((char *)line + strlen(_content_len_));
}

static int getRequestParam(const char *line, HttpRequest *hr) {
  if (!strncmp(line, "POST ", 5) || !strncmp(line, "GET ", 4)) {
    if (!strncmp(line, "POST ", 5))
      hr->method = HTTP_REQUEST_METHOD_POST;
    else
      hr->method = HTTP_REQUEST_METHOD_GET;

    get_uri(line, hr);
  } else if (!strncmp(line, _content_len_, strlen(_content_len_))) {
    get_content(line, hr);
  }

  return 0;
}

int event_sender() {
  char buf[BUFSIZE * 2];
  memset(buf, 0, BUFSIZE * 2);

  int64_t i = 0;
  while (g_run) {
    snprintf(buf + BUFSIZE, BUFSIZE,
             "{ \"result\" = \"abcdef%ld\", \"code\" = %ld }\r\n", i, i);
    snprintf(buf, BUFSIZE, "%lx\r\n%s\r\n", strlen(buf + BUFSIZE),
             buf + BUFSIZE);

    auto *guard = new std::lock_guard<std::mutex>(g_mutex_event);
    for (auto it = g_event_clients.begin(); it != g_event_clients.end();) {
      if (write((*it)->socket, buf, strlen(buf)) != (ssize_t)strlen(buf)) {
        close((*it)->socket);
        delete (*it);
        it = g_event_clients.erase(it);
      } else
        ++it;
    }
    delete guard;

    ++i;
    sleep(1);
  }

  return 0;
}


// **************************************************************************************
// Client Api
// **************************************************************************************
static int get_response_code(const char *str) {
  return atoi((char *)str + strlen(_http_header_));
}

static size_t get_content_len(const char *str) {
  return (size_t)atoll((char *)str + strlen(_content_len_));
}

static int analyse_response(char **buf, int buflen, int *rescode,
                            char **json_data, int64_t *data_len) {
  char *tmp = *buf;
  char *line = tmp;
  int finished = 0;

  while (tmp - *buf < buflen) {
    if (*tmp != '\r')
      ++tmp;
    else {
      // this is a new line
      if (!*rescode &&
          (strncmp(line, _http_header_, strlen(_http_header_)) == 0)) {
        *rescode = get_response_code(line);
#ifdef _DEBUG_
// printf("response code %d\n", *rescode);
#endif //_DEBUG_
      }

      if (!*data_len &&
          (strncmp(line, _content_len_, strlen(_content_len_)) == 0))
        *data_len = get_content_len(line);

      if (*line == '\r') {
        *json_data = tmp + 2; // skip 2 bytes for '\r\n'
      }

      if (*json_data) {
        if (!*data_len)
          break;
        if (0 != (finished = (*data_len == (((*buf) + buflen) - (*json_data)))))
          break;
      }

      // GO TO NEXT LINE
      tmp += 2; // skip 2 bytes for '\r\n'
      if (!*json_data)
        line = tmp;
    }
  }

  *buf = line;
  return finished;
}

int itat_httpc(HTTP_API_PARAM& param, HTTPBUF buf, const char *cmd) {
  int sockfd, n, total_len;
  struct sockaddr_in serveraddr;
  struct hostent *server;
  char *ptr;
  int ret;
  char *json_data;
  int64_t data_len;
  char *anaptr;
  int rescode;

restart_client:
  sockfd = 0, n = 0, total_len = 0;
  server = 0;
  ptr = buf;
  ret = 0;
  json_data = 0;
  data_len = 0;
  anaptr = ptr;
  rescode = 0;

  /* socket: create the socket */
  ret = ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0);

  /* gethostbyname: get the server's DNS entry */
  if (!ret)
    ret = ((server = gethostbyname(param.hostname)) == NULL);

  if (!ret) {
    /* build the server's Internet address */
    bzero((char *)&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serveraddr.sin_addr.s_addr,
          server->h_length);
    serveraddr.sin_port = htons(param.port);

    /* connect: create a connection with the server */
    ret = (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) <
           0);
  }
/* send the message line to the server */
#ifdef _DEBUG_
// printf("CMD: %s\n", cmd);
#endif //_DEBUG_

  if (!ret)
    ret = ((n = write(sockfd, cmd, strlen(cmd))) < 0);

  while (!ret && (total_len < BUFSIZE)) {
    ret = ((n = read(sockfd, ptr, BUFSIZE - total_len)) <= 0);
    if (n == 0)
      continue;
    if (!ret) {
      total_len += n;
      if (analyse_response(&anaptr, ptr + n - anaptr, &rescode, &json_data,
                           &data_len))
        break;
      ptr += n;
      if (json_data && !data_len)
        goto run_receive_long_data;
    }
  }

  if (!ret && param.rf)
    ret = (0 != param.rf(json_data, data_len, param.param1, param.param2));

  if (rescode != 200)
    ret = rescode;

  close(sockfd);
  return ret;

run_receive_long_data : {
  char *tmp = json_data;
  char *line = tmp;
  // #ifndef _DEBUG_
  {
    struct timeval timeout;
    timeout.tv_sec = 30;
    timeout.tv_usec = 0;

    if (0 != (ret = (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,
                                (char *)&timeout, sizeof(timeout)) < 0)))
      printf("setsockopt failed\n");

    if (0 != (ret = (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO,
                                (char *)&timeout, sizeof(timeout)) < 0)))
      printf("setsockopt failed\n");
  }
  // #endif //no _DEBUG_
  while (!ret && g_run) {
    while (tmp < ptr) {
      if (*tmp != '\r')
        ++tmp;
      else {
        // printf("************** %s\n", line);
        if (param.rf) // do not check error
          param.rf(line, tmp - line, param.param1, param.param2);
        // ret = (0 != parse_fun(line, tmp - line, param1, 0));

        // next line
        tmp += 2;
        line = tmp;
      }
    }

    // reset pointer to begin of buffer
    if (line == tmp || (BUFSIZE - total_len < 8 * 1024)) {
      ptr = buf, total_len = 0, tmp = buf, line = buf;
    }

    // read next package
    // printf("************** reading *********************\n");
    n = read(sockfd, ptr, BUFSIZE - total_len);
    if (n < 0) {
      if (n != 11)
        fprintf(stdout,
                "i got n = %d bytes, error no %d, errmsg %s total_len is %d\n",
                n, errno, strerror(errno), total_len);
      ret = (errno == 11) ? 0 : 1;
    } else if (n == 0) {
      fprintf(stdout,
              "i got n = %d bytes, error no %d, errmsg %s total_len is %d\n", n,
              errno, strerror(errno), total_len);
    } else {
      ptr += n, total_len += n;
    }
  }

  close(sockfd);

  return ret;
}

  if (!ret && g_run) {
    fprintf(stdout, "ReStart Http Event Client!\n");
    goto restart_client;
  }

  fprintf(stdout, "exit http_client event listener\n");
  return ret;
}

void show_cstring(const char *cstring, size_t len) {
  std::cout << "|-->";
  for (size_t i = 0; cstring && *cstring && i < len; i++)
    std::cout << cstring[i];
  std::cout << "<--|\n";
}
