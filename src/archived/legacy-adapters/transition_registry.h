#ifndef TRANSITION_REGISTRY_H
#define TRANSITION_REGISTRY_H

#include <stddef.h>
#include <stdint.h>

#include "bop_rewrite.h"

#define TRANSITION_PATTERN_MAX_BYTES 16u
#define TRANSITION_REGISTRY_MAX_PATTERNS 32u

typedef enum transition_disposition {
    TRANSITION_UNHANDLED = 0,
    TRANSITION_HANDLED_RESUME = 1,
    TRANSITION_STOP = 2,
    TRANSITION_FAULT = 3
} transition_disposition;

typedef struct transition_pattern_registration {
    uint8_t bytes[TRANSITION_PATTERN_MAX_BYTES];
    size_t length;
    uint8_t trap_port;
} transition_pattern_registration;

typedef struct transition_registry {
    transition_pattern_registration patterns[TRANSITION_REGISTRY_MAX_PATTERNS];
    size_t count;
    int frozen;
} transition_registry;

void transition_registry_init(transition_registry *registry);
int transition_registry_register(transition_registry *registry,
    const uint8_t *bytes, size_t length, uint8_t trap_port);
int transition_registry_freeze(transition_registry *registry);
int transition_registry_is_frozen(const transition_registry *registry);

/* Matches and rewrites only the supplied private physical guest-memory image. */
int transition_registry_apply(const transition_registry *registry,
    uint8_t *memory, size_t bytes, bop_rewrite_entry *entries,
    size_t entry_capacity, size_t *entry_count);

int transition_disposition_is_valid(transition_disposition disposition);

#endif
