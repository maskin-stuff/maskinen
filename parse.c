#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include "parse.h"

#ifdef _WIN32
char *strndup(const char *str, size_t n);
#endif

enum token_tag
{
	TT_DONE = 0,
	TT_ERROR,
	TT_WORD,
	TT_LANG, /* < */
	TT_RANG, /* > */
	TT_EQL,  /* = */
	TT_PIPE, /* | */
	TT_SEMI, /* ; */
};

static const char *s_error_string;
static size_t s_error_location;
static size_t s_token_location;
static const char *s_src;
static size_t s_len;
static size_t s_cursor;
static char *s_curr_str;
static int s_peek; /* -1 if no peek */

static void set_error(const char *str, size_t loc)
{
	s_error_string = str;
	s_error_location = loc;
}

static enum token_tag take();

static enum token_tag peek(void)
{
	if (s_peek < 0)
	{
		s_peek = take();
	}

	return s_peek;
}

static int iswordchar(int c)
{
	return isalpha(c) || strchr("åäö", c);
}

static enum token_tag take(void)
{
	size_t start;
	enum token_tag tmp;

	if (s_peek >= 0)
	{
		tmp = s_peek;
		s_peek = -1;
		return tmp;
	}

	if (s_cursor == s_len)
	{
		s_token_location = s_cursor - 1;
		return TT_DONE;
	}

	while (s_src[s_cursor] == ' ' || s_src[s_cursor] == '\n'
	       || s_src[s_cursor] == '\t' || s_src[s_cursor] == '%')
	{
		if (s_src[s_cursor] == '%') {
			while (s_src[s_cursor] != '\n' && s_cursor != s_len) {
				++s_cursor;
				if (s_cursor == s_len)
					return TT_DONE;
			}
			continue;
		}
		++s_cursor;
		if (s_cursor == s_len)
			return TT_DONE;
	}

	switch (s_src[s_cursor])
	{
	case '<':
		s_token_location = s_cursor;
		++s_cursor;
		return TT_LANG;
	case '>':
		s_token_location = s_cursor;
		++s_cursor;
		return TT_RANG;
	case '=':
		s_token_location = s_cursor;
		++s_cursor;
		return TT_EQL;
	case ';':
		s_token_location = s_cursor;
		++s_cursor;
		return TT_SEMI;
	case '|':
		s_token_location = s_cursor;
		++s_cursor;
		return TT_PIPE;
	default:
		break;
	}

	start = s_cursor;
	s_token_location = start;

	char c = s_src[s_cursor];
	while (iswordchar(s_src[s_cursor]) || c == ',' || c == '?' || c == '!'
	       || c == '.' || c == '+' || isdigit(s_src[s_cursor])
	       || c == '-' || c == '\\' || c == ':')
	{
		++s_cursor;
		if (s_cursor == s_len)
			break;
		c = s_src[s_cursor];
	}

	if (start == s_cursor)
	{
		/* Not a word */
		set_error("Invalid character", start);
		return TT_ERROR;
	}

	s_curr_str = strndup(&s_src[start], s_cursor - start);
	return TT_WORD;
}

static struct ast *parse_spec(void);
static struct ast *parse_decl(void);
static struct ast *parse_constr(void);
static struct ast *parse_choice(void);
static struct ast *parse_list(void);

struct ast *parse(const char *src, size_t len)
{
	s_error_string = NULL;
	s_error_location = 0;
	s_src = src;
	s_len = len;
	s_cursor = 0;
	s_peek = -1;

	return parse_spec();
}

static struct ast *parse_spec(void)
{
	struct ast *ast, *lhs, *rhs;
	enum token_tag tt;

	lhs = parse_decl();

	if (!lhs)
	{
		return NULL;
	}

	tt = peek();
	if (tt != TT_DONE)
	{
		rhs = parse_spec();
		if (!rhs)
		{
			ast_destroy(lhs);
			return NULL;
		}

		ast = calloc(1, sizeof(*ast));
		ast->tag = AT_SPEC;
		ast->lhs = lhs;
		ast->rhs = rhs;
		return ast;
	}

	return lhs;
}

static struct ast *parse_decl(void)
{
	struct ast *ast, *lhs, *rhs;
	enum token_tag tt;

	lhs = parse_constr();
	if (!lhs)
	{
		return NULL;
	}

	tt = take();
	switch (tt)
	{
	case TT_ERROR:
		ast_destroy(lhs);
		return NULL;
	case TT_EQL:
		break;
	default:
		ast_destroy(lhs);
		set_error("Unexpected token or EOF, expected =", s_token_location);
		return NULL;
	}

	rhs = parse_choice();
	if (!rhs)
	{
		ast_destroy(lhs);
		return NULL;
	}

	tt = peek();
	switch (tt)
	{
	case TT_ERROR:
		ast_destroy(lhs);
		ast_destroy(rhs);
		return NULL;
	case TT_SEMI:
		(void) take();
		break;
	default:
		set_error("Unexpected token or EOF, expected ;", s_token_location);
		ast_destroy(lhs);
		ast_destroy(rhs);
		return NULL;
	}

	ast = calloc(1, sizeof(*ast));
	ast->tag = AT_DECL;
	ast->lhs = lhs;
	ast->rhs = rhs;

	return ast;
}

static struct ast *parse_constr(void)
{
	struct ast *ast;
	char *word;
	enum token_tag tt;

	tt = take();
	if (tt == TT_ERROR)
	{
		return NULL;
	}
	if (tt != TT_LANG)
	{
		set_error("Unexpected token or EOF, expected <", s_token_location);
		return NULL;
	}

	tt = take();
	if (tt == TT_ERROR)
	{
		return NULL;
	}
	if (tt != TT_WORD)
	{
		set_error("Unexpected token or EOF, expected word", s_token_location);
		return NULL;
	}

	word = s_curr_str;

	tt = take();
	if (tt == TT_ERROR)
	{
		return NULL;
	}
	if (tt != TT_RANG)
	{
		set_error("Unexpected token or EOF, expected >", s_token_location);
		return NULL;
	}

	ast = calloc(1, sizeof(*ast));
	ast->tag = AT_CONSTR;
	ast->str = word;

	return ast;
}

static struct ast *parse_choice(void)
{
	struct ast *ast, *lhs, *rhs;
	enum token_tag tt;

	lhs = parse_list();
	if (!lhs)
	{
		return NULL;
	}

	tt = peek();
	switch (tt)
	{
	case TT_SEMI:
		return lhs;
	case TT_PIPE:
	{
		(void) take();
		rhs = parse_choice();
		if (!rhs)
		{
			ast_destroy(lhs);
			return NULL;
		}

		ast = calloc(1, sizeof(*ast));
		ast->tag = AT_CHOICE;
		ast->lhs = lhs;
		ast->rhs = rhs;
		return ast;
	}
	case TT_ERROR:
		ast_destroy(lhs);
		return NULL;
	default:
		break;
	}

	set_error("Unexpected token or EOF, expected ; or |", s_token_location);
	return NULL;
}

static struct ast *parse_list(void)
{
	struct ast *ast, *lhs, *rhs;
	enum token_tag tt;

	tt = peek();

	switch (tt)
	{
	case TT_LANG:
		lhs = parse_constr();
		if (!lhs)
		{
			return NULL;
		}
		break;
	case TT_WORD:
		(void) take();
		lhs = calloc(1, sizeof(*lhs));
		lhs->tag = AT_WORD;
		lhs->str = s_curr_str;
		break;
	case TT_ERROR:
		return NULL;
	default:
		set_error("Unexpected token or EOF, expected word or construct", s_token_location);
		return NULL;
	}

	tt = peek();

	switch (tt)
	{
	case TT_PIPE:
	case TT_SEMI:
		return lhs;
	default:
		rhs = parse_list();
		if (!rhs)
		{
			ast_destroy(lhs);
			return NULL;
		}
		break;
	}

	ast = calloc(1, sizeof(*ast));
	ast->tag = AT_LIST;
	ast->lhs = lhs;
	ast->rhs = rhs;
	return ast;
}

const char *parse_get_error_string(void)
{
	return s_error_string;
}

size_t parse_get_error_location(void)
{
	return s_error_location;
}

void ast_destroy(struct ast *ast)
{
	if (!ast)
	{
		return;
	}

	ast_destroy(ast->lhs);
	ast_destroy(ast->rhs);
	free(ast->str);
	free(ast);
}

void ast_debug_dump(struct ast *ast, FILE *f)
{
	const char *ast_name;

	switch (ast->tag)
	{
	case AT_SPEC:
		ast_name = "spec";
		break;
	case AT_DECL:
		ast_name = "decl";
		break;
	case AT_WORD:
		ast_name = "word";
		break;
	case AT_CONSTR:
		ast_name = "constr";
		break;
	case AT_CHOICE:
		ast_name = "choice";
		break;
	case AT_LIST:
		ast_name = "list";
		break;
	default: assert(0 && "invalid ast tag");
	}

	fprintf(f, "(%s", ast_name);
	if (ast->str)
	{
		fprintf(f, " \"%s\"", ast->str);
	}
	if (ast->lhs)
	{
		fprintf(f, " ");
		ast_debug_dump(ast->lhs, f);
	}
	if (ast->rhs)
	{
		fprintf(f, " ");
		ast_debug_dump(ast->rhs, f);
	}
	fprintf(f, ")");
}

void parse_print_error(FILE *f)
{
	size_t pos, lineno, begin, end, offset;

	lineno = 1;
	begin = 0;
	end = 0;
	offset = 0;
	pos = 0;

	for (; pos < s_error_location && pos < s_len; pos++)
	{
		if (s_src[pos] == '\n')
		{
			++lineno;
			begin = pos + 1;
		}
	}

	for (; pos < s_len; pos++)
	{
		if (s_src[pos] == '\n')
		{
			break;
		}
	}

	end = pos;
	offset = s_error_location - begin;

	fprintf(f, "\x1b[31merror\x1b[0m (line %zu): %s\n",
	       lineno, s_error_string);

	fprintf(f, "%.*s\n", (int)(end - begin), &s_src[begin]);
	while (offset--)
		fputc(' ', f);
	fprintf(f, "^\n");
}

size_t ast_count_choices(struct ast *ast)
{
	if (ast->tag != AT_CHOICE)
		return 1;
	return 1 + ast_count_choices(ast->rhs);
}

struct ast *ast_get_rhs_depth(struct ast *ast, size_t depth)
{
	assert(ast && "Called ast_get_lhs_depth on NULL");

	if (depth == 0)
		return ast;
	return ast_get_rhs_depth(ast->rhs, depth - 1);
}

struct ast *ast_get_constr(struct ast *ast, const char *name)
{
 	struct ast *result = NULL;

 	assert(ast && name && "Called ast_get_constr on NULL");

	switch (ast->tag)
	{
	case AT_SPEC:
		result = ast_get_constr(ast->lhs, name);
		if (!result)
			result = ast_get_constr(ast->rhs, name);
		break;
	case AT_DECL:
		if (strcmp(ast->lhs->str, name) == 0)
			result = ast->rhs;
		break;
	default:
		assert(0 && "Called get_constr on wrong ast type");
	}

	return result;
}
