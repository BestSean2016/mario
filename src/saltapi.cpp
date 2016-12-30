#include "saltapi.hpp"

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
 * @brief itat_httpc send cmd and receive response from salt api
 * @param hostname in, hostname
 * @param portno   in, port number
 * @param buf      in, buffer for receive
 * @param cmd      in, the command to run
 * @param parse_fun in, the parse response function
 * @param salt_job  in, the pointer to save salt_job
 * @param job       in, the prepared job
 * @return zero for good, otherwise for bad
 */

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
#ifdef _DEBUG_
    std::cout << g_token << std::endl;
#endif //_DEBUG_
  } else {
    show_cstring(ptr, len);
    return -1;
  }
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
  return itat_httpc(hostname, port, buf_login, buf_login, salt_cb.parse_token_cb, nullptr, nullptr);
}


int salt_api_testping(const char *hostname, int port, const char* target, PARAM param1, PARAM param2) {
  char* content = strstr((char*)salt_api_str[SALT_API_TYPE_TESTPING], "client=local_async");
  if (!content) return -1;

  bzero(tmp_buf, TEMPBUF_LEN);
  snprintf(tmp_buf, TEMPBUF_LEN - 1, content, target);
  snprintf(buf_test_ping, BUFSIZE, salt_api_str[SALT_API_TYPE_TESTPING],
           hostname, port, g_token, strlen(tmp_buf), tmp_buf);
  return itat_httpc(hostname, port, buf_test_ping, buf_test_ping, salt_cb.parase_my_job_cb, param1, param2);
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
  return itat_httpc(hostname, port, buf, cmd, salt_cb.parase_my_job_cb,
                     param1, param2); // parse_cmd_return
}

int salt_api_cmd_runall(const char *hostname, int port, const char *target,
                        const char *script, PARAM param1, PARAM param2) {
  (void)script;
  char buf[BUFSIZE * 2];
  char cmd[1024];
  if (!target) return -1;
  snprintf(cmd, 1024, salt_api_str[SALT_API_TYPE_RUNALL], g_token, target);
  int ret = itat_httpc(hostname, port, buf, cmd, salt_cb.parase_my_job_cb,
                     param1, param2); // parse_cmd_return
  if (ret)
    std::cerr << "Wo caO!!!!\n";
  return ret;
}

int salt_api_events(const char *hostname, int port, PARAM param1, PARAM param2) {
  char buf[BUFSIZE * 2];
  char cmd[1024];

  snprintf(cmd, 1024, salt_api_str[SALT_API_TYPE_EVENTS], g_token);

  return itat_httpc(hostname, port, buf, cmd, salt_cb.process_event_cb, param1, param2);
}

