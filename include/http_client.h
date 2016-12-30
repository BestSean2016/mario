#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include "salt_api.h"

typedef enum SALT_API_TYPE {
  SALT_API_TYPE_LOGIN,
  SALT_API_TYPE_TESTPING,
  SALT_API_TYPE_TEST_CMDRUN,
  SALT_API_TYPE_EVENTS,
  SALT_API_TYPE_RUNALL,
} SALT_API_TYPE;

typedef int (*response_function) (const char* json_data, size_t datalen, void* param1, void* param2);


extern int salt_api_login(const char *hostname, int port);
extern int salt_api_testping(const char *hostname, int port, int64_t pid, int64_t nodeid);
extern int salt_api_test_cmdrun(const char* hostname, int port, int64_t pid, int64_t nodeid);
extern int salt_api_events(const char* hostname, int port);
extern int salt_api_cmd_runall(const char* hostname, int port, const char* minion, const char* script, int64_t pid, int64_t nodeid);

extern int http_client(const char *hostname, int portno, char* buf,
                       const char* cmd, response_function parse_fun, SALT_JOB* jobmap, SALT_JOB* job);


#endif // HTTP_CLIENT_H
