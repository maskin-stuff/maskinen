#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "buf.h"
#include "udp.h"
#include "osc.h"
#include "parse.h"
#include "gen.h"

int main(int argc, char **argv)
{
	struct buf buf = buf_init();

	char c;
	while ((c = fgetc(stdin)) != EOF)
	{
		buf_append1(&buf, c);
	}

	struct ast *ast = parse(buf.data, buf.size);

	if (ast == NULL)
	{
		parse_print_error(stderr);
		exit(EXIT_FAILURE);
	}

	buf_reset(&buf);
	assert(gen(&buf, ast) == 0);
	buf_append1(&buf, 0);

	printf("%s\n", (const char *) buf.data);
}
