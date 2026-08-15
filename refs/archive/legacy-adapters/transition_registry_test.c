#include "transition_registry.h"

#include <stdio.h>
#include <string.h>

int wmain(void)
{
    transition_registry registry;
    const uint8_t first[] = { 0xc4u, 0xc4u, 0x50u, 0x11u };
    const uint8_t second[] = { 0xc4u, 0xc4u, 0x52u, 0x01u };
    const uint8_t prefix[] = { 0xc4u, 0xc4u, 0x50u };
    uint8_t image[] = { 0x90u, 0xc4u, 0xc4u, 0x50u, 0x11u,
        0x90u, 0xc4u, 0xc4u, 0x52u, 0x01u, 0x90u };
    uint8_t before[sizeof(image)];
    bop_rewrite_entry entries[2] = {0};
    size_t count = 0u;

    transition_registry_init(&registry);
    if (!transition_registry_register(&registry, first, sizeof(first), 0xe9u) ||
        transition_registry_register(&registry, first, sizeof(first), 0xe9u) ||
        transition_registry_register(&registry, prefix, sizeof(prefix), 0xeau) ||
        !transition_registry_register(&registry, second, sizeof(second), 0xeau)) return 1;
    memcpy(before, image, sizeof(image));
    if (transition_registry_apply(&registry, image, sizeof(image), entries,
        2u, &count) || memcmp(image, before, sizeof(image)) != 0) return 1;
    if (!transition_registry_freeze(&registry) ||
        transition_registry_register(&registry, first, sizeof(first), 0xe9u) ||
        !transition_registry_is_frozen(&registry)) return 1;
    if (transition_registry_apply(&registry, image, sizeof(image), entries,
        1u, &count) || memcmp(image, before, sizeof(image)) != 0) return 1;
    if (!transition_registry_apply(&registry, image, sizeof(image), entries,
        2u, &count) || count != 2u || entries[0].offset != 1u ||
        entries[1].offset != 6u || image[1] != 0xe6u || image[2] != 0xe9u ||
        image[6] != 0xe6u || image[7] != 0xeau) return 1;
    bop_rewrite_restore(image, entries, count);
    if (memcmp(image, before, sizeof(image)) != 0 ||
        !transition_disposition_is_valid(TRANSITION_UNHANDLED) ||
        !transition_disposition_is_valid(TRANSITION_HANDLED_RESUME) ||
        !transition_disposition_is_valid(TRANSITION_STOP) ||
        !transition_disposition_is_valid(TRANSITION_FAULT) ||
        transition_disposition_is_valid((transition_disposition)4)) return 1;
    wprintf(L"transition-registry-test: frozen physical-pattern registry and atomic rewrite verified\n");
    return 0;
}
