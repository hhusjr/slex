//
// Created by 沈俊儒 on 2020/5/23.
//
#include <stdbool.h>
#include "fa.h"

#ifndef DEMO_REGEXP_H
#define DEMO_REGEXP_H

#define INVALID_CHAR -1

#define CHARSET_BUCKET_HASH_MOD 233
#define CHARSET_BUCKET_HASH_BASE 19

#define MAX_ALLOWED_RAW_DFA_STATES 1024

struct char_belongs_to {
    unsigned int charset;
    struct char_belongs_to *next;
};

struct char_desc {
    struct char_belongs_to *belongs_to;
};

struct charset_bucket_item {
    struct char_belongs_to *charset;
    struct charset_bucket_item *next;
    charclass_t charclass;
};

struct charset_union {
    char_t from, to;
    struct charset_union *next;
};
struct charset {
    struct charset_union *charset_union;
    bool reversed;
};

enum regexp_token_type {
    ALTERNATION,
    REPETITIVE,
    ONE_MORE_REPETITIVE,
    ZERO_OR_ONE,
    CHAR_SET,
    SUB_REGEXP,
    L_PAR,
    R_PAR,
    CONCATENATION,

    STUB
};
union regexp_token_desc {
    char_t *sub_regexp_name;
    struct charset *charset;
};
struct regexp_token {
    enum regexp_token_type type;
    union regexp_token_desc desc;
};
struct regexp_operator_stack {
    struct regexp_token *token;
    struct regexp_operator_stack *next;
};

struct regexp {
    char_t *regexp;
    unsigned int cur_pos;
};
struct regexp_ast {
    struct regexp_token *token;
    struct regexp_ast *l_operand;
    struct regexp_ast *r_operand;

    struct slex_item *item;
};
struct regexp_operand_stack {
    struct regexp_ast *ast;
    struct regexp_operand_stack *next;
};

struct regexp *regexp_init(char_t *pattern);

struct charset_union *get_charset_union(char_t from, char_t to, struct charset_union *next);

struct charset_union *
get_charset_union_by_escape_seq(const char_t *escape_seq, struct charset_union *next, const char_t **look_at);

struct regexp_token *regexp_next_token(struct regexp *regexp);

void regexp_consumed_token(struct regexp_token *token);

void operand_stack_push(struct regexp_token *token, struct regexp_operand_stack **operand_stack);

void operator_stack_push(struct regexp_token *token, struct regexp_operator_stack **operator_stack,
                         struct regexp_operand_stack **operand_stack);

struct regexp_ast *regexp_parse(struct regexp *regexp, struct slex_item *regexp_items);

int get_priority(struct regexp_token *token);

charclass_t regexp_parse_charclass(struct slex_item *items, charclass_t **charclass_save);

struct nfa_trans {
    state_t state;
    charclass_t charclass;
    state_t target_state;
    struct slex_item *item;

    struct nfa_trans *next;
};

state_t tree_to_nfa(state_t start_state, struct regexp_ast *ast, struct nfa_trans **head, charclass_t n_charclass,
                    charclass_t *charclass);

void
regexp_body_add(unsigned int token_class, char_t *pattern, struct slex_item **head);

void regexp_decl_add(char_t *name, char_t *pattern, struct slex_item **head);

void
regexp_mixd_add(unsigned int token_class, char_t *name, char_t *pattern, struct slex_item **head);

struct slex_item *regexp_item_lookup(const char_t *name, struct slex_item *head);

void
regexp_fa_build(struct slex_item *items, unsigned int *n_dfa_state, unsigned int *dfa_start_state, state_t ***dfa_delta,
                struct state_set ***dfa_states, charclass_t *n_charclass_save, charclass_t **charclass_save);

#endif //DEMO_REGEXP_H
