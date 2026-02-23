#ifndef IT_H
#define IT_H

#include "buf.h"

struct it;

struct it_value
{
	const char *word;
	int sylls;
};

struct it *it_init(const struct buf *buf);
void it_free(struct it *it);
struct it_value it_next(struct it *it);

#endif
