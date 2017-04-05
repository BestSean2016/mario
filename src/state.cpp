#include "state.hpp"

namespace itat {


void dfNodeStateMachine::test() {
  nodesm p;
  p.start();

  p.process_event(event_check(g_, n_));
  pstate(p);

  p.process_event(event_check_successe(g_, n_));
  pstate(p);

  p.process_event(event_check(g_, n_));
  pstate(p);

  p.process_event(event_check_error(g_, n_));
  pstate(p);

  p.process_event(event_check(g_, n_));
  pstate(p);

  p.process_event(event_check_successe(g_, n_));
  pstate(p);

}



} //namespace itat
