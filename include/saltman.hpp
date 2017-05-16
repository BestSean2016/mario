#ifndef SALTMAN_HPP
#define SALTMAN_HPP

#define _USE_VECTOR_AS_SET_

#include <igraph/igraph.h>
#include "saltapi.hpp"
#include "state.hpp"

namespace itat {


typedef struct NODESET {
#ifdef _USE_VECTOR_AS_SET_
  std::vector<int> running_;
  std::vector<int> running_set_;
  std::vector<int> run_set_;
  std::vector<int> done_set_;
#else //#define _USE_VECTOR_AS_SET_
  std::set<int> running_;
  std::set<int> running_set_;
  std::set<int> run_set_;
  std::set<int> done_set_;
#endif //#define _USE_VECTOR_AS_SET_
}NODESET;


class Pipeline;
class iNode;

class saltman {
public:
    saltman();
    ~saltman();

    void init(Pipeline* pl);
    int start();
    void stop();

    void dump_jobmap();
    void check_time_out();

    STATE_TYPE check_node(iNode* node);
    STATE_TYPE run_node(iNode *node);

private:
    std::thread thread_event_;
    std::thread thread_timeout_;

    HTTP_API_PARAM param_check_;
    HTTP_API_PARAM param_run_;
    HTTP_API_PARAM param_event_;

    MAP_SALT_JOB map_job_;
    Pipeline* pl_ = nullptr;
    igraph_t* g_ = nullptr;

    NODESET* nodeset_ = nullptr;
};


extern bool vec_find(std::vector<int> &vec, int a);
extern void vec_insert(std::vector<int> &vec, int a);
extern void vec_erase(std::vector<int> &vec, int a);

}



#endif // SALTMAN_HPP
