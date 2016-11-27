#include <iostream>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rapidjson/document.h"
#include "salt_api.h"
#include <curl/curl.h>

#include <chrono>
#include <mutex>
#include <thread>

std::mutex g_maps_mutex;
static JOBMAP gjobmap;

static size_t _json_error_ = 0;

static int parse_salt_job_ret(SALT_JOB_RET *job, rapidjson::Document &doc);
static int parse_salt_job_new(SALT_JOB_NEW *job, rapidjson::Document &doc);

static bool run = true;

//static int curl_run_cmd(int cmd_index);

static void erase_return_by_jid(std::string &jid) {
  MapJid2Minions::iterator mj2mIter = gjobmap.minions.find(jid);
  MapMinionRet *retset = (mj2mIter)->second;
  if (retset) {
    for (MapMinionRet::iterator iter = retset->begin(); iter != retset->end();
         ++iter) {
      // std::cout << "erase minion -> " << (iter)->first << std::endl;
      if (((iter)->second))
        delete ((iter)->second);
    }
    retset->clear();
    // std::cout << "erase minion set " << jid << std::endl;
    delete retset;
    gjobmap.minions.erase(mj2mIter);
  } else {
    std::cerr << "Erase Minion's Return " << jid << std::endl;
  }
}

static void erase_job(MapJid2Job::iterator iter) {
  erase_return_by_jid(((iter)->second)->jid);
  std::cout << "erase job " << ((iter)->second)->jid << std::endl;
  delete ((iter)->second);
  gjobmap.jobs.erase(iter);
}

void thread_check_timer_out() {
  while (run) {
    // simulate a long page fetch
    std::this_thread::sleep_for(std::chrono::seconds(60));

    {
      std::lock_guard<std::mutex> *guard =
          new std::lock_guard<std::mutex>(g_maps_mutex);
      time_t now = time(0);
      // for (const auto&p : gjobmap.jobs) {
      //    if (now - (p.second)->stamp_sec >= (p.second)->timerout) {
      //        SALT_JOB_NEW* jobnew = p.second;
      //        erase_return_by_jid(jobnew->jid);
      //        std::cout << "erase job " << jobnew->jid << std::endl;
      //        gjobmap.jobs.erase(jobnew->jid);
      //        delete jobnew;
      //
      //    }
      //}
      for (MapJid2Job::iterator iter = gjobmap.jobs.begin();
           iter != gjobmap.jobs.end(); ++iter) {
        if (((iter)->second) &&
            (now - ((iter)->second)->stamp_sec >= ((iter)->second)->timerout)) {
          erase_job(iter);
        }
      }
      delete guard;
    }
  }
}

void thread_run_pipeline() {
  srand(time(0));
  while(run) {
    std::this_thread::sleep_for(std::chrono::seconds(5));
    curl_run_cmd(1);
    curl_run_cmd(0);
  }
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
data: {"tag": "salt/job/20161123065414567343/new", "data": {"tgt_type":
"glob", "jid": "20161123065414567343", "tgt": "*", "_stamp":
"2016-11-22T22:54:14.569305", "user": "root", "arg": [], "fun": "test.ping",
"minions": ["minion1", "minion2", "minion3", "minion4", "minion5", "minion6",
"minion7", "minion8", "new080027006B3F", "new08002700A6BA", ..., ]}}
*/

int parse_salt_job_new(SALT_JOB_NEW *job, const char *json_data) {
  rapidjson::StringStream ss(json_data);
  rapidjson::Document doc;
  doc.ParseStream(ss);

  if (doc.HasParseError()) {
    std::cerr << "Error at " << doc.GetErrorOffset() << std::endl
              << json_data + doc.GetErrorOffset() << std::endl;
    return -1;
  }

  return parse_salt_job_new(job, doc);
}

static int parse_salt_job_new(SALT_JOB_NEW *job, rapidjson::Document &doc) {
  if (doc.HasMember("tag"))
    job->tag = doc["tag"].GetString();
  else
    return -2;

  if (doc.HasMember("data")) {
    // parse_new_job_data(doc["data"]);
    rapidjson::Value &data = doc["data"];
    if (data.HasMember("tgt_type"))
      job->tgt_type = data["tgt_type"].GetString();
    else
      return -4;

    if (data.HasMember("jid"))
      job->jid = data["jid"].GetString();
    else
      return -5;

    if (data.HasMember("tgt"))
      job->tgt = data["tgt"].GetString();
    else
      return -6;

    if (data.HasMember("_stamp"))
      job->stamp = data["_stamp"].GetString();
    else
      return -7;

    if (data.HasMember("user"))
      job->user = data["user"].GetString();
    else
      return -8;

    if (data.HasMember("arg")) {
      if (parse_string_array(job->arg, data["arg"]))
        return -11;
    } else
      return -9;

    if (data.HasMember("fun"))
      job->fun = data["fun"].GetString();
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

/*
data: {"tag": "salt/job/20161123065414567343/ret/old0800277AF5BE", "data":
{"_stamp": "2016-11-22T22:54:14.580881", "return": true, "retcode": 0,
"success": true, "cmd": "_return", "jid": "20161123065414567343", "fun":
"test.ping", "id": "old0800277AF5BE"}}

data: {"tag": "salt/job/20161123065056424864/ret/old08002759F4B6", "data":
{"_stamp": "2016-11-22T22:50:56.749570", "return": {"pid": 2976, "retcode": 0,
"stderr": "", "stdout": ""}, "retcode": 0, "success": true, "cmd": "_return",
"jid": "20161123065056424864", "fun": "cmd.run_all", "id": "old08002759F4B6"}}
*/
int parse_salt_job_ret(SALT_JOB_RET *job, const char *json_data) {
  rapidjson::StringStream ss(json_data);
  rapidjson::Document doc;
  doc.ParseStream(ss);

  if (doc.HasParseError()) {
    std::cerr << "Error at " << doc.GetErrorOffset() << std::endl
              << json_data + doc.GetErrorOffset() << std::endl;
    return -1;
  }

  return parse_salt_job_ret(job, doc);
}

static int parse_salt_job_ret(SALT_JOB_RET *job, rapidjson::Document &doc) {
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
        else
          return -16;
        if (ret.HasMember("retcode"))
          job->retcode = ret["retcode"].GetInt();
        else
          return -17;

        if (ret.HasMember("stderr"))
          job->stderr = ret["stderr"].GetString();
        else
          return -18;
        if (ret.HasMember("stdout"))
          job->stdout = ret["stdout"].GetString();
        else
          return -18;

        job->rettype = RETURN_TYPE_OBJECT;
      } else if (data["return"].IsBool()) {
        job->pid = 0;
        job->stderr = "";
        job->stdout = "";
        job->rettype = RETURN_TYPE_BOOL;
      }
    } else
      return -15;
  }
  return 0;
}

static int parse_(SALT_JOB *job, rapidjson::Document &doc) {
  if (doc.HasMember("tag")) {
    const char *tag = doc["tag"].GetString();
    if (strstr(tag, "/new")) {
      job->type = SALT_JOB_TYPE_NEW;
      job->ptr = new SALT_JOB_NEW;
      ((SALT_JOB_NEW *)job->ptr)->stamp_sec = time(0);
      ((SALT_JOB_NEW *)job->ptr)->timerout = 60;
      return parse_salt_job_new((SALT_JOB_NEW *)job->ptr, doc);
    } else if (strstr(tag, "/ret")) {
      job->type = SALT_JOB_TYPE_RET;
      job->ptr = new SALT_JOB_RET;
      ((SALT_JOB_RET *)job->ptr)->stamp_sec = 0;
      ((SALT_JOB_RET *)job->ptr)->stamp_usec = 0;
      return parse_salt_job_ret((SALT_JOB_RET *)job->ptr, doc);
    } else {
      job->type = SALT_JOB_TYPE_IGNORE;
      job->ptr = 0;
    }
  } else
    return -2;

  return 0;
}

int parse_salt_job(SALT_JOB *job, const char *json_data, size_t len) {
  rapidjson::Document doc;
  doc.Parse((char *)json_data, len);

  if (doc.HasParseError()) {
    std::cerr << "Error at " << doc.GetErrorOffset() << std::endl
              //<< json_data + doc.GetErrorOffset() << std::endl;
              << json_data << std::endl;
    ++_json_error_;
    return -1;
  }

  return parse_(job, doc);
}

int parse_salt_job(SALT_JOB *job, const char *json_data) {
  rapidjson::Document doc;
  doc.Parse((char *)json_data + 6);

  if (doc.HasParseError()) {
    std::cerr << "Error at " << doc.GetErrorOffset() << std::endl
              //<< json_data + doc.GetErrorOffset() << std::endl;
              << json_data << std::endl;
    return -1;
  }

  return parse_(job, doc);
}
static SALT_JOB_PTR _parse_with_type_(rapidjson::Document &doc,
                                      SALT_JOB_TYPE *type) {
  SALT_JOB_PTR job = 0;
  if (doc.HasMember("tag")) {
    const char *tag = doc["tag"].GetString();
    if (strstr(tag, "/new")) {
      *type = SALT_JOB_TYPE_NEW;
      SALT_JOB_NEW *jobnew = new SALT_JOB_NEW;
      jobnew->stamp_sec = time(0);
      jobnew->timerout = 60;
      jobnew->retnum = 0;
      if (parse_salt_job_new(jobnew, doc)) {
        delete jobnew;
        return 0;
      }
      job = jobnew;
    } else if (strstr(tag, "/ret")) {
      *type = SALT_JOB_TYPE_RET;
      SALT_JOB_RET *jobret = new SALT_JOB_RET;
      jobret->stamp_sec = 0;
      jobret->stamp_usec = 0;
      if (parse_salt_job_ret(jobret, doc)) {
        delete jobret;
        return 0;
      }
      job = jobret;
    } else {
      *type = SALT_JOB_TYPE_IGNORE;
    }
  } else
    return 0;

  return job;
}

// static SALT_JOB *_parse__(rapidjson::Document &doc) {
//   SALT_JOB *job = new SALT_JOB;
//   if (doc.HasMember("tag")) {
//     const char *tag = doc["tag"].GetString();
//     if (strstr(tag, "/new")) {
//       job->type = SALT_JOB_TYPE_NEW;
//       job->ptr = new SALT_JOB_NEW;
//       ((SALT_JOB_NEW *)job->ptr)->stamp_sec = 0;
//       ((SALT_JOB_NEW *)job->ptr)->timerout = 0;
//       ((SALT_JOB_NEW *)job->ptr)->retnum = 0;
//       if (parse_salt_job_new((SALT_JOB_NEW *)job->ptr, doc)) {
//         delete ((SALT_JOB_NEW *)job->ptr);
//         delete job;
//         return 0;
//       }
//     } else if (strstr(tag, "/ret")) {
//       job->type = SALT_JOB_TYPE_RET;
//       job->ptr = new SALT_JOB_RET;
//       ((SALT_JOB_RET *)job->ptr)->stamp_sec = 0;
//       ((SALT_JOB_RET *)job->ptr)->stamp_usec = 0;
//       if (parse_salt_job_ret((SALT_JOB_RET *)job->ptr, doc)) {
//         delete ((SALT_JOB_RET *)job->ptr);
//         delete job;
//         return 0;
//       }
//     } else {
//       job->type = SALT_JOB_TYPE_IGNORE;
//       job->ptr = 0;
//     }
//   } else
//     return 0;
//
//   return job;
// }

static void free_job(SALT_JOB_TYPE type, SALT_JOB_PTR job) {
  switch (type) {
  case SALT_JOB_TYPE_NEW:
    delete (SALT_JOB_NEW *)job;
    break;
  case SALT_JOB_TYPE_RET:
    delete (SALT_JOB_RET *)job;
    break;
  default:
    break;
  }
}

static void show_json_string(const char *json_data, size_t len) {
  std::cerr << "Original sting len " << len << "-->" << json_data
            << "--- Original Over\n";
  std::cerr << "Begin: --- \n";
  for (size_t i = 0; i < len; i++)
    std::cout << json_data[i];
  std::cerr << "--- Over\n";
}

int parse_salt_jobmap(JOBMAP *jobmap, const char *json_data, size_t len) {
  rapidjson::Document doc;
  doc.Parse((char *)json_data, len);

  if (doc.HasParseError()) {
    std::cerr << "doc has error\n";
    show_json_string(json_data, len);
    return -2;
  }

  SALT_JOB_TYPE type = SALT_JOB_TYPE_IGNORE;
  SALT_JOB_PTR job = _parse_with_type_(doc, &type);
  if (!job && type != SALT_JOB_TYPE_IGNORE) {
    std::cerr << "parser got error\n";
    show_json_string(json_data, len);
    return -3;
  }
  std::lock_guard<std::mutex> *guard = 0;
  switch (type) {
  case SALT_JOB_TYPE_NEW:
    // add job to set and map
    {
      // std::cout << "waiting for new\n";
      guard = new std::lock_guard<std::mutex>(g_maps_mutex);
      bool found1 =
          (jobmap->jobs.find(((SALT_JOB_NEW *)job)->jid) != jobmap->jobs.end());
      if (found1) {
        std::cerr << "error @ jobmap->jobs.find job_NEW "
                  << ((SALT_JOB_NEW *)job)->jid << "\n";
      }
      bool found2 = (jobmap->minions.find(((SALT_JOB_NEW *)job)->jid) !=
                     jobmap->minions.end());
      if (found2) {
        std::cerr << "error @ jobmap->jobs.minions job_NEW "
                  << ((SALT_JOB_NEW *)job)->jid << "\n";
      }

      if (found1 || found2)
        goto error_exit;

      // insert new job
      jobmap->jobs.insert(std::pair<std::string, SALT_JOB_NEW *>(
          ((SALT_JOB_NEW *)job)->jid, (SALT_JOB_NEW *)job));
      std::cout << "JOBS insert job " << ((SALT_JOB_NEW *)job)->jid << " => "
                << job << std::endl;
      MapMinionRet *mset = new MapMinionRet();
      jobmap->minions.insert(std::pair<std::string, MapMinionRet *>(
          ((SALT_JOB_NEW *)job)->jid, mset));
      // std::cout << "MINIONS insert job " << ((SALT_JOB_NEW *)job)->jid
      //           << std::endl;
      for (auto &p : ((SALT_JOB_NEW *)job)->minions) {
        mset->insert(std::pair<std::string, SALT_JOB_RET *>(p, nullptr));
        // std::cout << p << " => "
        //           << "nullptr" << std::endl;
      }
    }
    break;
  case SALT_JOB_TYPE_RET:
    // remove job from map and set
    {
      guard = new std::lock_guard<std::mutex>(g_maps_mutex);
      MapJid2Job::iterator jobIter =
          jobmap->jobs.find(((SALT_JOB_RET *)job)->jid);
      bool found1 = (jobIter != jobmap->jobs.end());
      MapJid2Minions::iterator minionIter =
          jobmap->minions.find(((SALT_JOB_RET *)job)->jid);
      bool found2 = (minionIter != jobmap->minions.end());
      MapMinionRet *mset = minionIter->second;
      MapMinionRet::iterator minRetIter;
      bool found3 = false;
      if (mset) {
        minRetIter = mset->find(((SALT_JOB_RET *)job)->minion_id);
        found3 = (minRetIter != mset->end());
      }

      if (!found1) {
        std::cerr << "error @ jobmap->jobs.find job_new\n";
      }
      if (!found2) {
        std::cerr << "error @ jobmap->jobs.minions find job_ret_set\n";
      }
      if (!found3) {
        std::cerr << "error @ jobmap->jobs.ret.set find job_ret\n";
      }

      if (!found1 || !found2 || !found3)
        goto error_exit;

      // got job return
      if (minRetIter->second == nullptr) {
        minRetIter->second = (SALT_JOB_RET *)job;
        ++(((jobIter)->second)->retnum);
        if (((jobIter)->second)->retnum ==
            ((jobIter)->second)->minions.size()) {
          std::cout << "mission " << ((jobIter)->second)->jid <<" finished\n";
          erase_job(jobIter);
        }
        // std::cout << "update returun " << ((SALT_JOB_RET *)job)->jid << ", "
        //           << ((SALT_JOB_RET *)job)->minion_id << "=>" << job
        //           << std::endl;
      } else {
        // std::cout << "drop duplicated return " << ((SALT_JOB_RET *)job)->jid
        //           << ", " << ((SALT_JOB_RET *)job)->minion_id << "=>" << job
        //           << std::endl;
        delete ((SALT_JOB_RET *)job);
      }
    }
    break;

  default:
    break;
  }
  if (guard)
    delete guard;
  return 0;

error_exit:
  if (guard)
    delete guard;
  free_job(type, job);
  return -1;
}

void free_salt_job(SALT_JOB *job) {
  if (job->ptr) {
    if (job->type == SALT_JOB_TYPE_NEW)
      delete (SALT_JOB_NEW *)(job->ptr);
    else if (job->type == SALT_JOB_TYPE_RET)
      delete (SALT_JOB_RET *)(job->ptr);
  }
  job->type = SALT_JOB_TYPE_IGNORE;
  job->ptr = 0;
}

std::ostream &operator<<(std::ostream &out, SALT_JOB_NEW &jobnew) {
  out << "JOB NEW: " << jobnew.ple_id << ", " << jobnew.tag << ", "
      << jobnew.tgt_type << ", " << jobnew.jid << ", " << jobnew.tgt << ", "
      << jobnew.stamp << ", " << jobnew.stamp_sec << ", " << jobnew.timerout
      << ", " << jobnew.user << ", " << jobnew.arg.size() << ", " << jobnew.fun
      << ", " << jobnew.minions.size() << std::endl;
  return out;
}

std::ostream &operator<<(std::ostream &out, SALT_JOB_RET &jobret) {
  out << "JOB RET: " << jobret.ple_id << ", " << jobret.rettype << ", "
      << jobret.tag << ", " << jobret.stamp << ", " << jobret.stamp_sec << ", "
      << jobret.stamp_usec << ", " << jobret.pid << ", " << jobret.retcode
      << ", " << jobret.stderr << ", " << jobret.stdout << ", "
      << jobret.success << ", " << jobret.cmd << ", " << jobret.jid << ", "
      << jobret.fun << ", " << jobret.minion_id << std::endl;
  return out;
}

void show_job(SALT_JOB *job) {
  if (_json_error_)
    std::cout << "json_error: = " << _json_error_ << std::endl;
  switch (job->type) {
  case SALT_JOB_TYPE_NEW:
    std::cout << *(SALT_JOB_NEW *)(job->ptr);
    break;
  case SALT_JOB_TYPE_RET:
    std::cout << *(SALT_JOB_RET *)(job->ptr);
    break;
  default:
    break;
  }
}

void init_string(struct cstring *s) {
  s->len = 0;
  s->ptr = (char *)malloc(s->len + 1);
  if (s->ptr == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  s->ptr[0] = '\0';
}

void free_string(struct cstring *s) {
  if (s->ptr)
    free(s->ptr);
  s->ptr = 0;
  s->len = 0;
}

size_t writeone(void *ptr, size_t size, size_t nmemb, struct cstring *s) {
  size_t new_len = size * nmemb;
  s->ptr = (char *)realloc(s->ptr, new_len + 1);
  if (s->ptr == NULL) {
    fprintf(stderr, "realloc() failed\n");
    exit(EXIT_FAILURE);
  }
  memcpy(s->ptr, ptr, size * nmemb);
  s->ptr[new_len] = '\0';
  s->len = new_len;

  return size * nmemb;
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct cstring *s) {
  size_t new_len = s->len + size * nmemb;
  s->ptr = (char *)realloc(s->ptr, new_len + 1);
  if (s->ptr == NULL) {
    fprintf(stderr, "realloc() failed\n");
    exit(EXIT_FAILURE);
  }
  memcpy(s->ptr + s->len, ptr, size * nmemb);
  s->ptr[new_len] = '\0';
  s->len = new_len;

  return size * nmemb;
}

size_t print_one(void *ptr, size_t size, size_t nmemb, struct cstring *s) {
  (void)s;
  fprintf(stdout, "%s\n", (char *)ptr);
  return size * nmemb;
}

size_t get_token(void *ptr, size_t size, size_t nmemb, char *token) {
  // fprintf(stdout, "%s", (char *)ptr);
  //"token": "897b0cc93d59f10aaa46159e7dfba417d225b2cd"
  char *pos = strstr((char *)ptr, "\"token\": \"");
  if (pos) {
    pos += strlen("\"token\": \"");
    char *end = strchr(pos, '\"');
    if (!end)
      return 0;
    strncpy(token, pos, end - pos);
    token[end - pos + 1] = 0;
    // fprintf(stdout, "Token is %s", token);
  } else
    return 0;
  return size * nmemb;
}

char *get_line(const char *buf, int *lines) {
  if (!buf || !(*buf))
    return nullptr;
  char *ptr = (char *)buf;

  while (ptr && *ptr && *ptr != '\n' && *ptr != '\r')
    ++ptr;

  ++*lines;
  return ptr + 1;
}

char *get_line(const char *buf) {
  if (!buf || !(*buf))
    return nullptr;
  char *ptr = (char *)buf;

  while (ptr && *ptr && *ptr != '\n' && *ptr != '\r')
    ++ptr;

  return ptr + 1;
}

size_t parse_json(void *ptr, size_t size, size_t nmemb, SALT_JOB *job) {
  // data: {\"tag\": \"salt/job/
  char *buf = (char *)ptr;
  char *tmp = buf;
  while (nullptr != (tmp = get_line(buf))) {
    if (!strncmp(buf, "data: ", 6)) {
      free_salt_job(job);
      // fprintf(stdout, "%s", (char *)pos);
      parse_salt_job(job, (const char *)buf + 6, tmp - buf - 6);
      show_job(job);
    }
    buf = tmp;
  }
  return size * nmemb;
}

size_t parse_job(void *ptr, size_t size, size_t nmemb, JOBMAP *jobmap) {
  // data: {\"tag\": \"salt/job/
  char *buf = (char *)ptr;
  char *tmp = buf;
  while (nullptr != (tmp = get_line(buf))) {
    if (!strncmp(buf, "data: ", 6)) {
      // fprintf(stdout, "%s", (char *)pos);
      if (parse_salt_jobmap(jobmap, (const char *)buf + 6, tmp - buf - 6) < -1)
        return size * nmemb; // 0;
    }
    buf = tmp;
  }
  return size * nmemb;
}

static void jobmap_cleanup(JOBMAP *jm) { (void)jm; }

// static void print_cookies(CURL *curl) {
//   CURLcode res;
//   struct curl_slist *cookies;
//   struct curl_slist *nc;
//   int i;
//
//   printf("Cookies, curl knows:\n");
//   res = curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);
//   if (res != CURLE_OK) {
//     fprintf(stderr, "Curl curl_easy_getinfo failed: %s\n",
//             curl_easy_strerror(res));
//     exit(1);
//   }
//   nc = cookies, i = 1;
//   while (nc) {
//     printf("[%d]: %s\n", i, nc->data);
//     nc = nc->next;
//     i++;
//   }
//   if (i == 1) {
//     printf("(none)\n");
//   }
//   curl_slist_free_all(cookies);
// }

#ifdef EXPECT_NE
#undef EXPECT_NE
#endif // EXPECT_NE
#define EXPECT_NE(x, y)                                                        \
  if ((x) == (y)) {                                                            \
    printf("%s %d, %ld vs %ld\n", __FILE__, __LINE__, (int64_t)(x),            \
           (int64_t)(y));                                                      \
    goto error_exit;                                                           \
  }

#ifdef EXPECT_EQ
#undef EXPECT_EQ
#endif // EXPECT_EQ
#define EXPECT_EQ(x, y)                                                        \
  if ((x) != (y)) {                                                            \
    printf("%s %d, %ld vs %ld\n", __FILE__, __LINE__, (int64_t)(x),            \
           (int64_t)(y));                                                      \
    goto error_exit;                                                           \
  }

char gtoken[32] = {0};

void curl_get_token() {
  CURL *curl = 0;
  CURLcode res = CURLE_OK;
  int rspcode = 0;
  struct curl_slist *headers = NULL; /* init to NULL is important */

  /* get a curl handle */
  curl = curl_easy_init();
  EXPECT_NE(curl, (void *)NULL);
  if (curl) {
    /* First set the URL that is about to receive our POST. This URL can
     * just as well be a https:// URL if that is what should receive the
     * data. */
    EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_URL,
                                  "http://10.10.10.19:8000/login"));
    /* Now specify the POST data */
    EXPECT_EQ(0,
              curl_easy_setopt(curl, CURLOPT_POSTFIELDS,
                               "username=sean&password=hongt@8a51&eauth=pam"));
    EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_token));
    EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_WRITEDATA, gtoken));

    EXPECT_NE((void *)0,
              headers = curl_slist_append(headers, "Accept: application/json"));

    /* pass our list of custom made headers */
    EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers));

    /* Perform the request, res will get the return code */
    EXPECT_EQ(0, res = curl_easy_perform(curl));
    EXPECT_EQ(0, curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &rspcode));
    EXPECT_EQ(200, rspcode);

    /* always cleanup */
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
  }

  return;

error_exit:
  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);
  return;
}

void shot_token() {
  std::cout << gtoken << std::endl;
}

int curl_salt_event() {
  CURL *curl = 0;
  CURLcode res = CURLE_OK;
  int rspcode = 0;
  struct curl_slist *headers = NULL; /* init to NULL is important */

  /* get a curl handle */
  curl = curl_easy_init();
  EXPECT_NE(curl, (void *)NULL);
  if (curl) {
    EXPECT_NE((void *)0,
              headers = curl_slist_append(headers, "Accept: application/json"));
    char x_token[128];
    snprintf(x_token, 128, "X-Auth-Token: %s", gtoken);
    EXPECT_NE((void *)0,
              headers = curl_slist_append(headers, x_token));
    EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers));

    EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_URL, "http://10.10.10.19:8000/events"));
    EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, parse_job));
    EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_WRITEDATA, &gjobmap));

    curl_easy_setopt(curl, (CURLoption)43, 1L);
    curl_easy_setopt(curl, (CURLoption)42, 0L);
    curl_easy_setopt(curl, (CURLoption)61, 0L);
    curl_easy_setopt(curl, (CURLoption)45, 0L);
    curl_easy_setopt(curl, (CURLoption)46, 0L);
    curl_easy_setopt(curl, (CURLoption)48, 0L);
    curl_easy_setopt(curl, (CURLoption)50, 0L);
    curl_easy_setopt(curl, (CURLoption)53, 0L);
    curl_easy_setopt(curl, (CURLoption)155, 0L);
    curl_easy_setopt(curl, (CURLoption)52, 0L);
    curl_easy_setopt(curl, (CURLoption)105, 0L);
    curl_easy_setopt(curl, (CURLoption)58, 0L);
    curl_easy_setopt(curl, (CURLoption)68, 50L);
    curl_easy_setopt(curl, (CURLoption)161, 0L);
    curl_easy_setopt(curl, (CURLoption)19, 0L);
    curl_easy_setopt(curl, (CURLoption)20, 0L);
    curl_easy_setopt(curl, (CURLoption)64, 1L);
    curl_easy_setopt(curl, (CURLoption)27, 0L);
    curl_easy_setopt(curl, (CURLoption)96, 0L);
    curl_easy_setopt(curl, (CURLoption)34, 0L);
    curl_easy_setopt(curl, (CURLoption)156, 0L);
    curl_easy_setopt(curl, (CURLoption)110, 0L);
    curl_easy_setopt(curl, (CURLoption)113, 0L);
    curl_easy_setopt(curl, (CURLoption)136, 0L);
    curl_easy_setopt(curl, (CURLoption)137, 0L);
    curl_easy_setopt(curl, (CURLoption)138, 0L);
    curl_easy_setopt(curl, (CURLoption)213, 1L);

    curl_easy_setopt(curl, (CURLoption)30145, 0L);
    curl_easy_setopt(curl, (CURLoption)30146, 0L);
    curl_easy_setopt(curl, (CURLoption)30116, 0L);

    curl_easy_setopt(curl, (CURLoption)10004, 0);
    curl_easy_setopt(curl, (CURLoption)10006, 0);
    curl_easy_setopt(curl, (CURLoption)10177, 0);
    curl_easy_setopt(curl, (CURLoption)10005, 0);
    curl_easy_setopt(curl, (CURLoption)10007, 0);
    curl_easy_setopt(curl, (CURLoption)10016, 0);
    curl_easy_setopt(curl, (CURLoption)10017, 0);
    curl_easy_setopt(curl, (CURLoption)10026, 0);
    curl_easy_setopt(curl, (CURLoption)10153, 0);
    curl_easy_setopt(curl, (CURLoption)10152, 0);
    curl_easy_setopt(curl, (CURLoption)10162, 0);
    curl_easy_setopt(curl, (CURLoption)10025, 0);
    curl_easy_setopt(curl, (CURLoption)10086, 0);
    curl_easy_setopt(curl, (CURLoption)10087, 0);
    curl_easy_setopt(curl, (CURLoption)10088, 0);
    curl_easy_setopt(curl, (CURLoption)10036, 0);
    curl_easy_setopt(curl, (CURLoption)10062, 0);
    curl_easy_setopt(curl, (CURLoption)10063, 0);
    curl_easy_setopt(curl, (CURLoption)10076, 0);
    curl_easy_setopt(curl, (CURLoption)10077, 0);
    curl_easy_setopt(curl, (CURLoption)10134, 0);
    curl_easy_setopt(curl, (CURLoption)10147, 0);

    EXPECT_EQ(0, res = curl_easy_perform(curl));
    EXPECT_EQ(0, curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &rspcode));
    EXPECT_EQ(200, rspcode);

    /* always cleanup */
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
  }

  jobmap_cleanup(&gjobmap);
  return (0);

error_exit:
  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);
  jobmap_cleanup(&gjobmap);

  return -1;
}

/*
curl_easy_setopt lval 43 1L
curl_easy_setopt lval 42 0L
curl_easy_setopt lval 61 0L
curl_easy_setopt lval 45 0L
curl_easy_setopt lval 46 0L
curl_easy_setopt lval 48 0L
curl_easy_setopt lval 50 0L
curl_easy_setopt lval 53 0L
curl_easy_setopt lval 155 0L
curl_easy_setopt lval 52 0L
curl_easy_setopt lval 105 0L
curl_easy_setopt lval 58 0L
curl_easy_setopt lval 68 50L
curl_easy_setopt lval 161 0L
curl_easy_setopt lval 19 0L
curl_easy_setopt lval 20 0L
curl_easy_setopt lval 64 1L
curl_easy_setopt lval 27 0L
curl_easy_setopt lval 96 0L
curl_easy_setopt lval 34 0L
curl_easy_setopt lval 156 0L
curl_easy_setopt lval 110 0L
curl_easy_setopt lval 113 0L
curl_easy_setopt lval 136 0L
curl_easy_setopt lval 137 0L
curl_easy_setopt lval 138 0L
curl_easy_setopt lval 213 1L


curl_easy_setopt oval 30145 0
curl_easy_setopt oval 30146 0
curl_easy_setopt oval 30116 0


curl_easy_setopt pval 10001 7fffffffd8a0
curl_easy_setopt pval 10195 7fffffffd8a0
curl_easy_setopt pval 20011 40431d
curl_easy_setopt pval 10009 7fffffffd9c0
curl_easy_setopt pval 20012 403f7c
curl_easy_setopt pval 10168 7fffffffd9c0
curl_easy_setopt pval 20167 4040bc
curl_easy_setopt pval 10002 668278
curl_easy_setopt pval 10010 7fffffffda90
curl_easy_setopt pval 10018 666e98
curl_easy_setopt pval 10183 668358
curl_easy_setopt pval 10031 666e68
curl_easy_setopt pval 10037 7ffff72bd1c0
curl_easy_setopt pval 20079 4034ec
curl_easy_setopt pval 10029 7fffffffda30
curl_easy_setopt pval 10004 0
curl_easy_setopt pval 10006 0
curl_easy_setopt pval 10177 0
curl_easy_setopt pval 10005 0
curl_easy_setopt pval 10007 0
curl_easy_setopt pval 10016 0
curl_easy_setopt pval 10017 0
curl_easy_setopt pval 10026 0
curl_easy_setopt pval 10153 0
curl_easy_setopt pval 10152 0
curl_easy_setopt pval 10162 0
curl_easy_setopt pval 10025 0
curl_easy_setopt pval 10086 0
curl_easy_setopt pval 10087 0
curl_easy_setopt pval 10088 0
curl_easy_setopt pval 10036 0
curl_easy_setopt pval 10062 0
curl_easy_setopt pval 10063 0
curl_easy_setopt pval 10076 0
curl_easy_setopt pval 10077 0
curl_easy_setopt pval 10134 0
curl_easy_setopt pval 10147 0
*/


static const char* cmd_str[2] = {
  "client=local_async&fun=test.ping&tgt=*",
  "client=local_async&fun=cmd.run_all&tgt=old08002759F4B6&arg=\"c:\new_salt\ExecClient.exe abcd\""
};

/*
curl http://10.10.10.19:8000 -H "Accept: application/json" -X POST
 --data-urlencode "client=local_async"
 --data-urlencode "fun=test.ping"
 --data-urlencode "tgt=*"
 -H 'X-Auth-Token: 09459e1b1044ed0041186a59a34d5656bb243a3f'
*/

int curl_run_cmd(int cmd_index) {
  CURL *curl = 0;
  CURLcode res = CURLE_OK;
  int rspcode = 0;
  struct curl_slist *headers = NULL; /* init to NULL is important */
  cstring s;
  init_string(&s);

  /* get a curl handle */
  curl = curl_easy_init();
  if (!curl) return -1;
  if (curl) {
    EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_URL, "http://10.10.10.19:8000"));
    /* Now specify the POST data */
    EXPECT_EQ(0,
              curl_easy_setopt(curl, CURLOPT_POSTFIELDS, cmd_str[cmd_index]));
    // EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, print_one));
    // EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s));

    EXPECT_NE((void *)0,
              headers = curl_slist_append(headers, "Accept: application/json"));
    char x_token[128];
    snprintf(x_token, 128, "X-Auth-Token: %s", gtoken);
    std::cout << gtoken << std::endl;
    EXPECT_NE((void *)0,
              headers = curl_slist_append(headers, x_token));
    EXPECT_EQ(0, curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers));

    /* Perform the request, res will get the return code */
    EXPECT_EQ(0, res = curl_easy_perform(curl));
    EXPECT_EQ(0, curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &rspcode));
    EXPECT_EQ(200, rspcode);

    /* always cleanup */
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
  }

  free_string(&s);
  return 0;

error_exit:
  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);
  free_string(&s);
  return -1;
}


void test_get_token() {
  CURL *curl;
  int rspcode = 0;
  struct curl_slist *headers = NULL;

  /* get a curl handle */
  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "http://10.10.10.19:8000/login");
    /* Now specify the POST data */
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS,
                     "username=sean&password=hongt@8a51&eauth=pam");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_token);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, gtoken);

    headers = curl_slist_append(headers, "Accept: application/json");

    /* pass our list of custom made headers */
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    /* Perform the request, res will get the return code */
    curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &rspcode);

    /* always cleanup */
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
  }
}
