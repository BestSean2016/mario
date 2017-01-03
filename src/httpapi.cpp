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

static threadpool_t *g_thpool_httpd;
static std::mutex g_mutex_event;
static int serv_sock;

static std::set<HTTP_CLIENT_PARAM *> g_event_clients;

const char *http_ret_200 = "HTTP/1.0 200 OK\r\n";

const char *http_ret_500 = "HTTP/1.0 500 Server Internal Error\r\n"
                           "Content-Length: 0\r\n\r\n";

const char *http_ret_501 = "HTTP/1.0 501 Not Implemented\r\n"
                           "Content-Length: 0\r\n\r\n";
const char *http_ret_400 = "HTTP/1.0 400 Bad Request\r\n";

const char *http_chuncked = "HTTP/1.1 200 OK\r\n"
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

static int parse_http_request_(HttpRequest &req) {
  std::vector<std::string> elems = split(req.content, '&');
  for (auto &str : elems) {
    std::vector<std::string> e = split(str, '=');
    if (e.size() == 2)
      req.request.insert(std::pair<std::string, std::string>(e[0], e[1]));
  }
  return 0;
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

static void do_http_request(void *param) {
  HTTP_CLIENT_PARAM *clisock = (HTTP_CLIENT_PARAM *)param;
  char buf[BUFSIZE * 2];
  memset(buf, 0, BUFSIZE * 2);

  int n = 0;
  char *line = buf;

  int len = 0;
  bool is_api = false;
  HttpRequest hr;
  //
  // todo, asuming read onece to get all data
  //
  n = read(clisock->socket, buf, BUFSIZE - 1);
  if (n <= 0)
    goto error_exit;
  buf[n] = 0;

  while (nullptr != (line = get_line(line, &len))) {
    char c = line[len];
    line[len] = 0;

    if (*line == 0) {
      // get the content
      *line = '\r';
      line += 2;
#ifdef _DEBUG_
      printf("Content -> %s, %lu\n", line, strlen(line));
#endif //_DEBUG
      if (hr.method == HTTP_REQUEST_METHOD_POST) {
        // if ((int)strlen(line) != hr.content_len)
        //   return; // error
        hr.content = line;
      }
    } else {
#ifdef _DEBUG_
      printf("Line -> %s\n", line);
#endif //_DEBUG
      getRequestParam(line, &hr);
    }

    line[len] = c;
    line += len + 2;
  }

  if (hr.content_len > 0 && (int)(hr.content.size()) != hr.content_len) {
    int left = hr.content_len - (int)(hr.content.size());
    if (left != (n = read(clisock->socket, buf, left)))
      goto error_exit;
    else {
      buf[n] = 0;
      hr.content.append(buf);
#ifdef _DEBUG_
      printf("Content Line -> %s\n", buf);
#endif //_DEBUG
    }
  }

  parse_http_request_(hr);

  if (clisock->uris.find("api") == clisock->uris.end())
    write(clisock->socket, http_ret_400, strlen(http_ret_400));
  else {
    if (clisock->uris["api"].find(hr.uri) == clisock->uris["api"].end())
      is_api = false;
    else {
      // it is api
      // return 200 ok for now
      is_api = true;
      write(clisock->socket, http_ret_200, strlen(http_ret_200));
    }
  }

  if (clisock->uris.find("event") == clisock->uris.end()) {
    if (!is_api)
      write(clisock->socket, http_ret_400, strlen(http_ret_400));
  } else if (hr.uri == *(clisock->uris["event"].begin())) {
    // it's an event listner
    write(clisock->socket, http_chuncked, strlen(http_chuncked));
    auto *guard = new std::lock_guard<std::mutex>(g_mutex_event);
#ifdef _DEBUG_
    printf("Insert a socket %d\n", clisock->socket);
#endif //_DEBUG_

    g_event_clients.insert(clisock);
    delete guard;
    return;
  }

error_exit:
  close(clisock->socket);
  delete clisock;
}

int itat_httpd(short int portno, URI_REQUEST *uris) {
  g_thpool_httpd = threadpool_create(5, 10, 0);
  if (!g_thpool_httpd)
    return -1;

  signal(SIGPIPE, SIG_IGN);

  int newsockfd, clilen;
  struct sockaddr_in serv_addr, cli_addr;

  /* First call to socket() function */
  serv_sock = socket(AF_INET, SOCK_STREAM, 0);

  if (serv_sock < 0) {
    perror("ERROR opening socket");
    exit(1);
  }

  /* Initialize socket structure */
  bzero((char *)&serv_addr, sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  /* Now bind the host address using bind() call.*/
  if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("ERROR on binding");
    exit(1);
  }

  /* Now start listening for the clients, here process will
     * go in sleep mode and will wait for the incoming connection
  */

  listen(serv_sock, 5);

  while (g_run) {
    int iResult;
    struct timeval tv;
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(serv_sock, &rfds);

    tv.tv_sec = (long)10;
    tv.tv_usec = 0;

    iResult = select(serv_sock + 1, &rfds, (fd_set *)0, (fd_set *)0, &tv);
    if (iResult > 0) {
      clilen = sizeof(cli_addr);

      /* Accept actual connection from the client */
      newsockfd =
          accept(serv_sock, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);

      if (newsockfd < 0) {
        perror("ERROR on accept");
        break;
      }

      HTTP_CLIENT_PARAM *clisock = new HTTP_CLIENT_PARAM(*uris);
      clisock->socket = newsockfd;

      memcpy(&clisock->cli_addr, &cli_addr, sizeof(sockaddr_in));
      if (threadpool_add(g_thpool_httpd, do_http_request, clisock, 0)) {
        close(newsockfd);
        delete clisock;
      }
    } else {
// always here, even if i connect from another application
#ifdef _DEBUG_
      printf("select timeout\n");
#endif //_DEBUG_
    }
  }

  close(serv_sock);
#ifdef _DEBUG_
  printf("close server socket fd");
#endif //_DEBUG_

  std::this_thread::sleep_for(std::chrono::seconds(15));
  {
    auto *guard = new std::lock_guard<std::mutex>(g_mutex_event);
    for (auto it = g_event_clients.begin(); it != g_event_clients.end(); ++it) {
      close((*it)->socket);
      delete (*it);
    }
    g_event_clients.clear();
    delete guard;
  }

  threadpool_destroy(g_thpool_httpd, 0);
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
