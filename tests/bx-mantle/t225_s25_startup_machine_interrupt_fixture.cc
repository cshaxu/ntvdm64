#include "bochs.h"
#include "bx-mantle/bx_ntvdm_minimal_machine.h"
#include "bx-vdm/bx_ntvdm_boot_namespace_composition_v1.h"

extern "C" int bx_ntvdm_mantle_generic_ud_bridge_v1(
    const struct bx_ntvdm_generic_ud_event_v1 *,
    struct bx_ntvdm_generic_ud_outcome_v1 *) { return 0; }

static void event_for(struct bx_ntvdm_generic_ud_event_v1 *event,
    unsigned char selector, unsigned long long rip)
{
    memset(event, 0, sizeof(*event));
    event->magic = BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC;
    event->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    event->struct_bytes = sizeof(*event);
    event->vector = 6u;
    event->execution_mode = BX_NTVDM_CPU_EXECUTION_REAL;
    event->fault_rip = rip;
    event->window_bytes = 3u;
    event->window[0] = 0xc4u;
    event->window[1] = 0xc4u;
    event->window[2] = selector;
}

static int resumed(const struct bx_ntvdm_generic_ud_outcome_v1 *out,
    unsigned long long rip)
{
    return out->disposition == BX_NTVDM_GENERIC_UD_RESUME &&
        out->resume_rip == rip + 3u;
}

int main()
{
    bx_ntvdm_minimal_machine_c machine;
    bx_ntvdm_boot_namespace_composition_v1 composition;
    bx_ntvdm_generic_ud_event_v1 event;
    bx_ntvdm_generic_ud_outcome_v1 out;
    byob_profile_selection profile = {};
    uint8_t ntdos_bytes[] = { 0xf4u };
    uint8_t command_bytes[] = { 0xf4u };
    uint8_t target_bytes[] = { 0xf4u };
    byob_image ntdos = { ntdos_bytes, sizeof(ntdos_bytes) };
    byob_image command = { command_bytes, sizeof(command_bytes) };
    byob_image target = { target_bytes, sizeof(target_bytes) };
    Bit8u frame[4] = { 0x34u, 0x12u, 0x70u, 0u };
    Bit8u table[4] = { 0x34u, 0x12u, 0x78u, 0x56u };
    Bit8u stream_offset[2] = { 0x70u, 0u };
    Bit8u value = 0u;

    profile.guest_display_state = BYOB_GUEST_DISPLAY_STATE_STREAM_IO_V1;
    if (machine.initialize(0x200000, 0x200000) != BX_NTVDM_MINIMAL_MACHINE_OK ||
        !bx_ntvdm_boot_namespace_composition_v1_initialize(&composition,
            &ntdos, &command, &target, 0, &profile) ||
        !bx_ntvdm_boot_namespace_composition_v1_bind(&composition)) return 1;

    event_for(&event, 0x02u, 0x200u);
    if (!bx_ntvdm_boot_namespace_composition_v1_handle(&event, &out) ||
        !resumed(&out, 0x200u) ||
        !bx_mem.copy_from_ordinary_ram(0x46bu, 1u, &value) || value != 0xffu) return 2;

    if (!bx_mem.copy_to_ordinary_ram(0xb79u, 4u, frame)) return 3;
    event_for(&event, 0x06u, 0x300u); event.ss = 0x70u; event.esp = 0x479u;
    if (!bx_ntvdm_boot_namespace_composition_v1_handle(&event, &out) ||
        !resumed(&out, 0x300u) ||
        !bx_mem.copy_from_ordinary_ram(0xb79u, 1u, &value) || value != 0x35u) return 4;

    event_for(&event, 0x12u, 0x400u);
    if (!bx_ntvdm_boot_namespace_composition_v1_handle(&event, &out) ||
        !resumed(&out, 0x400u) || out.gpr16_write_mask != 1u ||
        out.gpr16_values[0] != 0x027fu) return 5;
    event_for(&event, 0x15u, 0x500u); event.eax = 0x8800u;
    if (!bx_ntvdm_boot_namespace_composition_v1_handle(&event, &out) ||
        !resumed(&out, 0x500u) || out.gpr16_write_mask != 1u ||
        out.gpr16_values[0] != 0x0c00u) return 6;
    event_for(&event, 0x15u, 0x510u); event.eax = 0x8700u;
    if (bx_ntvdm_boot_namespace_composition_v1_handle(&event, &out)) return 7;

    event_for(&event, 0x17u, 0x600u);
    if (!bx_ntvdm_boot_namespace_composition_v1_handle(&event, &out) ||
        !resumed(&out, 0x600u) || out.gpr16_write_mask != 0u) return 8;
    event_for(&event, 0x66u, 0x700u); event.ebx = 0xffffu;
    if (!bx_ntvdm_boot_namespace_composition_v1_handle(&event, &out) ||
        !resumed(&out, 0x700u) || out.gpr16_write_mask != (1u << 3u) ||
        out.gpr16_values[3] != 0u) return 9;

    if (!bx_mem.copy_to_ordinary_ram(0x10062u, 2u, stream_offset)) return 10;
    event_for(&event, 0x5fu, 0x800u); event.eax = 0xbeefu;
    event.cs = event.ds = 0x1000u; event.esi = 0x40u;
    if (!bx_ntvdm_boot_namespace_composition_v1_handle(&event, &out) ||
        !resumed(&out, 0x800u) ||
        !bx_mem.copy_from_ordinary_ram(0x10070u, 1u, &value) ||
        value != BYOB_GUEST_DISPLAY_STATE_STREAM_IO_V1) return 11;

    if (!bx_mem.copy_to_ordinary_ram(0x10080u, 4u, table)) return 12;
    event_for(&event, 0xc8u, 0x900u); event.cs = 0x1000u; event.ebx = 0x80u;
    if (!bx_ntvdm_boot_namespace_composition_v1_handle(&event, &out) ||
        !resumed(&out, 0x900u) || !bx_mem.copy_from_ordinary_ram(0xccu, 4u, table) ||
        table[0] != 0x34u || table[1] != 0x12u || table[2] != 0x78u || table[3] != 0x56u) return 13;
    event_for(&event, 0xc8u, 0x910u); event.cs = 0xffffu; event.ebx = 0xffffu;
    if (bx_ntvdm_boot_namespace_composition_v1_handle(&event, &out)) return 14;
    event_for(&event, 0x02u, 0xa00u); event.execution_mode = BX_NTVDM_CPU_EXECUTION_PROTECTED;
    if (bx_ntvdm_boot_namespace_composition_v1_handle(&event, &out)) return 15;

    bx_ntvdm_boot_namespace_composition_v1_unbind(&composition);
    return machine.cleanup() == BX_NTVDM_MINIMAL_MACHINE_OK ? 0 : 16;
}