#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H


typedef enum SALT_API_TYPE {
  SALT_API_TYPE_LOGIN,
  SALT_API_TYPE_TESTPING,
  SALT_API_TYPE_TEST_CMDRUN,
  SALT_API_TYPE_EVENTS,
} SALT_API_TYPE;

typedef int (*parse_response) (const char* json_data, size_t datalen, void* object);

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

extern int salt_api_login(const char *hostname, int port);
extern int salt_api_testping(const char *hostname, int port);
extern int salt_api_testrun(const char* hostname, int port);
extern int salt_api_events(const char* hostname, int port, int *run);

extern int http_client(const char *hostname, int portno, char* buf,
                       const char* cmd, parse_response parse_fun, void *object, int *run);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // HTTP_CLIENT_H
