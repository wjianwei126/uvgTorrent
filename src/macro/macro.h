/* MACRO DEFINITIONS */
#define _POSIX_C_SOURCE 1

/* GLOBAL NEW MACRO 
*  initializes a struct based on convention: 
*  NEW(TYPE, ARGS, ...) -> TYPE_new(sizeof(TYPE), ARGS, ...); */
#define NEW(T, ...) T ##_new(sizeof(T), ##__VA_ARGS__)

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"