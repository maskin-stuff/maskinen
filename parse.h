#ifndef PARSE_H
#define PARSE_H

#include <stdio.h>

#define BASE_NAME "base"

enum ast_tag
{
	AT_SPEC,      /* lhs: first decl, rhs: next spec */
	AT_DECL,      /* lhs: construct, rhs: definition */
	AT_WORD,      /* str: word */
	AT_CONSTR,    /* str: name */
	AT_CHOICE,    /* lhs: first list, rhs: next choice */
	AT_LIST,      /* lhs: first construct/word, rhs: next list */
};

struct ast
{
	enum ast_tag tag;
	struct ast *lhs;
	struct ast *rhs;
	int freq;
	char *str;
};

struct ast *parse(const char *src, size_t len);
void ast_destroy(struct ast *ast);

/* Get description and location of error on failed parse */
const char *parse_get_error_string(void);
size_t parse_get_error_location(void);
void parse_print_error(FILE *f);

void ast_debug_dump(struct ast *ast, FILE *f);

size_t ast_count_choices(struct ast *ast);
struct ast *ast_get_rhs_depth(struct ast *ast, size_t depth);
struct ast *ast_get_constr(struct ast *ast, const char *name);

#endif
