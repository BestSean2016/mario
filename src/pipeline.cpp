#include "pipeline.h"
#include "http_client.h"
#include "mario_data.h"
#include "mario_mysql.h"
#include <igraph.h>
#include <thread>
#include <vector>
#include <set>

extern int run;

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

  int64_t id_start = 0;
  set_start_and_stop(g_nodes, id_start);

  for (size_t i = 1; i < g_nodes.size - 1; ++i) {
    g_nodes[i].id = id_start + i;
    g_nodes[i].ple_id = 2;
    g_nodes[i].script_id = 11;
    g_nodes[i].host_id = (i - 1) % g_hosts.size;
    g_nodes[i].timerout = 90;
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


extern char server_ip[16];
extern int server_port;

static void* run_task(void* arg) {
  MR_REAL_NODE* node = (MR_REAL_NODE*)arg;
  int64_t ret = salt_api_cmd_runall(server_ip, server_port,
                                    g_hosts  [node->host_id].minion_id,
                                    g_scripts[node->script_id].script,
                                    node->ple_id,
                                    node->id);
  pthread_t t = pthread_self();
  pthread_detach(t);
  return (void*)ret;
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
  }
  igraph_vector_destroy(&y);

  igraph_vector_init(&y, 0);
  for (int64_t i = 1; (*run) && (i < last_node_id); ++i) {
    igraph_neighbors(&g_graph, &y, i, IGRAPH_OUT);
    if (igraph_vector_size(&y) == 1 && VECTOR(y)[0] == last_node_id)
      end_node_set.insert((int)i);
  }

  igraph_vector_destroy(&y);

  if (*run)
    for (auto& p: start_node) {
      pthread_t thread;
      (void)pthread_create(&thread, 0, run_task, g_nodes.data + p);
    }

  return (0);
}


static int continue_run_task(MR_REAL_NODE& node) {
  (void)node;
  return true;
}

static int should_run_this_node(int id) {
  igraph_vector_t y;
  igraph_vector_init(&y, 0);
  igraph_neighbors(&g_graph, &y, id, IGRAPH_IN);
  int go = 0;
  for (int i = 0; i < igraph_vector_size(&y); ++i) {
    if (g_nodes[(int)(VECTOR(y)[i])].status > JOB_STATUS_TYPE_RUNNING)
       ++go;
  }
  int ok = true;

  if (go != igraph_vector_size(&y)) {
    //std::cout << "node " << id << " have not been over\n";
    ok = false;
  }

  if (g_nodes[id].status) {
    std::cout << "node " << id << " has alrady runing\n";
    run = 0;
    std::this_thread::sleep_for(std::chrono::seconds(180));
    exit(-12345);
    ok = false;
  }

  if (!continue_run_task(g_nodes[id])) {
    //std::cout << "node " << id << " can not run continuely\n";
    ok = false;
  }

  igraph_vector_destroy(&y);
  return ok;
}


int node_job_finished(SALT_JOB* job, MapMinionRet* rset) {
  (void)rset;
  //std::cout << "job finised " << job->node_id << ", result " << job->status << std::endl;

  std::set<int>::iterator iter = end_node_set.find(job->node_id);
  if (iter != end_node_set.end()) {
    //remove from set and check the
    end_node_set.erase(iter);
    std::cout << "remove node " << job->node_id << std::endl;
    if (end_node_set.size() == 0) {
        std::cout << "All Tasks have been just over!\n";
        run = 0;
        return ALL_TASK_FINISHED;
    }
  } else {
    if (job->status == JOB_STATUS_TYPE_SUCCESSED
        || continue_run_task(g_nodes[job->node_id])) {
      //run next node
      igraph_vector_t y;
      igraph_vector_init(&y, 0);
      igraph_neighbors(&g_graph, &y, job->node_id, IGRAPH_OUT);
      std::cout << job->node_id << " --> ";
      for (int i = 0; i < igraph_vector_size(&y); ++i) {
        std::cout << (int)(VECTOR(y)[i]);
        if (should_run_this_node((int)(VECTOR(y)[i]))) {
          std::cout << " > ";
          pthread_t thread;
          (void)pthread_create(&thread, 0, run_task, g_nodes.data + (int)(VECTOR(y)[i]));
          // std::this_thread::sleep_for(std::chrono::seconds(2));
        } else {
          std::cout << " | ";
        }
      }
      std::cout << std::endl;
      igraph_vector_destroy(&y);
    }
  }
  return 0;
}
