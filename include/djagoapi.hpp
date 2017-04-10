#ifndef DJIAGO_API_HPP
#define DJIAGO_API_HPP

#include "itat.h"
#include "itat_global.h"
#include "state.hpp"

namespace itat {

int djagno_api_send_graph_status(int64_t graph_id, int64_t node_id, MARIO_STATE_TYPE run_state, MARIO_STATE_TYPE check_state,
                                 int code = 0, const std::string &stdout = "",
                                 const std::string &stderr = "");

} // namespace itat

#endif // DJIAGO_API_HPP
