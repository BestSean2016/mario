#ifndef SALT_API_H
#define SALT_API_H

#include <vector>
#include <map>

typedef struct salt_job_new {
  int64_t     ple_id;            ///PIPELINE EXECUTIVE ID
  time_t      stamp_sec;
  uint32_t    timerout;
  size_t      retnum;
  std::string tag;
  std::string tgt_type;
  std::string jid;
  std::string tgt;
  std::string stamp;  
  std::string user;
  std::vector<std::string> arg;
  std::string fun;
  std::vector<std::string> minions;
} SALT_JOB_NEW;

typedef enum RETURN_TYPE {
    RETURN_TYPE_OBJECT,
    RETURN_TYPE_BOOL,
} RETURN_TYPE;

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

typedef void* SALT_JOB_PTR;
typedef enum SALT_JOB_TYPE {
    SALT_JOB_TYPE_IGNORE,
    SALT_JOB_TYPE_NEW,
    SALT_JOB_TYPE_RET,
} SALT_JOB_TYPE;



typedef struct salt_job {
    SALT_JOB_TYPE type;
    SALT_JOB_PTR  ptr;
} SALT_JOB;

typedef std::string JOBID;
typedef std::string MINION;
typedef std::map<MINION, SALT_JOB_RET*> MapMinionRet;   //job return
typedef std::map<JOBID, SALT_JOB_NEW*> MapJid2Job;      //job new
typedef std::map<JOBID, MapMinionRet*> MapJid2Minions;
typedef MapJid2Minions::iterator MJ2M_Iterator;

typedef struct JobMap {
  MapJid2Job     jobs;
  MapJid2Minions minions;
} JOBMAP;


typedef struct cstring {
  char *ptr;
  size_t len;
} CSTRING;

extern char* get_line(const char* buf, int* lines);
extern char* get_line(const char* buf);


extern void init_string(struct cstring *s);
extern void free_string(struct cstring* s);
extern size_t writeone(void *ptr, size_t size, size_t nmemb, struct cstring *s);
extern size_t writefunc(void *ptr, size_t size, size_t nmemb, struct cstring *s);
extern size_t print_one(void *ptr, size_t size, size_t nmemb, struct cstring *s);
extern size_t parse_json(void *ptr, size_t size, size_t nmemb, SALT_JOB* job);


extern void curl_get_token();
extern int curl_salt_event();


extern int parse_salt_job_new(SALT_JOB_NEW *job, const char* json_data);
extern int parse_salt_job_ret(SALT_JOB_RET *job, const char* json_data);
extern int parse_salt_job(SALT_JOB *job, const char* json_data);
extern int parse_salt_job(SALT_JOB *job, const char *json_data, size_t len);
extern int parse_salt_jobmap(JOBMAP* jobmap, const char* json_data, size_t len);

extern void free_salt_job(SALT_JOB *job);


extern std::ostream& operator << (std::ostream& out, SALT_JOB_NEW& jobnew);
extern std::ostream& operator << (std::ostream& out, SALT_JOB_RET& jobret);
extern void show_job(SALT_JOB* job);

extern void thread_check_timer_out();
extern void thread_run_pipeline();

extern void shot_token();
extern int curl_run_cmd(int cmd_index);

extern void test_get_token();
#endif // SALT_API_H

