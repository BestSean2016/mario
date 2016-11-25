#include "salt_api.h"
#include "rapidjson/document.h"

static size_t _json_error_ = 0;

static int parse_salt_job_ret(SALT_JOB_RET *job, rapidjson::Document& doc);
static int parse_salt_job_new(SALT_JOB_NEW *job, rapidjson::Document& doc);

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


static int parse_salt_job_new(SALT_JOB_NEW *job, rapidjson::Document& doc) {
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

static int parse_salt_job_ret(SALT_JOB_RET *job,   rapidjson::Document& doc) {
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

static int parse_(SALT_JOB* job, rapidjson::Document& doc) {
  if (doc.HasMember("tag")) {
    const char* tag = doc["tag"].GetString();
    if (strstr(tag, "/new"))  {
        job->type = SALT_JOB_TYPE_NEW;
        job->ptr = new SALT_JOB_NEW;
        ((SALT_JOB_NEW*)job->ptr)->stamp_sec = 0;
        ((SALT_JOB_NEW*)job->ptr)->stamp_usec = 0;
        return parse_salt_job_new((SALT_JOB_NEW*)job->ptr, doc);
    } else if (strstr(tag, "/ret")) {
        job->type = SALT_JOB_TYPE_RET;
        job->ptr = new SALT_JOB_RET;
        ((SALT_JOB_RET*)job->ptr)->stamp_sec = 0;
        ((SALT_JOB_RET*)job->ptr)->stamp_usec = 0;
        return parse_salt_job_ret((SALT_JOB_RET*)job->ptr, doc);
    } else {
        job->type = SALT_JOB_TYPE_IGNORE;
        job->ptr = 0;
    }
  }else
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


void free_salt_job(SALT_JOB* job) {
  if (job->ptr) {
    if (job->type == SALT_JOB_TYPE_NEW)
      delete (SALT_JOB_NEW*)(job->ptr);
    else if (job->type == SALT_JOB_TYPE_RET)
      delete (SALT_JOB_RET*)(job->ptr);
  }
  job->type = SALT_JOB_TYPE_IGNORE;
  job->ptr = 0;
}

std::ostream& operator << (std::ostream& out, SALT_JOB_NEW& jobnew) {
  out << "JOB NEW: "
      << jobnew.ple_id              << ", "
      << jobnew.tag                 << ", "
      << jobnew.tgt_type            << ", "
      << jobnew.jid                 << ", "
      << jobnew.tgt                 << ", "
      << jobnew.stamp               << ", "
      << jobnew.stamp_sec           << ", "
      << jobnew.stamp_usec          << ", "
      << jobnew.user                << ", "
      << jobnew.arg.size()          << ", "
      << jobnew.fun                 << ", "
      << jobnew.minions.size()      << std::endl;
  return out;
}

std::ostream& operator << (std::ostream& out, SALT_JOB_RET& jobret) {
  out << "JOB RET: "
      << jobret.ple_id        << ", "
      << jobret.rettype       << ", "
      << jobret.tag           << ", "
      << jobret.stamp         << ", "
      << jobret.stamp_sec     << ", "
      << jobret.stamp_usec    << ", "
      << jobret.pid           << ", "
      << jobret.retcode       << ", "
      << jobret.stderr        << ", "
      << jobret.stdout        << ", "
      << jobret.success       << ", "
      << jobret.cmd           << ", "
      << jobret.jid           << ", "
      << jobret.fun           << ", "
      << jobret.minion_id     << std::endl;
  return out;
}

void show_job(SALT_JOB* job) {
  if (_json_error_) std::cout << "json_error: = " << _json_error_ << std::endl;
  switch(job->type) {
  case SALT_JOB_TYPE_NEW:
    std::cout << *(SALT_JOB_NEW*)(job->ptr);
    break;
  case SALT_JOB_TYPE_RET:
    std::cout << *(SALT_JOB_RET*)(job->ptr);
    break;
  default:
    break;
  }
}
