#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <time.h>

#define is_equal(a, b)           \
({                               \
    unsigned ta = (unsigned)(a); \
    unsigned tb = (unsigned)(b); \
    !(ta - tb);                  \
})

#define offsetof(type, member) ((unsigned) &((type *)0)->member)

#define container_of(ptr, type, member)                     \
({                                                          \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetof(type,member) );     \
})

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#define NSECS_PER_MSEC 1000000UL
#define NSECS_PER_SEC 1000000000UL
    
#define DIFFNS(begin, end) ((end.tv_sec - begin.tv_sec) * NSECS_PER_SEC \
                            + (end.tv_nsec - begin.tv_nsec))

long long tm_to_ns(struct timespec tm);
struct timespec ns_to_tm(long long ns);

#endif