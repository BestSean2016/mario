#include "saltapi.hpp"
#include "rapidjson/document.h"
#include "mario_sql.h"
#include "node.hpp"

namespace  itat {

//JOBMAP g_jobmap;
char g_token[TOKEN_LEN] = {0};

#define TEMPBUF_LEN 2048

std::mutex g_job_mutex;

bool g_run_check_timer_out = false;

static int parse_salt_job_ret_async(SALT_JOB_RET *job, rapidjson::Document &doc);
static std::string salt_get_string(rapidjson::Value& v) {
    if (v.IsString()) {
        return v.GetString();
    } else if (v.IsArray()) {
        rapidjson::Value& va = v[0];
        if (va.IsString())
            return va.GetString();
    }
    return "";
}

/**
 * @brief itat_httpc send cmd and receive response from salt api
 * @param hostname in, hostname
 * @param portno   in, port number
 * @param buf      in, buffer for receive
 * @param cmd      in, the command to run
 * @param parse_fun in, the parse response function
 * @param salt_job  in, the pointer to save salt_job
 * @param job       in, the prepared job
 * @return zero for good, otherwise for bad
 */
static const char *salt_api_str[] = {
    // ------- SALT_API_TYPE_LOGIN ------
    "POST /login HTTP/1.1\r\n"
    "Host: %s:%d\r\n"
    "Accept: application/json\r\n"
    "Content-Length: %d\r\n"
    "Content-type: application/json\r\n"
    "\r\n"
    "{\"username\":\"%s\",\"password\":\"%s\",\"eauth\":\"pam\"}",
    // ------ SALT_API_TYPE_ASYNC_TESTPING -------------
    "POST / HTTP/1.1\r\n"
    "Host: %s:%d\r\n"
    "Accept: application/json\r\n"
    "X-Auth-Token: %s\r\n"
    "Content-Length: %d\r\n"
    "Content-type: application/json\r\n"
    "\r\n"
    "{\"client\":\"local_async\",\"fun\":\"test.ping\",\"tgt\":\"%s\"}",
    // -------------------- SALT_API_TYPE_ASYNC_RUNALL -----------------
    "POST / HTTP/1.1\r\n"
    "Host: %s:%d\r\n"
    "Accept: application/json\r\n"
    "X-Auth-Token: %s\r\n"
    "Content-Length: %d\r\n"
    "Content-type: application/json\r\n"
    "\r\n"
    "{\"client\":\"local_async\",\"fun\":\"cmd.run_all\",\"tgt\":\"%s\",\"arg\":[\"%s\"]}",
    // ------------------ SALT_API_TYPE_RUNALL -----------------
    "POST / HTTP/1.1\r\n"
    "Host: %s:%d\r\n"
    "Accept: application/json\r\n"
    "X-Auth-Token: %s\r\n"
    "Content-Length: %d\r\n"
    "Content-type: application/json\r\n"
    "\r\n"
    "{\"client\":\"local\",\"fun\":\"cmd.run_all\",\"tgt\":\"%s\",\"arg\":\"%s\"}",
    // ------------------ SALT_API_TYPE_EVENTS -----------------
    "GET /events HTTP/1.1\r\n"
    "Host: 10.10.10.19:8000\r\n"
    "Accept: application/json\r\n"
    "X-Auth-Token: %s\r\n"
    "\r\n",
    // ------------------ SALT_API_TYPE_CP_GETFILE --------------
    "POST / HTTP/1.1\r\n"
    "Host: %s:%d\r\n"
    "Accept: application/json\r\n"
    "X-Auth-Token: %s\r\n"
    "Content-Length: %d\r\n"
    "Content-type: application/json\r\n"
    "\r\n"
    "[{\"client\":\"local\", \"tgt\":\"%s\", \"fun\":\"cp.get_file\", \"arg\":[\"%s\", \"%s\"]}]"

    "client=local_async&fun=cmd.run_all&tgt=%s&arg=c:"
    "\\hongt\\Client\\ExecClient.exe abcd",
    // ------ SALT_API_TYPE_TESTPING -------------
    "POST / HTTP/1.1\r\n"
    "Host: %s:%d\r\n"
    "Accept: application/json\r\n"
    "X-Auth-Token: %s\r\n"
    "Content-Length: %d\r\n"
    "Content-type: application/json\r\n"
    "\r\n"
    "{\"client\":\"local\",\"fun\":\"test.ping\",\"tgt\":\"%s\"}",

    // ------ SALT_API_TYPE_FILE_EXISTS -------------
    "POST / HTTP/1.1\r\n"
    "Host: %s:%d\r\n"
    "Accept: application/json\r\n"
    "X-Auth-Token: %s\r\n"
    "Content-Length: %d\r\n"
    "Content-type: application/json\r\n"
    "\r\n"
    "{\"client\":\"local\",\"fun\":\"file.file_exists\",\"tgt\":\"%s\",\"arg\":\"%s\"}",

    "{\"client\":\"local_async\",\"tgt\":\"old080027856836\",\"fun\":\"cmd.run_all\",\"arg\":\"c:\\python27\\python C:\\FileMatch\\fmServer.py  30001\"}",

};
//client=local_async&fun=cmd.run_all&tgt=old080027C8BFA4&arg=c:\hongt\Client\ExecClient.exe abcd
//
// static char *get_line(const char *buf) {
//   char *ptr = (char *)buf;
//
//   if (!buf || !(*buf))
//     return 0;
//
//   while (ptr && *ptr && *ptr != '\r' && *ptr != '\n')
//     ++ptr;
//
//   if (*ptr == '\r') {
//     *ptr = 0;
//     ptr += 2;
//   }
//   return ptr;
// }
//
/*
HTTP/1.1 nnn OK
Content-Length: nnn
......

<data>
*/




static int run_something__(std::vector<SALT_JOB*>& vec) {
  //run
  for(auto& p : vec) {
      auto job = (SALT_JOB*)p;
      assert(job != nullptr);
      assert(job->inode != nullptr);
      //printf("run_something__%d\n", job->status);
      switch (job->status) {
      case ST_succeed:
          ((iNode*)(job->inode))->on_run_ok(nullptr);
          break;
      case ST_error:
          ((iNode*)(job->inode))->on_run_error(nullptr);
          break;
      case ST_timeout:
          ((iNode*)(job->inode))->on_run_timeout(nullptr);
          break;
      default:
          break;
      }
  }
  vec.clear();

  return 0;
}



void thread_check_timer_out(MAP_SALT_JOB* jobmap) {
  std::vector<SALT_JOB*> should_run;
  while (g_run_check_timer_out) {
    should_run.clear();
    std::this_thread::sleep_for(std::chrono::seconds(3));
    //printf("i want to  get a lock ... ");
    std::lock_guard<std::mutex> *guard =
        new std::lock_guard<std::mutex>(g_job_mutex);
    //printf("i got it ... ");
    time_t now = time(0);
    for (auto iter = jobmap->begin();
         iter != jobmap->end(); ++iter) {
      SALT_JOB* job = iter->second;
      assert(job != nullptr);

      if (job->node_id >= 0 && job->status != ST_succeed
              && job->status != ST_error && job->status != ST_waiting_for_confirm
              && job->status != ST_timeout) {
        if (now - job->stamp_sec >= job->timerout) {
          if (job->timerout) {
            job->status = ST_timeout;
            //std::cout << "   |--> timerout job " << job->node_id
            //          << std::endl;
            //output_vector(job->minions);

            // change the status
            // g_nodes[job->node_id].status = job->status;
            // SEAN SEAN SEAN
            job->timerout = 0;
            should_run.emplace_back(job);
          }
        }
      } else {
        //it is not my job for now
      }
    }
    delete guard;
    // printf("TimerOut Checked.\n");
    run_something__(should_run);
  }

#ifdef _DEBUG_
  cout << "exit check timeout therad\n";
#endif //_DEBUG_
}


int parse_token_fn(const char *data, size_t len, void* param1, void* param2) {
  // fprintf(stdout, "%s", (char *)ptr);
  //"token": "897b0cc93d59f10aaa46159e7dfba417d225b2cd"
  UNUSE(len);
  UNUSE(param1);
  UNUSE(param2);

  char *pos = strstr((char*)data, "\"token\": \"");
  if (pos) {
    pos += strlen("\"token\": \"");
    char *end = strchr(pos, '\"');
    if (!end)
      return 0;
    if (end - pos > TOKEN_LEN - 1) return -2;
    memset(g_token, 0, TOKEN_LEN);
    strncpy(g_token, pos, end - pos);
#ifdef _DEBUG_
    std::cout << g_token << std::endl;
#endif //_DEBUG_
  } else {
#ifdef _DEBUG_
    itat::show_cstring(data, len);
#endif //_DEBUG_
    return -1;
  }
  return 0;
}



static char* get_contnt(const char* str) {
  char* content = strstr((char*)str, "\r\n\r\n");
  if (!content) return nullptr;
  else return content + 4;
}

#define SET_CONTENT(api_str_index) \
  char buffer[BUFSIZE * 2];\
  char* cmd = buffer + BUFSIZE;\
  char* tmp_buf = cmd + BUFSIZE / 2;\
  \
  char* content = get_contnt(salt_api_str[(api_str_index)]);\
  assert(content != nullptr);\



int salt_api_login(HTTP_API_PARAM *param, const char* user, const char* pass) {
  SET_CONTENT(SALT_API_TYPE_LOGIN);
  snprintf(tmp_buf, BUFSIZE / 2, content, user, pass);
  snprintf(cmd, BUFSIZE / 2, salt_api_str[SALT_API_TYPE_LOGIN], param->hostname, param->port, strlen(tmp_buf), user, pass);
  // show_cstring(buf_login, strlen(buf_login));
  return itat_httpc(param, buffer, cmd);
}


int salt_api_testping(HTTP_API_PARAM *param, const char* target) {
  SET_CONTENT(SALT_API_TYPE_TESTPING);

  snprintf(tmp_buf, BUFSIZE / 2, content, target);
  snprintf(cmd, BUFSIZE / 2, salt_api_str[SALT_API_TYPE_TESTPING],
           param->hostname, param->port, g_token, strlen(tmp_buf), target);
  // show_cstring(buf_test_ping, strlen(buf_test_ping));
  return itat_httpc(param, buffer, cmd);
}


// static int parse_cmd_return(const char *ptr, size_t len, void *obj) {
//   (void)len;
//   if (!obj && ptr) {
//     *((char *)ptr + len) = 0;
//     printf("%s<--|\n", ptr);
//     return 0;
//   }
//
//   return 0;
// }

/**
 * @brief salt_api_async_cmd_runall
 * @param param, rf is parse_salt_myjob_jobmap,
 *               param1 is map of jobs;
 *               param2 is iNode pointer
 * @param target
 * @param script
 * @return
 */
int salt_api_async_cmd_runall(HTTP_API_PARAM *param, const char *target,
                        const char *script) {
  SET_CONTENT(SALT_API_TYPE_ASYNC_RUNALL);

  snprintf(tmp_buf, BUFSIZE / 2, content, target, script);
  snprintf(cmd, BUFSIZE / 2, salt_api_str[SALT_API_TYPE_ASYNC_RUNALL],
           param->hostname, param->port, g_token, strlen(tmp_buf), target, script);
#ifdef _DEBUG_
  std::cout << std::endl << cmd << std::endl;
#endif //_DEBUG_
  param->rf = parse_salt_myjob_jobmap;
  assert(param->param1 != nullptr);
  assert(param->param2 != nullptr);

  int ret = itat_httpc(param, buffer, cmd);
  if (ret)
    std::cerr << "Wo caO!!!!\n";

  return ret;
}

int salt_api_cmd_runall(HTTP_API_PARAM *param, const char *target, const char *script) {
  SET_CONTENT(SALT_API_TYPE_RUNALL);

  snprintf(tmp_buf, BUFSIZE / 2, content, target, script);
  snprintf(cmd, BUFSIZE / 2, salt_api_str[SALT_API_TYPE_RUNALL],
           param->hostname, param->port, g_token, strlen(tmp_buf), target, script);
#ifdef _DEBUG_
  std::cout << std::endl << cmd << std::endl;
#endif //_DEBUG_
  param->rf = parse_salt_runall_ret; //salt_cb.parse_job_ret_cb;

  int ret = itat_httpc(param, buffer, cmd);
  if (ret)
    std::cerr << "Wo caO!!!! " << ret << std::endl;

  return ret;
}

int salt_api_cp_getfile(HTTP_API_PARAM *param, const char* target, const char* src_file, const char* des_file) {
    SET_CONTENT(SALT_API_TYPE_CP_GETFILE);
    snprintf(tmp_buf, BUFSIZE / 2, content, target, src_file, des_file);
    snprintf(cmd, BUFSIZE / 2, salt_api_str[SALT_API_TYPE_ASYNC_RUNALL],
             param->hostname, param->port, g_token, strlen(tmp_buf), target, src_file, des_file);
    return itat_httpc(param, buffer, cmd);
}


int salt_api_file_exists(HTTP_API_PARAM* param, const char* target, const char* script) {
    SET_CONTENT(SALT_API_TYPE_FILE_EXISTS);

    snprintf(tmp_buf, BUFSIZE / 2, content, target, script);
    snprintf(cmd, BUFSIZE / 2, salt_api_str[SALT_API_TYPE_FILE_EXISTS],
             param->hostname, param->port, g_token, strlen(tmp_buf), target, script);
// #ifdef _DEBUG_
    std::cout << std::endl << cmd << std::endl;
// #endif //_DEBUG_

    int ret = itat_httpc(param, buffer, cmd);
    if (ret)
      std::cerr << "Wo caO!!!! " << ret << std::endl;

    return ret;
}

int salt_api_events(HTTP_API_PARAM* param) {
  char buf[BUFSIZE * 2];
  char cmd[1024];

  memset(buf, 0, BUFSIZE * 2);
  memset(cmd, 0, 1024);

  param->rf = parse_job;
  snprintf(cmd, 1024, salt_api_str[SALT_API_TYPE_EVENTS], g_token);

  return itat_httpc(param, buf, cmd);
}



static int parse_string_array(std::vector<std::string> &vec,
                              rapidjson::Value &array) {
  if (array.IsArray()) {
    for (rapidjson::SizeType i = 0; i < array.Size(); ++i)
      vec.push_back(array[i].GetString());
    return 0;
  }
  return -1;
}

/*
{"return": [{"jid": "20161128184515112266", "minions": ["old08002759F4B6"]}]}
*/
static int parse_salt_my_job(SALT_JOB *job, rapidjson::Document &doc) {
  if (doc.HasMember("return") && doc["return"].IsArray()) {
    rapidjson::Value &array = doc["return"];
    if (array.Size() < 1)
      return -2;
    rapidjson::Value &data = array[0];
    if (data.HasMember("jid"))
      job->jid = salt_get_string(data["jid"]);
    else
      return -5;

    if (data.HasMember("minions")) {
      if (parse_string_array(job->minions, data["minions"]))
        return -12;
    } else
      return -3;
    return 0;
  }
  return -1;
}



static void copy_job_status(SALT_JOB *dst_job, SALT_JOB *src_job) {
  dst_job->ple_id = src_job->ple_id;
  dst_job->node_id = src_job->node_id;
  dst_job->inode = src_job->inode;
  dst_job->retnum = src_job->retnum;
  dst_job->success_num = src_job->success_num;
  dst_job->stamp_sec = src_job->stamp_sec;
  dst_job->status = src_job->status;
  dst_job->timerout = src_job->timerout;
}



/*
data: {"tag": "salt/job/20161123065414567343/new", "data": {"tgt_type":
"glob", "jid": "20161123065414567343", "tgt": "*", "_stamp":
"2016-11-22T22:54:14.569305", "user": "root", "arg": [], "fun": "test.ping",
"minions": ["minion1", "minion2", "minion3", "minion4", "minion5", "minion6",
"minion7", "minion8", "new080027006B3F", "new08002700A6BA", ..., ]}}
*/

static int parse_salt_job_new(SALT_JOB *job, rapidjson::Document &doc) {
  if (doc.HasMember("tag"))
    job->tag = salt_get_string(doc["tag"]);
  else
    return -2;

  if (doc.HasMember("data")) {
    // parse_new_job_data(doc["data"]);
    rapidjson::Value &data = doc["data"];
    if (data.HasMember("tgt_type"))
      job->tgt_type = salt_get_string(data["tgt_type"]);
    else
      return -4;

    if (data.HasMember("jid"))
      job->jid = salt_get_string(data["jid"]);
    else
      return -5;

    if (data.HasMember("tgt")) {
      job->tgt = salt_get_string(data["tgt"]);
    } else
      return -6;

    if (data.HasMember("_stamp"))
      job->stamp = salt_get_string(data["_stamp"]);
    else
      return -7;

    if (data.HasMember("user"))
      job->user = salt_get_string(data["user"]);
    else
      return -8;

    if (data.HasMember("arg")) {
      if (parse_string_array(job->arg, data["arg"]))
        return -11;
    } else
      return -9;

    if (data.HasMember("fun"))
      job->fun = salt_get_string(data["fun"]);
    else
      return -10;

    if (data.HasMember("minions")) {
      if (parse_string_array(job->minions, data["minions"]))
        return -12;
    } else
      return -3;
  }
  return 0;
}

int parse_salt_job_new(SALT_JOB *job, const char *json_data) {
  rapidjson::StringStream ss(json_data);
  rapidjson::Document doc;
  doc.ParseStream(ss);

  if (doc.HasParseError()) {
    std::cout << "Error at " << doc.GetErrorOffset() << std::endl
              << json_data + doc.GetErrorOffset() << std::endl;
    return -1;
  }

  return parse_salt_job_new(job, doc);
}


static void* _parse_with_type_(rapidjson::Document &doc,
                                      SALT_JOB_EVENT_TYPE *type) {
  void* job = nullptr;
  if (doc.HasMember("tag")) {
    const char *tag = doc["tag"].GetString();
    if (strstr(tag, "/new")) {
      *type = SALT_JOB_TYPE_NEW;
      SALT_JOB *jobnew = new SALT_JOB();
      if (parse_salt_job_new(jobnew, doc)) {
        delete jobnew;
        return 0;
      }
      job = jobnew;
    } else if (strstr(tag, "/ret")) {
      *type = SALT_JOB_TYPE_RET;
      auto jobret = new SALT_JOB_RET;
      jobret->stamp_sec = 0;
      jobret->stamp_usec = 0;
      if (parse_salt_job_ret_async(jobret, doc)) {
        delete jobret;
        return 0;
      }
      job = jobret;
    } else {
      *type = SALT_JOB_TYPE_IGNORE;
    }
  } else
    return nullptr;

  return job;
}



/*
{"return": [{"jid": "20161128184515112266", "minions": ["old08002759F4B6"]}]}
*/
int parse_salt_myjob_jobmap(const char *json_data, size_t len,
                                   PARAM p1, PARAM p2) {
#ifdef _DEBUG_
  // std::cout << "parse_salt_myjob_jobmap: \n";
  // show_cstring(json_data, len);
#endif //_DEBUG_

  assert(p1 != nullptr);
  assert(p2 != nullptr);

  MAP_SALT_JOB* jobs = reinterpret_cast<MAP_SALT_JOB*>(p1);
  iNode* inode = reinterpret_cast<iNode*>(p2);

  rapidjson::Document doc;
  doc.Parse((char *)json_data, len);

  if (doc.HasParseError()) {
    std::cout << "doc has error\n";
    show_cstring(json_data, len);
    return -2;
  }

  SALT_JOB* job = new SALT_JOB();

  if (parse_salt_my_job(job, doc) < 0) {
    std::cout << "parse_salt_new_job error\n";
    show_cstring(json_data, len);
    delete job;
    return -3;
  }

  // show_cstring(json_data, len);
  std::lock_guard<std::mutex> *guard =
      new std::lock_guard<std::mutex>(g_job_mutex);

  auto iter = jobs->find(job->jid);
  bool found = (iter != jobs->end());

  job->status = ST_running;
  job->node_id = inode->get_nodemaps()->inodeid_2_ignodeid[inode->get_id()];
  job->inode = inode;

  if (!found) {
    // insert new job
    jobs->insert(std::make_pair(job->jid, job));
#ifdef _DEBUG_
    // std::cout << "=.= JOBS insert job " << job->ple_id << ", " <<
    // job->node_id
    //           << ", " << job->jid << " => " << job << std::endl;
    // std::cout << "MINIONS insert job " << ((SALT_JOB *)job)->jid <<
    // std::endl;
#endif //_DEBUG_
    for (auto &p : job->minions) {
      job->minion_ret.insert(std::make_pair(p, nullptr));
#ifdef _DEBUG_
      // std::cout << p << " => "
      //           << "nullptr" << std::endl;
#endif //_DEBUG_
    }
  } else {
    // update my new job
    if ((iter->second)->ple_id == 0) {
#ifdef _DEBUG_
      // std::cout << "=.= Update Job " << job->ple_id << ", " << job->node_id
      //           << ", " << job->jid << std::endl;
#endif //_DEBUG_
      copy_job_status(iter->second, job);
    }
    delete job;
  }

  delete guard;
  return 0;
}



static int parse_salt_job_ret_async(SALT_JOB_RET *job, rapidjson::Document &doc) {
  if (doc.HasMember("tag"))
    job->tag = doc["tag"].GetString();
  else
    return -2;

  if (doc.HasMember("data")) {
    // parse_new_job_data(doc["data"]);
    rapidjson::Value &data = doc["data"];

    if (data.HasMember("jid"))
      job->jid = data["jid"].GetString();
    else
      return -5;

    if (data.HasMember("_stamp"))
      job->stamp = data["_stamp"].GetString();
    else
      return -7;

    if (data.HasMember("fun"))
      job->fun = data["fun"].GetString();
    else
      return -10;

    if (data.HasMember("id"))
      job->minion_id = data["id"].GetString();
    else
      return -11;
    if (data.HasMember("cmd"))
      job->cmd = data["cmd"].GetString();
    else
      return -12;

    if (data.HasMember("success"))
      job->success = data["success"].GetBool();
    else
      return -13;

    if (data.HasMember("retcode"))
      job->retcode = data["retcode"].GetInt();
    else
      return -14;

    if (data.HasMember("return")) {
      if (data["return"].IsObject()) {
        rapidjson::Value &ret = data["return"];
        if (ret.HasMember("pid"))
          job->pid = ret["pid"].GetInt();
        // else
        //   return -16;

        if (ret.HasMember("retcode"))
          job->retcode = ret["retcode"].GetInt();
        // else
        //   return -17;

        if (ret.HasMember("stdout"))
          job->stdout = ret["stdout"].GetString();
        // else
        //   return -18;
        if (ret.HasMember("stderr"))
          job->stderr = ret["stderr"].GetString();
        // else
        //   return -18;

        job->rettype = RETURN_TYPE_OBJECT;
      } else if (data["return"].IsBool()) {
        job->pid = 0;
        job->stderr = "";
        job->stdout = "";
        job->rettype = RETURN_TYPE_BOOL;
      } else if (data["return"].IsString()) {
        job->pid = 0;
        if (job->success)
          job->stdout = data["return"].GetString(), job->stderr = "";
        else
          job->stderr = data["return"].GetString(), job->stdout = "";
        job->rettype = RETURN_TYPE_STRING;
      }
    } // else
      // return -15;
  }
  return 0;
}


static int parse_salt_runall_ret(SALT_JOB_RET *job, char* minion, rapidjson::Document &doc) {
    job->retcode = 0;
    if (doc.HasMember("return")) {
        if (doc["return"].IsArray()) {
        rapidjson::Value& ret = doc["return"][0];
        if (ret.HasMember(minion)) {
          rapidjson::Value& m = ret[minion];
          if (m.HasMember("retcode"))
            job->retcode = m["retcode"].GetInt();
          else
            job->retcode = -17;

          if (m.HasMember("stdout"))
            job->stdout = m["stdout"].GetString();
          else
            job->retcode =  -18;

          if (m.HasMember("stderr"))
            job->stderr = m["stderr"].GetString();
          else
            job->retcode =  -18;

        } else
          job->retcode =  -16;
        } else
            job->retcode =  -14;
    } else
      job->retcode =  -15;

  return job->retcode;
}

/* {"return": [{"old0800274353F3": true}]} */
int parse_salt_testping_ret(const char *json_data, size_t len, void* param1, void* param2) {
  if (!len) return -2;

  char p = *(((char*)json_data) + len);
  *(((char*)json_data) + len) = 0;
  rapidjson::StringStream ss(json_data);
#ifdef _DEBUG_
  // parse_salt_testping_ret
  // show_cstring(json_data, len);
#endif //_DEBUG_

  rapidjson::Document doc;
  doc.ParseStream(ss);

  if (doc.HasParseError()) {
    *(((char*)json_data) + len) = p;
    std::cout << "Error at " << doc.GetErrorOffset() << std::endl
              << json_data + doc.GetErrorOffset() << std::endl;
    return -1;
  }
  *(((char*)json_data) + len) = p;
  SALT_JOB_RET *job = (SALT_JOB_RET*)param1;
  const char* minion = (const char*)param2;
  // return parse_salt_runall_ret(job, (char*)param2, doc);

  job->retcode = 0;
  if (doc.HasMember("return")) {
      if (doc["return"].IsArray()) {
      rapidjson::Value& ret = doc["return"][0];
      if (ret.HasMember(minion)) {
        rapidjson::Value& m = ret[minion];
        if (m.IsBool())
            job->retcode = (m.GetBool()) ? 0 : -18;
        else
            job->retcode = -17;
      } else
        job->retcode =  -16;
    } else
      job->retcode =  -15;
  } else
      job->retcode = -14;

  return job->retcode;

}


//
//{"return": [{"old080027789636": {"pid": 3036, "retcode": 0, "stderr": "", "stdout": ""}}]}
//
int parse_salt_runall_ret(const char *json_data, size_t len, void* param1, void* param2) {
  if (!len) return -1;

  char p = *(((char*)json_data) + len);
  *(((char*)json_data) + len) = 0;
  rapidjson::StringStream ss(json_data);
#ifdef _DEBUG_
  // std::cout << "parse salt runall ret\n";
  // show_cstring(json_data, len);
#endif //_DEBUG_

  rapidjson::Document doc;
  doc.ParseStream(ss);

  if (doc.HasParseError()) {
    *(((char*)json_data) + len) = p;
    std::cout << "Error at " << doc.GetErrorOffset() << std::endl
              << json_data + doc.GetErrorOffset() << std::endl;
    return -1;
  }
  *(((char*)json_data) + len) = p;
  SALT_JOB_RET *job = (SALT_JOB_RET*)param1;
  return parse_salt_runall_ret(job, (char*)param2, doc);
}

static void free_job(SALT_JOB_EVENT_TYPE type, void* job) {
  switch (type) {
  case SALT_JOB_TYPE_NEW:
    delete (SALT_JOB *)job;
    break;
  case SALT_JOB_TYPE_RET:
    delete (SALT_JOB_RET *)job;
    break;
  default:
    break;
  }
}


static void insert_new_job_into_map_(MAP_SALT_JOB* jobmap, SALT_JOB* job) {
  jobmap->insert(std::make_pair(job->jid, job));
  // std::cout << "^_^ JOBS insert job " << job->ple_id
  //           << ", " << job->node_id << ", "
  //           << job->jid << " => " << jobptr << std::endl;
  for (auto &p : job->minions) {
    job->minion_ret.insert(std::pair<std::string, SALT_JOB_RET *>(p, nullptr));
    // std::cout << p << " => "
    //           << "nullptr" << std::endl;
  }
}

static int parse_salt_jobmap(const char *json_data, size_t len, MAP_SALT_JOB *jobmap) {
  rapidjson::Document doc;
  doc.Parse((char *)json_data, len);

  if (doc.HasParseError()) {
    std::cout << "doc has error\n";
    show_cstring(json_data, len);
    return -2;
  }

  SALT_JOB_EVENT_TYPE type = SALT_JOB_TYPE_IGNORE;
  void* jobptr = _parse_with_type_(doc, &type);
  if (!jobptr && type != SALT_JOB_TYPE_IGNORE) {
    std::cout << "_parse_with_type_ parser got error\n";
    show_cstring(json_data, len);
    return -3;
  }

  // show_cstring(json_data, len);
  std::lock_guard<std::mutex> *guard =
      new std::lock_guard<std::mutex>(g_job_mutex);
  std::vector<SALT_JOB*> should_run;
  switch (type) {
  case SALT_JOB_TYPE_NEW:
    // add job to set and map
    {
      SALT_JOB *job = (SALT_JOB *)jobptr;
      // std::cout << "waiting for new\n";
      auto jobIter = jobmap->find(job->jid);
      bool found1 = (jobIter != jobmap->end());

      if (!found1) {
        //job not found
        //but the job have already had a node id
        if (job->node_id >= 0) {
            //change the node's status
            //g_nodes[job->node_id].status = JOB_STATUS_TYPE_RUNNING;
        }

        insert_new_job_into_map_(jobmap, job);
      } else  {
        if (job->node_id >= 0) {
            //change the node's status
            //g_nodes[job->node_id].status = JOB_STATUS_TYPE_RUNNING;
        }

        // update new job
        if ((jobIter->second)->ple_id != 0) {
          SALT_JOB *prevjob = jobIter->second;
          // std::cout << "^_^ Update Job " << prevjob->ple_id << ", "
          //           << prevjob->node_id << ", "
          //           << prevjob->jid << std::endl;
          copy_job_status(job, prevjob);
          jobIter->second = job;
          delete prevjob;
        } else {
          // std::cout << "Drop duplicated job " << ((SALT_JOB_NEW *)job)->jid
          //           << std::endl;
          //Drop duplicated job
          delete job;
        }
      }
    }
    break;
  case SALT_JOB_TYPE_RET:
    // remove job from map and set
    {
      SALT_JOB_RET *jobret = (SALT_JOB_RET *)jobptr;
      // show_json_string(json_data, len);
      auto jobIter = jobmap->find(jobret->jid);
      bool found1 = (jobIter != jobmap->end());

      if (!found1) {
        std::cout << "error @ jobmap->jobs.find job_new " << jobret->jid << ", "
                  << jobret->minion_id << std::endl;
        goto error_exit;
      }

      SALT_JOB* job = jobIter->second;
      assert(job != nullptr);

      auto miniIter = job->minion_ret.find(jobret->minion_id);
      bool found3 = (miniIter != job->minion_ret.end());
      if (!found3) {
        std::cout << "error @ jobmap->job->minion_ret find job_ret " << jobret->jid
                  << ", " << jobret->minion_id << std::endl;
        goto error_exit;
      }

      // got a new return of one job
      if (miniIter->second == nullptr) {
        miniIter->second = jobret;
        ++(job->retnum);

        //successful node
        if (jobret->retcode == 0)
          ++(job->success_num);

        if (job->timerout == 0) {
          /// std::cout << job->node_id << " has already ???  .>_<.!\n";
          /// ???? SEAN SEAN SEAN, what is this ????
          delete jobret;
        } else if (job->retnum == job->minions.size()) {
          // all minion has finished
          job->timerout = 0;  //it's over, do not check time out again
          job->status =
              (job->success_num == job->minions.size())
                  ? ST_succeed
                  : ST_error;
          if (job->node_id >= 0) {
            //
            // change the status of node
            // g_nodes[job->node_id].status = job->status;
            //

            // std::cout << "job-finish return 2 .. ";
            should_run.emplace_back(job);
            //int ret = node_job_finished(jobIter->second, mset);
            // std::cout << ret << " in salt return\n";
          }
        }
        // std::cout << "update returun " << jobret->jid << ", "
        //           << jobret->minion_id << "=>" << jobptr
        //           << std::endl;
        // if (job->status == ST_succeed)
        //   std::cout << "All Successed!\n";
        // else
        //   std::cout << "But NOt All Successed! "
        //             << job->success_num << " but expect "
        //             << ((jobIter)->second)->retnum << std::endl;
      } else {
        std::cout << "drop duplicated return " << jobret->jid << ", "
                  << jobret->minion_id << "=>" << jobptr << std::endl;
        delete jobret;
      }
    }
    break;

  default:
    free_job(type, jobptr);
    break;
  }
  delete guard;
  run_something__(should_run);

  return (0);

error_exit:
  free_job(type, jobptr);
  delete guard;
  return -1;
}



/**
 * @brief parse_job parse the http response for event
 * @param json_data the json sting
 * @param size the length of json_data
 * @param param1 the struct MAP_SALT_JOB
 * @param param2 is nullptr
 * @return 0 is ok
 */
int parse_job(const char *json_data, size_t size, void *param1, void *param2) {
  // data: {\"tag\": \"salt/job/
  (void)size;
  (void)param2;
  MAP_SALT_JOB *jobmap = (MAP_SALT_JOB*)param1;
#ifdef _DEBUG_
  // std::cout << "parse_job:\n";
  // show_cstring(json_data, size);
#endif //_DEBUG_

  if (!strncmp(json_data, "data: ", 6)) {
    if (parse_salt_jobmap(json_data + 6, size - 6, jobmap) < -1)
      return -1;
  }

  return 0;
}




}//namespace itat


std::ostream& operator<< (std::ostream& out, itat::SALT_JOB_RET& ret) {
    out << "SALT JOB RET:\n"
        << "ple_id     " << ret.ple_id     << ','            ///PIPELINE EXECUTIVE ID
        << "rettype    " << ret.rettype    << ','
        << "tag        " << ret.tag        << ','
        << "stamp      " << ret.stamp      << ','
        << "stamp_sec  " << ret.stamp_sec  << ','
        << "stamp_usec " << ret.stamp_usec << ','
        << "pid        " << ret.pid        << ','
        << "retcode    " << ret.retcode    << ','
        << "stderr     " << ret.stderr     << ','
        << "stdout     " << ret.stdout     << ','
        << "success    " << ret.success    << ','
        << "cmd        " << ret.cmd        << ','
        << "jid        " << ret.jid        << ','
        << "fun        " << ret.fun        << ','
        << "minion_id  " << ret.minion_id  << endl;
    return out;
}

template<typename T>
std::ostream& operator<< (std::ostream& out, std::vector<T> v) {
    for (auto&p : v)
        out << p << ',';
    out << std::endl;
    return out;
}

std::ostream& operator<< (std::ostream& out, itat::SALT_JOB& job) {
    out << "SALT_JOB: \n"
        << "ple_id     " << job.ple_id      << ','
        << "node_id    " << job.node_id     << ','
        << "stamp_sec  " << job.stamp_sec   << ','
        << "timerout   " << job.timerout    << ','
        << "retnum     " << job.retnum      << ','
        << "success_num" << job.success_num << ','
        << "status     " << job.status      << ','
        << "tag        " << job.tag         << ','
        << "tgt_type   " << job.tgt_type    << ','
        << "jid        " << job.jid         << ','
        << "tgt        " << job.tgt         << ','
        << "stamp      " << job.stamp       << ','
        << "user       " << job.user        << ','
        << "fun        " << job.fun         << std::endl
        << "args       " << job.arg         << std::endl
        << "minions    " << job.minions     << std::endl;

    for (auto& p : job.minion_ret) {
        out << "\t" << p.first << " ---> ";
        if ((p.second == nullptr))
            out << "Nullptr";
        else
            out << *(p.second);
        out << std::endl;
    }

    return out;
}
