#include "mvdm_gdp_state.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* The selected generated header currently has 692 direct GDP forms.  Keep a
 * small amount of spare capacity so a source-version correction can be
 * audited without silently reallocating an active slot table. */
#define MVDM_GDP_SLOT_CAPACITY 704u
#define MVDM_GDP_MAGIC 0x47504453u

typedef struct mvdm_gdp_slot_record {
    unsigned int original_offset;
    size_t native_width;
    void *storage;
} mvdm_gdp_slot_record;

typedef struct mvdm_gdp_state {
    uint32_t magic;
    unsigned int count;
    mvdm_gdp_slot_record slots[MVDM_GDP_SLOT_CAPACITY];
} mvdm_gdp_state;

void *mvdm_gdp_create(void)
{
    mvdm_gdp_state *state = (mvdm_gdp_state *)calloc(1u, sizeof(*state));

    if (state != NULL) state->magic = MVDM_GDP_MAGIC;
    return state;
}

void mvdm_gdp_destroy(void *value)
{
    mvdm_gdp_state *state = (mvdm_gdp_state *)value;
    unsigned int index;

    if (state == NULL || state->magic != MVDM_GDP_MAGIC) return;
    for (index = 0u; index < state->count; ++index) free(state->slots[index].storage);
    state->magic = 0u;
    free(state);
}

void *mvdm_gdp_slot(const void *value, unsigned int original_offset,
    size_t native_width)
{
    mvdm_gdp_state *state = (mvdm_gdp_state *)value;
    mvdm_gdp_slot_record *slot;
    unsigned int index;

    if (state == NULL || state->magic != MVDM_GDP_MAGIC || native_width == 0u)
        return NULL;
    for (index = 0u; index < state->count; ++index) {
        slot = &state->slots[index];
        if (slot->original_offset != original_offset) continue;
        return slot->native_width == native_width ? slot->storage : NULL;
    }
    if (state->count == MVDM_GDP_SLOT_CAPACITY) return NULL;
    slot = &state->slots[state->count];
    slot->storage = calloc(1u, native_width);
    if (slot->storage == NULL) return NULL;
    slot->original_offset = original_offset;
    slot->native_width = native_width;
    ++state->count;
    return slot->storage;
}
