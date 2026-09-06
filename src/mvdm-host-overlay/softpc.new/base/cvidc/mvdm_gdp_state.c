#include "mvdm_gdp_state.h"

#include "insignia.h"
#include "evid_c.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Generated rules can reach historical offsets beyond the static header.
 * Individual field storage is separately allocated, so only this index moves. */
#define MVDM_GDP_INITIAL_SLOT_CAPACITY 128u
#define MVDM_GDP_MAGIC 0x47504453u

typedef struct softpc_gdp_slot_record {
    unsigned int original_offset;
    size_t native_width;
    void *storage;
} softpc_gdp_slot_record;

typedef struct mvdm_gdp_state {
    uint32_t magic;
    unsigned int count;
    unsigned int capacity;
    softpc_gdp_slot_record *slots;
} mvdm_gdp_state;

void *softpc_gdp_create(void)
{
#if defined(_M_IX86)
    /* The selected runtime is the original 32-bit CCPU carrier.  Its generated
     * rules address Gdp directly, so retain the source allocation shape rather
     * than substituting independent native-width fields. */
    return malloc(64u * 1024u);
#else
    mvdm_gdp_state *state = (mvdm_gdp_state *)calloc(1u, sizeof(*state));

    if (state != NULL) state->magic = MVDM_GDP_MAGIC;
    return state;
#endif
}

void softpc_gdp_destroy(void *value)
{
#if defined(_M_IX86)
    free(value);
#else
    mvdm_gdp_state *state = (mvdm_gdp_state *)value;
    unsigned int index;

    if (state == NULL || state->magic != MVDM_GDP_MAGIC) return;
    for (index = 0u; index < state->count; ++index) free(state->slots[index].storage);
    free(state->slots);
    state->magic = 0u;
    free(state);
#endif
}

void *softpc_gdp_slot(const void *value, unsigned int original_offset,
    size_t native_width)
{
#if defined(_M_IX86)
    (void)native_width;
    return (unsigned char *)value + original_offset;
#else
    mvdm_gdp_state *state = (mvdm_gdp_state *)value;
    softpc_gdp_slot_record *slot;
    unsigned int index;

    if (state == NULL || state->magic != MVDM_GDP_MAGIC || native_width == 0u)
        return NULL;
    for (index = 0u; index < state->count; ++index) {
        slot = &state->slots[index];
        if (slot->original_offset != original_offset) continue;
        return slot->native_width == native_width ? slot->storage : NULL;
    }
    if (state->count == state->capacity) {
        unsigned int new_capacity = state->capacity == 0u
            ? MVDM_GDP_INITIAL_SLOT_CAPACITY : state->capacity * 2u;
        softpc_gdp_slot_record *new_slots = (softpc_gdp_slot_record *)realloc(
            state->slots, new_capacity * sizeof(*new_slots));

        if (new_slots == NULL) return NULL;
        state->slots = new_slots;
        state->capacity = new_capacity;
    }
    slot = &state->slots[state->count];
    slot->storage = calloc(1u, native_width);
    if (slot->storage == NULL) return NULL;
    slot->original_offset = original_offset;
    slot->native_width = native_width;
    ++state->count;
    return slot->storage;
#endif
}

#if !defined(_M_IX86)
static size_t softpc_gdp_vga_native_offset(unsigned int original_offset)
{
    switch (original_offset) {
    case 1280u: return offsetof(struct VGAGLOBALSETTINGS, latches);
    case 1284u: return offsetof(struct VGAGLOBALSETTINGS, VGA_rplane);
    case 1288u: return offsetof(struct VGAGLOBALSETTINGS, VGA_wplane);
    case 1292u: return offsetof(struct VGAGLOBALSETTINGS, scratch);
    case 1296u: return offsetof(struct VGAGLOBALSETTINGS, sr_masked_val);
    case 1300u: return offsetof(struct VGAGLOBALSETTINGS, sr_nmask);
    case 1304u: return offsetof(struct VGAGLOBALSETTINGS, data_and_mask);
    case 1308u: return offsetof(struct VGAGLOBALSETTINGS, data_xor_mask);
    case 1312u: return offsetof(struct VGAGLOBALSETTINGS, latch_xor_mask);
    case 1316u: return offsetof(struct VGAGLOBALSETTINGS, bit_prot_mask);
    case 1320u: return offsetof(struct VGAGLOBALSETTINGS, plane_enable);
    case 1324u: return offsetof(struct VGAGLOBALSETTINGS, plane_enable_mask);
    case 1328u: return offsetof(struct VGAGLOBALSETTINGS, sr_lookup);
    case 1332u: return offsetof(struct VGAGLOBALSETTINGS, fwd_str_read_addr);
    case 1336u: return offsetof(struct VGAGLOBALSETTINGS, bwd_str_read_addr);
    case 1340u: return offsetof(struct VGAGLOBALSETTINGS, dirty_total);
    case 1344u: return offsetof(struct VGAGLOBALSETTINGS, dirty_low);
    case 1348u: return offsetof(struct VGAGLOBALSETTINGS, dirty_high);
    case 1352u: return offsetof(struct VGAGLOBALSETTINGS, video_copy);
    case 1356u: return offsetof(struct VGAGLOBALSETTINGS, mark_byte);
    case 1360u: return offsetof(struct VGAGLOBALSETTINGS, mark_word);
    case 1364u: return offsetof(struct VGAGLOBALSETTINGS, mark_string);
    case 1368u: return offsetof(struct VGAGLOBALSETTINGS, read_shift_count);
    case 1372u: return offsetof(struct VGAGLOBALSETTINGS, read_mapped_plane);
    case 1376u: return offsetof(struct VGAGLOBALSETTINGS, colour_comp);
    case 1380u: return offsetof(struct VGAGLOBALSETTINGS, dont_care);
    case 1384u: return offsetof(struct VGAGLOBALSETTINGS, v7_bank_vid_copy_off);
    /* The original 32-bit C-VID name at this offset is
     * video_base_lin_addr.  The selected CCPU owns the shared aggregate and
     * represents the same slot as video_base_ls0 at native pointer width. */
    case 1388u: return offsetof(struct VGAGLOBALSETTINGS, video_base_ls0);
    case 1392u: return offsetof(struct VGAGLOBALSETTINGS, route_reg1);
    case 1396u: return offsetof(struct VGAGLOBALSETTINGS, route_reg2);
    case 1400u: return offsetof(struct VGAGLOBALSETTINGS, screen_ptr);
    case 1404u: return offsetof(struct VGAGLOBALSETTINGS, rotate);
    case 1408u: return offsetof(struct VGAGLOBALSETTINGS, calc_data_xor);
    case 1412u: return offsetof(struct VGAGLOBALSETTINGS, calc_latch_xor);
    case 1416u: return offsetof(struct VGAGLOBALSETTINGS, read_byte_addr);
    case 1420u: return offsetof(struct VGAGLOBALSETTINGS, v7_fg_latches);
    case 1424u: return offsetof(struct VGAGLOBALSETTINGS, GCRegs);
    case 1428u: return offsetof(struct VGAGLOBALSETTINGS, lastGCindex);
    case 1429u: return offsetof(struct VGAGLOBALSETTINGS, dither);
    case 1430u: return offsetof(struct VGAGLOBALSETTINGS, wrmode);
    case 1431u: return offsetof(struct VGAGLOBALSETTINGS, chain);
    case 1432u: return offsetof(struct VGAGLOBALSETTINGS, wrstate);
    default: return (size_t)-1;
    }
}
#endif

void *softpc_gdp_rule_slot(void *state, unsigned int original_offset,
    size_t native_width)
{
#if defined(_M_IX86)
    (void)native_width;
    return (unsigned char *)state + original_offset;
#else
    size_t member_offset = softpc_gdp_vga_native_offset(original_offset);
    if (member_offset != (size_t)-1) {
        void *vga = softpc_gdp_slot(state, 1280u,
            sizeof(struct VGAGLOBALSETTINGS));
        return vga == NULL ? NULL : (unsigned char *)vga + member_offset;
    }
    return softpc_gdp_slot(state, original_offset, native_width);
#endif
}

void *softpc_gdp_rule_address(void *state, uintptr_t original_address,
    size_t native_width)
{
#if defined(_M_IX86)
    (void)state;
    (void)native_width;
    return (void *)original_address;
#else
    return original_address < 4096u
        ? softpc_gdp_slot(state, (unsigned int)original_address, native_width)
        : (void *)original_address;
#endif
}
