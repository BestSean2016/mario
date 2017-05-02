#ifndef PYAXAMYSQL_H
#define PYAXAMYSQL_H
#include <functional>
#include <iostream>
#include <string.h>
#include <time.h>

#include <map>
#include <set>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

typedef struct COPYFILE_RULE {
  int    exeid;
  string MatchID;
  string ScriptID;
  string Rules;
  int    RunOrder;
  string TargetPC;
  int    TargetPathDayFormat;
  int    TargetFileDayFormat;
  string TargetPath;
  int    TargetCreateOkFile;
  int    TargetPort;
  unsigned int FileCount;
  string TargetPyPath;
  string TargetScriptPath;
} COPYFILE_RULE;

typedef struct COPYFILE_SCRIPT {
  int    exeid;
  string MatchID;
  string MinionID;
  string Name;
  string Describe;
  string SourcePath;
  int    CopyRules;
  int    IsOK;
  int    IsTime;
  int    filepathDayFormat;
  int    IsRunOnly;
  time_t RunOnlyTime;
  int    RunCycle;
  int    RunCycle1;
} COPYFILE_SCRIPT;


typedef struct COPYFILE_EXERECORD {
  int    exeid         ;
  string MatchID       ;
  string SourcePathFile;
  string SourcePC      ;
  string TargetPC      ;
  int    TargetPort    ;
  string TargetPathFile;
  string State         ;
  time_t BegTime       ;
  time_t EndTime       ;
  string Note          ;
  string RunCommand    ;
  string ResultState   ;
  string ResultInfo    ;

  COPYFILE_EXERECORD()
   : exeid(0), TargetPort(0) {  }
} COPYFILE_EXERECORD;

// MySQL [copyfile]> desc k_defaultcalendar;
// +----------+--------------+------+-----+---------+-------+
// | Field    | Type         | Null | Key | Default | Extra |
// +----------+--------------+------+-----+---------+-------+
// | sName    | varchar(128) | NO   | PRI |         |       |
// | iYear    | int(11)      | NO   | PRI | 0       |       |
// | iMonth   | int(11)      | NO   | PRI | 0       |       |
// | iDay     | int(11)      | NO   | PRI | 0       |       |
// | InitFlag | int(11)      | YES  |     | 0       |       |
// | TimeFlag | varchar(20)  | YES  |     | NULL    |       |
// +----------+--------------+------+-----+---------+-------+

typedef struct COPYFILE_DEFCALENDAR {
  string sName;
  int iYear;
  int iMonth;
  int iDay;
  int InitFlag;
  string TimeFlag;

  COPYFILE_DEFCALENDAR()
      : iYear(0), iMonth(0), iDay(0), InitFlag(0) {}

  time_t get_time_zero() {
    time_t t = time(0);
    struct tm stm;
    localtime_r(&t, &stm);
    stm.tm_sec = stm.tm_min = stm.tm_hour = 0;
    stm.tm_year = iYear - 1900;
    stm.tm_mon = iMonth - 1;
    stm.tm_mday = iDay;
    return mktime(&stm);
  }

} COPYFILE_DEFCALENDAR;


typedef struct COPYFILE_FILE_MATCH {
  int id;
  string minion;
  string python;
  string matchpy;
} COPYFILE_FILE_MATCH;


typedef struct COPYFILE_AUTOID {
  int nextid;
}COPYFILE_AUTOID;

#include <mysql/mysql.h>

#define mysql_scpy(str, len, dptr)                                             \
  if ((dptr))                                                                  \
    strcpy_s((str), (len), (dptr));                                            \
  else                                                                         \
    *str = 0;

#define mysql_str(dptr) ((dptr) ? dptr : "")

#define mysql_atoll(dptr) ((dptr) ? atoll(dptr) : 0)

#define mysql_atol(dptr) ((dptr) ? atol(dptr) : 0)

#define mysql_atoi(dptr) ((dptr) ? atoi(dptr) : 0)

typedef void *DBHANDLE;
/*
template <typename T> T *new_obj() {
  T *t = new T;
  memset(t, 0, sizeof(T));
  return t;
}

template <typename T>
int query_data(std::vector<T> &vecDataSet, DBHANDLE dbh, const char *sql,
               std::function<int(void*, MYSQL_ROW &)> get_field,
               const char *where = 0) {
  if (!dbh)
    return -1;

  int nCount = 0;

  char *query = 0;
  if (where) {
    query = new char[strlen(sql) + strlen(where) + strlen(" WHERE ") + 1];
    snprintf(query, strlen(sql) + strlen(where) + strlen(" WHERE ") + 1,
             "%s WHERE %s", sql, where);
  } else
    query = (char *)sql;

  int res = mysql_query(reinterpret_cast<MYSQL *>(dbh), query);
  if (res) {
    fprintf(stdout, "SELECT error: %s by %s\n",
            mysql_error(reinterpret_cast<MYSQL *>(dbh)), query);
    if (where)
      delete query;
    return -2;
  } else {
    MYSQL_RES *res_ptr = mysql_store_result(reinterpret_cast<MYSQL *>(dbh));

    if (res_ptr) {
      nCount = mysql_num_rows(res_ptr);
      if (nCount <= 0) {
        mysql_free_result(res_ptr);
        vecDataSet.clear();
        return 0;
      }

      MYSQL_ROW mysql_row;
      while ((mysql_row = mysql_fetch_row(res_ptr))) {
        T t;
        get_field(&t, mysql_row);
        vecDataSet.emplace_back(t);
      }

      if (mysql_errno(reinterpret_cast<MYSQL *>(dbh))) {
        fprintf(stdout, "Retrive an error: %s\n",
                mysql_error(reinterpret_cast<MYSQL *>(dbh)));
        nCount = -3;
      }
      mysql_free_result(res_ptr);
    }
  }
  if (where)
    delete[] query;
  return nCount;
}
*/
extern const char* pyaxa_query_sql[];
extern const char* pyaxa_write_sql[];

extern DBHANDLE connect_db(const char *host, int port, const char *db,
                           const char *user, const char *passwd);
extern void disconnect_db(DBHANDLE dbh);
extern int exec_db(DBHANDLE db, const char*sql);

extern int get_rules(void* r, MYSQL_ROW& row);
extern int get_scripts(void* s, MYSQL_ROW& row);
extern int get_exec_record(void* s, MYSQL_ROW& row);
extern int get_caledar(void* s, MYSQL_ROW& row);
extern int get_match_file(void* s, MYSQL_ROW& row);
extern int get_next_record_id(void*s, MYSQL_ROW& row);

const char comma = ',';
const char que = '\'';
const char spc = ' ';

const std::string strtype(typeid(std::string("123")).name());
const std::string timetype(typeid(time_t(0)).name());

template <typename T>
bool is_string(T& t) { return strtype == typeid(t).name();}
template <typename T>
bool is_timet(T& t) { return timetype == typeid(t).name();}

extern ostream& operator << (ostream& out, COPYFILE_RULE& rule);
extern ostream& operator << (ostream& out, COPYFILE_SCRIPT& script);
extern ostream& operator << (ostream& out, COPYFILE_EXERECORD& script);
extern ostream& operator << (ostream& out, COPYFILE_DEFCALENDAR& caledar);
extern ostream& operator << (ostream& out, COPYFILE_FILE_MATCH & fm);

extern istream& operator >> (istream& in, COPYFILE_RULE& rule);

template<typename T>
ostream& operator << (ostream& out, std::vector<T> vec) {
  for(auto& p: vec)
    out << p << std::endl;
  out << std::endl;
  return out;
}

template <typename T>
string prefix(T& t);
template <typename T>
string postfix(T& t);

#endif // PYAXAMYSQL_H
