#include "pleditor.h"
#include "rapidjson/document.h"
#include <iostream>

namespace PLED {

static void show_cstring(const char *cstring, size_t len) {
  std::cout << "|-->";
  for (size_t i = 0; cstring && *cstring && i < len; i++)
    std::cout << cstring[i];
  std::cout << "<--|\n";
}

/*
{
    "y": -116,
    "x": -702,
    "infoData": {
        "old_id": "{CD6E7DDB-3863-4E2A-A59B-557F37E6FDD0}",
        "param": "1",
        "minion_id": "old080027807D65",
        "timeout": 0,
        "ref_type": 1,
        "ref_id": 1,
        "desc": "10.10.202.101 打开计算器 "
    },
    "name": "10.10.202.101 打开计算器 ",
    "id": 31,
    "front_id": 123
},
*/

static int pharser_node_info___(rapidjson::Value& v, NODEINFO& info) {
    if (v.HasMember("old_id"))
        info.old_id = v["old_id"].GetString();
    else
        return -120;

    if (v.HasMember("param"))
        info.param = v["param"].GetString();
    else
        return -121;

    if (v.HasMember("minion_id"))
        info.param = v["minion_id"].GetString();
    else
        return -122;

    if (v.HasMember("timeout"))
        info.timeout = v["timeout"].GetInt();
    else
        return -123;

    if (v.HasMember("ref_type"))
        info.ref_type = v["ref_type"].GetInt();
    else
        return -124;

    if (v.HasMember("ref_id"))
        info.ref_id = v["ref_id"].GetInt();
    else
        return -125;

    if (v.HasMember("desc"))
        info.desc = v["desc"].GetString();
    else
        return -126;

    return 0;
}

static int pharser_one_node__(rapidjson::Value& v, NODE* node) {
    if (v.HasMember("x"))
        node->x = v["x"].GetInt();
    else
        return -110;

    if (v.HasMember("y"))
        node->y = v["y"].GetInt();
    else
        return -111;

    if (v.HasMember("name"))
        node->name = v["name"].GetString();
    else
        return -112;

    if (v.HasMember("infoData")) {
        int ret = pharser_node_info___(v["infoData"], node->info);
        if (ret) return ret;
    } else
        return -113;

    if (v.HasMember("id"))
        node->id = v["id"].GetInt();
    else
        return -114;

    if (v.HasMember("front_id"))
        node->front_id = v["front_id"].GetInt();
    else
        return -115;

    return 0;
}

static int pharser_nodes_(rapidjson::Document& doc, NodeArray& na) {
    if (!doc.HasMember("nodes")) return -100;
    if (!doc["nodes"].IsArray()) return -101;

    int ret = 0;
    rapidjson::Value &array = doc["nodes"];
    for (rapidjson::SizeType i = 0; !ret && i < array.Size(); ++i) {
      NODE* node = new NODE;
      ret = pharser_one_node__(array[i], node);
      if (!ret)
          na.emplace_back(node);
      else {
          delete node;
          for (auto& p: na)
              delete p;
          na.clear();
      }
    }

    return ret;
}


/*
    {
        "to": 25,
        "from": 24,
        "front_to": 125,
        "front_from": 124,
        "name": "开始->10.10.202.101 打开计算器 ",
        "infoData": {
            "type": 1,
            "desc": "开始->10.10.202.101 打开计算器 "
        }
    },
*/

static int pharser_edge_info___(rapidjson::Value& v, EDGEINFO& info) {
    if (v.HasMember("type"))
        info.type = v["type"].GetInt();
    else
        return -130;

    if (v.HasMember("desc"))
        info.desc = v["desc"].GetString();
    else
        return -131;

    return 0;
}

static int pharser_one_edge__(rapidjson::Value& v, EDGE* edge) {
    if (v.HasMember("to"))
        edge->to = v["to"].GetInt();
    else
        return -210;

    if (v.HasMember("from"))
        edge->from = v["from"].GetInt();
    else
        return -211;

    if (v.HasMember("front_to"))
        edge->front_to = v["front_to"].GetInt();
    else
        return -212;


    if (v.HasMember("front_from"))
        edge->front_from = v["front_from"].GetInt();
    else
        return -213;

    if (v.HasMember("infoData")) {
        int ret = pharser_edge_info___(v["infoData"], edge->info);
        if (ret) return ret;
    } else
        return -214;

    if (v.HasMember("name"))
        edge->name = v["name"].GetString();
    else
        return -215;

    return 0;
}


static int pharser_edges_(rapidjson::Document& doc, EdgeArray& ea) {
    if (!doc.HasMember("edges")) return -200;
    if (!doc["edges"].IsArray()) return -201;

    int ret = 0;
    rapidjson::Value &array = doc["edges"];
    for (rapidjson::SizeType i = 0; !ret && i < array.Size(); ++i) {
        EDGE* edge = new EDGE;
        ret = pharser_one_edge__(array[i], edge);
        if (!ret)
            ea.emplace_back(edge);
        else {
            delete edge;
            for (auto& p: ea)
                delete p;
            ea.clear();
        }
    }

    return 0;
}


/*
    {
        "name": "子流程_1 ",
        "infoData": {
            "old_id": "{CBD44FA5-E680-44CD-B66D-2DBC0838E842}",
            "param": "",
            "minion_id": "---",
            "timeout": 0,
            "ref_type": 3,
            "ref_id": 11,
            "desc": "子流程_1 "
        },
        "y": "71",
        "x": "127",
        "nodes": [
        ],
        "id": 23
    }
*/

static int pharser_group_info__(rapidjson::Value& v, GROUPINFO& info) {
    if (v.HasMember("old_id"))
        info.old_id = v["old_id"].GetString();
    else
        return -320;

    if (v.HasMember("param"))
        info.param = v["param"].GetString();
    else
        return -321;

    if (v.HasMember("minion_id"))
        info.param = v["minion_id"].GetString();
    else
        return -322;

    if (v.HasMember("timeout"))
        info.timeout = v["timeout"].GetInt();
    else
        return -323;

    if (v.HasMember("ref_type"))
        info.ref_type = v["ref_type"].GetInt();
    else
        return -324;

    if (v.HasMember("ref_id"))
        info.ref_id = v["ref_id"].GetInt();
    else
        return -325;

    if (v.HasMember("desc"))
        info.desc = v["desc"].GetString();
    else
        return -326;

    return 0;
}


static int pharser_one_group__(rapidjson::Value& v, GROUP* g) {
    if (v.HasMember("name"))
        g->name = v["name"].GetString();
    else
        return -310;

    if (v.HasMember("x"))
        g->x = v["x"].GetInt();
    else
        return -311;


    if (v.HasMember("y"))
        g->y = v["y"].GetInt();
    else
        return -312;

    if (v.HasMember("infoData")) {
        int ret = pharser_group_info__(v["infoData"], g->info);
        if (ret) return ret;
    } else
        return -313;

    if (v.HasMember("nodes")) {
        if (!v["nodes"].IsArray())
            return -314;

        int ret = 0;
        rapidjson::Value &array = v["nodes"];
        for (rapidjson::SizeType i = 0; !ret && i < array.Size(); ++i) {
          NODE* node = new NODE;
          ret = pharser_one_node__(array[i], node);
          if (!ret)
              g->node.emplace_back(node);
          else {
              delete node;
              for (auto& n : g->node)
                  delete n;
              g->node.clear();
              return ret;
          }
        }
    } else
        return -315;

    if (v.HasMember("id"))
        g->id = v["id"].GetInt();
    else
        return -316;


    return 0;
}

static int pharser_groups_(rapidjson::Document& doc, GroupArray& ga) {
    if (!doc.HasMember("groups")) return -300;
    if (!doc["groups"].IsArray()) return -301;

    int ret = 0;
    rapidjson::Value &array = doc["groups"];
    for (rapidjson::SizeType i = 0; !ret && i < array.Size(); ++i) {
        GROUP* g = new GROUP;
        ret = pharser_one_group__(array[i], g);
        if (!ret)
            ga.emplace_back(g);
        else {
            delete g;
            for (auto& p: ga)
                delete p;
            ga.clear();
        }
    }
    return ret;
}


int pharser_pipeline_json(const char* json_data, size_t len, PIPELINE& pl) {
    rapidjson::Document doc;
    doc.Parse((char *)json_data, len);

    if (doc.HasParseError()) {
      std::cout << "doc has error\n";
      show_cstring(json_data, len);
      return -2;
    }

    int ret = pharser_nodes_(doc, pl.na);

    if (!ret)
      ret = pharser_edges_(doc, pl.ea);

    if (!ret)
      ret = pharser_groups_(doc, pl.ga);

    return ret;
}


void clean_pipeline(PIPELINE& pl) {
    for (auto& n : pl.na)
        delete n;
    for (auto& e : pl.ea)
        delete e;
    for (auto& g : pl.ga) {
        for (auto& n : g->node)
            delete n;
        g->node.clear();
        delete g;
    }

    pl.na.clear();
    pl.ea.clear();
    pl.ga.clear();
}
} //namespace PLED
