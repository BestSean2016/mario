#include "http_api.hpp"
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

static threadpool_t *g_thpool_httpd;
static std::mutex g_mutex_event;
static int serv_sock;

typedef struct CLIENTSOCK {
  int socket;
  struct sockaddr_in cli_addr;
} CLIENTSOCK;

static std::set<CLIENTSOCK *> g_event_clients;

const char *http_err_500 = "HTTP/1.0 500 Server Internal Error\r\n"
                           "Content-Length: 0\r\n\r\n";

const char *http_err_501 = "HTTP/1.0 501 Not Implemented\r\n"
                           "Content-Length: 0\r\n\r\n";
const char *http_bad_req = "HTTP/1.0 400 Bad Request\r\n";

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

static const char *_content_len_ = {"Content-Length: "};
// "PUT /api/v1/foo HTTP/1.1\r\n"
// "Host: localhost:8000\r\n"
// "User-Agent: curl/7.51.0\r\n"
// "Accept: */*\r\n"
// "Content-Length: 7\r\n"
// "Content-Type: application/x-www-form-urlencoded\r\n"
// "\r\n"
// "aaa=bbb"

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
  CLIENTSOCK *clisock = (CLIENTSOCK *)param;
  char buf[BUFSIZE * 2];
  memset(buf, 0, BUFSIZE * 2);

  int n = 0;
  char *line = buf;

  int len = 0;

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
        if ((int)strlen(line) != hr.content_len)
          return; // error
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

  parse_http_request_(hr);

  if (std::string::npos != hr.uri.find("/api/pyaxa", 0, 4)) {
    // it's an api request
    return;
  } else if (hr.uri == "/event/pyaxa") {
    // it's an event listner
    write(clisock->socket, http_chuncked, strlen(http_chuncked));
    auto *guard = new std::lock_guard<std::mutex>(g_mutex_event);
    printf("Insert a socket %d\n", clisock->socket);
    g_event_clients.insert(clisock);
    delete guard;
    return;
  } else {
    // bad request
    write(clisock->socket, http_bad_req, strlen(http_bad_req));
  }
error_exit:
  close(clisock->socket);
  delete clisock;
}

int itat_httpd(short int portno) {
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

      CLIENTSOCK *clisock = new CLIENTSOCK;
      clisock->socket = newsockfd;
      memcpy(&clisock->cli_addr, &cli_addr, sizeof(sockaddr_in));
      if (threadpool_add(g_thpool_httpd, do_http_request, clisock, 0)) {
        close(newsockfd);
        delete clisock;
      }
    } else {
      // always here, even if i connect from another application
      printf("select timeout\n");
    }
  }

  close(serv_sock);
  printf("close server socket fd");

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

int itat_httpc(const char *hostname, int portno, char *buf, int buflen,
               const char *cmd) {
  int sockfd, n;
  struct sockaddr_in serveraddr;
  struct hostent *server;
  int ret;

  /* socket: create the socket */
  ret = ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0);

  /* gethostbyname: get the server's DNS entry */
  if (!ret)
    ret = ((server = gethostbyname(hostname)) == NULL);

  if (!ret) {
    /* build the server's Internet address */
    bzero((char *)&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serveraddr.sin_addr.s_addr,
          server->h_length);
    serveraddr.sin_port = htons(portno);

    /* connect: create a connection with the server */
    ret = (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) <
           0);
  }
/* send the message line to the server */
#ifdef _DEBUG_
// printf("CMD: %s\n", cmd);
#endif //_DEBUG_

  write(sockfd, cmd, strlen(cmd));

  while ((n = read(sockfd, buf, buflen)) > 0) {
    buf[n] = 0;
    printf(buf);
  }

  close(sockfd);
  return 0;
}
