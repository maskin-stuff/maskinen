#ifndef UDP_H
#define UDP_H

#include <stddef.h>

struct udp_sock;

struct udp_sock *udp_init(const char *addr, unsigned int port);
void udp_free(struct udp_sock *us);
int udp_send(struct udp_sock *us, const unsigned char *buf, size_t size);

#endif
