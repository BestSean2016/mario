#include "djangoapi.hpp"

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <threadpool.h>
#include <errno.h>
#include <error.h>


static char django_ip[64] = {"10.10.10.16"};
static short django_port = 8000;


#define THREAD_POOL_SIZE 1
#define THREAD_POOL_QUEUE_SIZE 4096


void set_django_ip_port(const char * ip, int port) {
    strcpy_s(django_ip, 64,  ip);
    django_port = (short)port;
}


extern int update_bill_exec_node(int pl_ex_id, int /*graph_id*/, int node_id,
                          itat::STATE_TYPE run_state, itat::STATE_TYPE check_state,
                          DBHANDLE h_db,
                          int /*code*/, const char */*strout*/,
                          const char */*strerr*/);
extern int update_bill_exec_pipeline(int pl_ex_id, int /*graph_id*/, int node_id,
                          itat::STATE_TYPE run_state, itat::STATE_TYPE check_state,
                          DBHANDLE h_db,
                          int /*code*/, const char */*strout*/,
                          const char */*strerr*/);
extern int update_bill_checked_node(int /*pl_ex_id*/, int graph_id, int node_id,
                          itat::STATE_TYPE run_state, itat::STATE_TYPE check_state,
                          DBHANDLE h_db,
                          int /*code*/, const char */*strout*/,
                          const char */*strerr*/);
extern int update_bill_checked_pipeline(int pl_ex_id, int graph_id, int node_id,
                          itat::STATE_TYPE run_state, itat::STATE_TYPE check_state,
                          DBHANDLE h_db,
                          int /*code*/, const char */*strout*/,
                          const char */*strerr*/, int);


static const char *str_status[] = {
  "ST_initial",           "ST_checking",            "ST_checked_err",
  "ST_checked_serr",      "ST_checked_herr",        "ST_checked_ok",
  "ST_running",           "ST_error",               "ST_timeout",
  "ST_succeed",           "ST_waiting_for_confirm", "ST_stoped",
  "ST_stoping",           "ST_paused",              "ST_pausing",
  "ST_waiting_for_input", "ST_running_one",         "ST_run_one_ok",
  "ST_run_one_err",       "ST_confirm_refused",
};

// std::string python_filename = { "bill_message" };


static void sock(void* param) {
  int sockfd = 0;
  int len = 0, n = 0;
  struct sockaddr_in address;
  int result = 0;
  itat::buffers* bufs = (itat::buffers*)param;

// #ifdef _DEBUG_
//   printf("oops: client1 socket mmmm.... %u %u %u %u\n", (uint64_t)param, (uint64_t)bufs, (uint64_t)bufs->buf, (uint64_t)bufs->cmd);
// #endif //_DEBUG_

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr(django_ip);
  address.sin_port = htons(django_port);
  len = sizeof(address);
  result = connect(sockfd, (struct sockaddr *)&address, len);

  if (result == -1) {
#ifdef _DEBUG_
      perror("oops: client1");
#endif //_DEBUG_
    bufs->ret = -1;
    goto error_exit;
  }

// #ifdef _DEBUG_
//   printf("mmmm send command %s\n", bufs->cmd);
// #endif //_DEBUG_

  n = write(sockfd, bufs->cmd, strlen(bufs->cmd));
  if (n != (int)(strlen(bufs->cmd))) {
#ifdef _DEBUG_
      printf("sock, write error %d %s\n", errno, strerror(errno));
#endif //_DEBUG_
      bufs->ret = -2;
      goto error_exit;
  }

// #ifdef _DEBUG_
//   printf("mmmm read \n");
// #endif //_DEBUG_

  memset(bufs->buf, 0, BUFSIZ * 4);
  n = read(sockfd, bufs->buf, BUFSIZ);
  if (n <= 0) {
#ifdef _DEBUG_
      printf("sock, read error %d %s\n", errno, strerror(errno));
#endif //_DEBUG_
      bufs->ret = -3;
      goto error_exit;
  }

// #ifdef _DEBUG_
//   printf("from server = \n%s\n", bufs->buf);
// #endif //_DEBUG_
  close(sockfd);
  bufs->ret = 0;

  delete [] bufs->buf;
  delete bufs;
  return;

error_exit:
  delete [] bufs->buf;
  delete bufs;
  close(sockfd);
  return;
}

namespace itat {

int global_userid_ = 0;

static threadpool_t* g_thpool;

DjangoAPI::DjangoAPI() {
  g_thpool = threadpool_create(THREAD_POOL_SIZE, THREAD_POOL_QUEUE_SIZE, 0);
}

static const char *sendingmsg = {
  "GET /api/accept?pl_ex_id=%d&pl_id=%d&node_id=%d&status=%d&\
check_status=%d&code=%d&str_out=%s&str_err=%s HTTP/1.1\r\n\
Host: %s:%d\r\n\
Accept: text/html,application/json\r\n\
Connection: keep-alive\r\n\
\r\n"
};



DjangoAPI::~DjangoAPI() {
  threadpool_destroy(g_thpool, 0);
  g_thpool = nullptr;
}

void DjangoAPI::make_send_msg() {}

int DjangoAPI::send_graph_status(int pl_ex_id, int graph_id, int node_id,
                                 STATE_TYPE run_state, STATE_TYPE check_state,
                                 int code, const char *strout,
                                 const char *strerr) {

  int ret = 0;


  update_bill_exec_node(pl_ex_id, graph_id, maps_->node_mysql_map[node_id],
                                  run_state, check_state, g_h_db_, code, strout,
                                  strerr);

  update_bill_exec_pipeline(pl_ex_id, graph_id, maps_->node_mysql_map[node_id],
                              run_state, check_state, g_h_db_, code, strout, strerr);

  update_bill_checked_pipeline(pl_ex_id, graph_id, maps_->node_mysql_map[node_id],
                               run_state, check_state, g_h_db_, code, strout,
                               strerr,
                               global_userid_);

  update_bill_checked_node(pl_ex_id, graph_id, maps_->node_mysql_map[node_id],
                             run_state, check_state, g_h_db_, code, strout, strerr);

  // if (!ret) {
  char* buf = new char[BUFSIZ * 8];
  char* cmd = buf + BUFSIZ * 4;
  snprintf(cmd, BUFSIZ * 4, sendingmsg, pl_ex_id, graph_id,
           maps_->node_mysql_map[node_id], run_state, check_state, code, strout,
           strerr, django_ip, django_port);
  buffers * bufs = new buffers;
  bufs->buf = buf;
  bufs->cmd = cmd;

// #ifdef _DEBUG_
//   printf("DEBUG DEBUG %u %u %u\n", (uint64_t)bufs, (uint64_t)bufs->buf, (uint64_t)bufs->cmd);
// #endif //_DEBUG_

  // ret = itat_httpc(&param_, buf_, buf_);
  //ret = sock(&bufs_);

  threadpool_add(g_thpool, sock, bufs, 0);

  // #ifdef _DEBUG_
  std::cout << global_userid_ << ", " << pl_ex_id << ", " << graph_id << ", "
            << node_id << ", " << maps_->node_mysql_map[node_id] << ", "
            << str_status[run_state] << ", " << str_status[check_state] << ", "
            << code << ", " << strout << ", " << strerr << std::endl;
  // #endif //_DEBUG
  //}
  return ret;
}

} // namespace itat
