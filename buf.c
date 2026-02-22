#include <stdlib.h>
#include <string.h>

#include "buf.h"

#define INIT_CAP 128

struct buf buf_init(void)
{
	return (struct buf) {
		.data = malloc(INIT_CAP),
		.size = 0,
		.cap = INIT_CAP,
	};
}

void buf_append(struct buf *buf, const void *data, size_t size)
{
	if (buf->size + size > buf->cap)
	{
		buf->cap *= 2;
		buf->data = realloc(buf->data, buf->cap);
	}

	memcpy(buf->data + buf->size, data, size);
	buf->size += size;
}

void buf_append1(struct buf *buf, unsigned char x)
{
	buf_append(buf, &x, 1);
}

void buf_free(struct buf *buf)
{
	free(buf->data);
	memset(buf, 0, sizeof *buf);
}

void buf_reset(struct buf *buf)
{
	buf->size = 0;
}

void buf_zalign(struct buf *buf, size_t align)
{
	unsigned char zero = 0;
	while (buf->size % align != 0)
	{
		buf_append(buf, &zero, 1);
	}
}
