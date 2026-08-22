#include "bx_ntvdm_engine_contract_v1.h"
#include "bx_ntvdm_cancellation_controller_v1.h"
#include "bx_ntvdm_machine_engine_v1.h"
#include "bop/dem_v2_runtime_session.h"
#include "bop/dem_v2_startup_composition.h"
#include "bop/command_v2_runtime_session.h"

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
    struct bx_ntvdm_machine_engine_v1_result machine_result;
    int ordinary_terminal = 0;
    if (result == 0) return 0;
    if (!bx_ntvdm_engine_request_v1_valid(request))
        return bx_ntvdm_engine_result_v1_set(result,
            BX_NTVDM_ENGINE_TERMINAL_V1_REJECTED_REQUEST, 1u);
    install_status = bx_ntvdm_dem_v2_startup_install(
        request->profile_descriptor, request->profile_descriptor_chars,
        request->root_descriptor, request->root_descriptor_chars,
        request->launch_descriptor, request->launch_descriptor_chars,
        request->admitted_drive_mask, request->excluded_drive_mask,
        request->mutation_mode);
    if (install_status != 1)
    {
        bx_ntvdm_dem_v2_startup_reset();
        return bx_ntvdm_engine_result_v1_set(result,
            BX_NTVDM_ENGINE_TERMINAL_V1_REJECTED_COMPOSITION,
            install_status < 0 ? (uint32_t)(-install_status) : 2u);
    }
    /* The retained startup composition has no authority over DEM dispatch.
       Bind the Direct OpenNT DEM session before any machine stage exists;
       its checked-RAM callbacks can succeed only while that stage is active. */
    if (!bx_ntvdm_dem_v2_runtime_session_bind() ||
        !bx_ntvdm_command_v2_runtime_session_bind_from_startup()) {
        bx_ntvdm_command_v2_runtime_session_reset();
        bx_ntvdm_dem_v2_runtime_session_reset();
        bx_ntvdm_dem_v2_startup_reset();
        return bx_ntvdm_engine_result_v1_set(result,
            BX_NTVDM_ENGINE_TERMINAL_V1_REJECTED_COMPOSITION, 5u);
    }
    if (!bx_ntvdm_dem_v2_startup_prepare_machine_stage_request(
            &machine_stage)) {
        bx_ntvdm_command_v2_runtime_session_reset();
        bx_ntvdm_dem_v2_runtime_session_reset();
        bx_ntvdm_dem_v2_startup_reset();
        return bx_ntvdm_engine_result_v1_set(result,
            BX_NTVDM_ENGINE_TERMINAL_V1_REJECTED_COMPOSITION, 3u);
    }
    if (!bx_ntvdm_dem_v2_startup_prepare_machine_stage_entry(&machine_entry)) {
        bx_ntvdm_command_v2_runtime_session_reset();
        bx_ntvdm_dem_v2_runtime_session_reset();
        bx_ntvdm_dem_v2_startup_reset();
        return bx_ntvdm_engine_result_v1_set(result,
            BX_NTVDM_ENGINE_TERMINAL_V1_REJECTED_COMPOSITION, 4u);
    }
    bx_ntvdm_machine_stage_v1_execution_request_clear(&machine_execution);
    machine_execution.ips = 1000000u;
    machine_execution.instruction_tick_budget = request->instruction_tick_budget;
    if (!bx_ntvdm_machine_engine_v1_run(&machine_stage, &machine_entry,
            &machine_execution, &machine_result)) {
        bx_ntvdm_command_v2_runtime_session_reset();
        bx_ntvdm_dem_v2_runtime_session_reset();
        bx_ntvdm_dem_v2_startup_reset();
        return bx_ntvdm_engine_result_v1_set(result,
            BX_NTVDM_ENGINE_TERMINAL_V1_MACHINE_FAILURE, 4u);
    }
    /* The engine sees only a post-stop opaque composition fact.  It cannot
       inspect the BOP or infer terminal meaning from the CPU result. */
    if (machine_result.execution_status ==
        BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_CONTROLLED_STOP)
        ordinary_terminal = bx_ntvdm_dem_v2_startup_copy_ordinary_terminal();
    bx_ntvdm_command_v2_runtime_session_reset();
    bx_ntvdm_dem_v2_runtime_session_reset();
    bx_ntvdm_dem_v2_startup_reset();
    if (machine_result.begin_status != BX_NTVDM_MACHINE_STAGE_V1_OK ||
        machine_result.entry_status != BX_NTVDM_MACHINE_STAGE_V1_OK ||
        machine_result.reset_status != BX_NTVDM_MACHINE_STAGE_V1_OK)
        return bx_ntvdm_engine_result_v1_set(result,
            BX_NTVDM_ENGINE_TERMINAL_V1_MACHINE_FAILURE,
            machine_result.begin_status != BX_NTVDM_MACHINE_STAGE_V1_OK ? machine_result.begin_status :
            machine_result.entry_status != BX_NTVDM_MACHINE_STAGE_V1_OK ?
                machine_result.entry_status : machine_result.reset_status);
    if (machine_result.execution_status == BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_FIRST_FAULT_STOP)
        return bx_ntvdm_engine_result_v1_set(result,
            BX_NTVDM_ENGINE_TERMINAL_V1_MACHINE_FAILURE, machine_result.execution_status);
    if (machine_result.execution_status == BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_BUDGET)
        return bx_ntvdm_engine_result_v1_set(result,
            BX_NTVDM_ENGINE_TERMINAL_V1_EXECUTION_BUDGET,
            machine_result.execution_status);
    if (machine_result.execution_status ==
        BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_CONTROLLED_STOP)
        return bx_ntvdm_engine_result_v1_set(result,
            ordinary_terminal ? BX_NTVDM_ENGINE_TERMINAL_V1_ORDINARY_GUEST_COMPLETION :
            BX_NTVDM_ENGINE_TERMINAL_V1_CONTROLLED_GUEST_TERMINAL,
            machine_result.execution_status);
    if (machine_result.execution_status ==
        BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_HOST_CANCELLATION)
        return bx_ntvdm_engine_result_v1_set(result,
            BX_NTVDM_ENGINE_TERMINAL_V1_HOST_CANCELLATION,
            machine_result.cancellation_reason != 0u ? machine_result.cancellation_reason :
            BX_NTVDM_CANCELLATION_V1_USER_REQUEST);
    return bx_ntvdm_engine_result_v1_set(result,
        BX_NTVDM_ENGINE_TERMINAL_V1_MACHINE_FAILURE,
        machine_result.execution_status);
}
