#include "config.h"
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <signal.h>

using namespace std;
#include "httpapi.hpp"
#include "pyaxa-mysql.hpp"
#include "saltapi.hpp"


using namespace itat;

static char  MYSQL_DB_HOST[41] = {"10.10.250.250"};
static short MYSQL_DB_PORT = 13579;
static char  MYSQL_DB_NAME[41] = {"copyfile"};
static char  MYSQL_DB_USER[41] = {"root"};
static char  MYSQL_DB_PASS[41] = {"hongt@8a51"};
static char  SALT_API_HOST[41] = {"10.10.10.12"};
static short SALT_API_PORT = 8000;
static char  SALT_API_USER[41] = {"sean"};
static char  SALT_API_PASS[41] = {"hongt@8a51"};

// {"return": [{"old080027856836": {"pid": 4020, "retcode": 0, "stderr": "",
// "stdout": "start...\r\nGroup_1\r\n624956f0-ce83-11e6-a274-080027856836|salt
// old080027856836 cmd.run_all 'C:\\python27\\python C:\\FileMatch\\fmClient.py
// 127.0.0.1 30001 C:\\test\\aaa.txt c:\\20161229\\aaa.txt
// -1'\r\n624956f0-ce83-11e6-a274-080027856836|salt old080027856836 cmd.run_all
// 'C:\\python27\\python C:\\FileMatch\\fmClient.py 127.0.0.1 30001
// C:\\test\\02.jpg c:\\20161230\\02.jpg -1'\r\nGroup_2\r\nfinish."}}]}

//
// start...\r\n
// Group_1\r\n
// 624956f0-ce83-11e6-a274-080027856836|salt old080027856836 cmd.run_all
// 'C:\\python27\\python C:\\FileMatch\\fmClient.py 127.0.0.1 30001
// C:\\test\\aaa.txt c:\\20161229\\aaa.txt -1'\r\n
// 624956f0-ce83-11e6-a274-080027856836|salt old080027856836 cmd.run_all
// 'C:\\python27\\python C:\\FileMatch\\fmClient.py 127.0.0.1 30001
// C:\\test\\02.jpg c:\\20161230\\02.jpg -1'\r\n
// Group_2\r\n
// finish.
//

const char *gnl("\\r\\n");
const char *start{"start...\\r\\n"};
const char *finish{"finish."};
const char *salt_cmd{"|salt "};
const char *cmd_runall{" cmd.run_all \'"};

// typedef struct copyjob {
//   int ret_code;
//   string result;
//
//   string copyid;
//   string cmd;
//   string minion;
//   string fun;       // cmd.run_all
//   string python;    // c:\\pythong\\python
//   string clientpy;  // c:\\filematch\\fmclient.py
//   string server_ip; // 127.0.0.1
//   int server_port;  // 30001
//   string source_path;
//   string dest_path;
//
// } COPYJOB;

int parase_file_match(const char *json, size_t len, PARAM p1, PARAM p2) {
  (void)p1;
  (void)p2;

// char json[512]{"{\"return\": [{\"old080027856836\": {\"pid\": 4020,
// \"retcode\": 0, \"stderr\": \"\", \"stdout\":
// \"start...\\r\\nGroup_1\\r\\n624956f0-ce83-11e6-a274-080027856836|salt
// old080027856836 cmd.run_all 'C:\\\\python27\\\\python
// C:\\\\FileMatch\\\\fmClient.py 127.0.0.1 30001 C:\\\\test\\\\aaa.txt
// c:\\\\20161229\\\\aaa.txt -1'\\r\\n624956f0-ce83-11e6-a274-080027856836|salt
// old080027856836 cmd.run_all 'C:\\\\python27\\\\python
// C:\\\\FileMatch\\\\fmClient.py 127.0.0.1 30001 C:\\\\test\\\\02.jpg
// c:\\\\20161230\\\\02.jpg -1'\\r\\nGroup_2\\r\\nfinish.\"}}]}"};
// size_t len = strlen(json);

#ifdef _DEBUG_
    show_cstring(json, len);
#endif //_DEBUG_

  char *jstr = (char *)json;
  char last = jstr[len];
  jstr[len] = 0;

  char *s = strstr(jstr, start);
  char *e = strstr(jstr, finish);

  if (!s || !e)
    return -1;

  char *ptr = s + strlen(start);
  char *line = ptr;
  auto jobs = (vector<COPYFILE_EXERECORD> *)p1;

  while (ptr < e) {
    if (!strncmp(ptr, gnl, strlen(gnl))) {
      *ptr = 0;

      if (strstr(line, salt_cmd)) {
        if (line[strlen(line) - 1] != '\'')
          return -2;
        if (!strstr(line, cmd_runall))
          return -3;

        cout << line << endl;

        COPYFILE_EXERECORD job;

        // job.result = line;

        char *tmp = strchr(line, '|');
        *tmp = 0;
        job.MatchID = line;
        *tmp = '|';

        line = tmp + strlen(salt_cmd);
        tmp = strchr(line, ' ');
        *tmp = 0;
        job.SourcePC = line;
        *tmp = ' ';

        line = tmp + strlen(cmd_runall);
        tmp = ptr - 1;
        *tmp = 0;
        job.RunCommand = line;
        *tmp = '\'';

        // C:\python27\python C:\FileMatch\fmClient.py 127.0.0.1 30001
        // C:\test\aaa.txt c:\20161229\aaa.txt -1
        tmp = line;
        tmp = strchr(tmp, ' ');
        *tmp = 0;
        // job.python = line;
        *tmp = ' ';

        line = ++tmp;
        tmp = strchr(tmp, ' ');
        *tmp = 0;
        // job.clientpy = line;
        *tmp = ' ';

        line = ++tmp;
        tmp = strchr(tmp, ' ');
        *tmp = 0;
        job.TargetPC = line;
        *tmp = ' ';

        line = ++tmp;
        tmp = strchr(tmp, ' ');
        *tmp = 0;
        job.TargetPort = atoi(line);
        *tmp = ' ';

        line = ++tmp;
        tmp = strchr(tmp, ' ');
        *tmp = 0;
        job.SourcePathFile = line;
        *tmp = ' ';

        line = ++tmp;
        tmp = strchr(tmp, ' ');
        *tmp = 0;
        job.TargetPathFile = line;
        *tmp = ' ';

        job.State = "Copying ... ";
        job.BegTime = time(0);

        jobs->emplace_back(job);
      }

      *ptr = '\\';
      line = ptr + strlen(gnl);
    }
    ++ptr;
  }

  jstr[len] = last;
  return 0;
}

int match_file(std::vector<COPYFILE_FILE_MATCH> &fm,
               vector<COPYFILE_EXERECORD> &jobs) {
  HTTP_API_PARAM param(SALT_API_HOST, SALT_API_PORT, parse_token_fn, nullptr,
                       nullptr);
  salt_api_login(&param, SALT_API_USER, SALT_API_PASS);

  param.rf = parase_file_match;
  param.param1 = &jobs;

  if (salt_api_cmd_runall(&param, fm[0].minion.c_str(),
                          (fm[0].python + " " + fm[0].matchpy).c_str()))
    return -1;

  return 0;
}

static int get_record_id(DBHANDLE &db) {
  vector<COPYFILE_AUTOID> aid;
  query_data(aid, db, pyaxa_query_sql[5], get_next_record_id);
  return aid[0].nextid;
}

static int get_retcode(const char *json) {
  static char retcode[] = {"\"retcode\": "};
  char *s = strstr((char *)json, retcode) + strlen(retcode);
  char *e = strchr(s, ',');
  if (!s || !e)
    return -1;

  *e = 0;
  int ret = atoi(s);
  *e = ',';

  return ret;
}

//{"return": [{"old080027856836": {"pid": 240, "retcode": 0, "stderr": "",
//"stdout": ""}}]}
static int parse_copy_result(const char *json, size_t len, PARAM p1, PARAM p2) {
  (void)p2;
#ifdef _DEBUG_
    show_cstring(json, len);
#endif //_DEBUG_

  COPYFILE_EXERECORD *er = (COPYFILE_EXERECORD *)p1;
  er->Note.append((char *)json, len);
  int retcode = get_retcode(json);
  er->EndTime = time(0);
  if (retcode == 0)
    er->State = "OK!";
  else
    er->State = "Error!";
  ostringstream oss;
  oss << retcode;
  er->ResultState = oss.str();
  return retcode;
}

#include <memory>   // For std::unique_ptr
#include <stdarg.h> // For va_start, etc.

std::string string_format(const std::string fmt_str, ...) {
  int final_n,
      n = ((int)fmt_str.size()) *
          2; /* Reserve two times as much as the length of the fmt_str */
  std::unique_ptr<char[]> formatted;
  va_list ap;
  while (1) {
    formatted.reset(
        new char[n]); /* Wrap the plain char array into the unique_ptr */
    strcpy(&formatted[0], fmt_str.c_str());
    va_start(ap, fmt_str);
    final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
    va_end(ap);
    if (final_n < 0 || final_n >= n)
      n += abs(final_n - n + 1);
    else
      break;
  }
  return std::string(formatted.get());
}

static void update_to_error(COPYFILE_EXERECORD &er, int errorcode) {
  er.EndTime = time(0);
  er.State = "Error!";
  ostringstream oss;
  oss << errorcode;
  er.ResultState = oss.str();
}

static int copy_file(vector<COPYFILE_EXERECORD> &jobs, DBHANDLE &db) {
  ostringstream oss;
  int ret = 0;

  for (auto &p : jobs) {
    p.exeid = get_record_id(db);

    // insert to db
    oss.str("");
    oss << pyaxa_write_sql[2] << p << ");";
    std::cout << oss.str() << std::endl;

    ret = exec_db(db, oss.str().c_str());
    if (ret)
      break;

    HTTP_API_PARAM param(SALT_API_HOST, SALT_API_PORT, parse_copy_result, &p,
                         nullptr);
    ret = salt_api_cmd_runall(&param, p.SourcePC.c_str(), p.RunCommand.c_str());

    // "UPDATE k_exerecord SET "
    // "State = \'%s\', "
    // "EndTime = now(), "
    // "ResultState = \'%s\', "
    // "Note = \'%s\' "
    // "WHERE exeid = %d",

    // update to db
    if (ret)
      update_to_error(p, ret);

    string update =
        string_format(pyaxa_write_sql[3], p.State.c_str(),
                      p.ResultState.c_str(), p.Note.c_str(), p.exeid);
    cout << update << endl;
    ret = exec_db(db, update.c_str());
    if (ret)
      break;
  }
  return ret;
}

static void make_not_work_set(std::vector<COPYFILE_DEFCALENDAR>&cs, std::set<time_t>& not_work) {
  for(auto &p : cs)
    not_work.insert(p.get_time_zero());
}

static bool today_is_not_workday(std::set<time_t>& not_work) {
  time_t t = time(0);
  struct tm stm;
  localtime_r(&t, &stm);
  stm.tm_sec = stm.tm_min = stm.tm_hour = 0;
  t = mktime(&stm);

  return (not_work.find(t) != not_work.end());
}

void signal_handle(int sig) {
  cout << "got signal " << sig << endl;
  g_run = 0;
}

//
// MYSQL_DB_HOST   10.10.250.250
// MYSQL_DB_PORT   13579
// MYSQL_DB_NAME   copyfile
// MYSQL_DB_USER   root
// MYSQL_DB_PASS   hongt@8a51
// SALT_API_HOST   10.10.10.12
// SALT_API_PORT   8000
// SALT_API_USER   sean
// SALT_API_PASS   hongt@8a51
//

static int read_config(const char* config) {
  FILE* fp = 0;
  fp = fopen(config, "r");

  if (!fp) {
    cout << "Can not open " << config  << endl;
    return -1;
  }

  char key[512];
  char value[512];
  char buf[1024];

  fgets(buf, 1023, fp);
  sscanf(buf, "%s%s", key, value);
  strcpy_s(MYSQL_DB_HOST, 41, value);

  fgets(buf, 1023, fp);
  sscanf(buf, "%s%s", key, value);
  MYSQL_DB_PORT = atoi(value);

  fgets(buf, 1023, fp);
  sscanf(buf, "%s%s", key, value);
  strcpy_s(MYSQL_DB_NAME, 41, value);

  fgets(buf, 1023, fp);
  sscanf(buf, "%s%s", key, value);
  strcpy_s(MYSQL_DB_USER, 41, value);

  fgets(buf, 1023, fp);
  sscanf(buf, "%s%s", key, value);
  strcpy_s(MYSQL_DB_PASS, 41, value);

  fgets(buf, 1023, fp);
  sscanf(buf, "%s%s", key, value);
  strcpy_s(SALT_API_HOST, 41, value);

  fgets(buf, 1023, fp);
  sscanf(buf, "%s%s", key, value);
  SALT_API_PORT = atoi(value);

  fgets(buf, 1023, fp);
  sscanf(buf, "%s%s", key, value);
  strcpy_s(SALT_API_USER, 41, value);

  fgets(buf, 1023, fp);
  sscanf(buf, "%s%s", key, value);
  strcpy_s(SALT_API_PASS, 41, value);

  fclose(fp);

  return 0;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: py-axa <config file>\n");
    exit(0);
  }

  if(read_config(argv[1])) return (-1);

  DBHANDLE db = connect_db(MYSQL_DB_HOST, MYSQL_DB_PORT, MYSQL_DB_NAME,
                           MYSQL_DB_USER, MYSQL_DB_PASS);

  std::vector<COPYFILE_RULE> rules;
  std::vector<COPYFILE_SCRIPT> scripts;
  std::vector<COPYFILE_EXERECORD> ers;
  std::vector<COPYFILE_DEFCALENDAR> cs;
  std::vector<COPYFILE_FILE_MATCH> fm;
  std::set<time_t>not_work;

  query_data(rules, db, pyaxa_query_sql[0], get_rules);
  query_data(scripts, db, pyaxa_query_sql[1], get_scripts);
  query_data(ers, db, pyaxa_query_sql[2], get_exec_record);
  query_data(cs, db, pyaxa_query_sql[3], get_caledar);
  query_data(fm, db, pyaxa_query_sql[4], get_match_file);

  disconnect_db(db);

  make_not_work_set(cs, not_work);
  if (today_is_not_workday(not_work)) return (0);

  signal(SIGINT, signal_handle);
  signal(SIGKILL, signal_handle);
  signal(SIGSTOP, signal_handle);

  // cout << rules;
  // cout << scripts;
  // cout << ers;
  // cout << cs;
  // cout << fm;

  // ostringstream oss;
  // oss << pyaxa_write_sql[1] << scripts[0] << ");";
  // std::cout << oss.str() << std::endl;
  //
  // oss.str("");
  // oss << pyaxa_write_sql[0] << rules[0] << ");";
  // std::cout << oss.str() << std::endl;

  // oss.str("");
  // oss << pyaxa_write_sql[2] << ers[0] << ");";
  // std::cout << oss.str() << std::endl;

  vector<COPYFILE_EXERECORD> jobs;

  g_run = 1;
  for (;g_run;) {
    int ret = match_file(fm, jobs);
    if (!ret) {
      db = connect_db(MYSQL_DB_HOST, MYSQL_DB_PORT, MYSQL_DB_NAME,
                      MYSQL_DB_USER, MYSQL_DB_PASS);
      ret = copy_file(jobs, db);
      disconnect_db(db);
      jobs.clear();

      this_thread::sleep_for(chrono::seconds(10));
    }
  }

  this_thread::sleep_for(chrono::seconds(30));
  return 0;
}
