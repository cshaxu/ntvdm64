#include "bx_ntvdm_composition_runtime_v1.h"
#include "bx_ntvdm_engine_contract_v1.h"

static int descriptor_set(uint16_t *out_value, uint32_t maximum,
    const wchar_t *text, uint32_t *out_chars)
{
    uint32_t index = 0u;
    if (out_value == 0 || text == 0 || out_chars == 0) return 0;
    while (text[index] != L'\0') {
        if (index >= maximum || text[index] > UINT16_MAX) return 0;
        out_value[index] = (uint16_t) text[index];
        ++index;
    }
    *out_chars = index;
    return 1;
}

static int request_set(struct bx_ntvdm_engine_request_v1 *request,
    const wchar_t *profile, const wchar_t *root)
{
    bx_ntvdm_engine_request_v1_clear(request);
    if (!descriptor_set(request->profile_descriptor,
            BX_NTVDM_ENGINE_V1_MAX_DESCRIPTOR_CHARS, profile,
            &request->profile_descriptor_chars) ||
        !descriptor_set(request->root_descriptor,
            BX_NTVDM_ENGINE_V1_MAX_DESCRIPTOR_CHARS, root,
            &request->root_descriptor_chars) ||
        !descriptor_set(request->launch_descriptor,
            BX_NTVDM_ENGINE_V1_MAX_LAUNCH_CHARS, L"2,1,e,00",
            &request->launch_descriptor_chars)) return 0;
    request->instruction_tick_budget = UINT64_C(1000000);
    request->admitted_drive_mask = UINT32_C(1) << 2u;
    return bx_ntvdm_engine_request_v1_valid(request);
}

int wmain(int argc, wchar_t **argv)
{
    struct bx_ntvdm_engine_request_v1 request;
    bx_ntvdm_startup_plan_v1 plan;
    const uint8_t *payload = 0;
    uint64_t payload_bytes = 0u;
    if (argc != 3 || !request_set(&request, argv[1], argv[2])) return 1;
    if (bx_ntvdm_composition_runtime_v1_install_from_copied_input(
            request.profile_descriptor, request.profile_descriptor_chars,
            request.root_descriptor, request.root_descriptor_chars,
            request.launch_descriptor, request.launch_descriptor_chars,
            request.admitted_drive_mask, request.excluded_drive_mask) != 1 ||
        !bx_ntvdm_composition_runtime_v1_prepare_startup_plan(&plan, &payload,
            &payload_bytes) || payload == 0 || payload_bytes != 0x8400u ||
        !bx_ntvdm_startup_plan_v1_preflight(&plan, 0x100000u, payload_bytes) ||
        plan.payload_write.guest_physical_address != 0x700u || plan.entry_cpu.cs != 0x70u ||
        plan.entry_cpu.eip != 0u || plan.preserved_state_address != 0x714u ||
        plan.preserved_state_bytes != 4u) return 2;
    bx_ntvdm_composition_runtime_v1_reset();
    if (bx_ntvdm_composition_runtime_v1_prepare_startup_plan(&plan, &payload,
            &payload_bytes)) return 3;
    return 0;
}
