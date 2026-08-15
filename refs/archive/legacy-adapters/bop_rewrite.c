#include "bop_rewrite.h"

#include <string.h>

static int overlaps(size_t left, size_t left_length, size_t right, size_t right_length)
{
    return left < right + right_length && right < left + left_length;
}

int bop_rewrite_apply(uint8_t *memory, size_t bytes,
    const bop_pattern *patterns, size_t pattern_count,
    bop_rewrite_entry *entries, size_t entry_capacity, size_t *entry_count)
{
    size_t pattern_index, offset, count = 0u;

    if (memory == NULL || patterns == NULL || entries == NULL || entry_count == NULL) return 0;
    for (pattern_index = 0u; pattern_index < pattern_count; ++pattern_index) {
        const bop_pattern *pattern = &patterns[pattern_index];
        if (pattern->bytes == NULL || pattern->length < 3u || pattern->length > 16u) return 0;
        if (pattern->length > bytes) continue;
        for (offset = 0u; offset <= bytes - pattern->length; ++offset) {
            size_t prior;
            if (memcmp(memory + offset, pattern->bytes, pattern->length) != 0) continue;
            if (count == entry_capacity) return 0;
            for (prior = 0u; prior < count; ++prior) {
                if (overlaps(offset, pattern->length, entries[prior].offset, entries[prior].length)) return 0;
            }
            entries[count].offset = offset;
            entries[count].length = pattern->length;
            entries[count].trap_port = pattern->trap_port;
            memcpy(entries[count].original, memory + offset, pattern->length);
            ++count;
        }
    }
    for (offset = 0u; offset < count; ++offset) {
        memory[entries[offset].offset] = 0xe6u;
        memory[entries[offset].offset + 1u] = entries[offset].trap_port;
        memset(memory + entries[offset].offset + 2u, 0x90, entries[offset].length - 2u);
    }
    *entry_count = count;
    return 1;
}

void bop_rewrite_restore(uint8_t *memory, const bop_rewrite_entry *entries,
    size_t entry_count)
{
    size_t index;
    if (memory == NULL || entries == NULL) return;
    for (index = 0u; index < entry_count; ++index) {
        memcpy(memory + entries[index].offset, entries[index].original, entries[index].length);
    }
}
