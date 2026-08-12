#ifndef BOP_REWRITE_H
#define BOP_REWRITE_H

#include <stddef.h>
#include <stdint.h>

typedef struct bop_pattern {
    const uint8_t *bytes;
    size_t length;
    uint8_t trap_port;
} bop_pattern;

typedef struct bop_rewrite_entry {
    size_t offset;
    size_t length;
    uint8_t original[16];
    uint8_t trap_port;
} bop_rewrite_entry;

/* Validates every match before modifying private guest memory. */
int bop_rewrite_apply(uint8_t *memory, size_t bytes,
    const bop_pattern *patterns, size_t pattern_count,
    bop_rewrite_entry *entries, size_t entry_capacity, size_t *entry_count);

void bop_rewrite_restore(uint8_t *memory, const bop_rewrite_entry *entries,
    size_t entry_count);

#endif
