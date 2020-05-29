//
// Created by 沈俊儒 on 2020/5/27.
//

#ifndef SLEX_SCANNER_H
#define SLEX_SCANNER_H

#include "slex_t.h"
#include <stdbool.h>
#include <cxxabi.h>

struct input {
    char_t ch;
    struct input *next;
    struct input *prev;
};
struct scanner {
    struct input *current_input;
    struct input *input_begin;
    struct input *input_end;
};

struct scanner *load_scanner();

struct scanner *load_scanner_from_file(const char *path, enum encoding encoding);

struct scanner *load_scanner_from_str(char_t *str);

void scanner_append(struct scanner *scanner, char_t ch);

void scanner_insert(struct scanner *scanner, char_t ch);

bool scanner_is_end(struct scanner *scanner);

bool scanner_is_begin(struct scanner *scanner);

void scanner_merge(struct scanner *dst, struct scanner *src);

char_t scanner_next(struct scanner *scanner);

char_t scanner_prev(struct scanner *scanner);

void scanner_reset_begin(struct scanner *scanner);

void scanner_reset_end(struct scanner *scanner);

void scanner_decode_buf(struct scanner *scanner, uint8_t *buf, size_t len, enum encoding encoding);

void scanner_debug(struct scanner *scanner);

#endif //SLEX_SCANNER_H
