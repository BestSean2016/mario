import libitat
import traceback



# class_<Mario>
#   ("Mario", init<int>())
#   .def("initial", &Mario::initial)
#   .def("test_setup", &Mario::test_setup)
#   .def("check", &Mario::check)
#   .def("run", &Mario::run)
#   .def("run_node", &Mario::run_node)
#   .def("pause", &Mario::pause)
#   .def("continue", &Mario::go_on)
#   .def("stop", &Mario::stop)
#   .def("confirm", &Mario::confirm)
#   .def("test_int", &Mario::test_int)
#   ;
# def("greet", greet);


mario = libitat.Mario(0)
mario.initial(0, "bill_message")

def thread_task():
  global mario
  STATE_TYPE_OK = 1
  mario.test_setup(10, 2, STATE_TYPE_OK, STATE_TYPE_OK, -1, -1, -1, -1, -1, -1, 1000)
  mario.run(0)


thread_task()

