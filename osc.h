#ifndef OSC_H
#define OSC_H

#include <stddef.h>

enum osc_type {
	OSC_EOL = -1,
	OSC_INTEGER = 0,
	OSC_STRING,
};

size_t osc_encode(unsigned char *buf, const char *addr, ...);

#endif
