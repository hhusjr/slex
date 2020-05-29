//
// Created by 沈俊儒 on 2020/5/25.
//

#ifndef SLEX_SLEX_H
#define SLEX_SLEX_H

#include <stdbool.h>
#include "regexp.h"
#include "slex_t.h"
#include "scanner.h"

#define REJECT -1

typedef void (*callback_t)(const char_t *lexeme, size_t len, unsigned int token_class, bool *more,
                           struct scanner *scanner, void *args);

struct slex_config {
    state_t **delta;
    unsigned int *token_class;
    charclass_t *charclass;

    state_t start_state;
    state_t n_state;
    charclass_t n_charclass;
};

struct slex_config *slex_compile(struct slex_item *regexps);

struct slex_config *slex_from_file(const char *path);

void slex_save(const char *path, struct slex_config *config);

struct slex_config *slex_load(const char *path, struct slex_item *regexps);

bool
slex(struct slex_config *config, struct scanner *scanner, callback_t callback, void *args);

#endif //SLEX_SLEX_H
