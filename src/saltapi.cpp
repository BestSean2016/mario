#include "saltapi.hpp"
#include "assert.h"

//JOBMAP g_jobmap;
char g_token[TOKEN_LEN] = {0};

#define TEMPBUF_LEN 2048
static SALT_CALLBACK salt_cb;

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
    "Content-Length: %d\r\n"
    "Content-type: application/json\r\n"
    "\r\n"
    "{\"username\":\"%s\",\"password\":\"%s\",\"eauth\":\"pam\"}",

    "POST / HTTP/1.1\r\n"
    "Host: %s:%d\r\n"
    "Accept: application/json\r\n"
    "X-Auth-Token: %s\r\n"
    "Content-Length: %d\r\n"
    "Content-type: application/json\r\n"
    "\r\n"
    "{\"client\":\"local_async\",\"fun\":\"test.ping\",\"tgt\":\"%s\"}",

    "POST / HTTP/1.1\r\n"
    "Host: %s:%d\r\n"
    "Accept: application/json\r\n"
    "X-Auth-Token: %s\r\n"
    "Content-Length: %d\r\n"
    "Content-type: application/json\r\n"
    "\r\n"
    "{\"client\":\"local_async\",\"fun\":\"cmd.run_all\",\"tgt\":\"%s\",\"arg\":\"%s\"}",

    "POST / HTTP/1.1\r\n"
    "Host: %s:%d\r\n"
    "Accept: application/json\r\n"
    "X-Auth-Token: %s\r\n"
    "Content-Length: %d\r\n"
    "Content-type: application/json\r\n"
    "\r\n"
    "{\"client\":\"local\",\"fun\":\"cmd.run_all\",\"tgt\":\"%s\",\"arg\":\"%s\"}",

    "GET /events HTTP/1.1\r\n"
    "Host: 10.10.10.19:8000\r\n"
    "Accept: application/json\r\n"
    "X-Auth-Token: %s\r\n"
    "\r\n",

    "POST / HTTP/1.1\r\n"
    "Host: %s:%d\r\n"
    "Accept: application/json\r\n"
    "X-Auth-Token: %s\r\n"
    "Content-Length: %d\r\n"
    "Content-type: application/json\r\n"
    "\r\n"
    "[{\"client\":\"local\", \"tgt\":\"%s\", \"fun\":\"cp.get_file\", \"arg\":[\"%s\", \"%s\"]}]"


    "client=local_async&fun=cmd.run_all&tgt=%s&arg=c:"
    "\\hongt\\Client\\ExecClient.exe abcd",


    "{\"client\":\"local_async\",\"tgt\":\"old080027856836\",\"fun\":\"cmd.run_all\",\"arg\":\"c:\\python27\\python C:\\FileMatch\\fmServer.py  30001\"}",

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
#ifdef _DEBUG_
    show_cstring(ptr, len);
#endif //_DEBUG_
    return -1;
  }
  return 0;
}

void set_default_callback() {
  salt_cb.parase_my_job_cb = 0;
  salt_cb.parse_job_cb = 0;
  salt_cb.parse_token_cb = parse_token_fn;
  salt_cb.process_event_cb = 0;
}


static char* get_contnt(const char* str) {
  char* content = strstr((char*)str, "\r\n\r\n");
  if (!content) return nullptr;
  else return content + 4;
}

#define SET_CONTENT(api_str_index) \
  char buffer[BUFSIZE * 2];\
  char* cmd = buffer + BUFSIZE;\
  char* tmp_buf = cmd + BUFSIZE / 2;\
  \
  char* content = get_contnt(salt_api_str[(api_str_index)]);\
  assert(content != nullptr);\



int salt_api_login(const char *hostname, int port, const char* user, const char* pass) {
  SET_CONTENT(SALT_API_TYPE_LOGIN);
  snprintf(tmp_buf, BUFSIZE / 2, content, user, pass);
  snprintf(cmd, BUFSIZE / 2, salt_api_str[SALT_API_TYPE_LOGIN], hostname, port, strlen(tmp_buf), user, pass);
  // show_cstring(buf_login, strlen(buf_login));
  return itat_httpc(hostname, port, buffer, cmd, salt_cb.parse_token_cb, nullptr, nullptr);
}


int salt_api_testping(const char *hostname, int port, const char* target, PARAM param1, PARAM param2) {
  SET_CONTENT(SALT_API_TYPE_TESTPING);

  snprintf(tmp_buf, BUFSIZE / 2, content, target);
  snprintf(cmd, BUFSIZE / 2, salt_api_str[SALT_API_TYPE_TESTPING],
           hostname, port, g_token, strlen(tmp_buf), target);
  // show_cstring(buf_test_ping, strlen(buf_test_ping));
  return itat_httpc(hostname, port, buffer, cmd, salt_cb.parase_my_job_cb, param1, param2);
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

int salt_api_async_cmd_runall(const char *hostname, int port, const char *target,
                        const char *script, PARAM param1, PARAM param2) {
  SET_CONTENT(SALT_API_TYPE_ASYNC_RUNALL);

  snprintf(tmp_buf, BUFSIZE / 2, content, target, script);
  snprintf(cmd, BUFSIZE / 2, salt_api_str[SALT_API_TYPE_ASYNC_RUNALL],
           hostname, port, g_token, strlen(tmp_buf), target, script);
  int ret = itat_httpc(hostname, port, buffer, cmd, salt_cb.parase_my_job_cb,
                     param1, param2);
  if (ret)
    std::cerr << "Wo caO!!!!\n";

  return ret;
}

int salt_api_cmd_runall(const char *hostname, int port, const char *target,
                        const char *script, PARAM param1, PARAM param2) {
  SET_CONTENT(SALT_API_TYPE_RUNALL);

  snprintf(tmp_buf, BUFSIZE / 2, content, target, script);
  snprintf(cmd, BUFSIZE / 2, salt_api_str[SALT_API_TYPE_RUNALL],
           hostname, port, g_token, strlen(tmp_buf), target, script);
  int ret = itat_httpc(hostname, port, buffer, cmd, salt_cb.parase_my_job_cb,
                     param1, param2);
  if (ret)
    std::cerr << "Wo caO!!!!\n";

  return ret;
}

int salt_api_cp_getfile(const char* hostname, int port, const char* target, const char* src_file, const char* des_file, PARAM param1, PARAM param2) {
    SET_CONTENT(SALT_API_TYPE_CP_GETFILE);
    snprintf(tmp_buf, BUFSIZE / 2, content, target, src_file, des_file);
    snprintf(cmd, BUFSIZE / 2, salt_api_str[SALT_API_TYPE_ASYNC_RUNALL],
             hostname, port, g_token, strlen(tmp_buf), target, src_file, des_file);
    return itat_httpc(hostname, port, buffer, cmd, salt_cb.parase_my_job_cb,
                         param1, param2);
}

int salt_api_events(const char *hostname, int port, PARAM param1, PARAM param2) {
  char buf[BUFSIZE * 2];
  char cmd[1024];

  snprintf(cmd, 1024, salt_api_str[SALT_API_TYPE_EVENTS], g_token);

  return itat_httpc(hostname, port, buf, cmd, salt_cb.process_event_cb, param1, param2);
}

