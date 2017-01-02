#include <iostream>
#include <thread>
#include "config.h"
#include <functional>
#include <sstream>
#include <string>

using namespace std;
#include "httpapi.hpp"
#include "pyaxa-mysql.hpp"
#include "saltapi.hpp"

const char* MYSQL_DB_HOST{"10.10.250.250"};
const short MYSQL_DB_PORT{13579};
const char* MYSQL_DB_NAME{"copyfile"};
const char* MYSQL_DB_USER{"root"};
const char* MYSQL_DB_PASS{"hongt@8a51"};
const char* SALT_API_HOST{"10.10.10.12"};
const short SALT_API_PORT{8000};
const char* SALT_API_USER{"sean"};
const char* SALT_API_PASS{"hongt@8a51"};

// {"return": [{"old080027856836": {"pid": 4020, "retcode": 0, "stderr": "", "stdout": "start...\r\nGroup_1\r\n624956f0-ce83-11e6-a274-080027856836|salt old080027856836 cmd.run_all 'C:\\python27\\python C:\\FileMatch\\fmClient.py 127.0.0.1 30001 C:\\test\\aaa.txt c:\\20161229\\aaa.txt -1'\r\n624956f0-ce83-11e6-a274-080027856836|salt old080027856836 cmd.run_all 'C:\\python27\\python C:\\FileMatch\\fmClient.py 127.0.0.1 30001 C:\\test\\02.jpg c:\\20161230\\02.jpg -1'\r\nGroup_2\r\nfinish."}}]}

//
// start...\r\n
// Group_1\r\n
// 624956f0-ce83-11e6-a274-080027856836|salt old080027856836 cmd.run_all 'C:\\python27\\python C:\\FileMatch\\fmClient.py 127.0.0.1 30001 C:\\test\\aaa.txt c:\\20161229\\aaa.txt -1'\r\n
// 624956f0-ce83-11e6-a274-080027856836|salt old080027856836 cmd.run_all 'C:\\python27\\python C:\\FileMatch\\fmClient.py 127.0.0.1 30001 C:\\test\\02.jpg c:\\20161230\\02.jpg -1'\r\n
// Group_2\r\n
// finish.
//


const char* gnl("\\r\\n");
const char* start{"start...\\r\\n"};
const char* finish{"finish."};
const char* salt_cmd{"|salt "};
const char* cmd_runall{" cmd.run_all \'"};

typedef struct copyjob {
  int ret_code;
  string result;

  string copyid;
  string cmd;
  string minion;
  string fun;  //cmd.run_all
  string python; //c:\\pythong\\python
  string clientpy; //c:\\filematch\\fmclient.py
  string server_ip; //127.0.0.1
  int    server_port; //30001
  string source_path;
  string dest_path;

} COPYJOB;



int parase_file_match(const char* jn, size_t ln, PARAM p1, PARAM p2) {
  (void)p1;
  (void)p2;

  char json[512]{"{\"return\": [{\"old080027856836\": {\"pid\": 4020, \"retcode\": 0, \"stderr\": \"\", \"stdout\": \"start...\\r\\nGroup_1\\r\\n624956f0-ce83-11e6-a274-080027856836|salt old080027856836 cmd.run_all 'C:\\\\python27\\\\python C:\\\\FileMatch\\\\fmClient.py 127.0.0.1 30001 C:\\\\test\\\\aaa.txt c:\\\\20161229\\\\aaa.txt -1'\\r\\n624956f0-ce83-11e6-a274-080027856836|salt old080027856836 cmd.run_all 'C:\\\\python27\\\\python C:\\\\FileMatch\\\\fmClient.py 127.0.0.1 30001 C:\\\\test\\\\02.jpg c:\\\\20161230\\\\02.jpg -1'\\r\\nGroup_2\\r\\nfinish.\"}}]}"};

#ifdef _DEBUG_
  show_cstring(json, strlen(json));
#endif //_DEBUG_

  size_t len = strlen(json);
  char* jstr = (char*)json;
  char last = jstr[ len ];
  jstr[len] = 0;

  char* s = strstr(jstr, start);
  char* e = strstr(jstr, finish);

  if (!s || !e) return -1;

  char* ptr = s+ strlen(start);
  char* line = ptr;
  auto  jobs = (vector<COPYJOB>*)p1;

  while(ptr < e) {
    if (!strncmp(ptr, gnl, strlen(gnl))) {
      *ptr = 0;

      if (strstr(line, salt_cmd)) {
          if (line[strlen(line) - 1] != '\'') return -2;
          if (!strstr(line, cmd_runall)) return -3;

          cout << line << endl;

          COPYJOB job;
          job.result = line;

          char* tmp = strchr(line, '|');
          *tmp = 0;
          job.copyid = line;
          *tmp = '|';

          line = tmp + strlen(salt_cmd);
          tmp = strchr(line, ' ');
          *tmp = 0;
          job.minion = line;
          *tmp = ' ';

          line = tmp + strlen(cmd_runall);
          tmp = ptr - 1;
          *tmp = 0;
          job.cmd = line;
          *tmp = '\'';

          //C:\python27\python C:\FileMatch\fmClient.py 127.0.0.1 30001 C:\test\aaa.txt c:\20161229\aaa.txt -1
          tmp = line;
          tmp = strchr(tmp, ' ');
          *tmp = 0;
          job.python = line;
          *tmp = ' ';

          line = ++tmp;
          tmp = strchr(tmp, ' ');
          *tmp = 0;
          job.clientpy = line;
          *tmp = ' ';

          line = ++tmp;
          tmp = strchr(tmp, ' ');
          *tmp = 0;
          job.server_ip = line;
          *tmp = ' ';

          line = ++tmp;
          tmp = strchr(tmp, ' ');
          *tmp = 0;
          job.server_port = atoi(line);
          *tmp = ' ';

          line = ++tmp;
          tmp = strchr(tmp, ' ');
          *tmp = 0;
          job.source_path = line;
          *tmp = ' ';

          line = ++tmp;
          tmp = strchr(tmp, ' ');
          *tmp = 0;
          job.dest_path = line;
          *tmp = ' ';

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

int match_file(std::vector<COPYFILE_FILE_MATCH>& fm, vector<COPYJOB>& jobs) {
    HTTP_API_PARAM param(SALT_API_HOST, SALT_API_PORT, parse_token_fn, nullptr, nullptr);
    salt_api_login(param, SALT_API_USER, SALT_API_PASS);

    param.rf = parase_file_match;
    param.param1 = &jobs;

    if (salt_api_cmd_runall(param, fm[0].minion.c_str(),
                        (fm[0].python + " " + fm[0].matchpy).c_str()))
      return -1;



    return 0;
}


int save_to_db(vector<COPYJOB>& jobs, DBHANDLE& db) {
  return 0;
}


static int parse_copy_result(const char* json, size_t len, PARAM p1, PARAM p2) {
  show_cstring(json, len);
  return 0;
}

int copy_file(vector<COPYJOB>& jobs, DBHANDLE& db) {
  for (auto& p : jobs) {
    HTTP_API_PARAM param(SALT_API_HOST, SALT_API_PORT, parse_copy_result, &p, nullptr);
    salt_api_cmd_runall(param, p.minion.c_str(), p.cmd.c_str());
    //save to db
  }
  return 0;
}

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;

  DBHANDLE db = connect_db(MYSQL_DB_HOST, MYSQL_DB_PORT, MYSQL_DB_NAME, MYSQL_DB_USER, MYSQL_DB_PASS);

  std::vector<COPYFILE_RULE> rules;
  std::vector<COPYFILE_SCRIPT> scripts;
  std::vector<COPYFILE_EXERECORD> ers;
  std::vector<COPYFILE_DEFCALENDAR> cs;
  std::vector<COPYFILE_FILE_MATCH> fm;

  query_data(rules, db, pyaxa_query_sql[0], get_rules);
  query_data(scripts, db, pyaxa_query_sql[1], get_scripts);
  query_data(ers, db, pyaxa_query_sql[2], get_exec_record);
  query_data(cs, db, pyaxa_query_sql[3], get_caledar);
  query_data(fm, db, pyaxa_query_sql[4], get_match_file);

  cout << rules;
  cout << scripts;
  cout << ers;
  cout << cs;
  cout << fm;

  ostringstream oss;
  oss << pyaxa_write_sql[1] << scripts[0] << ");";
  std::cout << oss.str() << std::endl;

  oss.str("");
  oss << pyaxa_write_sql[0] << rules[0] << ");";
  std::cout << oss.str() << std::endl;

  oss.str("");
  oss << pyaxa_write_sql[2] << ers[0] << ");";
  std::cout << oss.str() << std::endl;

  vector<COPYJOB> jobs;

  int ret = match_file(fm, jobs);
  if (!ret)
    ret = save_to_db(jobs, db);

  if (!ret)
    ret = copy_file(jobs, db);

  disconnect_db(db);

  return 0;
}
