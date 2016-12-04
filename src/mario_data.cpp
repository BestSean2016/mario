#include "mario_data.h"
#include "mario_mysql.h"


std::ostream& operator<<(std::ostream& out, MR_HOST& host) {
  out << "MR_HOST : "
      << host.id << ", "
      << host.host << ", "
      << host.name << ", "
      << host.ip << ", "
      << host.minion_id << std::endl;
  return out;
}


std::ostream& operator<<(std::ostream& out, MR_PIPELINE& pl) {
  out << "MR_PIPELINE : "
      << pl.id << ", "
      << pl.pl_name << ", "
      << pl.pl_oldid << ", "
      << pl.pl_create_tm << ", "
      << pl.pl_modify_tm << ", "
      << pl.pl_desc << ", "
      << pl.pl_schedule << std::endl;
  return out;
}

std::ostream& operator<<(std::ostream& out, MR_SCRIPT& script) {
    out << "MR_SCRIPT : "
        << script.id << ", "
        << script.scpt_name << ", "
        << script.host_id << ", "
        << script.script << ", "
        << script.scpt_type << ", "
        << script.scpt_timeout << ", "
        << script.desc << ", "
        << script.create_tm << ", "
        << script.modify_tm << ", "
        << std::endl;
    return out;
}

std::ostream& operator <<(std::ostream& out, MR_PIPELINE_EDGE edge) {
  out << "MR_PIPELINE_EDGE: "
      << edge.id      << ", "
      << edge.pl_id        << ", "
      << edge.src_id       << ", "
      << edge.trg_id       << ", "
      << edge.src_type     << ", "
      << edge.trg_type     << ", "
      << edge.src_min_id   << ", "
      << edge.trg_min_id  << ", "
      << edge.src_timeout  << ", "
      << edge.trg_timeout  << ", "
      << edge.err_prss_type<< ", "
      << edge.to_prss_type << ", "
      << edge.src_oldid    << ", "
      << edge.trg_oldid    << ", "
      << edge.edge_desc    << ", "
      << edge.create_tm    << ", "
      << edge.modify_tm    << std::endl;

  return out;
}


std::ostream& operator <<(std::ostream& out, MR_PIPELINE_EXEC& ple) {
  out << "MR_PIPELINE_EXEC: "
      << ple.id         << ", "
      << ple.pl_id         << ", "
      << ple.pe_stm        << ", "
      << ple.pe_etm        << ", "
      << ple.pe_desc       << ", "
      << ple.pe_ret_code   << ", "
      << ple.pe_ret_state  << ", "
      << ple.pe_type       << std::endl;
  return out;
}


std::ostream& operator <<(std::ostream& out, MR_PIPELINE_NODE_EXEC& plen) {\
  out << "MR_PIPELINE_NODE_EXEC: "
      << plen.id      << ", "
      << plen.pe_id        << ", "
      << plen.edge_id      << ", "
      << plen.salt_jid     << ", "
      << plen.pid          << ", "
      << plen.ret_code     << ", "
      << plen.pln_stm      << ", "
      << plen.pln_etm      << ", "
      << plen.ret_stderr   << ", "
      << plen.ret_stdout   << ", "
      << plen.minion_id    << ", "
      << plen.arg          << ", "
      << plen.fun          << std::endl;
  return out;
}


std::ostream& operator <<(std::ostream& out, MR_HOST_STATUS& hs) {\
  out << "MR_HOST_STATUS: "
      << hs.id  << ", "
      << hs.pe_id       << ", "
      << hs.host_id     << ", "
      << hs.host_status << ", "
      << hs.updatetime  << std::endl;

  return out;
}

// void free_script_set(struct DataSet<MR_SCRIPT>& set) {
//   if (set.data) {
//     for (size_t i = 0; i < set.size; ++i)
//       if (set.data[i].script) delete [] set.data[i].script;
//   }
//   set.free_data_set();
// }

void set_host_status_map(std::vector<MR_HOST_STATUS*>& status,
                                MapId2Ptr& mapPle,
                                MapId2Ptr& mapHost) {
  for(auto& p : status)
    p->pe = (MR_PIPELINE_EXEC*)(mapPle[p->pe_id]),
            p->host = (MR_HOST*)(mapHost[p->host_id]);
}

void set_plne_map(std::vector<MR_PIPELINE_NODE_EXEC*>& array,
                  MapId2Ptr& mapPle, MapId2Ptr& mapEdge, MapStr2Ptr& mapHost) {
  for(auto&p : array)
    p->ple = (MR_PIPELINE_EXEC*)(mapPle[p->pe_id]),
            p->edge = (MR_PIPELINE_EDGE*)(mapEdge[p->edge_id]),
            p->host = (MR_HOST*)mapHost[p->minion_id];
}


static const char* strstatus[] = {
  "did not run",
  "running ...",
  "pending ...",
  "part successed",
  "failed",
  "successed",
  "timerout and failed",
  "timerout and part successed",
};

const char* job_status(JOB_STATUS_TYPE status) {
  if (status < 0 || status > JOB_STATUS_TYPE_TIMEOUT_2)
    return 0;
  return strstatus[status];
}
