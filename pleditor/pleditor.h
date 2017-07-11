#ifndef PLEDITOR_H
#define PLEDITOR_H

#include "pleditor_global.h"
#include <string>
#include <vector>

namespace PLED {



/*
  "y": -292,
  "x": -756,
  "infoData": {
      "old_id": "{E3BE76AC-0AA3-400F-99AC-F102623E6496}",
      "param": "",
      "minion_id": "---",
      "timeout": 0,
      "ref_type": 4,
      "ref_id": 0,
      "desc": "开始"
  },
  "name": "开始",
  "id": 5
*/
typedef struct NODEINFO {
  int timeout;
  int ref_type;
  int ref_id;
  std::string old_id;
  std::string param;
  std::string minion_id;
  std::string desc;
} NODEINFO;



typedef struct NODE {
  int y;
  int x;
  int id;
  int front_id;
  std::string name;
  NODEINFO info;
} NODE;


typedef std::vector<NODE*> NodeArray;


/*
"to": 7,
"from": 5,
"name": "开始->10.10.202.101 打开计算器 ",
"infoData": {
    "type": 0,
    "desc": "开始->10.10.202.101 打开计算器 "
}
*/

typedef struct EDGEINFO {
  int type; //0
  std::string desc; //"开始->10.10.202.101 打开计算器"
} EDGEINFO;

typedef struct EDGE {
  int to;
  int from;
  int front_to;
  int front_from;
  std::string name;
  EDGEINFO info;
}EDGE;

typedef std::vector<EDGE*> EdgeArray;


/*
"name": "引用流程 [并联] ",
"infoData": {
    "old_id": "{A7BEDFB1-90E8-43FC-91EE-2C229E974B54}",
    "param": "1",
    "minion_id": "---",
    "timeout": 0,
    "ref_type": 3,
    "ref_id": 8,
    "desc": "引用流程 [并联] "
},
"y": "168",
"x": "142",
"node" : []
"id": 134
*/


typedef struct GROUPINFO {
  int timeout;
  int ref_type;
  int ref_id;
  std::string old_id;
  std::string param;
  std::string minion_id;
  std::string desc;
} GROUPINFO;

typedef struct GROUP {
  int id;
  std::string name;
  std::string x;
  std::string y;
  GROUPINFO info;
  NodeArray node;
} GROUP;

typedef std::vector<GROUP*> GroupArray;

typedef struct PIPELINE {
  NodeArray  na;
  EdgeArray  ea;
  GroupArray ga;
}PIPELINE;


int pharser_pipeline_json(const char* json_data, size_t len, PIPELINE& pl);
void clean_pipeline(PIPELINE& pl);

}// namespace PLED


#endif // PLEDITOR_H
