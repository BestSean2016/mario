
#define BUFSIZE 8192

TEST(http_server, api) {
  std::thread t(http_api, nullptr);
  std::this_thread::sleep_for(std::chrono::seconds(5));

  char buf[BUFSIZE];
  EXPECT_EQ(0, http_client(send_str[0], buf, BUFSIZE));
  EXPECT_EQ(0, strncmp(buf, ret_str[0], strlen(ret_str[0])));

  EXPECT_EQ(0, http_client(send_str[1], buf, BUFSIZE));
  EXPECT_EQ(0, strncmp(buf, ret_str[1], strlen(ret_str[1])));

  EXPECT_EQ(0, http_client(send_str[2], buf, BUFSIZE));
  EXPECT_EQ(0, strncmp(buf, ret_str[2], strlen(ret_str[2])));

  EXPECT_EQ(0, http_client(send_str[3], buf, BUFSIZE));
  EXPECT_EQ(0, strncmp(buf, ret_str[3], strlen(ret_str[3])));

  s_sig_num = 1;
  t.join();
}

#include "http_api.hpp"
static const char* send_str2[] = {
    "GET /api/pyaxa/foo HTTP/1.1\r\n"
    "Host: localhost:8000\r\n"
    "Accept: */*\r\n"
    "Content-Length: 28\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "\r\n"
    "aaa=bbb&action=get&user=sean",

    "POST /api/pyaxa/foo HTTP/1.1\r\n"
    "Host: localhost:8000\r\n"
    "Accept: */*\r\n"
    "Content-Length: 40\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "\r\n"
    "aaa=bbb&action=post&user=sean&pass=ooooo",

    "DELETE /api/pyaxa/foo HTTP/1.1\r\n"
    "Host: localhost:8000\r\n"
    "Accept: */*\r\n"
    "Content-Length: 28\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "\r\n"
    "aaa=bbb&action=get&user=sean",

    "POST /api/pyaxa/foo HTTP/1.1\r\n"
    "Host: localhost:8000\r\n"
    "Accept: */*\r\n"
    "Content-Length: 28\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "\r\n"
    "aaa=bbb&user=sean&pass=ooooo",


    "GET /event/pyaxa/foo HTTP/1.1\r\n"
    "Host: localhost:8000\r\n"
    "Accept: */*\r\n"
    "Content-Length: 41\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "\r\n"
    "aaa=bbb&action=event&user=sean&pass=ooooo",

    nullptr,
};

int _get_ (struct mg_connection *nc, const struct http_message *hm,
           const struct mg_str *key, const HttpReqKeyValue& requests) {
  (void)nc;
  (void)hm;
  (void)key;

  output_map(requests);
  mg_printf(nc, ret_str[0]);
  return 0;
}

int _post_ (struct mg_connection *nc, const struct http_message *hm,
           const struct mg_str *key, const HttpReqKeyValue& requests) {
  (void)nc;
  (void)hm;
  (void)key;
  output_map(requests);
  mg_printf(nc, ret_str[1]);
  return 0;
}


static const char* reply_chunk[] = {
    "HTTP/1.1 200 OK\r\n"
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
    "Set-Cookie: session_id=3c2ac6c088d0704fa70ccb9bd032424da1531888; expires=Fri, 16 Dec 2016 17:50:12 GMT; Path=/\r\n"
    "Transfer-Encoding: chunked\r\n\r\n",

    "\r\nretry: 400\n\r\n"
};

int _event_ (struct mg_connection *nc, const struct http_message *hm,
           const struct mg_str *key, const HttpReqKeyValue& requests) {
  (void)nc;
  (void)hm;
  (void)key;

  char buf[BUFSIZE];
  char result[BUFSIZE];
  output_map(requests);
  ssize_t wlen = write(nc->sock, reply_chunk[0], strlen(reply_chunk[0]));
  if (wlen < 0) {
    printf("a, %d %s\n", errno, strerror(errno));
  }
  wlen = write(nc->sock, reply_chunk[1], strlen(reply_chunk[1]));
  if (wlen < 0) {
    printf("b, %d %s\n", errno, strerror(errno));
  }

  int i = 0;
  for (;g_run;) {
    snprintf(result, BUFSIZE, "{ \"result\": %d }", i);
    snprintf(buf, BUFSIZE, "%s\r\n%lX\r\n", result, strlen(result));
    wlen = write(nc->sock, buf, strlen(buf));
    if (wlen < 0) {
      printf("i, %d, %d %s\n", i, errno, strerror(errno));
      break;
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));
    ++i;
  }

  mg_send_http_chunk(nc, "", 0); /* Send empty chunk, the end of response */
  return 0;
}

TEST(http_server, itat_http_api_server) {
  HttpServerParam hsp;
  hsp.actions.insert(AsPair("get", _get_));
  hsp.actions.insert(AsPair("post", _post_));
  hsp.uris.insert(KVPair("api", "/api/pyaxa"));
  hsp.port = "8000";


  g_run = 1;
  std::thread t(itat_http_api_server, &hsp);
  std::this_thread::sleep_for(std::chrono::seconds(5));

  char buf[BUFSIZE];
  EXPECT_EQ(0, http_client(send_str2[0], buf, BUFSIZE));
  EXPECT_EQ(0, strncmp(buf, ret_str[0], strlen(ret_str[0])));

  EXPECT_EQ(0, http_client(send_str2[1], buf, BUFSIZE));
  EXPECT_EQ(0, strncmp(buf, ret_str[1], strlen(ret_str[1])));

  EXPECT_EQ(0, http_client(send_str2[2], buf, BUFSIZE));
  EXPECT_EQ(0, strncmp(buf, ret_str[4], strlen(ret_str[4])));

  EXPECT_EQ(0, http_client(send_str2[3], buf, BUFSIZE));
  EXPECT_EQ(0, strncmp(buf, ret_str[3], strlen(ret_str[3])));

  g_run = 0;
  t.join();
}


TEST(http_server, itat_http_events_server) {
    HttpServerParam hsp;
    hsp.actions.insert(AsPair("get", _get_));
    hsp.actions.insert(AsPair("post", _post_));
    hsp.actions.insert(AsPair("event", _event_));

    hsp.uris.insert(KVPair("api", "/api/pyaxa"));
    hsp.uris.insert(KVPair("event", "/event/pyaxa"));
    hsp.port = "8000";


    g_run = 1;
    std::thread t(itat_http_api_server, &hsp);
    std::this_thread::sleep_for(std::chrono::seconds(5));

    char buf[BUFSIZE];
    EXPECT_EQ(0, http_client(send_str2[0], buf, BUFSIZE));
    EXPECT_EQ(0, strncmp(buf, ret_str[0], strlen(ret_str[0])));

    EXPECT_EQ(0, http_client(send_str2[1], buf, BUFSIZE));
    EXPECT_EQ(0, strncmp(buf, ret_str[1], strlen(ret_str[1])));

    EXPECT_EQ(0, http_client(send_str2[2], buf, BUFSIZE));
    EXPECT_EQ(0, strncmp(buf, ret_str[4], strlen(ret_str[4])));

    EXPECT_EQ(0, http_client(send_str2[3], buf, BUFSIZE));
    EXPECT_EQ(0, strncmp(buf, ret_str[3], strlen(ret_str[3])));

    EXPECT_EQ(0, http_client(send_str2[4], buf, BUFSIZE, true));
    // EXPECT_EQ(0, strncmp(buf, ret_str[3], strlen(ret_str[3])));
    std::this_thread::sleep_for(std::chrono::seconds(5));
    g_run = 0;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    t.join();
}




TEST(itat, login) {
  set_default_callback();
  EXPECT_EQ(0, salt_api_login("10.10.10.19", 8000, "sean", "hongt@8a51"));
}

TEST(itat, testping) {
  set_default_callback();
  EXPECT_EQ(0, salt_api_login("10.10.10.19", 8000, "sean", "hongt@8a51"));
  EXPECT_EQ(0, salt_api_testping("10.10.10.19", 8000, "old*", nullptr, nullptr));
}


#include "mongoose.h"

#define  API_OP_GET  1
#define  API_OP_SET  2
#define  API_OP_DEL  3

static const char *s_http_port = "8000";
static struct mg_serve_http_opts s_http_server_opts;
static int s_sig_num = 0;
static const struct mg_str s_get_method = MG_MK_STR("GET");
static const struct mg_str s_put_method = MG_MK_STR("PUT");
static const struct mg_str s_delele_method = MG_MK_STR("DELETE");

static void signal_handler(int sig_num) {
  signal(sig_num, signal_handler);
  s_sig_num = sig_num;
}

static int has_prefix(const struct mg_str *uri, const struct mg_str *prefix) {
  return uri->len > prefix->len && memcmp(uri->p, prefix->p, prefix->len) == 0;
}

static int is_equal(const struct mg_str *s1, const struct mg_str *s2) {
  return s1->len == s2->len && memcmp(s1->p, s2->p, s2->len) == 0;
}

static void db_op(struct mg_connection *nc, const struct http_message *hm,
           const struct mg_str *key, int op);

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
  static const struct mg_str api_prefix = MG_MK_STR("/api/pyaxa");
  struct http_message *hm = (struct http_message *) ev_data;
  struct mg_str key;

  switch (ev) {
    case MG_EV_HTTP_REQUEST:
      if (has_prefix(&hm->uri, &api_prefix)) {
        key.p = hm->uri.p + api_prefix.len;
        key.len = hm->uri.len - api_prefix.len;
        if (is_equal(&hm->method, &s_get_method)) {
          db_op(nc, hm, &key, API_OP_GET);
        } else if (is_equal(&hm->method, &s_put_method)) {
          db_op(nc, hm, &key, API_OP_SET);
        } else if (is_equal(&hm->method, &s_delele_method)) {
          db_op(nc, hm, &key, API_OP_DEL);
        } else {
          mg_printf(nc, "%s",
                    "HTTP/1.0 501 Not Implemented\r\n"
                    "Content-Length: 0\r\n\r\n");
        }
      } else {
        mg_serve_http(nc, hm, s_http_server_opts); /* Serve static content */
      }
      break;
    default:
      break;
  }
}


static const char* ret_str[] = {
  "HTTP/1.1 200 OK\r\nContent-Length: 4\r\n\r\nthis",
  "HTTP/1.1 200 OK\r\nContent-Length: 3\r\n\r\nset",
  "HTTP/1.1 200 OK\r\nContent-Length: 3\r\n\r\ndel",
  "HTTP/1.0 501 Not Implemented\r\nContent-Length: 0\r\n\r\n",
  "HTTP/1.0 500 Server Internal Error\r\nContent-Length: 0\r\n\r\n",
  "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n",
};

static void op_get(struct mg_connection *nc, const struct http_message *hm,
           const struct mg_str *key) {
  (void)nc;
  (void)hm;
  (void)key;

  //printf("Data is:\n%s\n||%lu\n", key->p, key->len);
  mg_printf(nc, "%s", ret_str[0]);
}


static void op_set(struct mg_connection *nc, const struct http_message *hm,
           const struct mg_str *key) {
  (void)nc;
  (void)hm;
  (void)key;

  mg_printf(nc, "%s", ret_str[1]);
}

static void op_del(struct mg_connection *nc, const struct http_message *hm,
           const struct mg_str *key) {
  (void)nc;
  (void)hm;
  (void)key;

  mg_printf(nc, "%s", ret_str[2]);
}

static void db_op(struct mg_connection *nc, const struct http_message *hm,
           const struct mg_str *key, int op) {
  switch (op) {
    case API_OP_GET:
      op_get(nc, hm, key);
      break;
    case API_OP_SET:
      op_set(nc, hm, key);
      break;
    case API_OP_DEL:
      op_del(nc, hm, key);
      break;
    default:
      mg_printf(nc, "%s", ret_str[3]);
      break;
  }
}

static void http_api(void* api_param) {
  struct mg_mgr mgr;
  struct mg_connection *nc;

  /* Open listening socket */
  mg_mgr_init(&mgr, NULL);
  EXPECT_NE(nullptr, (nc = mg_bind(&mgr, s_http_port, ev_handler)));
  mg_set_protocol_http_websocket(nc);
  mgr.user_data = api_param;

  s_http_server_opts.document_root = "web_root";

  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);

  /* Run event loop until signal is received */
  printf("Starting RESTful server on port %s\n", s_http_port);
  while (s_sig_num == 0) {
    mg_mgr_poll(&mgr, 1000);
  }

  /* Cleanup */
  mg_mgr_free(&mgr);

  printf("Exiting on signal %d\n", s_sig_num);
}

static const char* send_str[] = {
    "GET /api/pyaxa/foo HTTP/1.1\r\n"
    "Host: localhost:8000\r\n"
    "Accept: */*\r\n"
    "Content-Length: 7\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "\r\n"
    "aaa=bbb",

    "PUT /api/pyaxa/foo HTTP/1.1\r\n"
    "Host: localhost:8000\r\n"
    "Accept: */*\r\n"
    "Content-Length: 7\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "\r\n"
    "aaa=bbb",

    "DELETE /api/pyaxa/foo HTTP/1.1\r\n"
    "Host: localhost:8000\r\n"
    "Accept: */*\r\n"
    "Content-Length: 7\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "\r\n"
    "aaa=bbb",

    "KAO /api/pyaxa/foo HTTP/1.1\r\n"
    "Host: localhost:8000\r\n"
    "Accept: */*\r\n"
    "Content-Length: 7\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "\r\n"
    "aaa=bbb",

    nullptr,
};


static int http_client(const char* cmd, char* buf, size_t buflen, bool keep_alive = false) {
    int sockfd, n;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    int ret;

    sockfd = 0, n = 0;
    server = 0;
    ret = 0;

    /* socket: create the socket */
    ret = ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0);

    /* gethostbyname: get the server's DNS entry */
    if (!ret)
      ret = ((server = gethostbyname("127.0.0.1")) == NULL);

    if (!ret) {
      /* build the server's Internet address */
      bzero((char *)&serveraddr, sizeof(serveraddr));
      serveraddr.sin_family = AF_INET;
      bcopy((char *)server->h_addr, (char *)&serveraddr.sin_addr.s_addr,
            server->h_length);
      serveraddr.sin_port = htons(8000);

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

    if (keep_alive)
      while (!ret && g_run)
        ret = ((n = read(sockfd, buf, buflen)) <= 0);
    else
      ret = ((n = read(sockfd, buf, buflen)) <= 0);

    close(sockfd);
    return ret;
}



TEST(ZeroHttpd, subscribe) {
  g_run = 1;

  std::thread t(zero_event, "tcp://*:32001");

  void *context = zmq_ctx_new ();
  void *subscriber = zmq_socket (context, ZMQ_SUB);
  int rc = zmq_connect (subscriber, "tcp://localhost:32001");
  assert (rc == 0);

  rc = zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, nullptr, 0);
  assert (rc == 0);

  //  Process 100 updates
  for (int update_nbr = 0; update_nbr < 10; update_nbr++) {
      char buffer[1024];
      int n = zmq_recv (subscriber, buffer, 1024 - 1, 0);
      buffer[n] = 0;
      printf("Received %s\n", buffer);
  }
  zmq_close (subscriber);
  zmq_ctx_destroy (context);

  g_run = 0;

  std::this_thread::sleep_for(std::chrono::seconds(5));
  t.join();
}

