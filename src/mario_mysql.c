#include "mario_mysql.h"

#ifdef __USING_MYSQL__

static MYSQL *mysql = 0;

int connect_db(const char *host, int port, const char *db, const char *user,
               const char *passwd) {
  printf("connect_db ..... \n");

  mysql = mysql_init(NULL);

  if (mysql == 0) {
    fprintf(stderr, "%s\n", mysql_error(mysql));
    return -1;
  }

  if (mysql_real_connect(mysql, host, user, passwd, db, port, NULL, 0) ==
      NULL) {
    fprintf(stderr, "%s\n", mysql_error(mysql));
    mysql_close(mysql);
    return -2;
  }

  mysql_query(mysql, "SET NAMES utf8");

  // printf("connect mysql sucess!\n");
  return (0);
}

void disconnect_db(void) {
  if (mysql) {
    mysql_close(mysql);
    mysql = 0;
    mysql_server_end();
    printf("disconnected_db\n");
  }
}

/*
int query_alerts_and_send() {
  MYSQL_RES *res_ptr;
  MYSQL_ROW mysql_row;
  int nRet = 0;
  int nCount = 0;
  int nClock = 0;

  char row_data[1024] = {0};
  char buffer_to_send[1024] = {0};

  char sql[1024] = {0};
  sprintf(sql, "select clock,message,alerttype from alerts where clock>%d "
               "order by eventid asc",
          timestamp);

  printf("%s\n", sql);
  int res = mysql_query(mysql, sql);
  if (res) {
    fprintf(stderr, "SELECT error: %s\n", mysql_error(mysql));
    return -1;
  } else {
    res_ptr = mysql_store_result(mysql);
    if (res_ptr) {
      nCount = mysql_num_rows(res_ptr);
      if (nCount <= 0) {
        mysql_free_result(res_ptr);
        return 0;
      }

      while ((mysql_row = mysql_fetch_row(res_ptr))) {
        strcpy(row_data, mysql_row[1]);
        nClock = atoi(mysql_row[0]);
        timestamp = nClock > timestamp ? nClock : timestamp;
        // printf("[0:%s\n1:%s\n2:%s]\n", mysql_row[0], row_data, mysql_row[2]);
        sprintf(buffer_to_send, "%20u%s", (unsigned int)strlen(row_data),
                row_data);
        printf("%s\n", buffer_to_send);
      }

      if (mysql_errno(mysql)) {
        fprintf(stderr, "Retrive error: %s\n", mysql_error(mysql));
        nRet = -1;
      }
      mysql_free_result(res_ptr);
    }
  }
  return nRet;
}


int insertPolicyalarm(const char *message, int deviceid, const char *firstoccur,
                      const char *kpiid, int alarmLevelId, int icount,
                      int isclose, int isopen) {
  char szSql[512] = {0};
  int nDeviceID = getLocalIP();
  sprintf(szSql, "insert into policyalarm(message, icount,\
            deviceid, firstoccur,\
            kpiid, isclose, isopen, alarmLevelId) \
            values('%s', %d, %d, str_to_date('%s', '%%Y-%%m-%%d %%H:%%i:%%s'), %d, %d,%d,%d)",
          message, icount, nDeviceID, firstoccur, kpiid, isclose, isopen,
          alarmLevelId);

  printf("[policyalarm]:%s\n", szSql);
  if (0 != mysql_query(mysql, szSql)) {
    const char *szError = mysql_error(mysql);
    printf("[error]  %s\n", szError);
    return -1;
  }
  return 0;
}

int insertPolicyalarm(const char *message, int deviceid, const char *firstoccur,
                      const char *kpiid, int alarmLevelId, int icount,
                      int isclose, int isopen) {
  char szSql[512] = {0};
  int nDeviceID = getLocalIP();
  sprintf(szSql, "insert into policyalarm(message, icount,\
            deviceid, firstoccur,\
            kpiid, isclose, isopen, alarmLevelId) \
            values('%s', %d, %d, str_to_date('%s', '%%Y-%%m-%%d %%H:%%i:%%s'), %d, %d,%d,%d)",
          message, icount, nDeviceID, firstoccur, kpiid, isclose, isopen,
          alarmLevelId);

  printf("[policyalarm]:%s\n", szSql);
  if (0 != mysql_query(mysql, szSql)) {
    const char *szError = mysql_error(mysql);
    printf("[error]  %s\n", szError);
    return -1;
  }
  return 0;
}
*/
#endif //__USING_MYSQL__
