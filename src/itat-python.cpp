#include <boost/python.hpp>
#include "pipeline.hpp"
#include "mario.hpp"

using namespace itat;


// boost::python::tuple
// mario_pickle::getinitargs(Mario const& m)
// {
//    return boost::python::make_tuple();
// }
//
// boost::python::tuple
// mario_pickle::getstate(Mario const& m)
// {
//    return boost::python::make_tuple();
// }
//
// void
// mario_pickle::setstate(Mario& m, boost::python::tuple state)
// {
// }

char const* greet()
{
   std::cout << "greetings\n";
   return "hello, world";
}

int64_t new_mario(int plid) {
    return (uint64_t)(void*)(new Mario(plid));
}

int get_back_mario(int64_t id) {
    if (id) {
      Mario* m = (Mario*)id;
      return m->get_plid();
    }
    return -1;
}

void kill_mario(int64_t id) {
    if (id) {
      Mario* m = (Mario*)id;
      delete m;
    }
}

int initial(int64_t id, int real_run, const char* bill_message, int node_num, int branch_num) {
    if (id) {
      Mario* m = (Mario*)id;
      return m->initial(real_run, bill_message, node_num, branch_num);
    }

    return -1;
}

int run_mario(int64_t id, int start_id, int pleid) {
    if (id) {
      Mario* m = (Mario*)id;
      return m->run(start_id, pleid);
    }

    return -1;
}


int stop_mario(int64_t id) {
    if (id) {
      Mario* m = (Mario*)id;
      return m->stop();
    }

    return -1;
}


int check_mario(int64_t id) {
    if (id) {
        Mario* m = (Mario*)id;
        return m->check();
    }
    return -1;
}


int run_node(int64_t id, int node_id) {
    if (id) {
        Mario* m = (Mario*)id;
        return m->run_node(node_id);
    }

    return -1;
}

int pause_mario(int64_t id) {
    if (id) {
        Mario* m = (Mario*)id;
        return m->pause();
    }
    return -1;
}

int go_on(int64_t id) {
    if (id) {
        Mario* m = (Mario*)id;
        return m->go_on();
    }
    return -1;
}

int confirm(int64_t id, int node_id) {
    if (id) {
        Mario* m = (Mario*)id;
        return m->confirm(node_id);
    }
    return -1;
}

void set_user(int64_t id, int userid) {
    if (id) {
        Mario* m = (Mario*)id;
        return m->set_user(userid);
    }
}

int mario_is_done(int64_t id) {
    if (id) {
        Mario* m = (Mario*)id;
        return m->is_done();
    }
    return 1;
}

int test_setup(int64_t id,
               int check,
               int run,
               int check_err_id,
               int run_err_id,
               int timeout_id,
               int pause_id,
               int stop_id,
               int confirm_id,
               int sleep_interval) {
    if (id) {
        Mario* m = (Mario*)id;
        m->test_setup(check, run, check_err_id, run_err_id, timeout_id, pause_id, stop_id, confirm_id, sleep_interval);
        return 0;
    }

    return -1;
}

BOOST_PYTHON_MODULE(libitat)
{
    using namespace boost::python;
    class_<Mario>
      ("Mario", init<int>())
      .def("initial", &Mario::initial)
      .def("test_setup", &Mario::test_setup)
      .def("check", &Mario::check)
      .def("run", &Mario::run)
      .def("run_node", &Mario::run_node)
      .def("pause", &Mario::pause)
      .def("go_on", &Mario::go_on)
      .def("stop", &Mario::stop)
      .def("confirm", &Mario::confirm)
      .def("test_int", &Mario::test_int)
      // .def_pickle(mario_pickle())
      ;


    def("greet", greet);
    def("new_mario", new_mario);
    def("get_back_mario", get_back_mario);
    def("kill_mario", kill_mario);
    def("initial", initial);
    def("test_setup", test_setup);
    def("run_mario", run_mario);
    def("stop_mario", stop_mario);
    def("run_node", run_node);
    def("pause_mario", pause_mario);
    def("go_on", go_on);
    def("confirm", confirm);
    def("set_user", set_user);
    def("mario_is_done", mario_is_done);
    def("check_mario", check_mario);
}
