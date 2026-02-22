#ifndef UDP_H
#define UDP_H

#include <stddef.h>

#include "buf.h"

struct udp_sock;

struct udp_sock *udp_init(const char *addr, unsigned int port);
void udp_free(struct udp_sock *us);
int udp_send(struct udp_sock *us, const struct buf *buf);

#endif
