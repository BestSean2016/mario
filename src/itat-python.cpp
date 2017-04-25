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

uint64_t new_mario(int plid) {
    return (uint64_t)(void*)(new Mario(plid));
}

int get_back_mario(uint64_t id) {
    if (id) {
      Mario* m = (Mario*)id;
      return m->get_plid();
    }
    return -1;
}

void kill_mario(uint64_t id) {
    if (id) {
      Mario* m = (Mario*)id;
      delete m;
    }
}

int initial(uint64_t id, int real_run, const char* bill_message) {
    if (id) {
      Mario* m = (Mario*)id;
      return m->initial(real_run, bill_message);
    }

    return -1;
}

int run_mario(uint64_t id, int start_id) {
    if (id) {
      Mario* m = (Mario*)id;
      return m->run(start_id);
    }

    return -1;
}


int stop_mario(uint64_t id) {
    if (id) {
      Mario* m = (Mario*)id;
      return m->stop();
    }

    return -1;
}

int test_setup(uint64_t id,
               int node_num,
               int branch_num,
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
        m->test_setup(node_num, branch_num, check, run, check_err_id, run_err_id, timeout_id, pause_id, stop_id, confirm_id, sleep_interval);
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
}

