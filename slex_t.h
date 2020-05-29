//
// Created by 沈俊儒 on 2020/5/25.
//

#include "stdbool.h"
#include <wchar.h>

#ifndef DEMO_TYPES_H
#define DEMO_TYPES_H

#define SYS_ALPHABET_START 1
#define SYS_ALPHABET_END 65535

enum encoding {
    UTF8
};

typedef unsigned int state_t;
typedef unsigned int charclass_t;
typedef wchar_t char_t;

struct slex_item {
    char_t *name;
    unsigned int token_class;
    unsigned int priority;
    struct regexp_ast *ast;

    struct slex_item *next;
};

#define SLEX_MORE() \
do { \
    *more = true; \
} while(0)

#define SLEX_LESS(n) \
do { \
    *less = n; \
} while(0)

#define MOVE_INPUT(ptr) \
do { \
    *next_scan = ptr; \
} while(0)

#endif //DEMO_TYPES_H
