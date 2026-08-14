#include "bx_ntvdm_engine_contract_v1.h"
#include "bx_ntvdm_composition_runtime_v1.h"

int bx_ntvdm_engine_run_v1(const struct bx_ntvdm_engine_request_v1 *request,
    struct bx_ntvdm_engine_result_v1 *result)
{
    int install_status;
    struct bx_ntvdm_machine_stage_v1_request machine_stage;
    struct bx_ntvdm_machine_stage_v1_entry machine_entry;
    uint32_t machine_status, machine_entry_status, machine_reset_status;
    if (result == 0) return 0;
    if (!bx_ntvdm_engine_request_v1_valid(request))
        return bx_ntvdm_engine_result_v1_set(result,
            BX_NTVDM_ENGINE_TERMINAL_V1_REJECTED_REQUEST, 1u);
    install_status = bx_ntvdm_composition_runtime_v1_install_from_copied_input(
        request->profile_descriptor, request->profile_descriptor_chars,
        request->root_descriptor, request->root_descriptor_chars,
        request->launch_descriptor, request->launch_descriptor_chars,
        request->admitted_drive_mask, request->excluded_drive_mask);
    if (install_status != 1)
    {
        bx_ntvdm_composition_runtime_v1_reset();
        return bx_ntvdm_engine_result_v1_set(result,
            BX_NTVDM_ENGINE_TERMINAL_V1_REJECTED_COMPOSITION,
            install_status < 0 ? 1u : 2u);
    }
    if (!bx_ntvdm_composition_runtime_v1_prepare_machine_stage_request(
            &machine_stage)) {
        bx_ntvdm_composition_runtime_v1_reset();
        return bx_ntvdm_engine_result_v1_set(result,
            BX_NTVDM_ENGINE_TERMINAL_V1_REJECTED_COMPOSITION, 3u);
    }
    machine_status = bx_ntvdm_machine_stage_v1_begin(&machine_stage);
    machine_entry_status = machine_status == BX_NTVDM_MACHINE_STAGE_V1_OK &&
        bx_ntvdm_composition_runtime_v1_prepare_machine_stage_entry(
            &machine_entry) ?
        bx_ntvdm_machine_stage_v1_arm_real_mode_entry(&machine_entry) :
        BX_NTVDM_MACHINE_STAGE_V1_REJECTED_ENTRY;
    machine_reset_status = bx_ntvdm_machine_stage_v1_reset();
    bx_ntvdm_composition_runtime_v1_reset();
    if (machine_status != BX_NTVDM_MACHINE_STAGE_V1_OK ||
        machine_entry_status != BX_NTVDM_MACHINE_STAGE_V1_OK ||
        machine_reset_status != BX_NTVDM_MACHINE_STAGE_V1_OK)
        return bx_ntvdm_engine_result_v1_set(result,
            BX_NTVDM_ENGINE_TERMINAL_V1_MACHINE_FAILURE,
            machine_status != BX_NTVDM_MACHINE_STAGE_V1_OK ? machine_status :
            machine_entry_status != BX_NTVDM_MACHINE_STAGE_V1_OK ?
                machine_entry_status : machine_reset_status);
    return bx_ntvdm_engine_result_v1_set(result,
        BX_NTVDM_ENGINE_TERMINAL_V1_NONE, 0u);
}
