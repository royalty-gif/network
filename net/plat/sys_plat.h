#ifndef _SYS_PLAT_H_
#define _SYS_PLAT_H_

#include <pthread.h>
#include <stdint.h>
#include <string.h>  // 字符串相关函数

// 系统配置
static const char netdev0_ip[] = "192.168.74.2";
static const char netdev0_mask[] = "255.255.255.0";
static const char netdev0_gw[] = "192.168.74.1";
static const uint8_t netdev0_hwaddr[] = { 0x00, 0x16, 0x3e, 0x03, 0x32, 0xe5 };
static const char netdev0_phy_ip[] = "172.19.127.142";

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

// 定时器相关
uint32_t sys_now(void);

#endif