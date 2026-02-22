#ifndef OSC_H
#define OSC_H

#include <stddef.h>

#include "buf.h"

enum osc_type
{
	OSC_EOL = -1,
	OSC_INTEGER = 0,
	OSC_STRING,
};

void osc_encode(struct buf *buf, const char *addr, ...);

#endif
