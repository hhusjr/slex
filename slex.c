//
// Created by 沈俊儒 on 2020/5/25.
//

#include <stdlib.h>
#include "string.h"
#include <unistd.h>
#include "slex.h"
#include "regexp.h"

struct slex_config *slex_compile(struct slex_item *regexps) {
    unsigned int n_dfa_state;
    unsigned int dfa_start_state;
    state_t **dfa_delta;
    struct state_set **dfa_states;
    charclass_t n_charclass;
    charclass_t *charclass;
    regexp_fa_build(regexps, &n_dfa_state, &dfa_start_state, &dfa_delta, &dfa_states, &n_charclass, &charclass);

    struct slex_config *config = malloc(sizeof(struct slex_config));
    config->charclass = charclass;
    config->delta = dfa_delta;
    config->n_charclass = n_charclass;
    config->n_state = n_dfa_state;
    config->start_state = dfa_start_state;
    config->token_class = malloc(sizeof(unsigned int) * n_dfa_state);
    for (int i = 0; i < n_dfa_state; i++) {
        config->token_class[i] = !dfa_states[i]->is_accept ? REJECT : dfa_states[i]->item->token_class;
    }

    return config;
}

struct slex_config *slex_from_file(const char *path) {
    FILE *fp = fopen(path, "rb");
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    uint8_t *buf = malloc(size);
    fseek(fp, 0, SEEK_SET);
    fread(buf, 1, size, fp);

    struct slex_config *config = malloc(sizeof(struct slex_config));
    uint8_t *p = buf;
    config->n_state = *((state_t *) p);
    p += sizeof(state_t);
    config->start_state = *((state_t *) p);
    p += sizeof(state_t);
    config->n_charclass = *((charclass_t *) p);
    p += sizeof(charclass_t);

    config->delta = malloc(sizeof(state_t *) * config->n_state);
    for (state_t i = 0; i < config->n_state; i++) {
        config->delta[i] = malloc(sizeof(state_t) * config->n_charclass);
        for (charclass_t j = 0; j <= config->n_charclass; j++) {
            config->delta[i][j] = *((state_t *) p);
            p += sizeof(state_t);
        }
    }

    config->token_class = malloc(sizeof(unsigned int) * config->n_state);
    memcpy(config->token_class, p, sizeof(unsigned int) * config->n_state);
    p += sizeof(unsigned int) * config->n_state;

    config->charclass = malloc(sizeof(charclass_t) * (SYS_ALPHABET_END + 1));
    memcpy(config->charclass, p, sizeof(charclass_t) * (SYS_ALPHABET_END + 1));

    fclose(fp);
    free(buf);

    return config;
}

void slex_save(const char *path, struct slex_config *config) {
    /*
     * format:
     * n_state start_state n_charclass delta token_class charclass
     */
    size_t size = sizeof(state_t) + sizeof(state_t) + sizeof(charclass_t); // n_state start_state n_charclass
    size += sizeof(state_t) * config->n_state * config->n_charclass; // delta
    size += sizeof(unsigned int) * config->n_state; // tokenclass
    size += sizeof(charclass_t) * (SYS_ALPHABET_END + 1); // charclass

    uint8_t *buf = malloc(size);
    uint8_t *p = buf;

    *((state_t *) p) = config->n_state;
    p += sizeof(state_t);
    *((state_t *) p) = config->start_state;
    p += sizeof(state_t);
    *((charclass_t *) p) = config->n_charclass;
    p += sizeof(charclass_t);

    for (state_t i = 0; i < config->n_state; i++) {
        for (charclass_t j = 0; j <= config->n_charclass; j++) {
            *((state_t *) p) = config->delta[i][j];
            p += sizeof(state_t);
        }
    }

    for (unsigned int i = 0; i < config->n_state; i++) {
        *((unsigned int *) p) = config->token_class[i];
        p += sizeof(unsigned int);
    }

    for (charclass_t i = 0; i <= SYS_ALPHABET_END; i++) {
        *((charclass_t *) p) = config->charclass[i];
        p += sizeof(charclass_t);
    }

    FILE *fp = fopen(path, "wb");
    fwrite(buf, 1, size, fp);
    fflush(fp);
    fclose(fp);

    free(buf);
}

bool
slex(struct slex_config *config, struct scanner *scanner, callback_t callback, void *args) {
    state_t current_state = config->start_state;
    state_t last_accept_state = NO_STATE;
    struct input *current_lexeme_pointer = scanner->current_input;
    struct input *last_accept_state_lexeme_pointer = NULL;

    while (true) {
        bool terminate = false;
        char_t ch;
        if (!scanner_is_end(scanner)) {
            ch = scanner_next(scanner);
            if (ch < SYS_ALPHABET_START || ch > SYS_ALPHABET_END) {
                wprintf(L"Unexpected character %lc\n", ch);
                return false;
            }
            unsigned int idx = config->charclass[ch];
            if (idx <= 0 || idx > config->n_charclass) {
                wprintf(L"Unexpected character %lc\n", ch);
                return false;
            }

            current_state = config->delta[current_state][idx];
            if (current_state != NO_STATE && config->token_class[current_state] != REJECT) {
                last_accept_state = current_state;
                last_accept_state_lexeme_pointer = scanner->current_input;
            }
        } else {
            current_state = NO_STATE;
            terminate = true;
        }

        if (current_state == NO_STATE && current_lexeme_pointer != scanner->input_end) {
            current_state = last_accept_state;

            scanner->current_input = current_lexeme_pointer;
            size_t length = 0;
            while (scanner->current_input != last_accept_state_lexeme_pointer) {
                scanner_next(scanner);
                length++;
            }
            char_t *lexeme = malloc(sizeof(char_t) * (length + 1));
            scanner->current_input = current_lexeme_pointer;
            size_t cur = 0;
            while (scanner->current_input != last_accept_state_lexeme_pointer) {
                lexeme[cur++] = scanner_next(scanner);
            }
            lexeme[length] = '\0';

            bool more = false;
            callback(lexeme, length, config->token_class[current_state], &more, scanner, args);
            free(lexeme);

            current_state = config->start_state;
            if (!more) {
                current_lexeme_pointer = scanner->current_input;
            }
        }

        if (terminate) {
            break;
        }
    }
    return true;
}

struct slex_config *slex_load(const char *path, struct slex_item *regexps) {
    if (access(path, F_OK) == -1) {
        struct slex_config *config = slex_compile(regexps);
        slex_save(path, config);
        return config;
    }
    return slex_from_file(path);
}
