#ifndef _dbg_h
#define _dbg_h

#include <stdio.h>
#include <errno.h>
#include <string.h>

#ifdef NDEBUG
#define debug(M, ...)
#else
#define debug(M, ...) fprintf(stderr, KCYN "[DEBUG] %s:%d: " M KNRM "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#define log_err(M, ...) fprintf(stderr, KRED "[ERROR] %s:%d: errno: %s: " M KNRM "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_warn(M, ...) fprintf(stderr, KYEL "[WARN]  %s:%d: errno: %s: " M KNRM "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_info(M, ...) fprintf(stderr, KBLU "[INFO]  %s:%d: " M KNRM "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define log_confirm(M, ...) fprintf(stderr, KBLU "[INFO]  %s:%d: " M KNRM, __FILE__, __LINE__, ##__VA_ARGS__)

#define assert(A, M, ...) if(!(A)) { log_err(M, ##__VA_ARGS__); errno=0; goto error; }

#define throw(M, ...)  { log_err(M, ##__VA_ARGS__); errno=0; goto error; }

#define check_mem(A) assert((A), "Out of memory.")

#define assert_debug(A, M, ...) if(!(A)) { debug(M, ##__VA_ARGS__); errno=0; goto error; }

#endif
