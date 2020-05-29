//
// Created by 沈俊儒 on 2020/5/27.
//

#include "scanner.h"
#include <stdlib.h>
#include <stdbool.h>

struct scanner *load_scanner() {
    struct scanner *scanner = malloc(sizeof(struct scanner));
    scanner->input_begin = malloc(sizeof(struct input));
    scanner->input_end = malloc(sizeof(struct input));

    scanner->input_begin->prev = NULL;
    scanner->input_begin->next = scanner->input_end;

    scanner->input_end->prev = scanner->input_begin;
    scanner->input_end->next = NULL;
    return scanner;
}

void scanner_decode_buf(struct scanner *scanner, uint8_t *buf, size_t len, enum encoding encoding) {
    switch (encoding) {
        case UTF8: {
            size_t pos = 0;
            while (pos < len) {
                if (buf[pos] <= 0x7F) {
                    scanner_append(scanner, buf[pos]);
                    pos++;
                    continue;
                }

                uint8_t lookup_mask = 1u << 7u;
                size_t byte_cnt;
                for (byte_cnt = 0; byte_cnt < 8; byte_cnt++) {
                    if (!(buf[pos] & lookup_mask)) {
                        break;
                    }
                    lookup_mask >>= 1u;
                }

                char_t current_ch = 0;
                for (size_t i = 0; i < byte_cnt; i++) {
                    current_ch += buf[pos + i] << (8u * i);
                    pos++;
                }
                scanner_append(scanner, buf[pos]);
            }
        }

        default:
            break;
    }
}

struct scanner *load_scanner_from_file(const char *path, enum encoding encoding) {
    FILE *fp = fopen(path, "r");
    fseek(fp, 0, SEEK_END);
    size_t byte_cnt = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    uint8_t *buf = malloc(byte_cnt);
    fread(buf, 1, byte_cnt, fp);
    fclose(fp);
    struct scanner *scanner = load_scanner();
    scanner_decode_buf(scanner, buf, byte_cnt, encoding);
    scanner_reset_begin(scanner);
    free(buf);
    return scanner;
}

struct scanner *load_scanner_from_str(char_t *str) {
    struct scanner *scanner = load_scanner();
    while (*str != '\0') {
        scanner_append(scanner, *str);
        str++;
    }
    scanner_reset_begin(scanner);
    return scanner;
}

void scanner_append(struct scanner *scanner, char_t ch) {
    struct input *tmp = malloc(sizeof(struct input));
    tmp->next = scanner->input_end;
    tmp->prev = scanner->input_end->prev;
    tmp->ch = ch;
    scanner->input_end->prev->next = tmp;
    scanner->input_end->prev = tmp;
}

void scanner_insert(struct scanner *scanner, char_t ch) {
    if (scanner->current_input == scanner->input_begin) {
        return;
    }
    struct input *tmp = malloc(sizeof(struct input));
    tmp->ch = ch;
    tmp->next = scanner->current_input;
    tmp->prev = scanner->current_input->prev;
    scanner->current_input->prev->next = tmp;
    scanner->current_input->prev = tmp;
    scanner->current_input = tmp;
}

void scanner_debug(struct scanner *scanner) {
    struct input *last = scanner->current_input;
    scanner_reset_begin(scanner);
    while (!scanner_is_end(scanner)) {
        char_t ch = scanner_next(scanner);
        if (scanner->current_input->prev == last) {
            wprintf(L"[%lc]", ch);
        } else {
            wprintf(L"%lc", ch);
        }
    }
    wprintf(L"\n");
    scanner->current_input = last;
}

void scanner_merge(struct scanner *dst, struct scanner *src) {
    struct input *last = src->current_input;
    scanner_reset_end(src);
    while (!scanner_is_begin(src)) {
        scanner_insert(dst, scanner_prev(src));
    }
    src->current_input = last;
}

bool scanner_is_end(struct scanner *scanner) {
    return scanner->current_input == scanner->input_end;
}

bool scanner_is_begin(struct scanner *scanner) {
    return scanner->current_input->prev == scanner->input_begin;
}

char_t scanner_next(struct scanner *scanner) {
    char_t ch = scanner->current_input->ch;
    scanner->current_input = scanner->current_input->next;
    return ch;
}

char_t scanner_prev(struct scanner *scanner) {
    scanner->current_input = scanner->current_input->prev;
    return scanner->current_input->ch;
}

void scanner_reset_begin(struct scanner *scanner) {
    scanner->current_input = scanner->input_begin->next;
}

void scanner_reset_end(struct scanner *scanner) {
    scanner->current_input = scanner->input_end;
}
