#ifndef ITAT_H
#define ITAT_H

#include "itat_global.h"

#include <ctype.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>

#include <iostream>
#include <map>
#include <queue>
#include <vector>
#include <set>
#include <string>
#include <functional>
#include <thread>
#include <mutex>
#include <sstream>

#include <assert.h>

using std::cout;
using std::endl;

#define HOST_NAME_LENGTH 128
#define HOST_MINION_ID_LENGTH 32
#define SALT_JOB_ID_LENGTH 32
#define SALT_FUNCTION_LENGTH 32
#define SHORT_TEXT_LENGTH 255
#define IP_ADDRESS_LENGTH 15
#define OLD_SYS_ID_LENGTH 64
#define NORMAL_NAME_LENGTH 50
#define SCHEDULE_LENGTH    128



#define SafeDeletePtr(p) if (p) {delete p; p = 0;}
#define SafeDeleteAry(a) if (a) {delete [] a; a =0;}
#define PTR2INT(ptr) ((int)(uint64_t)((ptr)))

namespace itat {
template<typename T>
class ITAT_API MapStr2Ptr : public std::map<std::string, T*> {
public:
   MapStr2Ptr() {}
   ~MapStr2Ptr() {
     for (auto p = this->begin(); p != this->end(); ++p)
       delete (*p).second;
   }
};

} //namespace itat


#endif // ITAT_H
