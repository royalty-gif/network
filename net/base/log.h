#ifndef _LOG_H_
#define _LOG_H_

#include "zlog.h"

/*
 *  颜色配置宏
 */
#define ESC_START "\033["
#define ESC_END "\033[0m"
#define COLOR_FATAL "31;40;5m"
#define COLOR_ALERT "31;40;1m"
#define COLOR_CRIT "31;40;1m"
#define COLOR_ERROR "35;40;1m"
#define COLOR_WARN "33;40;1m"
#define COLOR_NOTICE "34;40;1m"
#define COLOR_INFO "32;40;1m"
#define COLOR_DEBUG "36;40;1m"

/*
 *  日志相关的宏
 */

#define LOG_CONF_PATH "../zlog.conf"
#define LOG_CONF_CATEGORY "my_cat"

extern zlog_category_t* plog_category;

#define fatal(fmt, args...) \
	zlog(plog_category, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	     ZLOG_LEVEL_FATAL, ESC_START COLOR_FATAL fmt ESC_END, ##args)

#define error(fmt, args...) \
	zlog(plog_category, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	     ZLOG_LEVEL_ERROR, ESC_START COLOR_ERROR fmt ESC_END, ##args)

#define warn(fmt, args...) \
	zlog(plog_category, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	     ZLOG_LEVEL_WARN, ESC_START COLOR_WARN fmt ESC_END, ##args)

#define notice(fmt, args...) \
	zlog(plog_category, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	     ZLOG_LEVEL_NOTICE, ESC_START COLOR_NOTICE fmt ESC_END, ##args)

#define info(fmt, args...) \
	zlog(plog_category, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	     ZLOG_LEVEL_INFO, ESC_START COLOR_INFO fmt ESC_END, ##args)

#define debug(fmt, args...) \
	zlog(plog_category, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	     ZLOG_LEVEL_DEBUG, ESC_START COLOR_DEBUG fmt ESC_END, ##args)

typedef enum {
	LOG_NO_ERROR = 0,
	LOG_PATH_ERROR = -1,
	LOG_CAT_ERROR  = -2,
} log_err_t;

log_err_t log_init(void);
void log_fini(void);


#endif


