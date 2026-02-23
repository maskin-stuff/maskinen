#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <getopt.h>

#include "it.h"
#include "buf.h"
#include "udp.h"
#include "osc.h"
#include "parse.h"
#include "gen.h"
#include "timer.h"
#include "palette.h"

static int port = 9000;
static const char *addr = "127.0.0.1";
static int tempo = 120;
static const char *palette_path = NULL;
static const char *grammar_path = NULL;

static struct udp_sock *us = NULL;

static void cleanup(void)
{
	if (us)
	{
		udp_free(us);
	}
}

static void usage(const char *program, FILE *f)
{
	fprintf(
		stderr,
		"Usage: %s [OPTION...]\n"
		"Options:\n"
		"\t-p PORT   \tUDP port (default 9000)\n"
		"\t-a ADDRESS\tUDP address (default 127.0.0.1)\n"
		"\t-t TEMPO  \tTempo in BPM (default 120)\n"
		"\t-c PALETTE\tPalette path (mandatory)\n"
		"\t-g GRAMMAR\tGrammar path (mandatory)\n",
		program
	);
}

static void parse_args(int argc, char **argv)
{
	const char *program = argv[0];

	char c;
	while ((c = getopt(argc, argv, "p:a:t:c:g:")) != -1)
	{
		switch (c)
		{
		case 'p':
			port = atoi(optarg);
			break;
		case 'a':
			addr = optarg;
			break;
		case 't':
			tempo = atoi(optarg);
			break;
		case 'c':
			palette_path = optarg;
			break;
		case 'g':
			grammar_path = optarg;
			break;
		case '?':
			usage(program, stderr);
			exit(EXIT_FAILURE);
			break;
		}
	}

	if (grammar_path == NULL)
	{
		usage(program, stderr);
		fprintf(stderr, "Error: Missing grammar path\n");
		exit(EXIT_FAILURE);
	}

	if (palette_path == NULL)
	{
		usage(program, stderr);
		fprintf(stderr, "Error: Missing palette path\n");
		exit(EXIT_FAILURE);
	}
}

static struct ast *parse_grammar(void)
{
	FILE *f = fopen(grammar_path, "r");

	if (f == NULL)
	{
		perror(grammar_path);
		exit(EXIT_FAILURE);
	}

	struct buf grammar = buf_init();
	assert(buf_readall(&grammar, f) == 0);
	fclose(f);

	struct ast *ast = parse(grammar.data, grammar.size);

	if (ast == NULL)
	{
		parse_print_error(stderr);
		exit(EXIT_FAILURE);
	}

	return ast;
}

int main(int argc, char **argv)
{
	parse_args(argc, argv);

	if ((us = udp_init(addr, port)) == NULL)
	{
		fprintf(stderr, "Error: Failed to create UDP socket\n");
		exit(EXIT_FAILURE);
	}

	atexit(cleanup);

	struct ast *ast = parse_grammar();
	int **palette = parse_palette(palette_path);
	int *const *palette_ptr = palette;

	if (palette == NULL)
	{
		exit(EXIT_FAILURE);
	}

	double duration = 60.0 / tempo;
	int timer = timer_open();
	struct buf buf = buf_init();
	struct buf osc = buf_init();
	struct it *it = NULL;
	struct it_value itv;
	int sylli = 0;

	for (;; sylli++)
	{
		if (it == NULL)
		{
			buf_reset(&buf);
			assert(gen(&buf, ast) == 0);
			it = it_init(&buf);
			itv = it_next(it);
			sylli = 0;

			fprintf(stderr, "sentence:\t%.*s\n", (int) buf.size, (const char *) buf.data);
		}

		if (sylli == itv.sylls)
		{
			itv = it_next(it);
			sylli = 0;

			timer_arm(timer, duration);
			timer_expire(timer);

			if (itv.word == NULL)
			{
				timer_arm(timer, duration);
				timer_expire(timer);

				it_free(it);
				it = NULL;

				continue;
			}
		}

		if (sylli == 0)
		{
			buf_reset(&osc);
			osc_encode(&osc,
			           "/word",
			           OSC_STRING, itv.word,
			           OSC_EOL);
			assert(udp_send(us, &osc) == 0);

			fprintf(stderr, "word:\t%s\n", itv.word);
		}

		if (*palette_ptr == NULL)
		{
			palette_ptr = palette;
		}

		fprintf(stderr, "chord:\t");

		for (int *key_ptr = *palette_ptr; *key_ptr >= 0; key_ptr++)
		{
			buf_reset(&osc);
			osc_encode(&osc,
			           "/midi",
			           OSC_INTEGER, *key_ptr,
			           OSC_EOL);
			assert(udp_send(us, &osc) == 0);
			fprintf(stderr, "%d ", *key_ptr);
		}

		fprintf(stderr, "\n");

		palette_ptr++;

		timer_arm(timer, duration);
		timer_expire(timer);
	}
}
