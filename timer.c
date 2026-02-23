#include <math.h>
#include <unistd.h>
#include <stdint.h>
#include <assert.h>
#include <sys/timerfd.h>

#include "timer.h"

int timer_open(void)
{
	int fd = timerfd_create(CLOCK_MONOTONIC, 0);
	assert(fd >= 0);
	return fd;
}

void timer_arm(int fd, double t)
{
	double sec, nsec;
	nsec = modf(t, &sec);
	nsec *= 1000000000.0;

	struct itimerspec ts = {
		.it_value.tv_sec = sec,
		.it_value.tv_nsec = nsec,
	};

	assert(timerfd_settime(fd, 0, &ts, NULL) == 0);
}

void timer_expire(int fd)
{
	uint64_t v;
	assert(read(fd, &v, sizeof v) == sizeof v);
}

void timer_close(int fd)
{
	close(fd);
}
