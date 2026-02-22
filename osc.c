#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <arpa/inet.h>

#include "osc.h"

size_t osc_encode(unsigned char *buf, const char *addr, ...)
{
	unsigned char tag[4096] = ",";
	size_t tag_size = 1;

	unsigned char args[4096];
	size_t args_size = 0;

	size_t addr_size = strlen(addr) + 1;
	memcpy(buf, addr, addr_size);

	while (addr_size % 4 != 0)
	{
		buf[addr_size++] = 0;
	}

	va_list ap;
	va_start(ap, addr);

	for (;;)
	{
		enum osc_type type = va_arg(ap, enum osc_type);
		int num;
		const char *str;

		switch (type)
		{
		case OSC_EOL:
			goto done;

		case OSC_INTEGER:
			tag[tag_size++] = 'i';

			num = htonl(va_arg(ap, int));
			memcpy(&args[args_size], &num, sizeof num);
			args_size += sizeof num;

			break;

		case OSC_STRING:
			tag[tag_size++] = 's';

			str = va_arg(ap, const char *);
			memcpy(&args[args_size], str, strlen(str) + 1);
			args_size += strlen(str) + 1;

			while (args_size % 4 != 0)
			{
				args[args_size++] = 0;
			}

			break;

		default:
			assert(0 && "invalid osc type");
		}
	}

done:
	va_end(ap);

	tag[tag_size++] = 0;
	while (tag_size % 4 != 0)
	{
		tag[tag_size++] = 0;
	}
	memcpy(&buf[addr_size], tag, tag_size);
	memcpy(&buf[addr_size + tag_size], args, args_size);

	return addr_size + tag_size + args_size;
}
