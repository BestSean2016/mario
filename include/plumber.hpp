#ifndef PLUMBER_H
#define PLUMBER_H


#include "zmq.hpp"
#include "zmq_addon.hpp"

#include <mutex>

extern int plumber();
extern std::mutex g_job_mutex;
extern int g_run;

#endif // PLUMBER_H
