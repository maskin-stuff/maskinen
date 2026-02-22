#ifndef BUF_H
#define BUF_H

#include <stddef.h>

struct buf
{
	void *data;
	size_t size;
	size_t cap;
};

struct buf buf_init(void);
void buf_append(struct buf *buf, const void *data, size_t size);
void buf_append1(struct buf *buf, unsigned char x);
void buf_free(struct buf *buf);
void buf_reset(struct buf *buf);
void buf_zalign(struct buf *buf, size_t align);

#endif
