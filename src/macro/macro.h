/* MACRO DEFINITIONS */
#define _POSIX_C_SOURCE 1

/* GLOBAL NEW MACRO 
*  initializes a struct based on convention: 
*  NEW(TYPE, ARGS, ...) -> TYPE_new(sizeof(TYPE), ARGS, ...); */
#define NEW(T, ...) T ##_new(sizeof(T), ##__VA_ARGS__)

/* CORE OBJECT FUNCTIONS */
#define CORE_FUNCTIONS = int (*init)(void *self);
void (*print)(void *self);
void (*destroy)(void *self);