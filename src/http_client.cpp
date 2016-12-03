#include "http_client.h"
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
#define BUFSIZE 65536

char g_token[32] = {0};

static const char *salt_api_str[] = {
    "POST /login HTTP/1.1\r\n"
    "Host: 10.10.10.19:8000\r\n"
    "Accept: application/json\r\n"
    "Content-Length: 43\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "\r\n"
    "username=sean&password=hongt@8a51&eauth=pam",

    "POST / HTTP/1.1\r\n"
    "Host: 10.10.10.19:8000\r\n"
    "Accept: application/json\r\n"
    "X-Auth-Token: %s\r\n"
    "Content-Length: 41\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "\r\n"
    "client=local_async&fun=test.ping&tgt=old*",

    "POST / HTTP/1.1\r\n"
    "Host: 10.10.10.19:8000\r\n"
    "Accept: application/json\r\n"
    "X-Auth-Token: %s\r\n"
    "Content-Length: 90\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "\r\n"
    "client=local_async&fun=cmd.run_all&tgt=old08002759F4B6&arg=c:\\new-"
    "salt\\ExecClient.exe abcd",

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

int http_client(const char *hostname, int portno, char *buf, const char *cmd,
                parse_response parse_fun, void *param1, void *param2) {
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
    // {
    //   struct timeval timeout;
    //   timeout.tv_sec =  30;
    //   timeout.tv_usec = 0;
    //
    //   if (0 != (ret = (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,
    //                               (char *)&timeout, sizeof(timeout)) < 0)))
    //     printf("setsockopt failed\n");
    //
    //   if (0 != (ret = (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO,
    //                               (char *)&timeout, sizeof(timeout)) < 0)))
    //     printf("setsockopt failed\n");
    // }

    while (!ret && *(int *)param2) {
      while (tmp < ptr) {
        if (*tmp != '\r')
          ++tmp;
        else {
          // printf("************** %s\n", line);
          if (parse_fun) // do not check error
            parse_fun(line, tmp - line, param1, 0);
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

  if (!ret && *(int *)param2) {
    fprintf(stdout, "ReStart Http Event Client!\n");
    goto restart_client;
  }
  return ret;
}

static int parse_token(const char *ptr, size_t len, void *ptrtoken,
                       void *param2) {
  // fprintf(stdout, "%s", (char *)ptr);
  //"token": "897b0cc93d59f10aaa46159e7dfba417d225b2cd"
  (void)len;
  (void)param2;
  char *pos = strstr((char *)ptr, "\"token\": \"");
  if (pos) {
    pos += strlen("\"token\": \"");
    char *end = strchr(pos, '\"');
    if (!end)
      return 0;
    strncpy((char *)ptrtoken, pos, end - pos);
    *((char *)ptrtoken + (end - pos + 1)) = 0;
    fprintf(stdout, "Token is %s\n", (char *)ptrtoken);
  } else
    return -1;
  return 0;
}

int salt_api_login(const char *hostname, int port) {
  char buf[BUFSIZE];

  return http_client(hostname, port, buf, salt_api_str[SALT_API_TYPE_LOGIN],
                     parse_token, g_token, 0);
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

int salt_api_testping(const char *hostname, int port, int64_t pid, int64_t nodeid) {
  char buf[BUFSIZE];
  char cmd[1024];
  snprintf(cmd, 1024, salt_api_str[SALT_API_TYPE_TESTPING], g_token);
  SALT_JOB* job = new SALT_JOB(pid, nodeid);
  return http_client(hostname, port, buf, cmd, parse_my_job, &gjobmap,
                     job); // parse_cmd_return
}

int salt_api_test_cmdrun(const char *hostname, int port, int64_t pid, int64_t nodeid) {
  char buf[BUFSIZE];
  char cmd[1024];
  snprintf(cmd, 1024, salt_api_str[SALT_API_TYPE_TEST_CMDRUN], g_token);
  SALT_JOB* job = new SALT_JOB(pid, nodeid);
  return http_client(hostname, port, buf, cmd, parse_my_job, &gjobmap,
                     job); // parse_cmd_return
}

int salt_api_cmd_runall(const char *hostname, int port, const char *minion,
                        const char *script, int64_t pid, int64_t nodeid) {
  (void)script;
  char buf[BUFSIZE];
  char cmd[1024];
  if (!minion) return -1;
  snprintf(cmd, 1024, salt_api_str[SALT_API_TYPE_RUNALL], g_token, minion);
  SALT_JOB* job = new SALT_JOB(pid, nodeid);
  return http_client(hostname, port, buf, cmd, parse_my_job, &gjobmap,
                     job); // parse_cmd_return
}

int salt_api_events(const char *hostname, int port, int *run) {
  char buf[BUFSIZE];
  char cmd[1024];

  snprintf(cmd, 1024, salt_api_str[SALT_API_TYPE_EVENTS], g_token);

  return http_client(hostname, port, buf, cmd, parse_job, &gjobmap, run);
}
