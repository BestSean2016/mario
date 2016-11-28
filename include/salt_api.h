#ifndef SALT_API_H
#define SALT_API_H

#include <vector>
#include <map>

typedef struct salt_job_new {
  uint64_t    ple_id;            ///PIPELINE EXECUTIVE ID
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
    RETURN_TYPE_STRING,
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

extern JOBMAP gjobmap;


extern int parse_job(const char* json_data, size_t len, void *param1, void *param2);
extern int parse_new_job(const char* json_data, size_t len, void *param1, void *param2);


extern std::ostream& operator << (std::ostream& out, SALT_JOB_NEW& jobnew);
extern std::ostream& operator << (std::ostream& out, SALT_JOB_RET& jobret);
extern void show_job(SALT_JOB* job);

extern void thread_check_timer_out(int *run);
extern void thread_run_pipeline();

extern void jobmap_cleanup(JOBMAP *jm);

#endif // SALT_API_H

