#ifndef PIPELINE_H
#define PIPELINE_H


extern int gen_diamond_pipeline(int node_num, int branch_num);
extern void release_pipeline();
extern int run_pipeline(int *run);

#endif // PIPELINE_H
