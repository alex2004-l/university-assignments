#include <internal/syscall.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>


unsigned int sleep(unsigned int seconds) {
    const struct timespec req = {seconds, 60 * seconds};
    struct timespec rem;
    return nanosleep(&req, &rem);
    return 0;
}


int nanosleep(const struct timespec *req, struct timespec *rem) {
    int ret = syscall(__NR_nanosleep, req, rem);

    if (ret < 0) {
		errno = -ret;
		return -1;
	}

	return ret;
}
