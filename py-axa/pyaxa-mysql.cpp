#include "pyaxa-mysql.hpp"

static const int SELECT_K_RULES = 0;

const char* pyaxa_query_sql[] = {
    //0
    "select * from k_rules",

    //1
    "SELECT                               "
    "  exeid,                             "
    "  matchid,                           "
    "  MinionID,                          "
    "  `Name`,                            "
    "  `Describe`,                        "
    "  SourcePath,                        "
    "  CopyRules,                         "
    "  IsOK,                              "
    "  IsTime,                            "
    "  filepathDayFormat,                 "
    "  IsRunOnly,                         "
    "  UNIX_TIMESTAMP(RunOnlyTime) AS rot,"
    "  RunCycle,                          "
    "  RunCycle1                          "
    "FROM                                 "
    "  k_script                           ",

    //2
    "SELECT                       "
    "  exeid,                     "
    "  matchid,                   "
    "  SourcePathFile,            "
    "  SourcePC,                  "
    "  TargetPC,                  "
    "  TargetPort,                "
    "  TargetPathFile,            "
    "  State,                     "
    "  UNIX_TIMESTAMP(BegTime) bt,"
    "  UNIX_TIMESTAMP(EndTime) et,"
    "  Note,                      "
    "  RunCommand,                "
    "  ResultState,               "
    "  ResultInfo                 "
    "FROM                         "
    "  k_exerecord                ",

    //3
    "SELECT               "
    "    sName,           "
    "    iYear,           "
    "    iMonth,          "
    "    iDay,            "
    "    InitFlag,        "
    "    TimeFlag         "
    "FROM                 "
    "    k_defaultcalendar",

    //4
    "select * from py_file_match",

    //5
    "SELECT `AUTO_INCREMENT` FROM  INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA = 'copyfile' AND   TABLE_NAME   = 'k_exerecord'",

};



const char* pyaxa_write_sql[] = {
    //0
    "insert into k_rules ("
    "exeid              ,"
    "matchid            ,"
    "ScriptID           ,"
    "Rules              ,"
    "RunOrder           ,"
    "TargetPC           ,"
    "TargetPathDayFormat,"
    "TargetFileDayFormat,"
    "TargetPath         ,"
    "TargetCreateOkFile ,"
    "TargetPort         ,"
    "FileCount          ,"
    "TargetPyPath       ,"
    "TargetScriptPath    "
    ") values (",


    //1
    "INSERT INTO k_script("
    "  exeid,             "
    "  matchid,           "
    "  MinionID,          "
    "  `Name`,            "
    "  `Describe`,        "
    "  SourcePath,        "
    "  CopyRules,         "
    "  IsOK,              "
    "  IsTime,            "
    "  filepathDayFormat, "
    "  IsRunOnly,         "
    "  RunOnlyTime,       "
    "  RunCycle,          "
    "  RunCycle1) VALUES( ",


    //2
    "INSERT INTO k_exerecord ("
    "  exeid,                 "
    "  matchid,               "
    "  SourcePathFile,        "
    "  SourcePC,              "
    "  TargetPC,              "
    "  TargetPort,            "
    "  TargetPathFile,        "
    "  State,                 "
    "  BegTime,               "
    "  Note,                  "
    "  RunCommand,            "
    "  ResultState,           "
    "  ResultInfo) VALUES(    ",

    //3
    "UPDATE k_exerecord SET "
    "State = \'%s\', "
    "EndTime = now(), "
    "ResultState = \'%s\', "
    "Note = \'%s\' "
    "WHERE exeid = %d",

};

DBHANDLE connect_db(const char *host, int port, const char *db,
                    const char *user, const char *passwd) {
  // printf("connect_db ..... \n");

  MYSQL *mysql = mysql_init(NULL);

  if (mysql == 0) {
    fprintf(stdout, "%s\n", mysql_error(mysql));
    return nullptr;
  }

  if (mysql_real_connect(mysql, host, user, passwd, db, port, NULL, 0) ==
      NULL) {
    fprintf(stdout, "%s\n", mysql_error(mysql));
    mysql_close(mysql);
    return nullptr;
  }

  mysql_query(mysql, "SET NAMES utf8");

  // printf("connect mysql sucess!\n");
  return mysql;
}

void disconnect_db(DBHANDLE dbh) {
  if (dbh) {
    mysql_close((MYSQL *)dbh);
    mysql_server_end();
    // printf("disconnected_db\n");
  }
}


int exec_db(DBHANDLE db, const char* sql) {
  int ret = (db == nullptr);

  if (!ret)
    ret = mysql_query((MYSQL*)db, sql);

  return (ret);
}

#define FIELD_VALUE(x) prefix((x)) << (x) << postfix((x))


int get_rules(void *r, MYSQL_ROW& row) {
  COPYFILE_RULE& rule = *(COPYFILE_RULE*)r;
  rule.exeid = mysql_atoi(row[0]);
  rule.MatchID = mysql_str(row[1]);
  rule.ScriptID = mysql_str(row[2]);
  rule.Rules = mysql_str(row[3]);
  rule.RunOrder = mysql_atoi(row[4]);
  rule.TargetPC = mysql_str(row[5]);
  rule.TargetPathDayFormat = mysql_atoi(row[6]);
  rule.TargetFileDayFormat = mysql_atoi(row[7]);
  rule.TargetPath          = mysql_str(row[8]);
  rule.TargetCreateOkFile  = mysql_atoi(row[9]);
  rule.TargetPort          = mysql_atoi(row[10]);
  rule.FileCount           = mysql_atoi(row[11]);
  rule.TargetPyPath        = mysql_str(row[12]);
  rule.TargetScriptPath    = mysql_str(row[13]);

  return 0;
}


ostream& operator << (ostream& out, COPYFILE_FILE_MATCH & fm) {
  out
    << FIELD_VALUE(fm.id) << comma
    << FIELD_VALUE(fm.minion) << comma
    << FIELD_VALUE(fm.python) << comma
    << FIELD_VALUE(fm.matchpy) ;
  return out;
}

std::istream& operator >> (std::istream& in, COPYFILE_RULE& rule) {
  in
    >> rule.exeid
    >> rule.MatchID
    >> rule.ScriptID
    >> rule.Rules
    >> rule.RunOrder
    >> rule.TargetPC
    >> rule.TargetPathDayFormat
    >> rule.TargetFileDayFormat
    >> rule.TargetPath
    >> rule.TargetCreateOkFile
    >> rule.TargetPort
    >> rule.FileCount
    >> rule.TargetPyPath
    >> rule.TargetScriptPath;

  return in;
}

std::ostream& operator<<(std::ostream& out, COPYFILE_RULE& rule) {
  out   
    << FIELD_VALUE(rule.exeid           ) << comma
    << prefix(rule.MatchID             ) << rule.MatchID             << postfix(rule.MatchID             ) << comma
    << prefix(rule.ScriptID            ) << rule.ScriptID            << postfix(rule.ScriptID            ) << comma
    << prefix(rule.Rules               ) << rule.Rules               << postfix(rule.Rules               ) << comma
    << prefix(rule.RunOrder            ) << rule.RunOrder            << postfix(rule.RunOrder            ) << comma
    << prefix(rule.TargetPC            ) << rule.TargetPC            << postfix(rule.TargetPC            ) << comma
    << prefix(rule.TargetPathDayFormat ) << rule.TargetPathDayFormat << postfix(rule.TargetPathDayFormat ) << comma
    << prefix(rule.TargetFileDayFormat ) << rule.TargetFileDayFormat << postfix(rule.TargetFileDayFormat ) << comma
    << prefix(rule.TargetPath          ) << rule.TargetPath          << postfix(rule.TargetPath          ) << comma
    << prefix(rule.TargetCreateOkFile  ) << rule.TargetCreateOkFile  << postfix(rule.TargetCreateOkFile  ) << comma
    << prefix(rule.TargetPort          ) << rule.TargetPort          << postfix(rule.TargetPort          ) << comma
    << prefix(rule.FileCount           ) << rule.FileCount           << postfix(rule.FileCount           ) << comma
    << prefix(rule.TargetPyPath        ) << rule.TargetPyPath        << postfix(rule.TargetPyPath        ) << comma
    << prefix(rule.TargetScriptPath    ) << rule.TargetScriptPath    << postfix(rule.TargetScriptPath    )
    ;
  return out;
}


int get_scripts(void* s, MYSQL_ROW& row) {
    COPYFILE_SCRIPT& script = *(COPYFILE_SCRIPT*)s;
    script.exeid             = mysql_atoi(row[0]);
    script.MatchID           = mysql_str(row[1]);
    script.MinionID          = mysql_str(row[2]);
    script.Name              = mysql_str(row[3]);
    script.Describe          = mysql_str(row[4]);
    script.SourcePath        = mysql_str(row[5]);
    script.CopyRules         = mysql_atoi(row[6]);
    script.IsOK              = mysql_atoi(row[7]);
    script.IsTime            = mysql_atoi(row[8]);
    script.filepathDayFormat = mysql_atoi(row[9]);
    script.IsRunOnly         = mysql_atoi(row[10]);
    script.RunOnlyTime       = mysql_atoi(row[11]);
    script.RunCycle          = mysql_atoi(row[12]);
    script.RunCycle1         = mysql_atoi(row[13]);

    return 0;
}


std::ostream& operator<<(std::ostream& out, COPYFILE_SCRIPT& script) {
  out
    << prefix(script.exeid            ) << script.exeid             << postfix(script.exeid            ) << comma
    << prefix(script.MatchID          ) << script.MatchID           << postfix(script.MatchID          ) << comma
    << prefix(script.MinionID         ) << script.MinionID          << postfix(script.MinionID         ) << comma
    << prefix(script.Name             ) << script.Name              << postfix(script.Name             ) << comma
    << prefix(script.Describe         ) << script.Describe          << postfix(script.Describe         ) << comma
    << prefix(script.SourcePath       ) << script.SourcePath        << postfix(script.SourcePath       ) << comma
    << prefix(script.CopyRules        ) << script.CopyRules         << postfix(script.CopyRules        ) << comma
    << prefix(script.IsOK             ) << script.IsOK              << postfix(script.IsOK             ) << comma
    << prefix(script.IsTime           ) << script.IsTime            << postfix(script.IsTime           ) << comma
    << prefix(script.filepathDayFormat) << script.filepathDayFormat << postfix(script.filepathDayFormat) << comma
    << prefix(script.IsRunOnly        ) << script.IsRunOnly         << postfix(script.IsRunOnly        ) << comma
    << prefix(script.RunOnlyTime      ) << script.RunOnlyTime       << postfix(script.RunOnlyTime      ) << comma
    << prefix(script.RunCycle         ) << script.RunCycle          << postfix(script.RunCycle         ) << comma
    << prefix(script.RunCycle1        ) << script.RunCycle1         << postfix(script.RunCycle1        )
    ;
  return out;
}



template <typename T>
string prefix(T& t) {
  if (is_string(t)) return "\'";
  else if (is_timet(t)) return "from_unixtime(";
  else return "";
}

template <typename T>
string postfix(T& t) {
  if (is_string(t)) return "\'";
  else if (is_timet(t)) return ")";
  else return "";
}



ostream& operator << (ostream& out, COPYFILE_EXERECORD& er) {
  out
     << FIELD_VALUE(er.exeid         ) << comma
     << FIELD_VALUE(er.MatchID       ) << comma
     << FIELD_VALUE(er.SourcePathFile) << comma
     << FIELD_VALUE(er.SourcePC      ) << comma
     << FIELD_VALUE(er.TargetPC      ) << comma
     << FIELD_VALUE(er.TargetPort    ) << comma
     << FIELD_VALUE(er.TargetPathFile) << comma
     << FIELD_VALUE(er.State         ) << comma
     << "now()" << comma
     << FIELD_VALUE(er.Note          ) << comma
     << FIELD_VALUE(er.RunCommand    ) << comma
     << FIELD_VALUE(er.ResultState   ) << comma
     << FIELD_VALUE(er.ResultInfo    )
  ;

  return out;
}

int get_exec_record(void* s, MYSQL_ROW& row) {
    COPYFILE_EXERECORD & er = *(COPYFILE_EXERECORD*)s;
    er.exeid          = mysql_atoi(row[0]);
    er.MatchID        = mysql_str (row[1]);
    er.SourcePathFile = mysql_str (row[2]);
    er.SourcePC       = mysql_str (row[3]);
    er.TargetPC       = mysql_str (row[4]);
    er.TargetPort     = mysql_atoi(row[5]);
    er.TargetPathFile = mysql_atoi(row[6]);
    er.State          = mysql_atoi(row[7]);
    er.BegTime        = mysql_atoi(row[8]);
    er.EndTime        = mysql_atoi(row[9]);
    er.Note           = mysql_str (row[10]);
    er.RunCommand     = mysql_str (row[11]);
    er.ResultState    = mysql_str (row[12]);
    er.ResultInfo     = mysql_str (row[13]);

    return 0;
}


ostream& operator << (ostream& out, COPYFILE_DEFCALENDAR& caledar) {
  out
    << prefix(caledar.sName   ) << caledar.sName    << postfix(caledar.sName   ) << comma
    << prefix(caledar.iYear   ) << caledar.iYear    << postfix(caledar.iYear   ) << comma
    << prefix(caledar.iMonth  ) << caledar.iMonth   << postfix(caledar.iMonth  ) << comma
    << prefix(caledar.iDay    ) << caledar.iDay     << postfix(caledar.iDay    ) << comma
    << prefix(caledar.InitFlag) << caledar.InitFlag << postfix(caledar.InitFlag) << comma
    << prefix(caledar.TimeFlag) << caledar.TimeFlag << postfix(caledar.TimeFlag);
  return out;
}


int get_caledar(void* s, MYSQL_ROW& row) {
    COPYFILE_DEFCALENDAR& calendar = *(COPYFILE_DEFCALENDAR*)s;
    calendar.sName    = mysql_str (row[0]);
    calendar.iYear    = mysql_atoi(row[1]);
    calendar.iMonth   = mysql_atoi(row[2]);
    calendar.iDay     = mysql_atoi(row[3]);
    calendar.InitFlag = mysql_atoi(row[4]);
    calendar.TimeFlag = mysql_str (row[5]);

    return 0;
}

int get_match_file(void* s, MYSQL_ROW& row) {
  COPYFILE_FILE_MATCH& fm = *(COPYFILE_FILE_MATCH*)s;
  fm.id      = mysql_atoi(row[0]);
  fm.minion  = mysql_str (row[1]);
  fm.python  = mysql_str (row[2]);
  fm.matchpy = mysql_str (row[3]);

  return 0;
}

int get_next_record_id(void*s, MYSQL_ROW& row) {
  COPYFILE_AUTOID& aid = *(COPYFILE_AUTOID*)s;
  aid.nextid = mysql_atoi(row[0]);

  return 0;
}
