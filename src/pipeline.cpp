#include "pipeline.h"
#include "http_client.h"
#include "mario_data.h"
#include "mario_mysql.h"
#include <igraph.h>
#include <thread>
#include <vector>
#include <set>

struct DataSet<MR_HOST> g_hosts;
struct DataSet<MR_PIPELINE> g_pls;
struct DataSet<MR_SCRIPT> g_scripts;
static igraph_t g_graph;
static DBHANDLE g_dbh = 0;
struct DataSet<MR_REAL_NODE> g_nodes;
struct DataSet<MR_REAL_EDGE> g_edges;


static std::set<int> end_node_set;
static int get_host_data() {
  if (0 >
      query_data(
          g_hosts, g_dbh, select_host_from_db, get_hosts,
          (const char *)"minion_id like 'old%' and ip not like '10.10.205%'"))
    return -1;

  return 0;
}

static int get_scripts_data() {
  if (0 > query_data(g_scripts, g_dbh, select_script_from_db, get_script,
                     (const char *)"host_id = 0"))
    return -1;

  return 0;
}

static int gen_tree(int node_num, int branch_num) {
  igraph_t gIn, gOut;
  igraph_vector_t edge;
  std::vector<int> e;
  int half = node_num / 2;

  igraph_vector_init(&edge, 0);

  igraph_tree(&gIn, half, branch_num, IGRAPH_TREE_IN);
  igraph_tree(&gOut, half, branch_num, IGRAPH_TREE_OUT);

  igraph_get_edgelist(&gOut, &edge, false);

  for (int64_t i = 0; i < igraph_vector_size(&edge); ++i)
    e.push_back(VECTOR(edge)[i]);

  {
    igraph_vector_t neinode;
    igraph_vector_init(&neinode, 0);
    for (int i = 0; i < half; ++i) {
      igraph_neighbors(&gOut, &neinode, i, IGRAPH_OUT);
      if (0 == igraph_vector_size(&neinode)) {
        e.push_back(i);
        e.push_back(i + ((half - i) - 1) * 2 + 1);
      }
    }
    igraph_vector_destroy(&neinode);
  }

  igraph_get_edgelist(&gIn, &edge, false);
  for (int64_t i = 0; i < igraph_vector_size(&edge); ++i) {
    int vid = VECTOR(edge)[i];
    e.push_back(vid + ((half - vid) - 1) * 2 + 1);
  }

  igraph_vector_destroy(&edge);

  igraph_vector_init(&edge, e.size());
  for (int i = 0; i < (int)e.size(); ++i)
    VECTOR(edge)[i] = e[i];

  igraph_destroy(&gOut);
  igraph_destroy(&gIn);

  igraph_i_set_attribute_table(&igraph_cattribute_table);
  igraph_create(&g_graph, &edge, half * 2, 1);
  igraph_vector_destroy(&edge);

  return (0);
}

static void set_start_and_stop(struct DataSet<MR_REAL_NODE> &nodes,
                               int64_t id_start) {
  nodes[0].id = id_start;
  nodes[0].ple_id = 2;
  nodes[0].script_id = 1;
  nodes[0].host_id = 0;
  nodes[0].timerout = 0;

  nodes[g_nodes.size - 1].id = g_nodes[0].id + g_nodes.size - 1;
  nodes[g_nodes.size - 1].ple_id = 2;
  nodes[g_nodes.size - 1].script_id = 2;
  nodes[g_nodes.size - 1].host_id = 0;
  nodes[g_nodes.size - 1].timerout = 0;
}

static int gen_nodes_edges() {
  igraph_vector_t edge;

  igraph_vector_init(&edge, 0);
  igraph_get_edgelist(&g_graph, &edge, false);

  g_nodes.init(igraph_vcount(&g_graph));
  g_edges.init(igraph_vector_size(&edge));

  int64_t id_start = 100;
  set_start_and_stop(g_nodes, id_start);

  for (size_t i = 1; i < g_nodes.size - 1; ++i) {
    g_nodes[i].id = id_start + i;
    g_nodes[i].ple_id = 2;
    g_nodes[i].script_id = 11;
    g_nodes[i].host_id = (i - 1) % g_hosts.size;
    g_nodes[i].timerout = 40;
  }

  for (size_t i = 0; i < g_edges.size; ++i) {
    // set edge to ...
  }

  igraph_vector_destroy(&edge);
  return (0);
}

int gen_diamond_pipeline(int node_num, int branch_num) {
  int ret = 0;
  ret = (nullptr == (g_dbh = connect_db("localhost", 3306, "mario", "mario",
                                        "chaojimali")));
  if (!ret)
    ret = (0 != get_host_data());
  if (!ret)
    ret = (0 != get_scripts_data());

  disconnect_db(g_dbh);

  if (!ret)
    ret = (0 != gen_tree(node_num, branch_num));

  if (!ret)
    ret = (0 != gen_nodes_edges());

  return ret;
}

void release_pipeline() {
  igraph_destroy(&g_graph);
  g_hosts.free_data_set();
  //free_script_set(g_scripts);
  g_scripts.free_data_set();
}

static int run_task(MR_REAL_NODE& node) {
  if ((size_t)node.host_id > g_hosts.size) {
    printf("node id %ld, host id %ld\n", node.id, node.host_id);
    exit(0);
  }
  return salt_api_cmd_runall((const char *)"10.10.10.19", 8000,
                               g_hosts[node.host_id].minion_id,
                               g_scripts[node.script_id].script,
                               node.ple_id,
                               node.id);
}

int run_pipeline(int *run) {
  std::vector<int> start_node;
  igraph_vector_t y;
  int64_t last_node_id = igraph_vcount(&g_graph) - 1;
  igraph_vector_init(&y, 0);
  for (int64_t i = 1; (*run) && (i < last_node_id); ++i) {
    igraph_neighbors(&g_graph, &y, i, IGRAPH_IN);
    if (igraph_vector_size(&y) == 1 && VECTOR(y)[0] == 0)
      start_node.push_back((int)i);
    igraph_neighbors(&g_graph, &y, i, IGRAPH_OUT);
    if (igraph_vector_size(&y) == 1 && VECTOR(y)[0] == last_node_id)
      end_node_set.insert((int)i);
  }
  igraph_vector_destroy(&y);

  if (*run)
    for (auto& p: start_node) {
      run_task(g_nodes[p]);
      // std::this_thread::sleep_for(std::chrono::seconds(120));
    }

  return (0);
}

int node_job_finished(SALT_JOB* job, MapMinionRet* rset) {
  std::set<int>::iterator iter = end_node_set.find(job->node_id);
  if (iter != end_node_set.end()) {
    //remove from set and check the
    end_node_set.erase(iter);
    if (end_node_set.size() == 0) return ALL_TASK_FINISHED;
  } else {
    //run next node

  }
  return 0;
}
