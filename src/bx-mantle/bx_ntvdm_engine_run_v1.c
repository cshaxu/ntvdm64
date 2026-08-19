#include "bx_ntvdm_engine_contract_v1.h"
#include "bx_ntvdm_cancellation_controller_v1.h"
#include "bop-v1/bx_ntvdm_composition_runtime_v1.h"

static int bx_ntvdm_engine_cancelled(uint32_t *reason)
{
    uint32_t value = bx_ntvdm_cancellation_controller_v1_requested_reason();
    if (value == BX_NTVDM_CANCELLATION_V1_NONE) return 0;
    if (reason != 0) *reason = value;
    return 1;
}

int bx_ntvdm_engine_request_cancellation_v1(uint32_t reason)
{
    return bx_ntvdm_cancellation_controller_v1_request(reason);
}

int bx_ntvdm_engine_run_v1(const struct bx_ntvdm_engine_request_v1 *request,
    struct bx_ntvdm_engine_result_v1 *result)
{
    int install_status;
    struct bx_ntvdm_machine_stage_v1_request machine_stage;
    struct bx_ntvdm_machine_stage_v1_entry machine_entry;
    struct bx_ntvdm_machine_stage_v1_execution_request machine_execution;
    uint32_t machine_status, machine_entry_status, machine_execution_status;
    uint32_t machine_reset_status, cancellation_reason = 0u;
    int ordinary_terminal = 0;
    if (result == 0) return 0;
    if (!bx_ntvdm_engine_request_v1_valid(request))
        return bx_ntvdm_engine_result_v1_set(result,
            BX_NTVDM_ENGINE_TERMINAL_V1_REJECTED_REQUEST, 1u);
    if (!bx_ntvdm_cancellation_controller_v1_activate())
        return bx_ntvdm_engine_result_v1_set(result,
            BX_NTVDM_ENGINE_TERMINAL_V1_MACHINE_FAILURE, 4u);
    install_status = bx_ntvdm_composition_runtime_v1_install_from_copied_input_with_mode(
        request->profile_descriptor, request->profile_descriptor_chars,
        request->root_descriptor, request->root_descriptor_chars,
        request->launch_descriptor, request->launch_descriptor_chars,
        request->admitted_drive_mask, request->excluded_drive_mask,
        request->mutation_mode);
    if (install_status != 1)
    {
        bx_ntvdm_composition_runtime_v1_reset();
        bx_ntvdm_cancellation_controller_v1_deactivate();
        return bx_ntvdm_engine_result_v1_set(result,
            BX_NTVDM_ENGINE_TERMINAL_V1_REJECTED_COMPOSITION,
            install_status < 0 ? (uint32_t)(-install_status) : 2u);
    }
    if (!bx_ntvdm_composition_runtime_v1_prepare_machine_stage_request(
            &machine_stage)) {
        bx_ntvdm_composition_runtime_v1_reset();
        bx_ntvdm_cancellation_controller_v1_deactivate();
        return bx_ntvdm_engine_result_v1_set(result,
            BX_NTVDM_ENGINE_TERMINAL_V1_REJECTED_COMPOSITION, 3u);
    }
    if (bx_ntvdm_engine_cancelled(&cancellation_reason)) {
        bx_ntvdm_composition_runtime_v1_reset();
        bx_ntvdm_cancellation_controller_v1_deactivate();
        return bx_ntvdm_engine_result_v1_set(result,
            BX_NTVDM_ENGINE_TERMINAL_V1_HOST_CANCELLATION, cancellation_reason);
    }
    machine_status = bx_ntvdm_machine_stage_v1_begin(&machine_stage);
    machine_entry_status = machine_status == BX_NTVDM_MACHINE_STAGE_V1_OK &&
        bx_ntvdm_composition_runtime_v1_prepare_machine_stage_entry(
            &machine_entry) ?
        bx_ntvdm_machine_stage_v1_arm_real_mode_entry(&machine_entry) :
        BX_NTVDM_MACHINE_STAGE_V1_REJECTED_ENTRY;
    bx_ntvdm_machine_stage_v1_execution_request_clear(&machine_execution);
    machine_execution.ips = 1000000u;
    machine_execution.instruction_tick_budget = request->instruction_tick_budget;
    machine_execution_status = machine_entry_status !=
        BX_NTVDM_MACHINE_STAGE_V1_OK ?
        BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_REJECTED_INACTIVE :
        bx_ntvdm_engine_cancelled(&cancellation_reason) ?
        BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_HOST_CANCELLATION :
        bx_ntvdm_machine_stage_v1_execute(&machine_execution);
    machine_reset_status = bx_ntvdm_machine_stage_v1_reset();
    /* The engine sees only a post-stop opaque composition fact.  It cannot
       inspect the BOP or infer terminal meaning from the CPU result. */
    if (machine_execution_status ==
        BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_CONTROLLED_STOP)
        ordinary_terminal = bx_ntvdm_composition_runtime_v1_copy_ordinary_terminal();
    bx_ntvdm_composition_runtime_v1_reset();
    bx_ntvdm_cancellation_controller_v1_deactivate();
    if (machine_status != BX_NTVDM_MACHINE_STAGE_V1_OK ||
        machine_entry_status != BX_NTVDM_MACHINE_STAGE_V1_OK ||
        machine_reset_status != BX_NTVDM_MACHINE_STAGE_V1_OK)
        return bx_ntvdm_engine_result_v1_set(result,
            BX_NTVDM_ENGINE_TERMINAL_V1_MACHINE_FAILURE,
            machine_status != BX_NTVDM_MACHINE_STAGE_V1_OK ? machine_status :
            machine_entry_status != BX_NTVDM_MACHINE_STAGE_V1_OK ?
                machine_entry_status : machine_reset_status);
    if (machine_execution_status == BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_FIRST_FAULT_STOP)
        return bx_ntvdm_engine_result_v1_set(result,
            BX_NTVDM_ENGINE_TERMINAL_V1_MACHINE_FAILURE, machine_execution_status);
    if (machine_execution_status == BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_BUDGET)
        return bx_ntvdm_engine_result_v1_set(result,
            BX_NTVDM_ENGINE_TERMINAL_V1_EXECUTION_BUDGET,
            machine_execution_status);
    if (machine_execution_status ==
        BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_CONTROLLED_STOP)
        return bx_ntvdm_engine_result_v1_set(result,
            ordinary_terminal ? BX_NTVDM_ENGINE_TERMINAL_V1_ORDINARY_GUEST_COMPLETION :
            BX_NTVDM_ENGINE_TERMINAL_V1_CONTROLLED_GUEST_TERMINAL,
            machine_execution_status);
    if (machine_execution_status ==
        BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_HOST_CANCELLATION)
        return bx_ntvdm_engine_result_v1_set(result,
            BX_NTVDM_ENGINE_TERMINAL_V1_HOST_CANCELLATION,
            cancellation_reason != 0u ? cancellation_reason :
            BX_NTVDM_CANCELLATION_V1_USER_REQUEST);
    return bx_ntvdm_engine_result_v1_set(result,
        BX_NTVDM_ENGINE_TERMINAL_V1_MACHINE_FAILURE,
        machine_execution_status);
}
