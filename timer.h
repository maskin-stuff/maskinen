#ifndef TIMER_H
#define TIMER_H

int timer_open(void);
void timer_arm(int fd, double t);
void timer_expire(int fd);
void timer_close(int fd);

#endif
