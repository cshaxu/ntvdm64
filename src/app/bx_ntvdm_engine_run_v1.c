#include "bx_ntvdm_engine_contract_v1.h"
#include "bx_ntvdm_machine_engine_v1.h"
#include "opennt-bop/ingress/dem_v2_runtime_session.h"
#include "opennt-bop/ingress/dem_v2_startup_composition.h"
#include "opennt-bop/ingress/command_v2_runtime_session.h"
#include "ntdos64_bop_composition_v1.h"
#include "adapter-softpc/bx_ntvdm_guest_pointer_manager.h"
#include "opennt-host/top_level/top_level_nosupport_shim.h"
#include "session/ntdos64_session_v1.h"
#include "session/ntdos64_session_cancellation_v1.h"

int bx_ntvdm_engine_request_cancellation_v1(uint32_t reason)
{
    return ntdos64_session_cancellation_v1_request(reason);
}

int bx_ntvdm_engine_run_v1(const struct bx_ntvdm_engine_request_v1 *request,
    struct bx_ntvdm_engine_result_v1 *result)
{
    int install_status;
    struct bx_ntvdm_machine_stage_v1_request machine_stage;
    struct bx_ntvdm_machine_stage_v1_entry machine_entry;
    struct bx_ntvdm_machine_stage_v1_execution_request machine_execution;
    struct bx_ntvdm_machine_engine_v1_result machine_result;
    ntdos64_session_v1 session;
    int ordinary_terminal = 0;
    if (result == 0) return 0;
    if (!bx_ntvdm_engine_request_v1_valid(request))
        return bx_ntvdm_engine_result_v1_set(result,
            BX_NTVDM_ENGINE_TERMINAL_V1_REJECTED_REQUEST, 1u);
    ntdos64_session_v1_initialize(&session);
    if (!ntdos64_session_v1_activate(&session) ||
        !bx_ntvdm_session_mapping_registry_bind(&session))
        return bx_ntvdm_engine_result_v1_set(result,
            BX_NTVDM_ENGINE_TERMINAL_V1_REJECTED_COMPOSITION, 7u);
    if (!ntdos64_session_v1_register_teardown(&session,
            bx_ntvdm_session_mapping_registry_reset)) {
        bx_ntvdm_session_mapping_registry_reset();
        ntdos64_session_v1_reset(&session);
        return bx_ntvdm_engine_result_v1_set(result,
            BX_NTVDM_ENGINE_TERMINAL_V1_REJECTED_COMPOSITION, 7u);
    }
    ntdos64_bop_composition_v1_reset();
    if (!ntdos64_bop_composition_v1_bind()) {
        ntdos64_session_v1_reset(&session);
        return bx_ntvdm_engine_result_v1_set(result,
            BX_NTVDM_ENGINE_TERMINAL_V1_REJECTED_COMPOSITION, 6u);
    }
    bx_ntvdm_top_level_nosupport_v2_reset_thread();
    install_status = bx_ntvdm_dem_v2_startup_install(
        request->profile_descriptor, request->profile_descriptor_chars,
        request->root_descriptor, request->root_descriptor_chars,
        request->launch_descriptor, request->launch_descriptor_chars,
        request->mutation_mode);
    if (install_status != 1)
    {
        bx_ntvdm_dem_v2_startup_reset();
        bx_ntvdm_top_level_nosupport_v2_reset_thread();
        ntdos64_session_v1_reset(&session);
        ntdos64_bop_composition_v1_reset();
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
        bx_ntvdm_top_level_nosupport_v2_reset_thread();
        bx_ntvdm_dem_v2_startup_reset();
        ntdos64_session_v1_reset(&session);
        ntdos64_bop_composition_v1_reset();
        return bx_ntvdm_engine_result_v1_set(result,
            BX_NTVDM_ENGINE_TERMINAL_V1_REJECTED_COMPOSITION, 5u);
    }
    if (!bx_ntvdm_dem_v2_startup_prepare_machine_stage_request(
            &machine_stage)) {
        bx_ntvdm_command_v2_runtime_session_reset();
        bx_ntvdm_dem_v2_runtime_session_reset();
        bx_ntvdm_top_level_nosupport_v2_reset_thread();
        bx_ntvdm_dem_v2_startup_reset();
        ntdos64_session_v1_reset(&session);
        ntdos64_bop_composition_v1_reset();
        return bx_ntvdm_engine_result_v1_set(result,
            BX_NTVDM_ENGINE_TERMINAL_V1_REJECTED_COMPOSITION, 3u);
    }
    machine_stage.guest_memory_bytes = request->guest_memory_bytes;
    machine_stage.reserved_memory_base = request->reserved_memory_base;
    machine_stage.reserved_memory_bytes = request->reserved_memory_bytes;
    if (!bx_ntvdm_dem_v2_startup_prepare_machine_stage_entry(&machine_entry)) {
        bx_ntvdm_command_v2_runtime_session_reset();
        bx_ntvdm_dem_v2_runtime_session_reset();
        bx_ntvdm_top_level_nosupport_v2_reset_thread();
        bx_ntvdm_dem_v2_startup_reset();
        ntdos64_session_v1_reset(&session);
        ntdos64_bop_composition_v1_reset();
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
        ntdos64_session_v1_reset(&session);
        ntdos64_bop_composition_v1_reset();
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
    bx_ntvdm_top_level_nosupport_v2_reset_thread();
    bx_ntvdm_dem_v2_startup_reset();
    ntdos64_session_v1_complete(&session,
        machine_result.execution_status);
    ntdos64_session_v1_reset(&session);
    ntdos64_bop_composition_v1_reset();
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
            NTDOS64_SESSION_CANCELLATION_V1_USER_REQUEST);
    return bx_ntvdm_engine_result_v1_set(result,
        BX_NTVDM_ENGINE_TERMINAL_V1_MACHINE_FAILURE,
        machine_result.execution_status);
}
