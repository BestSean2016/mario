#include "saltapi.hpp"
#include "rapidjson/document.h"
#include "mario.hpp"

namespace  itat {

//JOBMAP g_jobmap;
char g_token[TOKEN_LEN] = {0};

#define TEMPBUF_LEN 2048
static itat::SALT_CALLBACK salt_cb;

std::mutex g_job_mutex;

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
    // ------ SALT_API_TYPE_TESTPING -------------
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

int parse_token_fn(const char *data, size_t len, void* param1, void* param2) {
  // fprintf(stdout, "%s", (char *)ptr);
  //"token": "897b0cc93d59f10aaa46159e7dfba417d225b2cd"
  UNUSE(len);
  UNUSE(param1);
  UNUSE(param2);

  char *pos = strstr((char *)data, "\"token\": \"");
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

void set_default_callback() {
  salt_cb.parase_my_job_cb = 0;
  salt_cb.parse_job_cb = 0;
  salt_cb.parse_job_ret_cb = parse_salt_job_ret;
  salt_cb.parse_token_cb = parse_token_fn;
  salt_cb.process_event_cb = 0;
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

int salt_api_async_cmd_runall(HTTP_API_PARAM *param, const char *target,
                        const char *script) {
  SET_CONTENT(SALT_API_TYPE_ASYNC_RUNALL);

  snprintf(tmp_buf, BUFSIZE / 2, content, target, script);
  snprintf(cmd, BUFSIZE / 2, salt_api_str[SALT_API_TYPE_ASYNC_RUNALL],
           param->hostname, param->port, g_token, strlen(tmp_buf), target, script);
#ifdef _DEBUG_
  std::cout << std::endl << cmd << std::endl;
#endif //_DEBUG_
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
  std::cout << std::endl << cmd << std::endl;
  param->rf = salt_cb.parse_job_ret_cb;

  int ret = itat_httpc(param, buffer, cmd);
  if (ret)
    std::cerr << "Wo caO!!!!\n";

  return ret;
}

int salt_api_cp_getfile(HTTP_API_PARAM *param, const char* target, const char* src_file, const char* des_file) {
    SET_CONTENT(SALT_API_TYPE_CP_GETFILE);
    snprintf(tmp_buf, BUFSIZE / 2, content, target, src_file, des_file);
    snprintf(cmd, BUFSIZE / 2, salt_api_str[SALT_API_TYPE_ASYNC_RUNALL],
             param->hostname, param->port, g_token, strlen(tmp_buf), target, src_file, des_file);
    return itat_httpc(param, buffer, cmd);
}

int salt_api_events(HTTP_API_PARAM* param) {
  char buf[BUFSIZE * 2];
  char cmd[1024];

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
      job->jid = data["jid"].GetString();
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
  dst_job->retnum = src_job->retnum;
  dst_job->success_num = src_job->success_num;
  dst_job->stamp_sec = src_job->stamp_sec;
  dst_job->status = src_job->status;
  dst_job->timerout = src_job->timerout;
}


/*
{"return": [{"jid": "20161128184515112266", "minions": ["old08002759F4B6"]}]}
*/
static int parse_salt_myjob_jobmap(const char *json_data, size_t len,
                                   PARAM p1, PARAM p2) {
  MAP_SALT_JOB* jobs = reinterpret_cast<MAP_SALT_JOB*>(p1);
  HTRD_JOB* htrdjob = reinterpret_cast<HTRD_JOB*>(p2);

  assert(jobs != 0);
  assert(htrdjob != 0);

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

  if (!found) {
    htrdjob->status = JOB_STATUS_TYPE_RUNNING;
    // insert new job
    jobs->insert(std::make_pair(job->jid, job));
    // std::cout << "=.= JOBS insert job " << job->ple_id << ", " <<
    // job->node_id
    //           << ", " << job->jid << " => " << job << std::endl;
    // std::cout << "MINIONS insert job " << ((SALT_JOB *)job)->jid <<
    // std::endl;
    for (auto &p : job->minions) {
      job->minion_ret.insert(std::make_pair(p, nullptr));
      // std::cout << p << " => "
      //           << "nullptr" << std::endl;
    }
  } else {
    htrdjob->status = JOB_STATUS_TYPE_RUNNING;
    // update my new job
    if ((iter->second)->ple_id == 0) {
      // std::cout << "=.= Update Job " << job->ple_id << ", " << job->node_id
      //           << ", " << job->jid << std::endl;
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


static int parse_salt_job_ret(SALT_JOB_RET *job, char* minion, rapidjson::Document &doc) {
    if (doc.HasMember("return")) {
        if (doc["return"].IsArray()) {
        rapidjson::Value& ret = doc["return"][0];
        if (ret.HasMember(minion)) {
          rapidjson::Value& m = ret[minion];
          if (m.HasMember("retcode"))
            job->retcode = m["retcode"].GetInt();
          else
            return -17;

          if (m.HasMember("stdout"))
            job->stdout = m["stdout"].GetString();
          else
            return -18;

          if (m.HasMember("stderr"))
            job->stderr = m["stderr"].GetString();
          else
            return -18;

        } else
          return -16;
        } else return -14;
    } else
      return -15;

  return 0;
}


//
//{"return": [{"old080027789636": {"pid": 3036, "retcode": 0, "stderr": "", "stdout": ""}}]}
//
int parse_salt_job_ret(const char *json_data, size_t len, void* param1, void* param2) {
  char p = *(((char*)json_data) + len);
  *(((char*)json_data) + len) = 0;
  rapidjson::StringStream ss(json_data);
#ifdef _DEBUG_
  std::cout << json_data << std::endl;
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
  return parse_salt_job_ret(job, (char*)param2, doc);
}



int parse_salt_job(const char *json, size_t len, void* param1, void* param2) {
  return 0;
}

}//namespace itat


std::ostream& operator<< (std::ostream& out, itat::SALT_JOB_RET& ret) {
    out << "ple_id     " << ret.ple_id     << ','            ///PIPELINE EXECUTIVE ID
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
