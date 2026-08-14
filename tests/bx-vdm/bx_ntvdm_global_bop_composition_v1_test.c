#include "bx_ntvdm_boot_namespace_composition_v1.h"
#include "bx_ntvdm_native_bop_composition_v1.h"
#include "bx_ntvdm_vdm_generic_ud_bridge_v1.h"
#include "bx-mantle/bx_ntvdm_a20_capability_v1.h"
#include "bx-mantle/bx_ntvdm_extended_memory_v1.h"

#include <string.h>

/* A source-built composition witness: the mantle capability ABI is replaced
 * only by deterministic test doubles.  No Bochs device or guest trace is
 * involved. */
void bx_ntvdm_a20_capability_v1_set_lifecycle_active(uint32_t active)
{ (void)active; }
void bx_ntvdm_a20_capability_v1_dispatch(
    const struct bx_ntvdm_a20_capability_request_v1 *request,
    struct bx_ntvdm_a20_capability_result_v1 *result)
{
    result->status = BX_NTVDM_A20_CAPABILITY_OK;
    result->enabled = request->operation == BX_NTVDM_A20_CAPABILITY_SET ?
        request->requested_enabled : 1u;
}
void bx_ntvdm_extended_memory_v1_set_lifecycle_active(uint32_t active)
{ (void)active; }
void bx_ntvdm_extended_memory_v1_dispatch(
    const struct bx_ntvdm_extended_memory_request_v1 *request,
    struct bx_ntvdm_extended_memory_result_v1 *result)
{
    memset(result, 0, sizeof(*result));
    result->status = BX_NTVDM_EXTMEM_OK;
    result->kib = 3072u;
    if (request->operation == BX_NTVDM_EXTMEM_ALLOCATE) {
        result->handle = 1u;
        result->physical_address = UINT64_C(0x100000);
    }
}

static void profile_initialize(byob_profile_selection *profile)
{
    memset(profile, 0, sizeof(*profile));
    memcpy(profile->command_placement.path, L"\\COMMAND.COM",
        sizeof(L"\\COMMAND.COM"));
    memcpy(profile->target_placement.path, L"\\TARGET.COM",
        sizeof(L"\\TARGET.COM"));
    memcpy(profile->target.file_name, L"TARGET.COM", sizeof(L"TARGET.COM"));
    memcpy(profile->config_file.path, L"\\CONFIG.SYS", sizeof(L"\\CONFIG.SYS"));
    memcpy(profile->autoexec_file.path, L"\\AUTOEXEC.BAT", sizeof(L"\\AUTOEXEC.BAT"));
    profile->command_placement.drive_index = 2u;
    profile->target_placement.drive_index = 2u;
    profile->has_command_placement = profile->has_target_placement = 1u;
    profile->has_guest_search_metadata = profile->has_guest_boot_files = 1u;
    profile->config_file.materialization =
        BYOB_GUEST_BOOT_FILE_MINIMAL_COMMENT_V1;
    profile->autoexec_file.materialization = BYOB_GUEST_BOOT_FILE_EMPTY_V1;
    profile->command_metadata.attributes = profile->target_metadata.attributes =
        profile->config_metadata.attributes = profile->autoexec_metadata.attributes = 0x20u;
    profile->command_metadata.dos_date = profile->target_metadata.dos_date =
        profile->config_metadata.dos_date = profile->autoexec_metadata.dos_date = 1u;
}

static int call(uint8_t selector, uint8_t service,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    struct bx_ntvdm_generic_ud_event_v1 event;
    memset(&event, 0, sizeof(event));
    event.magic = BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC;
    event.abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    event.struct_bytes = sizeof(event);
    event.vector = 6u;
    event.execution_mode = BX_NTVDM_CPU_EXECUTION_REAL;
    event.fault_rip = UINT64_C(0x100);
    event.window_bytes = 4u;
    event.window[0] = 0xc4u; event.window[1] = 0xc4u;
    event.window[2] = selector; event.window[3] = service;
    return bx_ntvdm_mantle_generic_ud_bridge_v1(&event, outcome);
}

int main(void)
{
    uint8_t ntdos_bytes[] = { 0xf4u }, command_bytes[] = { 0xf4u };
    uint8_t target_bytes[] = { 0xf4u };
    byob_image ntdos = { ntdos_bytes, sizeof(ntdos_bytes) };
    byob_image command = { command_bytes, sizeof(command_bytes) };
    byob_image target = { target_bytes, sizeof(target_bytes) };
    byob_profile_selection profile;
    bx_ntvdm_boot_namespace_composition_v1 boot;
    bx_ntvdm_native_bop_composition_v1 native_bop;
    struct bx_ntvdm_generic_ud_outcome_v1 outcome;

    profile_initialize(&profile);
    if (!bx_ntvdm_boot_namespace_composition_v1_initialize(&boot, &ntdos,
            &command, &target, 0, &profile) ||
        !bx_ntvdm_native_bop_composition_v1_initialize(&native_bop) ||
        !bx_ntvdm_boot_namespace_composition_v1_bind(&boot) ||
        !bx_ntvdm_native_bop_composition_v1_bind(&native_bop)) return 1;
    /* The first composition declines XMS; the installed native package must
     * then supply it through the same generic #UD route. */
    if (!call(0x52u, 0u, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome.gpr16_values[0] != 1u) return 2;
    {
        uint32_t service;
        for (service = 0u; service < 12u; ++service) {
            int implemented = service == 0u || service == 2u || service == 3u ||
                service == 5u || service == 11u;
            if (!call(0x52u, (uint8_t)service, &outcome) ||
                outcome.disposition != (uint32_t)(implemented ?
                    BX_NTVDM_GENERIC_UD_RESUME : BX_NTVDM_GENERIC_UD_STOP))
                return (int)(10u + service);
        }
    }
    bx_ntvdm_native_bop_composition_v1_unbind(&native_bop);
    bx_ntvdm_boot_namespace_composition_v1_unbind(&boot);
    return 0;
}
