#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "regexp.h"
#include "fa.h"

#define SHOW_ERROR(...) \
do { \
printf(__VA_ARGS__); \
abort(); \
} while (0)

struct regexp *regexp_init(char_t *pattern) {
    struct regexp *rexp = (struct regexp *) malloc(sizeof(struct regexp));
    rexp->regexp = pattern;
    rexp->cur_pos = 0;
    return rexp;
}

struct charset_union *get_charset_union(char_t from, char_t to, struct charset_union *next) {
    struct charset_union *charset_union = (struct charset_union *) malloc(sizeof(struct charset_union));
    charset_union->from = from;
    charset_union->to = to;
    charset_union->next = next;
    return charset_union;
}

struct charset_union *
get_charset_union_by_escape_seq(const char_t *escape_seq, struct charset_union *next, const char_t **look_at) {
    switch (escape_seq[0]) {
        case 'n':
            *look_at = escape_seq + 1;
            return get_charset_union('\n', '\n', next);
        case 'r':
            *look_at = escape_seq + 1;
            return get_charset_union('\r', '\r', next);
        case 't':
            *look_at = escape_seq + 1;
            return get_charset_union('\t', '\t', next);
        case '\\':
            *look_at = escape_seq + 1;
            return get_charset_union('\\', '\\', next);
        case '|':
            *look_at = escape_seq + 1;
            return get_charset_union('|', '|', next);
        case '*':
            *look_at = escape_seq + 1;
            return get_charset_union('*', '*', next);
        case '+':
            *look_at = escape_seq + 1;
            return get_charset_union('+', '+', next);
        case '?':
            *look_at = escape_seq + 1;
            return get_charset_union('?', '?', next);
        case '{':
            *look_at = escape_seq + 1;
            return get_charset_union('{', '{', next);
        case '}':
            *look_at = escape_seq + 1;
            return get_charset_union('}', '}', next);
        case '(':
            *look_at = escape_seq + 1;
            return get_charset_union('(', '(', next);
        case ')':
            *look_at = escape_seq + 1;
            return get_charset_union(')', ')', next);
        case '[':
            *look_at = escape_seq + 1;
            return get_charset_union('[', '[', next);
        case ']':
            *look_at = escape_seq + 1;
            return get_charset_union(']', ']', next);
        case '-':
            *look_at = escape_seq + 1;
            return get_charset_union('-', '-', next);
        case '.':
            *look_at = escape_seq + 1;
            return get_charset_union('.', '.', next);
        case '^':
            *look_at = escape_seq + 1;
            return get_charset_union('^', '^', next);
        case 'd': // A-Za-z0-9_
            *look_at = escape_seq + 1;
            return get_charset_union('A', 'Z', get_charset_union('a', 'z', get_charset_union('0', '9',
                                                                                             get_charset_union('_', '_',
                                                                                                               next))));
    }
    SHOW_ERROR("unknown escape sequence %%%c", escape_seq[0]);
    return NULL;
}

struct regexp_token *regexp_next_token(struct regexp *regexp) {
    unsigned int cur = regexp->cur_pos;

    char_t current_ch = regexp->regexp[cur];

    struct regexp_token *token = (struct regexp_token *) malloc(sizeof(struct regexp_token));

    switch (current_ch) {
        case '\0':
            free(token);
            return NULL;
        case '|':
            token->type = ALTERNATION;
            cur++;
            break;
        case '*':
            token->type = REPETITIVE;
            cur++;
            break;
        case '+':
            token->type = ONE_MORE_REPETITIVE;
            cur++;
            break;
        case '?':
            token->type = ZERO_OR_ONE;
            cur++;
            break;
        case '(':
            token->type = L_PAR;
            cur++;
            break;
        case ')':
            token->type = R_PAR;
            cur++;
            break;
        case '{': {
            token->type = SUB_REGEXP;
            char_t *look_at = regexp->regexp + cur + 1;
            size_t length = 0;
            while (*look_at != '}') {
                if (*look_at == '\0') {
                    SHOW_ERROR("%s", "expected }");
                    return NULL;
                }
                look_at++;
                length++;
            }
            token->desc.sub_regexp_name = (char_t *) malloc(sizeof(char_t) * (length + 1));
            memcpy(token->desc.sub_regexp_name, regexp->regexp + cur + 1, length * sizeof(char_t));
            token->desc.sub_regexp_name[length] = '\0';
            cur += look_at - (regexp->regexp + cur) + 1;
            break;
        }
        case '\\': {
            token->type = CHAR_SET;
            token->desc.charset = (struct charset *) malloc(sizeof(struct charset));
            token->desc.charset->reversed = false;
            char_t *look_at;
            token->desc.charset->charset_union = get_charset_union_by_escape_seq(regexp->regexp + cur + 1, NULL,
                                                                                 (const char_t **) &look_at);
            cur += look_at - (regexp->regexp + cur);
            break;
        }
        case '.': {
            token->type = CHAR_SET;
            token->desc.charset = (struct charset *) malloc(sizeof(struct charset));
            token->desc.charset->reversed = false;
            token->desc.charset->charset_union = get_charset_union('\0', '\0', NULL);
            cur++;
            break;
        }
        case '[': {
            token->type = CHAR_SET;
            token->desc.charset = (struct charset *) malloc(sizeof(struct charset));
            char_t *look_at = regexp->regexp + cur + 1;
            bool reversed = false;
            if (*look_at == '^') {
                reversed = true;
                look_at++;
            }
            token->desc.charset->reversed = reversed;
            struct charset_union *cur_charset_union = NULL;
            while (*look_at != ']') {
                if (*look_at == '\0') {
                    SHOW_ERROR("expected ]");
                    return NULL;
                }
                if (*look_at == '\\') {
                    look_at++;
                    cur_charset_union = get_charset_union_by_escape_seq(look_at, cur_charset_union,
                                                                        (const char_t **) &look_at);
                } else {
                    if (*(look_at + 1) == '-') {
                        char_t to = *(look_at + 2);
                        if (to == '\0') {
                            SHOW_ERROR("excepted valid range end");
                            return NULL;
                        }
                        cur_charset_union = get_charset_union(*look_at, to, cur_charset_union);
                        look_at += 3;
                    } else {
                        cur_charset_union = get_charset_union(*look_at, *look_at, cur_charset_union);
                        look_at++;
                    }
                }
            }
            token->desc.charset->charset_union = cur_charset_union;
            cur += look_at - (regexp->regexp + cur) + 1;
            break;
        }
        default: {
            token->type = CHAR_SET;
            token->desc.charset = (struct charset *) malloc(sizeof(struct charset));
            token->desc.charset->reversed = false;
            token->desc.charset->charset_union = get_charset_union(current_ch, current_ch, NULL);
            cur++;
            break;
        }
    }

    regexp->cur_pos = cur;

    return token;
}

void regexp_consumed_token(struct regexp_token *token) {
    switch (token->type) {
        case SUB_REGEXP:
            free(token->desc.sub_regexp_name);
            break;
        case CHAR_SET: {
            struct charset_union *cu = token->desc.charset->charset_union;
            while (cu != NULL) {
                struct charset_union *tmp = cu->next;
                free(cu);
                cu = tmp;
            }
            free(token->desc.charset);
            break;
        }
        default:
            break;
    }
    free(token);
}

void operand_stack_push(struct regexp_token *token, struct regexp_operand_stack **operand_stack) {
    struct regexp_operand_stack *tmp = (struct regexp_operand_stack *) malloc(
            sizeof(struct regexp_operand_stack));
    tmp->ast = (struct regexp_ast *) malloc(
            sizeof(struct regexp_ast));
    tmp->ast->token = token;
    tmp->ast->l_operand = NULL;
    tmp->ast->r_operand = NULL;
    tmp->ast->item = NULL;
    tmp->next = *operand_stack;
    *operand_stack = tmp;
}

inline int get_priority(struct regexp_token *token) {
    switch (token->type) {
        case REPETITIVE:
        case ZERO_OR_ONE:
        case ONE_MORE_REPETITIVE:
            return 3;
        case CONCATENATION:
            return 2;
        case ALTERNATION:
            return 1;
        case STUB:
            return 0;
        default:
            break;
    }
    return -1;
}

void operator_stack_push(struct regexp_token *token, struct regexp_operator_stack **operator_stack,
                         struct regexp_operand_stack **operand_stack) {
    struct regexp_operator_stack *top = *operator_stack;

    while (token->type != L_PAR && top != NULL && top->token->type != L_PAR &&
           (get_priority(token) <= get_priority(top->token) || token->type == R_PAR)) {
        switch (top->token->type) {
            // Binary-op
            case ALTERNATION:
            case CONCATENATION: {
                struct regexp_ast *r_operand = (*operand_stack)->ast;
                *operand_stack = (*operand_stack)->next;
                struct regexp_ast *l_operand = (*operand_stack)->ast;
                struct regexp_operand_stack *result_operand = (struct regexp_operand_stack *) malloc(
                        sizeof(struct regexp_operand_stack));
                result_operand->ast = (struct regexp_ast *) malloc(
                        sizeof(struct regexp_ast));
                result_operand->ast->l_operand = l_operand;
                result_operand->ast->r_operand = r_operand;
                result_operand->ast->item = NULL;
                result_operand->ast->token = top->token;
                result_operand->next = (*operand_stack)->next;
                *operand_stack = result_operand;
                break;
            }

                // Unary-op
            case REPETITIVE:
            case ONE_MORE_REPETITIVE:
            case ZERO_OR_ONE: {
                struct regexp_ast *operand = (*operand_stack)->ast;
                struct regexp_operand_stack *result_operand = (struct regexp_operand_stack *) malloc(
                        sizeof(struct regexp_operand_stack));
                result_operand->ast = (struct regexp_ast *) malloc(
                        sizeof(struct regexp_ast));
                result_operand->ast->l_operand = operand;
                result_operand->ast->r_operand = NULL;
                result_operand->ast->token = top->token;
                result_operand->ast->item = NULL;
                result_operand->next = (*operand_stack)->next;
                *operand_stack = result_operand;
                break;
            }

            default:
                break;
        }
        top = top->next;
    }

    if (top != NULL && (token->type == R_PAR && top->token->type == L_PAR)) {
        top = top->next;
    }

    struct regexp_operator_stack *tmp;
    if (token->type != R_PAR) {
        tmp = (struct regexp_operator_stack *) malloc(
                sizeof(struct regexp_operator_stack));
        tmp->next = top;
        tmp->token = token;
    } else {
        tmp = top;
    }
    *operator_stack = tmp;
}

struct regexp_ast *regexp_parse(struct regexp *regexp, struct slex_item *regexp_items) {
    struct regexp_token *token = regexp_next_token(regexp);

    struct regexp_operator_stack *operator_stack = NULL;
    struct regexp_operand_stack *operand_stack = NULL;

    struct regexp_token *concatenation_token = (struct regexp_token *) malloc(sizeof(struct regexp_token));
    concatenation_token->type = CONCATENATION;

    while (token != NULL) {
        switch (token->type) {
            case CHAR_SET:
                operand_stack_push(token, &operand_stack);
                break;

            case SUB_REGEXP: {
                struct regexp_operand_stack *tmp = (struct regexp_operand_stack *) malloc(
                        sizeof(struct regexp_operand_stack));
                struct slex_item *item = regexp_item_lookup(token->desc.sub_regexp_name, regexp_items);
                if (item == NULL || item->ast == NULL) {
                    SHOW_ERROR("unknown regexp item, lookup failed");
                    return NULL;
                }
                tmp->ast = item->ast;
                tmp->next = operand_stack;
                operand_stack = tmp;
                break;
            }

            case ALTERNATION:
            case REPETITIVE:
            case ONE_MORE_REPETITIVE:
            case ZERO_OR_ONE:
            case L_PAR:
            case R_PAR:
                operator_stack_push(token, &operator_stack, &operand_stack);
                break;

            default:
                break;
        }
        struct regexp_token *last_token = token;
        token = regexp_next_token(regexp);
        if (token == NULL) {
            break;
        }
        if ((last_token->type != ALTERNATION && last_token->type != L_PAR) &&
            (token->type == CHAR_SET || token->type == SUB_REGEXP || token->type == L_PAR)) {
            operator_stack_push(concatenation_token, &operator_stack, &operand_stack);
        }
    }


    struct regexp_token *stub = (struct regexp_token *) malloc(sizeof(struct regexp_token));
    stub->type = STUB;
    operator_stack_push(stub, &operator_stack, &operand_stack);

    if (operand_stack == NULL) {
        return NULL;
    }
    return operand_stack->ast;
}

struct dfs_stack_node {
    struct regexp_ast *ast;
    struct dfs_stack_node *next;
};

void insert_to_belongs_to(unsigned int charset, struct char_belongs_to **head) {
    struct char_belongs_to *tmp = malloc(sizeof(struct char_belongs_to));
    tmp->charset = charset;
    tmp->next = *head;
    *head = tmp;
}

bool is_charset_equal(struct char_belongs_to *h1, struct char_belongs_to *h2) {
    while (h1 != NULL && h2 != NULL && h1->charset == h2->charset) {
        h1 = h1->next;
        h2 = h2->next;
    }

    return h1 == NULL && h2 == NULL;
}

charclass_t regexp_parse_charclass(struct slex_item *items, charclass_t **charclass_save) {
    charclass_t *charclass = malloc(sizeof(charclass_t) * (SYS_ALPHABET_END + 1));
    for (char_t ch = 0; ch <= SYS_ALPHABET_END; ch++) {
        charclass[ch] = INVALID_CHAR;
    }

    struct char_desc *sys_alphabet = malloc(sizeof(struct char_desc) * (SYS_ALPHABET_END + 1));
    for (char_t ch = SYS_ALPHABET_START; ch <= SYS_ALPHABET_END; ch++) {
        sys_alphabet[ch].belongs_to = NULL;
    }

    struct charset_bucket_item **charset_hashtable = malloc(
            sizeof(struct charset_bucket_item *) * CHARSET_BUCKET_HASH_MOD);
    for (unsigned int i = 0; i < CHARSET_BUCKET_HASH_MOD; i++) {
        charset_hashtable[i] = NULL;
    }

    unsigned int n_charset = 0;

    struct slex_item *item = items;

    while (item != NULL) {
        struct dfs_stack_node *top = malloc(sizeof(struct dfs_stack_node));
        top->next = NULL;
        top->ast = item->ast;

        while (top != NULL) {
            struct dfs_stack_node *cur = top;
            top = top->next;

            if (cur->ast->l_operand == NULL && cur->ast->r_operand == NULL) {
                if (cur->ast->token->type == CHAR_SET) {
                    struct charset *charset = cur->ast->token->desc.charset;
                    struct charset_union *p = charset->charset_union;

                    bool *used = malloc(sizeof(bool) * (SYS_ALPHABET_END + 1));
                    used[0] = false;
                    for (char_t i = 1; i <= SYS_ALPHABET_END; i++) {
                        used[i] = charset->reversed;
                    }

                    while (p != NULL) {
                        if (p->from == '\0') {
                            p->from = SYS_ALPHABET_START;
                        }
                        if (p->to == '\0') {
                            p->to = SYS_ALPHABET_END;
                        }
                        for (char_t ch = p->from; ch <= p->to; ch++) {
                            used[ch] = !charset->reversed;
                        }
                        p = p->next;
                    }

                    for (char_t ch = 0; ch <= SYS_ALPHABET_END; ch++) {
                        if (!used[ch]) {
                            continue;
                        }
                        insert_to_belongs_to(n_charset, &sys_alphabet[ch].belongs_to);
                    }

                    n_charset++;
                    free(used);
                }
            }

            if (cur->ast->l_operand != NULL) {
                struct dfs_stack_node *tmp = malloc(sizeof(struct dfs_stack_node));
                tmp->next = top;
                tmp->ast = cur->ast->l_operand;
                top = tmp;
            }

            if (cur->ast->r_operand != NULL) {
                struct dfs_stack_node *tmp = malloc(sizeof(struct dfs_stack_node));
                tmp->next = top;
                tmp->ast = cur->ast->r_operand;
                top = tmp;
            }

            free(cur);
        }

        item = item->next;
    }

    charclass_t n_charclass = 0;

    for (char_t ch = SYS_ALPHABET_START; ch <= SYS_ALPHABET_END; ch++) {
        unsigned int hash = 0, base = 1;
        struct char_belongs_to *p = sys_alphabet[ch].belongs_to;
        while (p != NULL) {
            hash += (base * (p->charset)) % CHARSET_BUCKET_HASH_MOD;
            hash %= CHARSET_BUCKET_HASH_MOD;
            base *= CHARSET_BUCKET_HASH_BASE;
            base %= CHARSET_BUCKET_HASH_MOD;
            p = p->next;
        }

        bool found_exist = false;
        struct charset_bucket_item *bucket = charset_hashtable[hash];
        while (bucket != NULL) {
            if (is_charset_equal(bucket->charset, sys_alphabet[ch].belongs_to)) {
                found_exist = true;
                break;
            }
            bucket = bucket->next;
        }

        if (!found_exist) {
            n_charclass++;
            struct charset_bucket_item *bucket_item = malloc(sizeof(struct charset_bucket_item));
            bucket_item->charset = sys_alphabet[ch].belongs_to;
            bucket_item->next = charset_hashtable[hash];
            bucket_item->charclass = n_charclass;
            charset_hashtable[hash] = bucket_item;
            charclass[ch] = n_charclass;
        } else {
            charclass[ch] = bucket->charclass;
        }
    }

    for (char_t ch = SYS_ALPHABET_START; ch <= SYS_ALPHABET_END; ch++) {
        struct char_belongs_to *p = sys_alphabet[ch].belongs_to;
        while (p != NULL) {
            struct char_belongs_to *tmp = p;
            p = p->next;
            free(tmp);
        }
    }
    free(sys_alphabet);

    for (unsigned int i = 0; i < CHARSET_BUCKET_HASH_MOD; i++) {
        struct charset_bucket_item *bucket = charset_hashtable[i];
        while (bucket != NULL) {
            struct charset_bucket_item *tmp = bucket;
            bucket = bucket->next;
            free(tmp);
        }
    }
    free(charset_hashtable);

    *charclass_save = charclass;
    return n_charclass;
}

void regexp_add_nfa_trans(state_t state, char_t input_ch, state_t target_state, struct slex_item *item,
                          struct nfa_trans **head) {
    struct nfa_trans *new_trans = malloc(sizeof(struct nfa_trans));
    new_trans->state = state;
    new_trans->charclass = input_ch;
    new_trans->target_state = target_state;
    new_trans->next = *head;
    new_trans->item = item;
    *head = new_trans;
}

state_t tree_to_nfa(state_t start_state, struct regexp_ast *ast, struct nfa_trans **head, charclass_t n_charclass,
                    charclass_t *charclass) {
    switch (ast->token->type) {
        case ALTERNATION: {
            regexp_add_nfa_trans(start_state, EPS, start_state + 1, NULL, head);
            state_t left_dfa_end_state = tree_to_nfa(start_state + 1, ast->l_operand, head, n_charclass, charclass);

            regexp_add_nfa_trans(start_state, EPS, left_dfa_end_state + 1, NULL, head);
            state_t right_dfa_end_state = tree_to_nfa(left_dfa_end_state + 1, ast->r_operand, head, n_charclass,
                                                      charclass);

            regexp_add_nfa_trans(left_dfa_end_state, EPS, right_dfa_end_state + 1, ast->item, head);
            regexp_add_nfa_trans(right_dfa_end_state, EPS, right_dfa_end_state + 1, ast->item, head);

            return right_dfa_end_state + 1;
        }

        case REPETITIVE: {
            regexp_add_nfa_trans(start_state, EPS, start_state + 1, NULL, head);
            state_t end_state = tree_to_nfa(start_state + 1, ast->l_operand, head, n_charclass, charclass);
            regexp_add_nfa_trans(start_state, EPS, end_state + 1, NULL, head);
            regexp_add_nfa_trans(end_state + 1, EPS, start_state + 1, NULL, head);
            regexp_add_nfa_trans(end_state, EPS, end_state + 1, ast->item, head);

            return end_state + 1;
        }

        case ONE_MORE_REPETITIVE: {
            regexp_add_nfa_trans(start_state, EPS, start_state + 1, NULL, head);
            state_t end_state = tree_to_nfa(start_state + 1, ast->l_operand, head, n_charclass, charclass);
            regexp_add_nfa_trans(end_state + 1, EPS, start_state + 1, NULL, head);
            regexp_add_nfa_trans(end_state, EPS, end_state + 1, ast->item, head);

            return end_state + 1;
        }

        case ZERO_OR_ONE: {
            regexp_add_nfa_trans(start_state, EPS, start_state + 1, NULL, head);
            state_t end_state = tree_to_nfa(start_state + 1, ast->l_operand, head, n_charclass, charclass);
            regexp_add_nfa_trans(end_state, EPS, end_state + 1, NULL, head);
            regexp_add_nfa_trans(start_state, EPS, end_state + 1, ast->item, head);

            return end_state + 1;
        }

        case CHAR_SET: {
            struct charset *charset = ast->token->desc.charset;
            struct charset_union *p = charset->charset_union;

            bool *used = malloc(sizeof(bool) * (n_charclass + 1));
            used[0] = false;
            for (unsigned int i = 1; i <= n_charclass; i++) {
                used[i] = charset->reversed;
            }

            while (p != NULL) {
                if (p->from == '\0') {
                    p->from = SYS_ALPHABET_START;
                }
                if (p->to == '\0') {
                    p->to = SYS_ALPHABET_END;
                }
                for (char_t ch = p->from; ch <= p->to; ch++) {
                    used[charclass[ch]] = !charset->reversed;
                }
                p = p->next;
            }

            for (charclass_t i = 0; i <= n_charclass; i++) {
                if (!used[i]) {
                    continue;
                }
                regexp_add_nfa_trans(start_state, i, start_state + 1, ast->item, head);
            }

            free(used);
            return start_state + 1;
        }

        case CONCATENATION: {
            regexp_add_nfa_trans(start_state, EPS, start_state + 1, NULL, head);

            state_t left_dfa_end_state = tree_to_nfa(start_state + 1, ast->l_operand, head, n_charclass, charclass);
            regexp_add_nfa_trans(left_dfa_end_state, EPS, left_dfa_end_state + 1, NULL, head);

            state_t right_dfa_end_state = tree_to_nfa(left_dfa_end_state + 1, ast->r_operand, head, n_charclass,
                                                      charclass);
            regexp_add_nfa_trans(right_dfa_end_state, EPS, right_dfa_end_state + 1, ast->item, head);

            return right_dfa_end_state + 1;
        }

        default:
            break;
    }
    return NO_STATE;
}

void
regexp_body_add(unsigned int token_class, char_t *pattern, struct slex_item **head) {
    regexp_mixd_add(token_class, NULL, pattern, head);
}

void
regexp_mixd_add(unsigned int token_class, char_t *name, char_t *pattern, struct slex_item **head) {
    struct slex_item *tmp = malloc(sizeof(struct slex_item));
    tmp->name = name;
    tmp->token_class = token_class;
    tmp->priority = (*head == NULL ? 0 : ((*head)->priority + 1));
    struct regexp *regexp = regexp_init(pattern);
    tmp->ast = regexp_parse(regexp, *head);
    tmp->next = *head;
    *head = tmp;
}

void regexp_decl_add(char_t *name, char_t *pattern, struct slex_item **head) {
    struct slex_item *tmp = malloc(sizeof(struct slex_item));
    tmp->name = name;
    tmp->priority = -1; // unsigned -1
    struct regexp *regexp = regexp_init(pattern);
    tmp->ast = regexp_parse(regexp, *head);
    tmp->next = *head;
    *head = tmp;
}

struct slex_item *regexp_item_lookup(const char_t *name, struct slex_item *head) {
    struct slex_item *p = head;
    while (p != NULL) {
        if (p->name != NULL && wcscmp(p->name, name) == 0) {
            return p;
        }
        p = p->next;
    }
    return NULL;
}

struct regexp_fa_accept_states {
    state_t state;
    struct regexp_fa_accept_states *next;
};

void
regexp_fa_build(struct slex_item *items, unsigned int *n_dfa_state, unsigned int *dfa_start_state, state_t ***dfa_delta,
                struct state_set ***dfa_states, charclass_t *n_charclass_save, charclass_t **charclass_save) {
    charclass_t *charclass = NULL;
    unsigned int n_charclass = regexp_parse_charclass(items, &charclass);

    *n_charclass_save = n_charclass;
    *charclass_save = charclass;

    struct nfa_trans *head = NULL;

    struct slex_item *item = items;
    struct regexp_fa_accept_states *accept_states = NULL;
    state_t current_state = 0;
    while (item != NULL) {
        regexp_add_nfa_trans(0, EPS, current_state + 1, NULL, &head);
        item->ast->item = item;
        current_state = tree_to_nfa(current_state + 1, item->ast, &head, n_charclass, charclass);

        struct regexp_fa_accept_states *accept_state = malloc(sizeof(struct regexp_fa_accept_states));
        accept_state->state = current_state;
        accept_state->next = accept_states;
        accept_states = accept_state;

        item = item->next;
    }

    unsigned int n_nfa_states = current_state + 1;

    struct nfa_trans *trans = head;
    struct state_set ***nfa = nfa_init(n_nfa_states, n_charclass);
    while (trans != NULL) {
        nfa_trans_add(trans->state, trans->charclass, trans->target_state, trans->item, nfa);
        trans = trans->next;
    }

    bool *nfa_is_accept_state = init_nfa_accept_states(n_nfa_states);
    while (accept_states != NULL) {
        add_nfa_accept_state(accept_states->state, nfa_is_accept_state);

        struct regexp_fa_accept_states *tmp = accept_states;
        accept_states = accept_states->next;
        free(tmp);
    }

    struct raw_dfa_edge_link **raw_dfa_edge = init_raw_dfa_edge(MAX_ALLOWED_RAW_DFA_STATES);
    unsigned int raw_dfa_n_state;
    state_t raw_dfa_start_state;
    struct state_set **raw_dfa_states = init_raw_dfa_states(MAX_ALLOWED_RAW_DFA_STATES);
    nfa_to_raw_dfa(raw_dfa_edge, &raw_dfa_n_state, &raw_dfa_start_state, raw_dfa_states, nfa, nfa_is_accept_state, 0,
                   n_charclass);
    dfa_minimize(raw_dfa_edge, raw_dfa_start_state, (const struct state_set **) raw_dfa_states, raw_dfa_n_state,
                 n_dfa_state, dfa_start_state, dfa_delta, dfa_states, n_charclass);
}
