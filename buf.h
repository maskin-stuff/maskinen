#ifndef BUF_H
#define BUF_H

#include <stddef.h>
#include <stdio.h>

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
char *buf_strdup(const struct buf *buf);
void buf_pushi(struct buf *buf, int i);
int buf_geti(const struct buf *buf, size_t i);
size_t buf_numi(const struct buf *buf);
void buf_pushp(struct buf *buf, void *p);
void *buf_getp(const struct buf *buf, size_t i);
size_t buf_nump(const struct buf *buf);
int buf_readall(struct buf *buf, FILE *f);

#endif
