#ifndef PIPELINE_H
#define PIPELINE_H

#include "mario_data.h"

extern struct DataSet<MR_HOST> g_hosts;
extern struct DataSet<MR_PIPELINE> g_pls;
extern struct DataSet<MR_SCRIPT> g_scripts;
extern struct DataSet<MR_REAL_NODE> g_nodes;
extern struct DataSet<MR_REAL_EDGE> g_edges;

extern int gen_diamond_pipeline(int node_num, int branch_num);
extern void release_pipeline();
extern int run_pipeline(int *run);
extern int node_job_finished(SALT_JOB* job, std::vector<int>* should_run);
extern int run_something(std::vector<int>* vec);

#endif // PIPELINE_H
