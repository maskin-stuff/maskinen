#include <stdio.h>
#include <assert.h>

#include "buf.h"
#include "udp.h"
#include "osc.h"

int main(int argc, char **argv)
{
	struct buf buf = buf_init();

	struct udp_sock *us = udp_init("127.0.0.1", 3819);
	assert(us);

	osc_encode(
		&buf,
		"/my/message",
		OSC_INTEGER, 123,
		OSC_STRING, "yoyoyo",
		OSC_INTEGER, 420,
		OSC_EOL
	);

	assert(udp_send(us, &buf) == 0);

	udp_free(us);

	FILE *f = popen("hexdump -C", "w");
	fwrite(buf.data, 1, buf.size, f);
	pclose(f);

	buf_free(&buf);
}
