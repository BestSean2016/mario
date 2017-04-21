#include <boost/python.hpp>
#include "mario.hpp"


using namespace itat;


char const* greet()
{
   std::cout << "greetings\n";
   return "hello, world";
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
      ;


    def("greet", greet);
}

