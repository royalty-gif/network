#include "sys_plat.h"
#include <unistd.h>
#include "log.h"

sys_thread_t sys_thread_create(sys_thread_func_t entry, void *arg) {
    pthread_t tid;

    int err = pthread_create(&tid, NULL, entry, arg);
    if( err < 0) {
        error("pthread create error: %d", err);
        return (pthread_t)0;
    }

    return tid;
}

void sys_thread_destroy(int err) {
    pthread_exit(&err);
}

sys_thread_t sys_thread_self(void) {
    return pthread_self();
}

void sys_sleep(int ms) {
    usleep(ms * 1000);
}