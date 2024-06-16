#ifndef _UTILITY_H_
#define _UTILITY_H_

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

#endif