#include "itat.h"
#include <errno.h>
#include <error.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define BUFSIZE 32768   //65536

int g_run = 0;
//JOBMAP g_jobmap;
char g_token[TOKEN_LEN] = {0};

#define TEMPBUF_LEN 2048
static SALT_CALLBACK salt_cb;
static char global_buffer[BUFSIZE * 2 * 5 + TEMPBUF_LEN];
static char* tmp_buf = 0;
static char* buf_login = 0;
static char* buf_test_ping = 0;
static char* buf_run_cmd = 0;

/**
 * @brief http_client send cmd and receive response from salt api
 * @param hostname in, hostname
 * @param portno   in, port number
 * @param buf      in, buffer for receive
 * @param cmd      in, the command to run
 * @param parse_fun in, the parse response function
 * @param salt_job  in, the pointer to save salt_job
 * @param job       in, the prepared job
 * @return zero for good, otherwise for bad
 */
static int http_client(const char *hostname, int portno, char* buf, const char* cmd, parse_response parse_fn, void* param1, void *param2);

static const char *salt_api_str[] = {
    "POST /login HTTP/1.1\r\n"
    "Host: %s:%d\r\n"
    "Accept: application/json\r\n"
    "Content-Length: 43\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "\r\n"
    "username=%s&password=%s&eauth=pam",

    "POST / HTTP/1.1\r\n"
    "Host: %s:%d\r\n"
    "Accept: application/json\r\n"
    "X-Auth-Token: %s\r\n"
    "Content-Length: %d\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "\r\n"
    "client=local_async&fun=test.ping&tgt=%s",

    "POST / HTTP/1.1\r\n"
    "Host: %s:%d\r\n"
    "Accept: application/json\r\n"
    "X-Auth-Token: %s\r\n"
    "Content-Length: %s\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "\r\n"
    "client=local_async&fun=cmd.run_all&tgt=%s&arg=%s",

    "GET /events HTTP/1.1\r\n"
    "Host: 10.10.10.19:8000\r\n"
    "Accept: application/json\r\n"
    "X-Auth-Token: %s\r\n"
    "\r\n",

    "POST / HTTP/1.1\r\n"
    "Host: 10.10.10.19:8000\r\n"
    "Accept: application/json\r\n"
    "X-Auth-Token: %s\r\n"
    "Content-Length: 94\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "\r\n"
    "client=local_async&fun=cmd.run_all&tgt=%s&arg=c:"
    "\\hongt\\Client\\ExecClient.exe abcd",
};
//client=local_async&fun=cmd.run_all&tgt=old080027C8BFA4&arg=c:\hongt\Client\ExecClient.exe abcd
//
// static char *get_line(const char *buf) {
//   char *ptr = (char *)buf;
//
//   if (!buf || !(*buf))
//     return 0;
//
//   while (ptr && *ptr && *ptr != '\r' && *ptr != '\n')
//     ++ptr;
//
//   if (*ptr == '\r') {
//     *ptr = 0;
//     ptr += 2;
//   }
//   return ptr;
// }
//
/*
HTTP/1.1 nnn OK
Content-Length: nnn
......

<data>
*/

static const char *_http_header_ = {"HTTP/1.1 "};
static const char *_conten_len_ = {"Content-Length: "};


static int parse_token_fn(const char *ptr, size_t len, void* param1, void* param2) {
  // fprintf(stdout, "%s", (char *)ptr);
  //"token": "897b0cc93d59f10aaa46159e7dfba417d225b2cd"
  UNUSE(len);
  UNUSE(param1);
  UNUSE(param2);

  char *pos = strstr((char *)ptr, "\"token\": \"");
  if (pos) {
    pos += strlen("\"token\": \"");
    char *end = strchr(pos, '\"');
    if (!end)
      return 0;
    if (end - pos > TOKEN_LEN - 1) return -2;
    memset(g_token, 0, TOKEN_LEN);
    strncpy(g_token, pos, end - pos);
  } else
    return -1;
  return 0;
}

void set_default_callback() {
  tmp_buf = global_buffer;
  buf_login = tmp_buf + TEMPBUF_LEN;
  buf_test_ping = buf_login + BUFSIZE * 2;
  buf_run_cmd = buf_test_ping + BUFSIZE * 2;

  salt_cb.parase_my_job_cb = 0;
  salt_cb.parse_job_cb = 0;
  salt_cb.parse_token_cb = parse_token_fn;
  salt_cb.process_event_cb = 0;
}

int salt_api_login(const char *hostname, int port, const char* user, const char* pass) {
  snprintf(buf_login, BUFSIZE, salt_api_str[SALT_API_TYPE_LOGIN], hostname, port, user, pass);
  return http_client(hostname, port, buf_login, buf_login, salt_cb.parse_token_cb, nullptr, nullptr);
}


int salt_api_testping(const char *hostname, int port, const char* target, PARAM param1, PARAM param2) {
  char* content = strstr((char*)salt_api_str[SALT_API_TYPE_TESTPING], "client=local_async");
  if (!content) return -1;

  bzero(tmp_buf, TEMPBUF_LEN);
  snprintf(tmp_buf, TEMPBUF_LEN - 1, content, target);
  snprintf(buf_test_ping, BUFSIZE, salt_api_str[SALT_API_TYPE_TESTPING],
           hostname, port, g_token, strlen(tmp_buf), tmp_buf);
  return http_client(hostname, port, buf_test_ping, buf_test_ping, salt_cb.parase_my_job_cb, param1, param2);
}

static int get_response_code(const char *str) {
  return atoi((char *)str + strlen(_http_header_));
}

static size_t get_content_len(const char *str) {
  return (size_t)atoll((char *)str + strlen(_conten_len_));
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
          (strncmp(line, _conten_len_, strlen(_conten_len_)) == 0))
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

static int http_client(const char *hostname, int portno, char *buf, const char *cmd,
                parse_response parse_fun, void *param1, void* param2) {
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

  if (!ret && parse_fun)
    ret = (0 != parse_fun(json_data, data_len, param1, param2));

  close(sockfd);
  return ret;

run_receive_long_data : {
    char *tmp = json_data;
    char *line = tmp;
// #ifndef _DEBUG_
    {
      struct timeval timeout;
      timeout.tv_sec =  30;
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
          if (parse_fun) // do not check error
            parse_fun(line, tmp - line, param1, param2);
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
                "i got n = %d bytes, error no %d, errmsg %s total_len is %d\n", n,
                errno, strerror(errno), total_len);
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
  }

  if (!ret && g_run) {
    fprintf(stdout, "ReStart Http Event Client!\n");
    goto restart_client;
  }

  fprintf(stdout, "exit http_client event listener\n");
  return ret;
}


// static int parse_cmd_return(const char *ptr, size_t len, void *obj) {
//   (void)len;
//   if (!obj && ptr) {
//     *((char *)ptr + len) = 0;
//     printf("%s<--|\n", ptr);
//     return 0;
//   }
//
//   return 0;
// }

int salt_api_test_cmdrun(const char *hostname, int port, PARAM param1, PARAM param2) {
  char buf[BUFSIZE * 2];
  char cmd[1024];
  snprintf(cmd, 1024, salt_api_str[SALT_API_TYPE_TEST_CMDRUN], g_token);
  return http_client(hostname, port, buf, cmd, salt_cb.parase_my_job_cb,
                     param1, param2); // parse_cmd_return
}

int salt_api_cmd_runall(const char *hostname, int port, const char *target,
                        const char *script, PARAM param1, PARAM param2) {
  (void)script;
  char buf[BUFSIZE * 2];
  char cmd[1024];
  if (!target) return -1;
  snprintf(cmd, 1024, salt_api_str[SALT_API_TYPE_RUNALL], g_token, target);
  int ret = http_client(hostname, port, buf, cmd, salt_cb.parase_my_job_cb,
                     param1, param2); // parse_cmd_return
  if (ret)
    std::cerr << "Wo caO!!!!\n";
  return ret;
}

int salt_api_events(const char *hostname, int port, PARAM param1, PARAM param2) {
  char buf[BUFSIZE * 2];
  char cmd[1024];

  snprintf(cmd, 1024, salt_api_str[SALT_API_TYPE_EVENTS], g_token);

  return http_client(hostname, port, buf, cmd, salt_cb.process_event_cb, param1, param2);
}

