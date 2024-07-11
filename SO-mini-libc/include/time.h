#ifndef __TIME_H__
#define	__TIME_H__	1

#ifdef __cplusplus
extern "C" {
#endif

#include <internal/types.h>

typedef int time_t;

struct timespec {
    time_t tv_sec;
    long tv_nsec;
};

// https://pubs.opengroup.org/onlinepubs/007904975/functions/nanosleep.html#:~:text=DESCRIPTION,or%20to%20terminate%20the%20process.
int nanosleep(const struct timespec *rqtp, struct timespec *rmtp);

#ifdef __cplusplus
}
#endif

#endif
