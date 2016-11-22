#ifndef MARIO_MYSQL_H
#define MARIO_MYSQL_H

#ifdef __USING_MYSQL__

#include "mario_types.h"
#include <mysql/mysql.h>


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


extern int connect_db(const char* host, int port, const char* db, const char* user, const char* passwd);

#ifdef __cplusplus
}
#endif //__cplusplus


#endif //__USING_MYSQL_

#endif // MARIO_MYSQL_H
