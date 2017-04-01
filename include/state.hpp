#ifndef STATE_HPP
#define STATE_HPP

#include <iostream>
#include <typeinfo>

// back-end
#include <boost/msm/back/state_machine.hpp>
// front-end
#include <boost/msm/front/state_machine_def.hpp>

namespace msm = boost::msm;
namespace mpl = boost::mpl;

namespace itat {
class dfnode;
class dfgraph;

static const char* node_state_name[] = {
  "initial          ",
  "checking         ",
  "checked_err      ",
  "checked_ok       ",
  "waiting_for_run  ",
  "running          ",
  "error            ",
  "timeout          ",
  "successed        ",
  "waiting_for_input",
  "stoped           ",
  "paused           ",
};

static const char* graph_state_name[] = {
  "initial          ",
  "checking         ",
  "checked_err      ",
  "checked_ok       ",
  "waiting_for_run  ",
  "running          ",
  "error            ",
  "timeout          ",
  "successed        ",
  "waiting_for_input",
  "stoped           ",
  "paused           ",
};


typedef enum CHECK_RESULT {
  CHECK_RESULT_UNKNOW = -1,
  CHECK_RESULT_OK,
  CHECK_RESULT_ERR,
} CHECK_RESULT;

typedef enum NODE_RUN_RESULT {
  NODE_RUN_RESULT_UNKNOW = -1,
  NODE_RUN_RESULT_SUCCEEDED,
  NODE_RUN_RESULT_ERROR,
  NODE_RUN_RESULT_TIMEOUT,
} NODE_RUN_RESULT;


struct event {
    event() {}
    event(dfgraph* g, dfnode* node) : g_(g), n_(node) {}
    dfnode* n_ = nullptr;
    dfgraph* g_ = nullptr;
};

class dfnode_state_machine {
public:
  dfnode_state_machine() {}
  dfnode_state_machine(dfgraph* graph, dfnode* node) : g_(graph), n_(node) {}
  ~dfnode_state_machine() {}

  // events
  struct event_run : public event {
      event_run(dfgraph*g, dfnode* n) : event(g, n) { }
  };
  struct event_check : public event {
      event_check(dfgraph*g, dfnode* n) : event(g, n) { }
  };
  struct event_stop : public event {
      event_stop(dfgraph*g, dfnode* n) : event(g, n) { }
  };
  struct event_pause : public event {
      event_pause(dfgraph*g, dfnode* n) : event(g, n) { }
  };
  struct event_continue : public event {
      event_continue(dfgraph*g, dfnode* n) : event(g, n) { }
  };
  struct event_check_error : public event {
      event_check_error(dfgraph*g, dfnode* n) : event(g, n) { }
  };
  struct event_check_successe : public event {
      event_check_successe(dfgraph*g, dfnode* n) : event(g, n) { }
  };

  CHECK_RESULT check_result;
  NODE_RUN_RESULT node_run_result;

  // A "complicated" event type that carries some data.
  // struct cd_detected
  // {
  //     cd_detected(std::string name)
  //         : name(name)
  //     {}
  //
  //     std::string name;
  // };

  // front-end: define the FSM structure
  struct nodesm_ : public msm::front::state_machine_def<nodesm_> {
    // The list of FSM states
    // the initial state of the player SM. Must be defined
    struct initial : public msm::front::state<> {
      // every (optional) entry/exit methods get the event passed
      template <class Event, class FSM> void on_entry(Event const& event, FSM& fsm) {
        std::cout << "entering: initial" << std::endl;
      }
      template <class Event, class FSM> void on_exit(Event const& event, FSM& fsm) {
        std::cout << "leaving: initial" << std::endl;
      }
      template <class Event, class FSM> void on_init_entry_(Event const& event, FSM& fsm, dfnode* node);
    };
    struct checking : public msm::front::state<> {
      template <class Event, class FSM> void on_entry(Event const &, FSM &) {
        std::cout << "\tstarting: Cheking" << std::endl;
      }
      template <class Event, class FSM> void on_exit(Event const &, FSM &) {
        std::cout << "\tfinishing: Cheking" << std::endl;
      }
    };
    struct checked_ok : public msm::front::state<> {
      template <class Event, class FSM> void on_entry(Event const &, FSM &) {
        std::cout << "\tstarting: Checking_OK" << std::endl;
      }
      template <class Event, class FSM> void on_exit(Event const &, FSM &) {
        std::cout << "\tfinishing: Checking_OK" << std::endl;
      }
    };
    struct checked_err : public msm::front::state<> {
      template <class Event, class FSM> void on_entry(Event const &, FSM &) {
        std::cout << "\tstarting: Checking_Err" << std::endl;
      }
      template <class Event, class FSM> void on_exit(Event const &, FSM &) {
        std::cout << "\tfinishing: Checking_Err" << std::endl;
      }
    };
    struct waiting_for_run : public msm::front::state<> {
      template <class Event, class FSM> void on_entry(Event const &, FSM &) {
        std::cout << "entering: waiting_for_run" << std::endl;
      }
      template <class Event, class FSM> void on_exit(Event const &, FSM &) {
        std::cout << "leaving: waiting_for_run" << std::endl;
      }
    };
    struct running : public msm::front::state<> {
      template <class Event, class FSM> void on_entry(Event const &, FSM &) {
        std::cout << "entering: running" << std::endl;
      }
      template <class Event, class FSM> void on_exit(Event const &, FSM &) {
        std::cout << "leaving: running" << std::endl;
      }
    };
    struct error : public msm::front::state<> {
      template <class Event, class FSM> void on_entry(Event const &, FSM &) {
        std::cout << "entering: error" << std::endl;
      }
      template <class Event, class FSM> void on_exit(Event const &, FSM &) {
        std::cout << "leaving: error" << std::endl;
      }
    };
    struct timeout : public msm::front::state<> {
      template <class Event, class FSM> void on_entry(Event const &, FSM &) {
        std::cout << "entering: timeout" << std::endl;
      }
      template <class Event, class FSM> void on_exit(Event const &, FSM &) {
        std::cout << "leaving: timeout" << std::endl;
      }
    };
    struct successed : public msm::front::state<> {
      template <class Event, class FSM> void on_entry(Event const &, FSM &) {
        std::cout << "entering: successed" << std::endl;
      }
      template <class Event, class FSM> void on_exit(Event const &, FSM &) {
        std::cout << "leaving: successed" << std::endl;
      }
    };
    struct waiting_for_input : public msm::front::state<> {
      template <class Event, class FSM> void on_entry(Event const &, FSM &) {
        std::cout << "entering: waiting_for_input" << std::endl;
      }
      template <class Event, class FSM> void on_exit(Event const &, FSM &) {
        std::cout << "leaving: waiting_for_input" << std::endl;
      }
    };
    struct stoped : public msm::front::state<> {
      template <class Event, class FSM> void on_entry(Event const &, FSM &) {
        std::cout << "entering: stoped" << std::endl;
      }
      template <class Event, class FSM> void on_exit(Event const &, FSM &) {
        std::cout << "leaving: stoped" << std::endl;
      }
    };
    struct paused : public msm::front::state<> {
      template <class Event, class FSM> void on_entry(Event const &, FSM &) {
        std::cout << "entering: paused" << std::endl;
      }
      template <class Event, class FSM> void on_exit(Event const &, FSM &) {
        std::cout << "leaving: paused" << std::endl;
      }
    };



    typedef initial initial_state;
    // guard conditions

    // transition actions
    void start_job(event_run const &) { std::cout << "node_sm::start_job\n"; }
    void check_node(event_check const &) { std::cout << "node_sm::check_node status\n"; }
    void check_error_occured(event_check_error const &) { std::cout << "node_sm::check::error occured\n"; }
    void check_successed(event_check_successe const &) { std::cout << "node_sm::check::Succeeded!\n"; }
    void recheck(event_check const &) { std::cout << "node_sm::check::Recheck!\n"; }

    // guard conditions

    // Transition table for player
    struct transition_table
        : mpl::vector<
              //      Start       Event                 Next          Action                Guard
              //    +-----------+---------------------+-------------+---------------------+----------------------+
              a_row<initial,     event_check,           checking,    &nodesm_::check_node                   >,
              a_row<checking,    event_check_error,     checked_err, &nodesm_::check_error_occured          >,
              a_row<checking,    event_check_successe,  checked_ok,  &nodesm_::check_successed              >,
              a_row<checked_err, event_check,           checking,    &nodesm_::recheck                      >,
              a_row<checked_ok,  event_check,           checking,    &nodesm_::recheck                      >,
              //    +-----------+----------------------+-------------+-------------------------+--------+
              a_row<checking,  event_run,      running,   &nodesm_::start_job>
              //    +-----------+- ------------+---------+---------------------+----------------------+
              > {};

    // Replaces the default no-transition response.
    template <class FSM, class Event>
    void no_transition(Event const &e, FSM &, int state) {
      std::cout << "NodeSM no transition from state " << state << " on event "
                << typeid(e).name() << std::endl;
    }
  };
  // Pick a back-end
  typedef msm::back::state_machine<nodesm_> nodesm;

  //
  // Testing utilities.
  //
  void pstate(nodesm const &p) {
    std::cout << " -> " << node_state_name[p.current_state()[0]] << std::endl;
  }


  void test();
private:
  dfgraph* g_ = nullptr;
  dfnode* n_ = nullptr;
};



class dfgraph_state_machine {
public:
    dfgraph_state_machine() {}
    dfgraph_state_machine(dfgraph* g) : g_(g) {}
    virtual ~dfgraph_state_machine() {}

    void set_graph(dfgraph* g) { g_= g; }

    // events
    struct event_run {};
    struct event_check {};
    struct event_stop {};
    struct event_pause {};
    struct event_continue {};
    struct event_check_error {};
    struct event_check_successe {};

    CHECK_RESULT check_result;
    NODE_RUN_RESULT run_result;

    // front-end: define the FSM structure
    struct graph_sm_ : public msm::front::state_machine_def<graph_sm_> {
      // The list of FSM states
      // the initial state of the player SM. Must be defined
      struct initial : public msm::front::state<> {
        // every (optional) entry/exit methods get the event passed
        template <class Event, class FSM> void on_entry(Event const &, FSM &) {
          std::cout << "entering: initial" << std::endl;
        }
        template <class Event, class FSM> void on_exit(Event const &, FSM &) {
          std::cout << "leaving: initial" << std::endl;
        }
      };
      struct checking : public msm::front::state<> {
        template <class Event, class FSM> void on_entry(Event const &, FSM &) {
          std::cout << "\tstarting: Cheking" << std::endl;
        }
        template <class Event, class FSM> void on_exit(Event const &, FSM &) {
          std::cout << "\tfinishing: Cheking" << std::endl;
        }
      };
      struct checked_ok : public msm::front::state<> {
        template <class Event, class FSM> void on_entry(Event const &, FSM &) {
          std::cout << "\tstarting: Checking_OK" << std::endl;
        }
        template <class Event, class FSM> void on_exit(Event const &, FSM &) {
          std::cout << "\tfinishing: Checking_OK" << std::endl;
        }
      };
      struct checked_err : public msm::front::state<> {
        template <class Event, class FSM> void on_entry(Event const &, FSM &) {
          std::cout << "\tstarting: Checking_Err" << std::endl;
        }
        template <class Event, class FSM> void on_exit(Event const &, FSM &) {
          std::cout << "\tfinishing: Checking_Err" << std::endl;
        }
      };
      struct waiting_for_run : public msm::front::state<> {
        template <class Event, class FSM> void on_entry(Event const &, FSM &) {
          std::cout << "entering: waiting_for_run" << std::endl;
        }
        template <class Event, class FSM> void on_exit(Event const &, FSM &) {
          std::cout << "leaving: waiting_for_run" << std::endl;
        }
      };
      struct running : public msm::front::state<> {
        template <class Event, class FSM> void on_entry(Event const &, FSM &) {
          std::cout << "entering: running" << std::endl;
        }
        template <class Event, class FSM> void on_exit(Event const &, FSM &) {
          std::cout << "leaving: running" << std::endl;
        }
      };
      struct error : public msm::front::state<> {
        template <class Event, class FSM> void on_entry(Event const &, FSM &) {
          std::cout << "entering: error" << std::endl;
        }
        template <class Event, class FSM> void on_exit(Event const &, FSM &) {
          std::cout << "leaving: error" << std::endl;
        }
      };
      struct timeout : public msm::front::state<> {
        template <class Event, class FSM> void on_entry(Event const &, FSM &) {
          std::cout << "entering: timeout" << std::endl;
        }
        template <class Event, class FSM> void on_exit(Event const &, FSM &) {
          std::cout << "leaving: timeout" << std::endl;
        }
      };
      struct successed : public msm::front::state<> {
        template <class Event, class FSM> void on_entry(Event const &, FSM &) {
          std::cout << "entering: successed" << std::endl;
        }
        template <class Event, class FSM> void on_exit(Event const &, FSM &) {
          std::cout << "leaving: successed" << std::endl;
        }
      };
      struct waiting_for_input : public msm::front::state<> {
        template <class Event, class FSM> void on_entry(Event const &, FSM &) {
          std::cout << "entering: waiting_for_input" << std::endl;
        }
        template <class Event, class FSM> void on_exit(Event const &, FSM &) {
          std::cout << "leaving: waiting_for_input" << std::endl;
        }
      };
      struct stoped : public msm::front::state<> {
        template <class Event, class FSM> void on_entry(Event const &, FSM &) {
          std::cout << "entering: stoped" << std::endl;
        }
        template <class Event, class FSM> void on_exit(Event const &, FSM &) {
          std::cout << "leaving: stoped" << std::endl;
        }
      };
      struct paused : public msm::front::state<> {
        template <class Event, class FSM> void on_entry(Event const &, FSM &) {
          std::cout << "entering: paused" << std::endl;
        }
        template <class Event, class FSM> void on_exit(Event const &, FSM &) {
          std::cout << "leaving: paused" << std::endl;
        }
      };



      typedef initial initial_state;
      // guard conditions

      // transition actions
      void start_job(event_run const &) { std::cout << "node_sm::start_job\n"; }
      void check_node(event_check const &) { std::cout << "node_sm::check_node status\n"; }
      void check_error_occured(event_check_error const &) { std::cout << "node_sm::check::error occured\n"; }
      void check_successed(event_check_successe const &) { std::cout << "node_sm::check::Succeeded!\n"; }
      void recheck(event_check const &) { std::cout << "node_sm::check::Recheck!\n"; }

      // guard conditions

      // Transition table for player
      struct transition_table
          : mpl::vector<
                //      Start       Event                 Next          Action                Guard
                //    +-----------+---------------------+-------------+---------------------+----------------------+
                a_row<initial,     event_check,           checking,    &graph_sm_::check_node                   >,
                a_row<checking,    event_check_error,     checked_err, &graph_sm_::check_error_occured          >,
                a_row<checking,    event_check_successe,  checked_ok,  &graph_sm_::check_successed              >,
                a_row<checked_err, event_check,           checking,    &graph_sm_::recheck                      >,
                a_row<checked_ok,  event_check,           checking,    &graph_sm_::recheck                      >,
                //    +-----------+----------------------+-------------+-------------------------+--------+
                a_row<checking,  event_run,      running,   &graph_sm_::start_job>
                //    +-----------+- ------------+---------+---------------------+----------------------+
                > {};

      // Replaces the default no-transition response.
      template <class FSM, class Event>
      void no_transition(Event const &e, FSM &, int state) {
        std::cout << "NodeSM no transition from state " << state << " on event "
                  << typeid(e).name() << std::endl;
      }
    };
    // Pick a back-end
    typedef msm::back::state_machine<graph_sm_> graph_sm;

    //
    // Testing utilities.
    //
    void pstate(graph_sm const &p) {
      std::cout << " -> " << node_state_name[p.current_state()[0]] << std::endl;
    }

    void test() {
      graph_sm p;
      p.start();

      p.process_event(event_check());
      pstate(p);

      p.process_event(event_check_successe());
      pstate(p);

      p.process_event(event_check());
      pstate(p);

      p.process_event(event_check_error());
      pstate(p);

      p.process_event(event_check());
      pstate(p);

      p.process_event(event_check_successe());
      pstate(p);

    }

private:
    dfgraph* g_ = nullptr;
};

} //namespace itat

#endif // STATE_HPP
