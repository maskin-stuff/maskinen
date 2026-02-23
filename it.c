#include <stdlib.h>
#include <string.h>

#include "it.h"

struct it
{
	char *sent;
	char *save_ptr;
	char *ptr;
};

static const char *vowels[] = {
	"a", "e", "i", "o", "u",
	"y", "å", "ä", "ö", NULL,
};

static const int match(const char *ptr)
{
	for (const char **v = vowels; *v; v++)
	{
		if (strncmp(ptr, *v, strlen(*v)) == 0)
		{
			return strlen(*v);
		}
	}

	return -1;
}

static int sylls(const char *word)
{
	if (word == NULL)
	{
		return -1;
	}

	int c = 0;
	int last = 0;

	while (*word)
	{
		int m = match(word);

		if (m < 0)
		{
			word++;
			last = 0;
			continue;
		}

		if (!last)
		{
			c++;
		}

		word += m;
		last = 1;
	}

	return c;
}

struct it *it_init(const struct buf *buf)
{
	struct it *it = calloc(1, sizeof *it);

	it->sent = buf_strdup(buf);
	it->save_ptr = NULL;
	it->ptr = it->sent;

	return it;
}

void it_free(struct it *it)
{
	free(it->sent);
	free(it);
}

struct it_value it_next(struct it *it)
{
	struct it_value itv;
	itv.word = strtok_r(it->ptr, " \n\t", &it->save_ptr);
	itv.sylls = sylls(itv.word);
	it->ptr = NULL;

	if (itv.sylls == 0)
	{
		itv = it_next(it);
	}

	return itv;
}
