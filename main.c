#include <stdio.h>
#include <assert.h>

#include "udp.h"
#include "osc.h"

int main(int argc, char **argv)
{
	unsigned char buf[4096];

	struct udp_sock *us = udp_init("127.0.0.1", 3819);
	assert(us);

	size_t size = osc_encode(
		buf,
		"/my/message",
		OSC_INTEGER, 123,
		OSC_STRING, "yoyoyo",
		OSC_INTEGER, 420,
		OSC_EOL);

	assert(udp_send(us, buf, size) == 0);

	udp_free(us);

	FILE *f = popen("hexdump -C", "w");
	fwrite(buf, 1, size, f);
	pclose(f);
}
