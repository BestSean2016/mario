#include "node.hpp"
#include "graph.hpp"

#include <memory.h>
#include <vector>

namespace itat {

igraph::igraph() {
    memset(&ig_, 0, sizeof(igraph_t));
}

igraph::~igraph() {
    jid_2_node_.clear();

    for (auto& n : node_) {
      delete n;
    }
    node_.clear();
  if (ig_.n > 0) igraph_destroy(&ig_);
}

/**
 * @brief diamod_simulator generate a graph
 * @param n number of nodes
 * @param b number of branches
 * @return 0 for good
 */
int igraph::diamod_simulator(int node_num, int branch_num) {
    gen_diamod_graph_(node_num, branch_num);
    gen_node_();

  return (0);
}

int igraph::gen_diamod_graph_(int node_num, int branch_num) {
    igraph_t gIn, gOut;
    igraph_vector_t edge;
    std::vector<int> e;
    int half = node_num / 2;

    //generate 2 tree type graphs with in and out style
    igraph_tree(&gIn, half, branch_num, IGRAPH_TREE_IN);
    igraph_tree(&gOut, half, branch_num, IGRAPH_TREE_OUT);

    //get the out style tree's all edges, the put them to vector e
    igraph_vector_init(&edge, 0);
    igraph_get_edgelist(&gOut, &edge, false);
    for (int64_t i = 0; i < igraph_vector_size(&edge); ++i)
      e.push_back(VECTOR(edge)[i]);

    {//connect out-tree to in-tree
      igraph_vector_t neinode;
      igraph_vector_init(&neinode, 0);
      for (int i = 0; i < half; ++i) {
        igraph_neighbors(&gOut, &neinode, i, IGRAPH_OUT);
        //if this node have no neighbors, then connect to the next part of graph
        if (0 == igraph_vector_size(&neinode)) {
          e.push_back(i);
          e.push_back(i + ((half - i) - 1) * 2 + 1);
        }
      }
      igraph_vector_destroy(&neinode);
    }

    //get all edges of in style graph
    igraph_get_edgelist(&gIn, &edge, false);
    for (int64_t i = 0; i < igraph_vector_size(&edge); ++i) {
      int vid = VECTOR(edge)[i];
      e.push_back(vid + ((half - vid) - 1) * 2 + 1);
    }

    igraph_vector_destroy(&edge);
    igraph_destroy(&gOut);
    igraph_destroy(&gIn);

    //
    // generate new graph with diamod style
    //
    // get all edges from vector e
    igraph_vector_init(&edge, e.size());
    for (int i = 0; i < (int)e.size(); ++i)
      VECTOR(edge)[i] = e[i];

    igraph_i_set_attribute_table(&igraph_cattribute_table);
    igraph_create(&ig_, &edge, half * 2, 1);
    igraph_vector_destroy(&edge);

    return (0);
}

int igraph::gen_node_() {
  for (int i = 0; i <ig_.n; ++i) {
    auto node = new inode(this);
    node->gen_pl_node(i);
    node_.emplace_back(node);
  }
  return (0);
}


int igraph::gen_piple_graph() {
  int ret = load_pipe_line_from_db_();
  if (!ret)
    ret = gen_piple_graph_();
  if (!ret)
    ret = gen_node_();

  return ret;
}

int igraph::load_pipe_line_from_db_() {
  //to do: load piple line from db by piplline id
  return 0;
}

int igraph::gen_piple_graph_() {
  return 0;
}


inode* igraph::get_node_by_jid(std::string& jid) {
    auto iter = jid_2_node_.find(jid);
    return (iter == jid_2_node_.end()) ? nullptr : iter->second;
}

} //namespace itat


