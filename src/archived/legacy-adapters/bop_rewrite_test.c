#include "bop_rewrite.h"
#include <stdio.h>
#include <string.h>

int wmain(void)
{
    uint8_t image[] = { 0x90, 0xc4, 0xc4, 0x50, 0x11, 0x90, 0xc4, 0xc4, 0x50, 0x11 };
    const uint8_t pattern_bytes[] = { 0xc4, 0xc4, 0x50, 0x11 };
    const bop_pattern pattern = { pattern_bytes, sizeof(pattern_bytes), 0xe9u };
    bop_rewrite_entry entries[2] = {0};
    uint8_t overlap_image[] = { 0xc4, 0xc4, 0x50, 0x11 };
    uint8_t overlap_before[sizeof(overlap_image)];
    const uint8_t short_bytes[] = { 0xc4, 0xc4, 0x50 };
    const bop_pattern overlap_patterns[] = {
        { pattern_bytes, sizeof(pattern_bytes), 0xe9u },
        { short_bytes, sizeof(short_bytes), 0xeau }
    };
    size_t count = 0u;

    if (!bop_rewrite_apply(image, sizeof(image), &pattern, 1u, entries,
        sizeof(entries) / sizeof(entries[0]), &count) || count != 2u || entries[0].offset != 1u ||
        entries[1].offset != 6u || image[1] != 0xe6u || image[2] != 0xe9u ||
        image[3] != 0x90u || image[4] != 0x90u) return 1;
    bop_rewrite_restore(image, entries, count);
    if (memcmp(image + 1u, pattern_bytes, sizeof(pattern_bytes)) != 0 ||
        memcmp(image + 6u, pattern_bytes, sizeof(pattern_bytes)) != 0) return 1;
    memcpy(overlap_before, overlap_image, sizeof(overlap_image));
    if (bop_rewrite_apply(overlap_image, sizeof(overlap_image), overlap_patterns,
        sizeof(overlap_patterns) / sizeof(overlap_patterns[0]), entries,
        sizeof(entries) / sizeof(entries[0]), &count) ||
        memcmp(overlap_image, overlap_before, sizeof(overlap_image)) != 0) return 1;
    wprintf(L"bop-rewrite-test: atomic private-image rewrite and restore verified\n");
    return 0;
}
