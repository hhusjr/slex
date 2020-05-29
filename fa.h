//
// Created by 沈俊儒 on 2020/5/21.
//

#include "slex_t.h"
#include <stdint.h>

#ifndef DEMO_NFA_TO_DFA_H
#define DEMO_NFA_TO_DFA_H

#define STATE_SET_BUCKET_CNT 64
#define STATE_SET_HASH_MOD  69061

#define NO_STATE -1
#define STATE_SET_NOT_FOUND -1

#define EPS 0

struct state_set {
    uint64_t states[STATE_SET_BUCKET_CNT];
    bool is_accept;

    unsigned int iter_idx;
    uint64_t iter_cur;

    struct slex_item *item;
};

struct states_in_hash_bucket {
    struct state_set *state_set;
    unsigned int idx_in_states;
    struct states_in_hash_bucket *next;
};

struct raw_dfa_edge_link {
    unsigned int char_idx;
    state_t state;
    struct raw_dfa_edge_link *next;
};

struct state_set ***nfa_init(unsigned int n_nfa_states, unsigned int n_alphabet);

bool *init_nfa_accept_states(unsigned int n_nfa_states);

void add_nfa_accept_state(state_t state, bool *is_acceptable_state);

unsigned int stateset_hash(struct state_set *set);

void stateset_clear(struct state_set *set);

struct state_set *stateset_init();

void stateset_destroy(struct state_set *p);

bool stateset_is_empty(struct state_set *set);

void stateset_insert(struct state_set *set, state_t val, struct slex_item *item);

void stateset_merge(struct state_set *dst, const struct state_set *src);

bool stateset_is_equal(const struct state_set *a, const struct state_set *b);

void stateset_begin(struct state_set *set);

state_t stateset_next(struct state_set *set);

void stateset_subscribe_is_accept(struct state_set *set, const bool *nfa_is_accept_state);

void nfa_trans_add(state_t state, unsigned int idx, state_t target_state, struct slex_item *item,
                   struct state_set ***nfa_edge);

void
add_to_state_set_map(unsigned int idx, struct state_set *state_set, struct states_in_hash_bucket **nfa_state_set_map);

unsigned int get_index_by_state_set(struct state_set *state_set, struct states_in_hash_bucket **nfa_state_set_map);

struct state_set *closure(struct state_set *s, struct state_set ***nfa_edge);

struct state_set *move(struct state_set *s, unsigned int idx, struct state_set ***nfa_edge);

struct raw_dfa_edge_link **init_raw_dfa_edge(unsigned int max_allowed_raw_dfa_state);

struct state_set **init_raw_dfa_states(unsigned int state_cnt);


void nfa_to_raw_dfa(struct raw_dfa_edge_link **raw_dfa_edge, unsigned int *raw_dfa_n_state,
                    state_t *raw_dfa_start_state, struct state_set **raw_dfa_states, struct state_set ***nfa_edge,
                    bool *nfa_is_accept_state, state_t nfa_start_state, unsigned int n_alphabet);

bool raw_dfa_edge_is_equal(const struct raw_dfa_edge_link *a, const struct raw_dfa_edge_link *b,
                           const state_t *state_categories,
                           unsigned int n_alphabet);

void raw_dfa_edge_insert(unsigned int char_idx, state_t target_state, state_t state,
                         struct raw_dfa_edge_link **raw_dfa_edge_link);

bool state_categories_is_equal(const state_t *state_categories1, const state_t *state_categories2,
                               unsigned int n_raw_dfa_state);

void state_categories_copy(state_t *dst, const state_t *src, unsigned int n_raw_dfa_state);

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
);

#endif //DEMO_NFA_TO_DFA_H
