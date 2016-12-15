#include <iostream>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rapidjson/document.h"
#include "salt_api.h"

#include <chrono>
#include <mutex>
#include <thread>

std::mutex g_maps_mutex;
JOBMAP gjobmap;

static size_t _json_error_ = 0;
extern int run;
extern struct DataSet<MR_REAL_NODE> g_nodes;

// static int curl_run_cmd(int cmd_index);
extern int node_job_finished(SALT_JOB *job, std::vector<int> *vec);
extern int run_something(std::vector<int>* vec);

// static void erase_return_by_jid(std::string &jid) {
//   MapJid2Minions::iterator mj2mIter = gjobmap.minions.find(jid);
//   MapMinionRet *retset = (mj2mIter)->second;
//   if (retset) {
//     for (MapMinionRet::iterator iter = retset->begin(); iter !=
//     retset->end();
//          ++iter) {
//       // std::cout << "erase minion -> " << (iter)->first << std::endl;
//       if (((iter)->second))
//         delete ((iter)->second);
//     }
//     retset->clear();
//     // std::cout << "erase minion set " << jid << std::endl;
//     delete retset;
//     gjobmap.minions.erase(mj2mIter);
//   } else {
//     std::cout << "Erase Minion's Return " << jid << std::endl;
//   }
// }

template <typename T>
static void output_vector(std::vector<T> vec) {
  std::cout << "[";
  for (auto &p : vec)
    std::cout << p << ", ";
  std::cout << "]\n";
}

void thread_check_timer_out() {
  while (run) {
    std::this_thread::sleep_for(std::chrono::seconds(30));
    //printf("i want to  get a lock ... ");
    std::lock_guard<std::mutex> *guard =
        new std::lock_guard<std::mutex>(g_maps_mutex);
    //printf("i got it ... ");
    time_t now = time(0);
    std::vector<int>* should_run = new std::vector<int>;
    for (MapJid2Job::iterator iter = gjobmap.jobs.begin();
         iter != gjobmap.jobs.end(); ++iter) {
      SALT_JOB* job = iter->second;
      if (!job) {
        std::cout << "WTF WTF WTF?\n";
        continue;
      }

      if (job->node_id >= 0) {
        if (now - job->stamp_sec >= job->timerout) {
          if (job->timerout) {
            if (job->status != JOB_STATUS_TYPE_SUCCESSED
               && job->status != JOB_STATUS_TYPE_FAILED) {
              job->status = JOB_STATUS_TYPE_TIMEOUT;
              //std::cout << "   |--> timerout job " << job->node_id
              //          << std::endl;
              //output_vector(job->minions);
              g_nodes[job->node_id].status = job->status;
            }
            job->timerout = 0;
          }

          //std::cout << "job-finish return 1 .. ";
          node_job_finished(iter->second,
                            should_run);
          //std::cout << ret << " in timerout checker\n";
        }
      } else {
        std::cerr << "job node id is < 0, " << *job << std::endl;
        // erase_return_by_jid((iter->second)->jid);
        // delete (iter->second);
        // iter = gjobmap.jobs.erase(iter);
      }
    }
    delete guard;
    // printf("TimerOut Checked.\n");
    run_something(should_run);
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

static int parse_salt_job_new(SALT_JOB *job, rapidjson::Document &doc) {
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

/*
data: {"tag": "salt/job/20161123065414567343/ret/old0800277AF5BE", "data":
{"_stamp": "2016-11-22T22:54:14.580881", "return": true, "retcode": 0,
"success": true, "cmd": "_return", "jid": "20161123065414567343", "fun":
"test.ping", "id": "old0800277AF5BE"}}

data: {"tag": "salt/job/20161123065056424864/ret/old08002759F4B6", "data":
{"_stamp": "2016-11-22T22:50:56.749570", "return": {"pid": 2976, "retcode": 0,
"stdout": "", "stdout": ""}, "retcode": 0, "success": true, "cmd": "_return",
"jid": "20161123065056424864", "fun": "cmd.run_all", "id": "old08002759F4B6"}}

data: {"tag": "salt/job/20161130080318083390/ret/minion3", "data": {"fun_args":
["20161130080110849910"], "jid": "20161130080318083390", "return": {"tgt_type":
"glob", "jid": "20161130080110849910", "tgt": "minion3", "pid": 21561, "ret":
"", "user": "root", "arg": ["VBoxHeadless --startvm old-3-3-1"], "fun":
"cmd.run_all"}, "retcode": 0, "success": true, "cmd": "_return", "_stamp":
"2016-11-30T00:03:18.145323", "fun": "saltutil.find_job", "id": "minion3"}}

{"tag": "salt/job/20161201142918499600/ret/old080027A63244", "data": {"jid":
"20161201142918499600", "return": "Traceback (most recent call last):\n  File
\"salt/minion.py\", line 722, in _thread_return\n  File \"salt/modules/cp.py\",
line 150, in get_file\n  File \"salt/fileclient.py\", line 708, in get_file\n
File \"salt/utils/__init__.py\", line 930, in fopen\nIOError: [Errno 13]
Permission denied: 'C:\\\\hongt\\\\Client'\n", "success": false, "cmd":
"_return", "_stamp": "2016-12-01T06:29:21.400408", "fun": "cp.get_file", "id":
"old080027A63244"}}
*/

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

int parse_salt_job_ret(SALT_JOB_RET *job, const char *json_data) {
  rapidjson::StringStream ss(json_data);
  rapidjson::Document doc;
  doc.ParseStream(ss);

  if (doc.HasParseError()) {
    std::cout << "Error at " << doc.GetErrorOffset() << std::endl
              << json_data + doc.GetErrorOffset() << std::endl;
    return -1;
  }

  return parse_salt_job_ret(job, doc);
}

static SALT_JOB_PTR _parse_with_type_(rapidjson::Document &doc,
                                      SALT_JOB_TYPE *type) {
  SALT_JOB_PTR job = 0;
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

static void free_job(SALT_JOB_TYPE type, SALT_JOB_PTR job) {
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

static void show_json_string(const char *json_data, size_t len) {
  std::cout << "|-->";
  for (size_t i = 0; i < len; i++)
    std::cout << json_data[i];
  std::cout << "<--|\n";
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

static int parse_salt_myjob_jobmap(const char *json_data, size_t len,
                                   JOBMAP *jobmap, SALT_JOB *job) {
  rapidjson::Document doc;
  doc.Parse((char *)json_data, len);

  if (doc.HasParseError()) {
    std::cout << "doc has error\n";
    show_json_string(json_data, len);
    return -2;
  }

  if (parse_salt_my_job(job, doc) < 0) {
    std::cout << "parse_salt_new_job error\n";
    show_json_string(json_data, len);
    delete job;
    return -3;
  }

  // show_json_string(json_data, len);
  std::lock_guard<std::mutex> *guard =
      new std::lock_guard<std::mutex>(g_maps_mutex);

  MapJid2Job::iterator iter = jobmap->jobs.find(job->jid);
  bool found1 = (iter != jobmap->jobs.end());
  bool found2 = (jobmap->minions.find(job->jid) != jobmap->minions.end());

  if (!found1 && !found2) {
    g_nodes[job->node_id].status = JOB_STATUS_TYPE_RUNNING;
    // insert new job
    jobmap->jobs.insert(std::pair<std::string, SALT_JOB *>(job->jid, job));
    // std::cout << "=.= JOBS insert job " << job->ple_id << ", " <<
    // job->node_id
    //           << ", " << job->jid << " => " << job << std::endl;
    MapMinionRet *mset = new MapMinionRet();
    jobmap->minions.insert(
        std::pair<std::string, MapMinionRet *>(job->jid, mset));
    // std::cout << "MINIONS insert job " << ((SALT_JOB *)job)->jid <<
    // std::endl;
    for (auto &p : job->minions) {
      mset->insert(std::pair<std::string, SALT_JOB_RET *>(p, nullptr));
      // std::cout << p << " => "
      //           << "nullptr" << std::endl;
    }
  } else if (found1 && found2) {
    g_nodes[job->node_id].status = JOB_STATUS_TYPE_RUNNING;
    // update my new job
    if ((iter->second)->ple_id == 0) {
      // std::cout << "=.= Update Job " << job->ple_id << ", " << job->node_id
      //           << ", " << job->jid << std::endl;
      copy_job_status(iter->second, job);
    }
    delete job;
  } else {
    std::cout << "WTF insert or update? WTF\n";
    delete job;
    delete guard;
    return -1;
  }

  delete guard;
  return 0;
}

int parse_salt_jobmap(const char *json_data, size_t len, JOBMAP *jobmap) {
  rapidjson::Document doc;
  doc.Parse((char *)json_data, len);

  if (doc.HasParseError()) {
    std::cout << "doc has error\n";
    show_json_string(json_data, len);
    return -2;
  }

  SALT_JOB_TYPE type = SALT_JOB_TYPE_IGNORE;
  SALT_JOB_PTR jobptr = _parse_with_type_(doc, &type);
  if (!jobptr && type != SALT_JOB_TYPE_IGNORE) {
    std::cout << "_parse_with_type_ parser got error\n";
    show_json_string(json_data, len);
    return -3;
  }

  // show_json_string(json_data, len);
  std::lock_guard<std::mutex> *guard =
      new std::lock_guard<std::mutex>(g_maps_mutex);
  std::vector<int>* should_run = new std::vector<int>;
  switch (type) {
  case SALT_JOB_TYPE_NEW:
    // add job to set and map
    {
      SALT_JOB *job = (SALT_JOB *)jobptr;
      // std::cout << "waiting for new\n";
      MapJid2Job::iterator jobIter = jobmap->jobs.find(job->jid);
      bool found1 = (jobIter != jobmap->jobs.end());
      bool found2 = (jobmap->minions.find(job->jid) != jobmap->minions.end());

      if (!found1 && !found2) {
        if (job->node_id >= 0) g_nodes[job->node_id].status = JOB_STATUS_TYPE_RUNNING;

        {
          // insert new job
          jobmap->jobs.insert(
              std::pair<std::string, SALT_JOB *>(job->jid, job));
          // std::cout << "^_^ JOBS insert job " << job->ple_id
          //           << ", " << job->node_id << ", "
          //           << job->jid << " => " << jobptr << std::endl;
          MapMinionRet *mset = new MapMinionRet();
          jobmap->minions.insert(
              std::pair<std::string, MapMinionRet *>(job->jid, mset));
          // std::cout << "    MINIONS insert job " << job->jid
          //           << std::endl;
          for (auto &p : job->minions) {
            mset->insert(std::pair<std::string, SALT_JOB_RET *>(p, nullptr));
            // std::cout << p << " => "
            //           << "nullptr" << std::endl;
          }
        }
      } else if (found1 && found2) {
        if (job->node_id >= 0) g_nodes[job->node_id].status = JOB_STATUS_TYPE_RUNNING;
        // update new job
        if ((jobIter->second)->ple_id != 0) {
          SALT_JOB *prevjob = jobIter->second;
          // std::cout << "^_^ Update Job " << prevjob->ple_id << ", "
          //           << prevjob->node_id << ", "
          //           << prevjob->jid << std::endl;
          copy_job_status(job, prevjob);
          jobIter->second = job;
          free_job(SALT_JOB_TYPE_NEW, prevjob);
        } else {
          // std::cout << "Drop duplicated job " << ((SALT_JOB_NEW *)job)->jid
          //           << std::endl;
          free_job(type, jobptr);
        }
      } else {

        goto error_exit;
      }
    }
    break;
  case SALT_JOB_TYPE_RET:
    // remove job from map and set
    {
      SALT_JOB_RET *jobret = (SALT_JOB_RET *)jobptr;
      // show_json_string(json_data, len);
      MapJid2Job::iterator jobIter = jobmap->jobs.find(jobret->jid);
      bool found1 = (jobIter != jobmap->jobs.end());
      MapJid2Minions::iterator minionIter = jobmap->minions.find(jobret->jid);
      bool found2 = (minionIter != jobmap->minions.end());
      MapMinionRet *mset = minionIter->second;
      MapMinionRet::iterator minRetIter;
      bool found3 = false;
      if (mset)
        found3 = ((minRetIter = mset->find(jobret->minion_id)) != mset->end());

      if (!found1) {
        std::cout << "error @ jobmap->jobs.find job_new " << jobret->jid << ", "
                  << jobret->minion_id << std::endl;
      }
      if (!found2) {
        std::cout << "error @ jobmap->jobs.minions find job_ret_set "
                  << jobret->jid << ", " << jobret->minion_id << std::endl;
      }
      if (!found3) {
        std::cout << "error @ jobmap->jobs.ret.set find job_ret " << jobret->jid
                  << ", " << jobret->minion_id << std::endl;
      }

      if (!found1 || !found2 || !found3)
        goto error_exit;

      // got job return
      if (minRetIter->second == nullptr) {
        SALT_JOB *job = jobIter->second;
        minRetIter->second = jobret;
        ++(job->retnum);

        if (jobret->retcode == 0)
          ++(job->success_num);

        if (job->timerout == 0) {
          /// std::cout << job->node_id << " has already ???  .>_<.!\n";
          free_job(type, jobptr);
        } else if (job->retnum == job->minions.size()) {
          job->timerout = 0;  //it's over, do not check time out again
          job->status =
              (job->success_num == job->minions.size())
                  ? JOB_STATUS_TYPE_SUCCESSED
                  : JOB_STATUS_TYPE_FAILED;
          if (job->node_id >= 0) {
            g_nodes[job->node_id].status = job->status;
            // std::cout << "job-finish return 2 .. ";
            node_job_finished(jobIter->second, should_run);
            //int ret = node_job_finished(jobIter->second, mset);
            // std::cout << ret << " in salt return\n";
          }
        }
        // std::cout << "update returun " << jobret->jid << ", "
        //           << jobret->minion_id << "=>" << jobptr
        //           << std::endl;
        // if (job->status == JOB_STATUS_TYPE_SUCCESSED)
        //   std::cout << "All Successed!\n";
        // else
        //   std::cout << "But NOt All Successed! "
        //             << job->success_num << " but expect "
        //             << ((jobIter)->second)->retnum << std::endl;
      } else {
        std::cout << "drop duplicated return " << jobret->jid << ", "
                  << jobret->minion_id << "=>" << jobptr << std::endl;
        free_job(type, jobptr);
      }
    }
    break;

  default:
    free_job(type, jobptr);
    break;
  }
  delete guard;
  run_something(should_run);

  return (0);

error_exit:
  free_job(type, jobptr);
  delete guard;
  return -1;
}

std::ostream &operator<<(std::ostream &out, SALT_JOB &jobnew) {
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
      << ", " << jobret.stdout << ", " << jobret.stderr << ", "
      << jobret.success << ", " << jobret.cmd << ", " << jobret.jid << ", "
      << jobret.fun << ", " << jobret.minion_id << std::endl;
  return out;
}

void show_job(SALT_JOB_DATA *job) {
  if (_json_error_)
    std::cout << "json_error: = " << _json_error_ << std::endl;
  switch (job->type) {
  case SALT_JOB_TYPE_NEW:
    std::cout << *(SALT_JOB *)(job->ptr);
    break;
  case SALT_JOB_TYPE_RET:
    std::cout << *(SALT_JOB_RET *)(job->ptr);
    break;
  default:
    break;
  }
}

// static char *get_line(const char *buf) {
//   if (!buf || !(*buf))
//     return nullptr;
//   char *ptr = (char *)buf;
//
//   while (ptr && *ptr && *ptr != '\n' && *ptr != '\r')
//     ++ptr;
//
//   return ptr + 1;
// }

int parse_my_job(const char *json_data, size_t size, void *param1,
                 void *param2) {
  (void)size;
  // show_json_string(json_data, size);
  if (parse_salt_myjob_jobmap(json_data, size, (JOBMAP*)param1,
                              (SALT_JOB *)param2) < -1)
    return -1;
  return 0;
}

int parse_job(const char *json_data, size_t size, void *param1, void *param2) {
  // data: {\"tag\": \"salt/job/
  (void)size;
  (void)param2;
  JOBMAP *jobmap = (JOBMAP*)param1;
  // show_json_string(json_data, size);
  if (!strncmp(json_data, "data: ", 6)) {
    if (parse_salt_jobmap(json_data + 6, size - 6, jobmap) < -1)
      return -1;
  }

  return 0;
}

void jobmap_cleanup(JOBMAP *jm) {
  std::cout << "cleanup wait a lock ... ";
  std::lock_guard<std::mutex> *guard =
      new std::lock_guard<std::mutex>(g_maps_mutex);
  std::cout << "erase everything\n";
  for (const auto &p : jm->jobs) {
    std::cout << "|-- " << (p.second)->node_id << " was "
              << job_status((p.second)->status) << std::endl;
    if ((p.second)->status != JOB_STATUS_TYPE_SUCCESSED)
      output_vector((p.second)->minions);
    delete (p.second);
  }
  jm->jobs.clear();

  for (const auto &p : jm->minions) {
    // std::cout << "erase minion set " << p.first << std::endl;
    MapMinionRet *set = (MapMinionRet *)p.second;
    for (const auto &k : *set) {
      if (nullptr != (k.second)) {
        // std::cout << "erase minion " << (k.second)->minion_id << std::endl;
        delete (k.second);
      }
    }
    delete set;
  }
  jm->minions.clear();

  std::cout << "|-- Done.\n";

  delete guard;
}
