#include "sys_plat.h"
#include <bits/time.h>
#include <time.h>
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

uint32_t sys_now(void) {
    struct timespec ts;
    uint32_t now;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    now = (uint32_t)(ts.tv_sec * 1000L + ts.tv_nsec / 1000000L); // 毫秒ms

    return now;
}