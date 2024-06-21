#include "log.h"
#include "zlog.h"

zlog_category_t* plog_category = NULL;
log_error_t log_init(void) {

    int rc = zlog_init(LOG_CONF_PATH);
    if (rc) {
        printf("init failed\n");
        return LOG_PATH_ERROR;
    }

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