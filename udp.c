#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "udp.h"

struct udp_sock
{
	int fd;
	struct sockaddr_in addr;
};

struct udp_sock *udp_init(const char *addr, unsigned int port)
{
	struct udp_sock *us = calloc(1, sizeof *us);

	if (inet_pton(AF_INET, addr, &us->addr) != 1)
	{
		fprintf(stderr, "invalid ipv4 address\n");
		free(us);
		return NULL;
	}

	us->addr.sin_family = AF_INET;
	us->addr.sin_port = htons(port);

	if ((us->fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket()");
		free(us);
		return NULL;
	}

	int yes = 1;
	if (setsockopt(us->fd, SOL_SOCKET, SO_BROADCAST, &yes, sizeof yes) < 0)
	{
		perror("setsockopt()");
		close(us->fd);
		free(us);
		return NULL;
	}

	return us;
}

void udp_free(struct udp_sock *us)
{
	close(us->fd);
	free(us);
}

int udp_send(struct udp_sock *us, const struct buf *buf)
{
	struct sockaddr *addrptr = (struct sockaddr *) &us->addr;

	if (sendto(us->fd, buf->data, buf->size, 0, addrptr, sizeof us->addr) < 0)
	{
		perror("sendto()");
		return -1;
	}

	return 0;
}
