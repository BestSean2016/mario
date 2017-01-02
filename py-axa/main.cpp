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
  auto  jobs = (vector<COPYFILE_EXERECORD>*)p1;

  while(ptr < e) {
    if (!strncmp(ptr, gnl, strlen(gnl))) {
      *ptr = 0;

      if (strstr(line, salt_cmd)) {
          if (line[strlen(line) - 1] != '\'') return -2;
          if (!strstr(line, cmd_runall)) return -3;

          cout << line << endl;

          COPYFILE_EXERECORD job;
          //job.result = line;

          char* tmp = strchr(line, '|');
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

          //C:\python27\python C:\FileMatch\fmClient.py 127.0.0.1 30001 C:\test\aaa.txt c:\20161229\aaa.txt -1
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

int match_file(std::vector<COPYFILE_FILE_MATCH>& fm, vector<COPYFILE_EXERECORD>& jobs) {
    HTTP_API_PARAM param(SALT_API_HOST, SALT_API_PORT, parse_token_fn, nullptr, nullptr);
    salt_api_login(param, SALT_API_USER, SALT_API_PASS);

    param.rf = parase_file_match;
    param.param1 = &jobs;

    if (salt_api_cmd_runall(param, fm[0].minion.c_str(),
                        (fm[0].python + " " + fm[0].matchpy).c_str()))
      return -1;



    return 0;
}


static int get_record_id(DBHANDLE& db) {
  vector<COPYFILE_AUTOID> aid;
  query_data(aid, db, pyaxa_query_sql[5], get_next_record_id);
  return aid[0].nextid;
}

string get_retcode(const char* json) {
  static char* retcode{"\retcode\": "};
  char* s = strstr((char*)json, retcode) + strlen(retcode);
  char* e = strchr(s, ',');
  *e = 0;
  string ret = s;
  *e = ',';
  return ret;
}


//{"return": [{"old080027856836": {"pid": 240, "retcode": 0, "stderr": "", "stdout": ""}}]}
static int parse_copy_result(const char* json, size_t len, PARAM p1, PARAM p2) {
  (void*)p2;
  show_cstring(json, len);
  COPYFILE_EXERECORD* er = (COPYFILE_EXERECORD*)p1;
  er->Note = json;
  er->ResultState = get_retcode(json);
  int retcode = atoi(er->ResultState.c_str());
  if (retcode == 0)
      er->State = "OK!";
  else
      er->State = "Error!";

  return retcode;
}

#include <stdarg.h>  // For va_start, etc.
#include <memory>    // For std::unique_ptr

std::string string_format(const std::string fmt_str, ...) {
    int final_n, n = ((int)fmt_str.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
    std::unique_ptr<char[]> formatted;
    va_list ap;
    while(1) {
        formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
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


int copy_file(vector<COPYFILE_EXERECORD>& jobs, DBHANDLE& db) {
  ostringstream oss;
  for (auto& p : jobs) {
    p.exeid = get_record_id(db) - 1;

    //insert to db
    oss.str("");
    oss << pyaxa_write_sql[2] << p << ");";
    std::cout << oss.str() << std::endl;

    HTTP_API_PARAM param(SALT_API_HOST, SALT_API_PORT, parse_copy_result, &p, nullptr);
    salt_api_cmd_runall(param, p.SourcePC.c_str(), p.RunCommand.c_str());

    // "UPDATE k_exerecord SET "
    // "State = \'%s\', "
    // "EndTime = now(), "
    // "ResultState = \'%s\', "
    // "Note = \'%s\' "
    // "WHERE exeid = %d",

    //update to db
COPYFILE_EXERECORD job;
    string update = string_format(pyaxa_write_sql[3],
      job.State,
            job.ResultState,
            job.Note,
            job.exeid);
    cout << update << endl;

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

  int ret = match_file(fm, jobs);

  if (!ret)
    ret = copy_file(jobs, db);

  disconnect_db(db);

  return 0;
}
