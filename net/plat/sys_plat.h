#ifndef _SYS_PLAT_H_
#define _SYS_PLAT_H_

#include <pthread.h>
#include <string.h>

#define plat_strlen    strlen
#define plat_strcpy    strcpy
#define plat_strncpy   strncpy
#define plat_strcmp    strcmp
#define plat_stricmp   strcasecmp
#define plat_memset    memset
#define plat_memcpy    memcpy
#define plat_memcmp    memcmp
#define plat_sprintf   sprintf
#define plat_vsprintf  vsprintf
#define plat_printf    printf

// 线程相关
typedef pthread_t sys_thread_t;

typedef void* (*sys_thread_func_t)(void* arg);
sys_thread_t sys_thread_create(sys_thread_func_t entry, void* arg);
void sys_thread_destroy(int err);
sys_thread_t sys_thread_self(void);

void sys_sleep(int ms);

#endif