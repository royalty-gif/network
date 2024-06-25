#include "utility.h"

long long tm_to_ns(struct timespec tm) {
    return tm.tv_sec * NSECS_PER_SEC + tm.tv_nsec;
}

struct timespec ns_to_tm(long long ns) {
    struct timespec tm;
    tm.tv_sec = ns / NSECS_PER_SEC;
    tm.tv_nsec = ns - (tm.tv_sec * NSECS_PER_SEC);
    return tm;
}