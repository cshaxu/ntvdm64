#include "bx_ntvdm_bop_catalog_v1.h"

#include <stdio.h>

static int observe(const uint8_t *bytes, uint32_t byte_count,
    bx_ntvdm_bop_catalog_v1_identity *identity)
{
    bx_ntvdm_exception_event_v1 event = { BX_NTVDM_EXCEPTION_ABI_MAGIC,
        BX_NTVDM_EXCEPTION_ABI_VERSION, sizeof(event),
        BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0u, 6u, 0u, 0u, 0x516u };
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    bx_ntvdm_instruction_window_v1_capture(&window, bytes, byte_count);
    return bx_ntvdm_bop_catalog_v1_observe(&event, &cpu, &window, identity);
}

int main(void)
{
    bx_ntvdm_bop_catalog_v1_identity identity;
    const uint8_t bios12[] = { 0xc4u, 0xc4u, 0x12u };
    const uint8_t nonbop[] = { 0x90u, 0x90u, 0x90u };
    const uint8_t table_only[] = { 0xc4u, 0xc4u, 0x06u };
    const uint8_t opennt_top_level[] = {
        0x50u, 0x51u, 0x52u, 0x53u, 0x54u, 0x56u, 0x57u, 0x59u,
        0x5au, 0x5bu, 0x5cu, 0x5du, 0x5eu, 0x5fu, 0xfdu, 0xfeu
    };
    const uint8_t families[][4] = {
        { 0xc4u, 0xc4u, 0x50u, 0x11u }, { 0xc4u, 0xc4u, 0x52u, 0x00u },
        { 0xc4u, 0xc4u, 0x53u, 0x00u }, { 0xc4u, 0xc4u, 0x54u, 0x05u },
        { 0xc4u, 0xc4u, 0x56u, 0x00u }, { 0xc4u, 0xc4u, 0x57u, 0x00u }
    };
    uint32_t index;
    uint32_t selector;
    if (!observe(bios12, sizeof(bios12), &identity) || !identity.is_bop ||
        identity.selector != 0x12u || identity.source_class !=
            BX_NTVDM_BOP_CATALOG_V1_SOURCE_SOFTPC_BIOS ||
        identity.family != BX_NTVDM_BOP_CATALOG_V1_FAMILY_NONE ||
        identity.service_state != BX_NTVDM_BOP_CATALOG_V1_SERVICE_NOT_APPLICABLE)
        return 1;
    for (index = 0u; index < 6u; index++) {
        if (!observe(families[index], sizeof(families[index]), &identity) ||
            !identity.is_bop || identity.family != index + 1u ||
            identity.service_state != BX_NTVDM_BOP_CATALOG_V1_SERVICE_PRESENT)
            return 2;
    }
    if (!observe(families[0], 3u, &identity) || !identity.is_bop ||
        identity.family != BX_NTVDM_BOP_CATALOG_V1_FAMILY_DOS ||
        identity.service_state != BX_NTVDM_BOP_CATALOG_V1_SERVICE_TRUNCATED)
        return 3;
    {
        const uint8_t undefined_dos[] = { 0xc4u, 0xc4u, 0x50u, 0x4au };
        if (!observe(undefined_dos, sizeof(undefined_dos), &identity) ||
            identity.service_state !=
                BX_NTVDM_BOP_CATALOG_V1_SERVICE_UNDEFINED ||
            identity.service != 0x4au)
            return 8;
    }
    {
        const uint8_t last_command[] = { 0xc4u, 0xc4u, 0x54u, 0x11u };
        if (!observe(last_command, sizeof(last_command), &identity) ||
            identity.service_state != BX_NTVDM_BOP_CATALOG_V1_SERVICE_PRESENT)
            return 9;
    }
    {
        const uint8_t last_redir[] = { 0xc4u, 0xc4u, 0x57u, 0x31u };
        const uint8_t undefined_redir[] = { 0xc4u, 0xc4u, 0x57u, 0x32u };
        if (!observe(last_redir, sizeof(last_redir), &identity) ||
            identity.service_state != BX_NTVDM_BOP_CATALOG_V1_SERVICE_PRESENT)
            return 10;
        if (!observe(undefined_redir, sizeof(undefined_redir), &identity) ||
            identity.service_state != BX_NTVDM_BOP_CATALOG_V1_SERVICE_UNDEFINED)
            return 11;
    }
    if (!observe(table_only, sizeof(table_only), &identity) || !identity.is_bop ||
        identity.source_class != BX_NTVDM_BOP_CATALOG_V1_SOURCE_SOFTPC_BIOS ||
        identity.family != BX_NTVDM_BOP_CATALOG_V1_FAMILY_NONE)
        return 4;
    /* The full SoftPC BIOS[] table has rows 00..FF, so every named OpenNT
     * selector overlaps that provenance.  The byte stream cannot choose an
     * owner; the listener deliberately retains the ambiguity. */
    for (index = 0u; index < sizeof(opennt_top_level); index++) {
        const uint8_t defined_bop[] = { 0xc4u, 0xc4u, opennt_top_level[index] };
        uint32_t expected = BX_NTVDM_BOP_CATALOG_V1_SOURCE_OVERLAPPING;
        if (!observe(defined_bop, sizeof(defined_bop), &identity) ||
            !identity.is_bop || identity.source_class != expected)
            return 5;
    }
    if (!observe(nonbop, sizeof(nonbop), &identity) || identity.is_bop ||
        identity.service_state != BX_NTVDM_BOP_CATALOG_V1_SERVICE_NOT_APPLICABLE)
        return 6;
    /* The listener is syntax-first and the explicit SoftPC table covers all
     * byte selectors. Source provenance never gates logging or dispatches. */
    for (selector = 0u; selector <= 0xffu; selector++) {
        const uint8_t any_bop[] = { 0xc4u, 0xc4u, (uint8_t)selector };
        if (!observe(any_bop, sizeof(any_bop), &identity) || !identity.is_bop ||
            identity.selector != (uint8_t)selector ||
            identity.source_class == BX_NTVDM_BOP_CATALOG_V1_SOURCE_UNCATALOGUED)
            return 7;
    }
    puts("bx-ntvdm BOP catalog: identity-only pass-through classification verified");
    return 0;
}
