#include <boost/python.hpp>
#include "pipeline.hpp"
#include "mario.hpp"

#ifdef ERROR_INVAILD_MARIO_INSTANCE
#undef ERROR_INVAILD_MARIO_INSTANCE
#endif // ERROR_INVAILD_MARIO_INSTANCE
#define ERROR_INVAILD_MARIO_INSTANCE -10004

using namespace itat;

std::mutex g_mario_mutex;
std::set<Mario *> mario_set;

extern void set_django_ip_port(const char *ip, int port);

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

static void insert_mario(Mario *m) {
  auto guard = new std::lock_guard<std::mutex>(g_mario_mutex);
  mario_set.insert(m);
  delete guard;
}

static bool remove_mario(Mario *m) {
  auto guard = new std::lock_guard<std::mutex>(g_mario_mutex);
  auto iter = mario_set.find(m);
  bool found = false;
  if (iter != mario_set.end()) {
    mario_set.erase(iter);
    found = true;
  }
  delete guard;
  return found;
}

int get_back_mario(int id) {
  Mario *m = (Mario *)id;

  int ret = 0;
  auto guard = new std::lock_guard<std::mutex>(g_mario_mutex);
  if (mario_set.find(m) != mario_set.end())
    ret = m->get_plid();
  else
    ret = ERROR_INVAILD_MARIO_INSTANCE;
  delete guard;
  return ret;
}

char const *greet() {
  std::cout << "greetings\n";
  return "hello, world";
}

int64_t new_mario(int plid) {
  Mario *m = new Mario(plid);
  insert_mario(m);
  return (uint64_t)(void *)(m);
}

int kill_mario(int64_t id) {
  Mario *m = (Mario *)id;
  if (remove_mario(m)) {
    delete m;
    return 0;
  } else
    return ERROR_INVAILD_MARIO_INSTANCE;
}

int initial(int64_t id, int real_run, const char *bill_message, int node_num,
            int branch_num) {
  Mario *m = (Mario *)id;
  int ret = 0;
  auto guard = new std::lock_guard<std::mutex>(g_mario_mutex);
  if (mario_set.find(m) != mario_set.end())
    ret = m->initial(real_run, bill_message, node_num, branch_num);
  else
    ret = ERROR_INVAILD_MARIO_INSTANCE;
  delete guard;
  return ret;
}

int run_mario(int64_t id, int start_id, int pleid) {
  Mario *m = (Mario *)id;

  int ret = 0;
  auto guard = new std::lock_guard<std::mutex>(g_mario_mutex);
  if (mario_set.find(m) != mario_set.end())
    ret = m->run(start_id, pleid);
  else
    ret = ERROR_INVAILD_MARIO_INSTANCE;

  delete guard;
  return ret;
}

int stop_mario(int64_t id, int code, const char *why) {
  Mario *m = (Mario *)id;
  int ret = 0;
  auto guard = new std::lock_guard<std::mutex>(g_mario_mutex);
  if (mario_set.find(m) != mario_set.end())
    ret = m->stop(code, why);
  else
    ret = ERROR_INVAILD_MARIO_INSTANCE;
  delete guard;
  return ret;
}

int check_mario(int64_t id) {
  Mario *m = (Mario *)id;
  int ret = 0;
  auto guard = new std::lock_guard<std::mutex>(g_mario_mutex);
  if (mario_set.find(m) != mario_set.end())
    ret = m->check();
  else
    ret = ERROR_INVAILD_MARIO_INSTANCE;

  delete guard;
  return ret;
}

int run_node(int64_t id, int node_id) {
#ifdef _DEBUG_
    cout << "run node " << id << ", " << node_id << endl;
#endif //_DEBUG_

  Mario *m = (Mario *)id;
  int ret = 0;
  auto guard = new std::lock_guard<std::mutex>(g_mario_mutex);
  if (mario_set.find(m) != mario_set.end())
    ret = m->run_node(node_id);
  else {
#ifdef _DEBUG_
    cout << "run node ERROR_INVAILD_MARIO_INSTANCE\n";
    ret = ERROR_INVAILD_MARIO_INSTANCE;
#endif //_DEBUG_
  }
  delete guard;
  return ret;
}

int pause_mario(int64_t id) {
  Mario *m = (Mario *)id;
  int ret = 0;
  auto guard = new std::lock_guard<std::mutex>(g_mario_mutex);
  if (mario_set.find(m) != mario_set.end())
    ret = m->pause();
  else
    ret = ERROR_INVAILD_MARIO_INSTANCE;
  delete guard;
  return ret;
}

int go_on(int64_t id) {
  Mario *m = (Mario *)id;
  int ret = 0;
  auto guard = new std::lock_guard<std::mutex>(g_mario_mutex);
  if (mario_set.find(m) != mario_set.end())
    ret = m->go_on();
  else
    ret = ERROR_INVAILD_MARIO_INSTANCE;
  delete guard;
  return ret;
}

int confirm(int64_t id, int node_id) {
  Mario *m = (Mario *)id;
  int ret = 0;
  auto guard = new std::lock_guard<std::mutex>(g_mario_mutex);
  if (mario_set.find(m) != mario_set.end())
    ret = m->confirm(node_id);
  else
    ret = ERROR_INVAILD_MARIO_INSTANCE;
  delete guard;
  return ret;
}

int set_user(int64_t id, int userid) {
  Mario *m = (Mario *)id;
  int ret = 0;
  auto guard = new std::lock_guard<std::mutex>(g_mario_mutex);
  if (mario_set.find(m) != mario_set.end())
    m->set_user(userid);
  else
    ret = ERROR_INVAILD_MARIO_INSTANCE;

  delete guard;
  return ret;
}

int mario_is_done(int64_t id) {
  Mario *m = (Mario *)id;
  int ret = 0;
  auto guard = new std::lock_guard<std::mutex>(g_mario_mutex);
  if (mario_set.find(m) != mario_set.end())
    ret = m->is_done();
  else
    ret = ERROR_INVAILD_MARIO_INSTANCE;
  delete guard;
  return ret;
}

int test_setup(int64_t id, int check, int run, int check_err_id, int run_err_id,
               int timeout_id, int pause_id, int stop_id, int confirm_id,
               int sleep_interval) {
  if (id) {
    Mario *m = (Mario *)id;
    auto guard = new std::lock_guard<std::mutex>(g_mario_mutex);
    if (mario_set.find(m) != mario_set.end())
      m->test_setup(check, run, check_err_id, run_err_id, timeout_id, pause_id,
                    stop_id, confirm_id, sleep_interval);
    delete guard;
  }

  return -1;
}

BOOST_PYTHON_MODULE(libitat) {
  using namespace boost::python;
  class_<Mario>("Mario", init<int>())
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
  def("set_django_ip_port", set_django_ip_port);
  def("get_back_mario", get_back_mario);
}
