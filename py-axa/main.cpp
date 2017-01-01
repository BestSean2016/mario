#include <iostream>
#include <thread>
#include "config.h"
#include <functional>
#include <sstream>
#include <string>

using namespace std;
#include "httpapi.hpp"
#include "pyaxa-mysql.hpp"

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;

  DBHANDLE db = connect_db("10.10.250.250", 13579, "copyfile", "root", "hongt@8a51");

  std::vector<COPYFILE_RULE> rules;
  std::vector<COPYFILE_SCRIPT> scripts;
  std::vector<COPYFILE_EXERECORD> ers;
  std::vector<COPYFILE_DEFCALENDAR> cs;

  query_data(rules, db, pyaxa_query_sql[0], get_rules);
  query_data(scripts, db, pyaxa_query_sql[1], get_scripts);
  query_data(ers, db, pyaxa_query_sql[2], get_exec_record);
  query_data(cs, db, pyaxa_query_sql[3], get_caledar);

  cout << rules;
  cout << scripts;
  cout << ers;
  cout << cs;

  disconnect_db(db);

  ostringstream oss;
  oss << pyaxa_write_sql[1] << scripts[0] << ");";
  std::cout << oss.str() << std::endl;

  oss.str("");
  oss << pyaxa_write_sql[0] << rules[0] << ");";
  std::cout << oss.str() << std::endl;

  oss.str("");
  oss << pyaxa_write_sql[2] << ers[0] << ");";
  std::cout << oss.str() << std::endl;

  return 0;
}
