#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <arpa/inet.h>

#include "osc.h"

void osc_encode(struct buf *buf, const char *addr, ...)
{
	struct buf tag = buf_init();
	struct buf args = buf_init();

	buf_append(&tag, ",", 1);
	buf_append(buf, addr, strlen(addr) + 1);
	buf_zalign(buf, 4);

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
			buf_append(&tag, "i", 1);
			num = htonl(va_arg(ap, int));
			buf_append(&args, &num, sizeof num);
			break;

		case OSC_STRING:
			buf_append(&tag, "s", 1);
			str = va_arg(ap, const char *);
			buf_append(&args, str, strlen(str) + 1);
			buf_zalign(&args, 4);
			break;

		default:
			assert(0 && "invalid osc type");
		}
	}

done:
	va_end(ap);

	buf_append1(&tag, 0);
	buf_zalign(&tag, 4);

	buf_append(buf, tag.data, tag.size);
	buf_append(buf, args.data, args.size);

	buf_free(&tag);
	buf_free(&args);
}
