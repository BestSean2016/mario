#include <gtest/gtest.h>
#include <iostream>
#include "itat.h"
#include "mongoose.h"
#include <thread>
#include <chrono>

using namespace std;

int main(int argc, char **argv) {

#ifdef _WINDOWS
#ifdef _DEBUG_
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
  _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
  _CrtDumpMemoryLeaks();
#endif //_DEBUG_
#endif //_WINDOWS

  // if (argc != 3) {
  //   printf("Usage testing <json config file> <json config file2>\n");
  //   return (0);
  // }

  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
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


static int http_client(const char* cmd, char* buf, size_t buflen) {
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

    if (!ret)
      ret = ((n = read(sockfd, buf, buflen)) <= 0);

    close(sockfd);
    return ret;
}

TEST(http_server, api) {
  std::thread t(http_api, nullptr);
  std::this_thread::sleep_for(std::chrono::seconds(5));

  char buf[256];
  EXPECT_EQ(0, http_client(send_str[0], buf, 256));
  EXPECT_EQ(0, strncmp(buf, ret_str[0], strlen(ret_str[0])));

  EXPECT_EQ(0, http_client(send_str[1], buf, 256));
  EXPECT_EQ(0, strncmp(buf, ret_str[1], strlen(ret_str[1])));

  EXPECT_EQ(0, http_client(send_str[2], buf, 256));
  EXPECT_EQ(0, strncmp(buf, ret_str[2], strlen(ret_str[2])));

  EXPECT_EQ(0, http_client(send_str[3], buf, 256));
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

    nullptr,
};

int _get_ (struct mg_connection *nc, const struct http_message *hm,
           const struct mg_str *key, const HttpRequest& requests) {
  (void)nc;
  (void)hm;
  (void)key;

  output_map(requests);
  mg_printf(nc, ret_str[0]);
  return 0;
}

int _post_ (struct mg_connection *nc, const struct http_message *hm,
           const struct mg_str *key, const HttpRequest& requests) {
  (void)nc;
  (void)hm;
  (void)key;
  output_map(requests);
  mg_printf(nc, ret_str[1]);
  return 0;
}

TEST(http_server, itat_http_api_server) {
  ActionSet as;
  as.insert(AsPair("get", _get_));
  as.insert(AsPair("post", _post_));

  g_run = 1;
  const char* port = "8000";
  std::thread t(itat_http_api_server, &as, port);
  std::this_thread::sleep_for(std::chrono::seconds(5));

  char buf[256];
  EXPECT_EQ(0, http_client(send_str2[0], buf, 256));
  EXPECT_EQ(0, strncmp(buf, ret_str[0], strlen(ret_str[0])));

  EXPECT_EQ(0, http_client(send_str2[1], buf, 256));
  EXPECT_EQ(0, strncmp(buf, ret_str[1], strlen(ret_str[1])));

  EXPECT_EQ(0, http_client(send_str2[2], buf, 256));
  EXPECT_EQ(0, strncmp(buf, ret_str[4], strlen(ret_str[4])));

  EXPECT_EQ(0, http_client(send_str2[3], buf, 256));
  EXPECT_EQ(0, strncmp(buf, ret_str[3], strlen(ret_str[3])));

  g_run = 0;
  t.join();
}
