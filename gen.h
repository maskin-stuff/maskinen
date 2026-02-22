#ifndef GEN_H
#define GEN_H

#include "parse.h"
#include "buf.h"

int gen(struct buf *buf, struct ast *ast);
const char *gen_get_error_string(void);

#endif
