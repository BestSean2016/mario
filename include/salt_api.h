#ifndef SALT_API_H
#define SALT_API_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>


typedef struct salt_job_new {
  int64_t     ple_id;            ///PIPELINE EXECUTIVE ID
  std::string tag;
  std::string tgt_type;
  std::string jid;
  std::string tgt;
  std::string stamp;
  time_t      stamp_sec;
  uint32_t    stamp_usec;
  std::string user;
  std::vector<std::string> arg;
  std::string fun;
  std::vector<std::string> minions;
} SALT_JOB_NEW;

typedef enum RETURN_TYPE {
    RETURN_TYPE_OBJECT,
    RETURN_TYPE_BOOL,
}RETURN_TYPE;

typedef struct salt_job_ret {
  int64_t ple_id;            ///PIPELINE EXECUTIVE ID
  RETURN_TYPE rettype;
  std::string tag;
  std::string stamp;
  time_t      stamp_sec;
  uint32_t    stamp_usec;
  uint32_t    pid;
  int         retcode;
  std::string stderr;
  std::string stdout;
  bool        success;
  std::string cmd;
  std::string jid;
  std::string fun;
  std::string minion_id;
} SALT_JOB_RET;


extern int parse_salt_job_new(SALT_JOB_NEW& job, const char* json_data);
extern int parse_salt_job_ret(SALT_JOB_RET& job, const char* json_data);

#endif // SALT_API_H
