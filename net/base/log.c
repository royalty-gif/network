#include "log.h"
#include "zlog.h"
#include <stdio.h>

zlog_category_t* plog_category = NULL;

// int output(zlog_msg_t *msg) /* 自定义输出函数 */
// {
//     printf("%s : %s%s%s%s\n", msg->path, ESC_START, COLOR_INFO, msg->buf, ESC_END);
//     return 0;
// }
log_err_t log_init(void) {

    int rc = zlog_init(LOG_CONF_PATH);
    if (rc) {
        printf("init failed\n");
        return LOG_PATH_ERROR;
    }

    printf("zlog version: %s\n", zlog_version());

    //zlog_set_record("myoutput", output);
    plog_category = zlog_get_category(LOG_CONF_CATEGORY);
    if (!plog_category) {
		printf("get cat fail\n");
		zlog_fini();
		return LOG_CAT_ERROR;
    }

    return LOG_NO_ERROR;
}

void log_fini(void) {
    return zlog_fini();
}