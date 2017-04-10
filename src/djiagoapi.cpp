#include "djagoapi.hpp"

namespace itat {

int djagno_api_send_graph_status(int64_t graph_id, int64_t node_id,
                                 MARIO_STATE_TYPE run_state,
                                 MARIO_STATE_TYPE check_state, int code,
                                 const std::string &stdout,
                                 const std::string &stderr) {
  if (node_id == NO_NODE)
    cout << "GRAPH " << graph_id << ", Status: " << mario_state_name[run_state]
         << ", CheckStatus: " << mario_state_name[check_state] << ", code "
         << code << ", "
         << "stdout " << stdout << " , err " << stderr << endl;
  else
    cout << "GRAPH " << graph_id << ", NODE: " << node_id
         << ", Status: " << mario_state_name[run_state]
         << ", CheckStatus: " << mario_state_name[check_state] << ", code "
         << code << ", "
         << "stdout " << stdout << " , err " << stderr << endl;
  return 0;
}

} // namespace itat
