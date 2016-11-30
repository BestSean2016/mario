#ifndef MARIO_MYSQL_H
#define MARIO_MYSQL_H

#ifdef __USING_MYSQL__

#include "mario_data.h"
#include <mysql/mysql.h>

#define mysql_scpy(str, len, dptr)                                             \
  if ((dptr))                                                                  \
    strcpy_s((str), (len), (dptr));                                            \
  else                                                                         \
    *str = 0;

#define mysql_sdup(dptr) ((dptr) ? strdup(dptr) : nullptr)

typedef void *DBHANDLE;
typedef void (*get_fields_callback)(void *, MYSQL_ROW &mysql_row);


template <typename T> T* new_obj() {
  T* t = new T;
  memset(t, 0, sizeof(T));
  return t;
}

template <typename T>
int get_array_from_db(std::vector<T *> &array, DBHANDLE dbh, const char *sql,
                      get_fields_callback get_field) {
  if (!dbh)
    return -1;

  int nCount = 0;

  int res = mysql_query(reinterpret_cast<MYSQL *>(dbh), sql);
  if (res) {
    fprintf(stdout, "SELECT error: %s by %s\n",
            mysql_error(reinterpret_cast<MYSQL *>(dbh)), sql);
    return -2;
  } else {
    MYSQL_RES *res_ptr = mysql_store_result(reinterpret_cast<MYSQL *>(dbh));

    if (res_ptr) {
      nCount = mysql_num_rows(res_ptr);
      if (nCount <= 0) {
        mysql_free_result(res_ptr);
        return 0;
      }

      MYSQL_ROW mysql_row;
      while ((mysql_row = mysql_fetch_row(res_ptr))) {
        T *t = new_obj<T>();
        get_field(t, mysql_row);
        array.push_back(t);
      }

      if (mysql_errno(reinterpret_cast<MYSQL *>(dbh))) {
        fprintf(stdout, "Retrive an error: %s\n",
                mysql_error(reinterpret_cast<MYSQL *>(dbh)));
        nCount = -3;
      }
      mysql_free_result(res_ptr);
    }
  }
  return nCount;
}

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

extern const char *select_host_from_db;
extern const char *select_pipeline_from_db;
extern const char *select_script_from_db;
extern const char* select_edge_from_db;
extern const char *select_plexec_from_db;
extern const char* select_plen_from_db;
extern const char* select_host_status_from_db;

extern DBHANDLE connect_db(const char *host, int port, const char *db,
                           const char *user, const char *passwd);
extern void disconnect_db(DBHANDLE dbh);

extern void get_hosts(void *host_ptr, MYSQL_ROW &mysql_row);
extern void get_pipeline(void *pipeline_ptr, MYSQL_ROW &mysql_row);
extern void get_script(void* script_ptr, MYSQL_ROW& mysql_row);
extern void get_edge(void* edge_ptr, MYSQL_ROW& mysql_row);
extern void get_pipeline_exec(void* ptr, MYSQL_ROW& row);
extern void get_plen_exec(void* ptr, MYSQL_ROW& row);
extern void get_host_status(void* ptr, MYSQL_ROW& row);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__USING_MYSQL_

#endif // MARIO_MYSQL_H
