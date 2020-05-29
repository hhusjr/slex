#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "fa.h"

struct state_set ***nfa_init(unsigned int n_nfa_states, unsigned int n_alphabet) {
    struct state_set ***nfa_edge = (struct state_set ***) malloc(sizeof(struct state_set **) * n_nfa_states);
    for (unsigned int i = 0; i < n_nfa_states; i++) {
        nfa_edge[i] = (struct state_set **) malloc(sizeof(struct state_set *) * (n_alphabet + 1));
        for (unsigned int j = 0; j <= n_alphabet; j++) {
            nfa_edge[i][j] = NULL;
        }
    }
    return nfa_edge;
}

inline bool *init_nfa_accept_states(unsigned int n_nfa_states) {
    bool *tmp = (bool *) malloc(sizeof(bool) * n_nfa_states);
    for (int i = 0; i < n_nfa_states; i++) {
        tmp[i] = false;
    }
    return tmp;
}

inline void add_nfa_accept_state(state_t state, bool *is_acceptable_state) {
    is_acceptable_state[state] = true;
}

unsigned int stateset_hash(struct state_set *set) {
    unsigned int hash = 0;
    for (int i = 0; i < STATE_SET_BUCKET_CNT; i++) {
        hash += (set->states[i] % STATE_SET_HASH_MOD);
        hash %= STATE_SET_HASH_MOD;
    }
    return hash;
}

void stateset_clear(struct state_set *set) {
    for (int i = 0; i < STATE_SET_BUCKET_CNT; i++) {
        set->states[i] = 0;
    }
}

struct state_set *stateset_init() {
    struct state_set *set = (struct state_set *) malloc(sizeof(struct state_set));
    stateset_clear(set);
    set->item = NULL;
    set->is_accept = false;
    return set;
}

void stateset_destroy(struct state_set *p) {
    free(p);
}

bool stateset_is_empty(struct state_set *set) {
    for (int i = 0; i < STATE_SET_BUCKET_CNT; i++) {
        if (set->states[i] != 0) {
            return false;
        }
    }

    return true;
}

void stateset_insert(struct state_set *set, state_t val, struct slex_item *item) {
    unsigned int bit_cnt_max = 8 * sizeof(uint64_t);
    unsigned int pos = val / bit_cnt_max;
    if (pos >= STATE_SET_BUCKET_CNT) {
        printf("stateset_insert: Too many dfa_states.");
        return;
    }

    unsigned int mask = val % bit_cnt_max;
    set->states[pos] |= 1ul << mask;

    if (item != NULL && (set->item == NULL || item->priority < set->item->priority)) {
        set->item = item;
    }
}

void stateset_merge(struct state_set *dst, const struct state_set *src) {
    for (int i = 0; i < STATE_SET_BUCKET_CNT; i++) {
        dst->states[i] |= src->states[i];
    }

    if (src->item == NULL) {
        return;
    }
    if (dst->item == NULL || src->item->priority < dst->item->priority) {
        dst->item = src->item;
    }
}

bool stateset_is_equal(const struct state_set *a, const struct state_set *b) {
    for (int i = 0; i < STATE_SET_BUCKET_CNT; i++) {
        if (a->states[i] != b->states[i]) {
            return false;
        }
    }

    return true;
}

void stateset_begin(struct state_set *set) {
    set->iter_cur = set->states[0];
    set->iter_idx = 0;
}

state_t stateset_next(struct state_set *set) {
    while (set->iter_cur == 0) {
        set->iter_idx++;
        if (set->iter_idx < STATE_SET_BUCKET_CNT) {
            set->iter_cur = set->states[set->iter_idx];
        } else {
            break;
        }
    }

    if (set->iter_idx >= STATE_SET_BUCKET_CNT || set->iter_cur == 0) {
        return NO_STATE;
    }

    uint64_t res = __builtin_ctzl(set->iter_cur) + sizeof(uint64_t) * 8 * set->iter_idx;
    set->iter_cur ^= set->iter_cur & (-set->iter_cur);

    return res;
}

void stateset_subscribe_is_accept(struct state_set *set, const bool *nfa_is_accept_state) {
    stateset_begin(set);
    state_t state = stateset_next(set);
    while (state != NO_STATE) {
        if (nfa_is_accept_state[state]) {
            set->is_accept = true;
            return;
        }
        state = stateset_next(set);
    }
    set->is_accept = false;
}

void nfa_trans_add(state_t state, unsigned int idx, state_t target_state, struct slex_item *item,
                   struct state_set ***nfa_edge) {
    if (nfa_edge[state][idx] == NULL) {
        nfa_edge[state][idx] = stateset_init();
    }
    stateset_insert(nfa_edge[state][idx], target_state, item);
}

void
add_to_state_set_map(unsigned int idx, struct state_set *state_set, struct states_in_hash_bucket **nfa_state_set_map) {
    struct states_in_hash_bucket **head = &nfa_state_set_map[stateset_hash(state_set)];

    struct states_in_hash_bucket *tmp = (struct states_in_hash_bucket *) malloc(sizeof(struct states_in_hash_bucket));
    tmp->state_set = state_set;
    tmp->next = *head;
    tmp->idx_in_states = idx;

    *head = tmp;
}

unsigned int get_index_by_state_set(struct state_set *state_set, struct states_in_hash_bucket **nfa_state_set_map) {
    struct states_in_hash_bucket *head = nfa_state_set_map[stateset_hash(state_set)];
    if (head == NULL) {
        return STATE_SET_NOT_FOUND;
    }

    struct states_in_hash_bucket *p = head;
    while (p != NULL) {
        if (stateset_is_equal(state_set, p->state_set)) {
            return p->idx_in_states;
        }
        p = p->next;
    }

    return STATE_SET_NOT_FOUND;
}

struct state_set *closure(struct state_set *s, struct state_set ***nfa_edge) {
    struct state_set *last_t = stateset_init();
    struct state_set *t = stateset_init();
    stateset_merge(t, s);
    do {
        stateset_clear(last_t);
        stateset_merge(last_t, t);
        stateset_begin(last_t);
        state_t state = stateset_next(last_t);
        while (state != NO_STATE) {
            if (nfa_edge[state][EPS] != NULL) {
                stateset_merge(t, nfa_edge[state][EPS]);
            }
            state = stateset_next(last_t);
        }
    } while (!stateset_is_equal(t, last_t));
    stateset_destroy(last_t);
    return t;
}

struct state_set *move(struct state_set *s, unsigned int idx, struct state_set ***nfa_edge) {
    struct state_set *t = stateset_init();
    if (s == NULL) {
        return t;
    }
    stateset_begin(s);
    state_t state = stateset_next(s);
    while (state != NO_STATE) {
        struct state_set *component = nfa_edge[state][idx];
        if (component != NULL) {
            stateset_merge(t, component);
        }
        state = stateset_next(s);
    }
    return t;
}

inline struct raw_dfa_edge_link **init_raw_dfa_edge(unsigned int max_allowed_raw_dfa_state) {
    struct raw_dfa_edge_link **raw_dfa_edge = (struct raw_dfa_edge_link **) malloc(
            sizeof(struct raw_dfa_edge_link *) * max_allowed_raw_dfa_state);
    for (int i = 0; i < max_allowed_raw_dfa_state; i++) {
        raw_dfa_edge[i] = NULL;
    }
    return raw_dfa_edge;
}

inline struct state_set **init_raw_dfa_states(unsigned int state_cnt) {
    struct state_set **dfa_states = (struct state_set **) malloc(state_cnt * sizeof(struct state_set *));
    for (int i = 0; i < state_cnt; i++) {
        dfa_states[i] = NULL;
    }
    return dfa_states;
}

void nfa_to_raw_dfa(struct raw_dfa_edge_link **raw_dfa_edge, unsigned int *raw_dfa_n_state,
                    state_t *raw_dfa_start_state, struct state_set **raw_dfa_states, struct state_set ***nfa_edge,
                    bool *nfa_is_accept_state, state_t nfa_start_state, unsigned int n_alphabet) {
    struct states_in_hash_bucket **nfa_state_set_map = malloc(
            sizeof(struct states_in_hash_bucket *) * STATE_SET_HASH_MOD);
    for (int i = 0; i < STATE_SET_HASH_MOD; i++) {
        nfa_state_set_map[i] = NULL;
    }

    struct state_set *q0 = stateset_init();
    stateset_insert(q0, nfa_start_state, NULL);
    raw_dfa_states[0] = closure(q0, nfa_edge);
    stateset_destroy(q0);
    *raw_dfa_start_state = 0;

    unsigned int current_state_index = 0, n_state_ = 1;
    while (current_state_index < n_state_) {
        for (unsigned int char_idx = 1; char_idx <= n_alphabet; char_idx++) {
            struct state_set *move_ch = move(raw_dfa_states[current_state_index], char_idx, nfa_edge);
            if (stateset_is_empty(move_ch)) {
                stateset_destroy(move_ch);
                continue;
            }
            struct state_set *state_set = closure(move_ch, nfa_edge);
            stateset_destroy(move_ch);
            unsigned int idx = get_index_by_state_set(state_set, nfa_state_set_map);

            if (idx != STATE_SET_NOT_FOUND) {
                raw_dfa_edge_insert(char_idx, idx, current_state_index, raw_dfa_edge);
                stateset_destroy(state_set);
            } else {
                raw_dfa_states[n_state_] = state_set;
                add_to_state_set_map(n_state_, state_set, nfa_state_set_map);
                raw_dfa_edge_insert(char_idx, n_state_, current_state_index, raw_dfa_edge);
                n_state_++;
            }
        }
        current_state_index++;
    }

    free(nfa_state_set_map);

    for (int i = 0; i < n_state_; i++) {
        stateset_subscribe_is_accept(raw_dfa_states[i], nfa_is_accept_state);
    }

    *raw_dfa_n_state = n_state_;
}

bool raw_dfa_edge_is_equal(const struct raw_dfa_edge_link *a, const struct raw_dfa_edge_link *b,
                           const state_t *state_categories,
                           unsigned int n_alphabet) {
    state_t *char_to_state_category = (state_t *) malloc(sizeof(state_t) * (n_alphabet + 1));
    for (unsigned int i = 0; i <= n_alphabet; i++) {
        char_to_state_category[i] = NO_STATE;
    }

    const struct raw_dfa_edge_link *p;

    p = a;
    unsigned int out_deg_a = 0;
    while (p != NULL) {
        out_deg_a++;
        char_to_state_category[p->char_idx] = state_categories[p->state];
        p = p->next;
    }

    p = b;
    unsigned int out_deg_b = 0;
    while (p != NULL) {
        out_deg_b++;
        if (char_to_state_category[p->char_idx] != state_categories[p->state]) {
            free(char_to_state_category);
            return false;
        }
        p = p->next;
    }

    free(char_to_state_category);
    return out_deg_a == out_deg_b;
}

void raw_dfa_edge_insert(unsigned int char_idx, state_t target_state, state_t state,
                         struct raw_dfa_edge_link **raw_dfa_edge_link) {
    struct raw_dfa_edge_link *tmp = (struct raw_dfa_edge_link *) malloc(sizeof(struct raw_dfa_edge_link));
    tmp->char_idx = char_idx;
    tmp->state = target_state;
    tmp->next = raw_dfa_edge_link[state];
    raw_dfa_edge_link[state] = tmp;
}

bool state_categories_is_equal(const state_t *state_categories1, const state_t *state_categories2,
                               unsigned int n_raw_dfa_state) {
    for (unsigned int i = 0; i < n_raw_dfa_state; i++) {
        if (state_categories1[i] != state_categories2[i]) {
            return false;
        }
    }
    return true;
}

inline void state_categories_copy(state_t *dst, const state_t *src, unsigned int n_raw_dfa_state) {
    memcpy(dst, src, n_raw_dfa_state * sizeof(state_t));
}

void dfa_minimize(
        struct raw_dfa_edge_link **raw_dfa_edge,
        state_t raw_dfa_start_state,
        const struct state_set **raw_dfa_states,
        unsigned int n_raw_dfa_state,

        unsigned int *n_dfa_state,
        unsigned int *dfa_start_state,
        state_t ***dfa_delta,
        struct state_set ***dfa_states,

        unsigned int n_alphabet
) {
    state_t *state_categories = malloc(sizeof(state_t) * n_raw_dfa_state);

    int sum = 0;
    for (unsigned int i = 0; i < n_raw_dfa_state; i++) {
        state_categories[i] = raw_dfa_states[i]->is_accept ? 1 : 0;
        sum += state_categories[i];
    }

    int n_categories = 2;
    if (sum == 0 || sum == n_raw_dfa_state) {
        n_categories--;
        for (unsigned int i = 0; i < n_raw_dfa_state; i++) {
            state_categories[i] = 0;
        }
    }

    state_t *last_state_categories = (state_t *) malloc(sizeof(state_t) * n_raw_dfa_state);
    state_t *state_category_standards = malloc(sizeof(state_t) * n_raw_dfa_state);
    state_t *state_category_another = (state_t *) malloc(sizeof(state_t) * n_raw_dfa_state);
    do {
        state_categories_copy(last_state_categories, state_categories, n_raw_dfa_state);
        for (int i = 0; i < n_categories; i++) {
            state_category_standards[i] = NO_STATE;
            state_category_another[i] = NO_STATE;
        }

        for (state_t i = 0; i < n_raw_dfa_state; i++) {
            state_t category = last_state_categories[i];
            if (state_category_standards[category] == NO_STATE) {
                state_category_standards[category] = i;
                continue;
            }
            state_t standard_state = state_category_standards[category];
            if (raw_dfa_edge_is_equal(raw_dfa_edge[standard_state], raw_dfa_edge[i], last_state_categories,
                                      n_alphabet) && raw_dfa_states[standard_state]->item == raw_dfa_states[i]->item) {
                continue;
            }

            if (state_category_another[category] == NO_STATE) {
                state_category_another[category] = n_categories++;
            }
            state_categories[i] = state_category_another[category];
        }
    } while (!state_categories_is_equal(last_state_categories, state_categories, n_raw_dfa_state));
    free(last_state_categories);
    free(state_category_standards);
    free(state_category_another);

    *dfa_delta = (state_t **) malloc(n_categories * sizeof(state_t *));
    for (int i = 0; i < n_categories; i++) {
        (*dfa_delta)[i] = (state_t *) malloc((n_alphabet + 1) * sizeof(state_t));
        for (int j = 0; j <= n_alphabet; j++) {
            (*dfa_delta)[i][j] = NO_STATE;
        }
    }

    *n_dfa_state = n_categories;
    *dfa_start_state = state_categories[raw_dfa_start_state];
    *dfa_states = init_raw_dfa_states(n_categories);

    for (unsigned int state = 0; state < n_raw_dfa_state; state++) {
        state_t category = state_categories[state];

        if ((*dfa_states)[category] == NULL) {
            (*dfa_states)[category] = stateset_init();
        }
        stateset_merge((*dfa_states)[category], raw_dfa_states[state]);

        if (raw_dfa_states[state]->is_accept) {
            (*dfa_states)[category]->is_accept = true;
        }

        struct raw_dfa_edge_link *p = raw_dfa_edge[state];
        while (p != NULL) {
            (*dfa_delta)[category][p->char_idx] = state_categories[p->state];
            p = p->next;
        }
    }
}
