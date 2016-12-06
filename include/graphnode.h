#ifndef GRAPHNODE_H
#define GRAPHNODE_H

#include <igraph.h>

typedef struct graphnode {
  size_t num_child;
  size_t num_father;
  int* children;
  int* father;
} GRAPH_NODE;

typedef struct SeanGraph {
  size_t   num_nodes;
  GRAPH_NODE** nodes;
  GRAPH_NODE* data;

  void init(size_t size) {
    num_nodes = size;
    //nodes = new nod
  }

  GRAPH_NODE* operator[] (size_t d) {
    return (nodes && d < num_nodes) ? nodes[d] : 0;
  }
}SeanGraph ;

extern int gen_sean_graph(SeanGraph* sg, igraph_t* g);
extern int destroy_sean_graph(SeanGraph* sg);

#endif // GRAPHNODE_H
