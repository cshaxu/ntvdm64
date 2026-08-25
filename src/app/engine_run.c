#include "engine_contract.h"
#include "machine_engine.h"
#include "opennt-bop/ingress/dem_runtime_session.h"
#include "opennt-bop/ingress/dem_startup_composition.h"
#include "opennt-bop/ingress/command_runtime_session.h"
#include "bop_composition.h"
#include "adapter-softpc/guest_pointer_manager.h"
#include "adapter-win32/facade/opennt_error_dialog_facade.h"
#include "session/session.h"
#include "session/session_cancellation.h"

int runtime_engine_request_cancellation(uint32_t reason)
{
    return app_session_cancellation_request(reason);
}

int runtime_engine_run(const struct runtime_engine_request *request,
    struct runtime_engine_result *result)
{
    int install_status;
    struct runtime_machine_stage_request machine_stage;
    struct runtime_machine_stage_entry machine_entry;
    struct runtime_machine_stage_execution_request machine_execution;
    struct runtime_machine_engine_result machine_result;
    app_session session;
    int ordinary_terminal = 0;
    if (result == 0) return 0;
    if (!runtime_engine_request_valid(request))
        return runtime_engine_result_set(result,
            RUNTIME_ENGINE_TERMINAL_REJECTED_REQUEST, 1u);
    app_session_initialize(&session);
    if (!app_session_activate(&session) ||
        !runtime_session_mapping_registry_bind(&session))
        return runtime_engine_result_set(result,
            RUNTIME_ENGINE_TERMINAL_REJECTED_COMPOSITION, 7u);
    if (!app_session_register_teardown(&session,
            runtime_session_mapping_registry_reset)) {
        runtime_session_mapping_registry_reset();
        app_session_reset(&session);
        return runtime_engine_result_set(result,
            RUNTIME_ENGINE_TERMINAL_REJECTED_COMPOSITION, 7u);
    }
    app_bop_composition_reset();
    if (!app_bop_composition_bind()) {
        app_session_reset(&session);
        return runtime_engine_result_set(result,
            RUNTIME_ENGINE_TERMINAL_REJECTED_COMPOSITION, 6u);
    }
    runtime_opennt_direct_access_reset_thread();
    install_status = runtime_dem_startup_install(
        request->profile_descriptor, request->profile_descriptor_chars,
        request->root_descriptor, request->root_descriptor_chars,
        request->launch_descriptor, request->launch_descriptor_chars,
        request->mutation_mode);
    if (install_status != 1)
    {
        runtime_dem_startup_reset();
        runtime_opennt_direct_access_reset_thread();
        app_session_reset(&session);
        app_bop_composition_reset();
        return runtime_engine_result_set(result,
            RUNTIME_ENGINE_TERMINAL_REJECTED_COMPOSITION,
            install_status < 0 ? (uint32_t)(-install_status) : 2u);
    }
    /* The retained startup composition has no authority over DEM dispatch.
       Bind the Direct OpenNT DEM session before any machine stage exists;
       its checked-RAM callbacks can succeed only while that stage is active. */
    if (!runtime_dem_runtime_session_bind() ||
        !runtime_command_runtime_session_bind_from_startup()) {
        runtime_command_runtime_session_reset();
        runtime_dem_runtime_session_reset();
        runtime_opennt_direct_access_reset_thread();
        runtime_dem_startup_reset();
        app_session_reset(&session);
        app_bop_composition_reset();
        return runtime_engine_result_set(result,
            RUNTIME_ENGINE_TERMINAL_REJECTED_COMPOSITION, 5u);
    }
    if (!runtime_dem_startup_prepare_machine_stage_request(
            &machine_stage)) {
        runtime_command_runtime_session_reset();
        runtime_dem_runtime_session_reset();
        runtime_opennt_direct_access_reset_thread();
        runtime_dem_startup_reset();
        app_session_reset(&session);
        app_bop_composition_reset();
        return runtime_engine_result_set(result,
            RUNTIME_ENGINE_TERMINAL_REJECTED_COMPOSITION, 3u);
    }
    machine_stage.guest_memory_bytes = request->guest_memory_bytes;
    machine_stage.reserved_memory_base = request->reserved_memory_base;
    machine_stage.reserved_memory_bytes = request->reserved_memory_bytes;
    if (!runtime_dem_startup_prepare_machine_stage_entry(&machine_entry)) {
        runtime_command_runtime_session_reset();
        runtime_dem_runtime_session_reset();
        runtime_opennt_direct_access_reset_thread();
        runtime_dem_startup_reset();
        app_session_reset(&session);
        app_bop_composition_reset();
        return runtime_engine_result_set(result,
            RUNTIME_ENGINE_TERMINAL_REJECTED_COMPOSITION, 4u);
    }
    runtime_machine_stage_execution_request_clear(&machine_execution);
    machine_execution.ips = 1000000u;
    machine_execution.instruction_tick_budget = request->instruction_tick_budget;
    if (!runtime_machine_engine_run(&machine_stage, &machine_entry,
            &machine_execution, &machine_result)) {
        runtime_command_runtime_session_reset();
        runtime_dem_runtime_session_reset();
        runtime_dem_startup_reset();
        app_session_reset(&session);
        app_bop_composition_reset();
        return runtime_engine_result_set(result,
            RUNTIME_ENGINE_TERMINAL_MACHINE_FAILURE, 4u);
    }
    /* The engine sees only a post-stop opaque composition fact.  It cannot
       inspect the BOP or infer terminal meaning from the CPU result. */
    if (machine_result.execution_status ==
        RUNTIME_MACHINE_STAGE_EXECUTION_CONTROLLED_STOP)
        ordinary_terminal = runtime_dem_startup_copy_ordinary_terminal();
    runtime_command_runtime_session_reset();
    runtime_dem_runtime_session_reset();
    runtime_opennt_direct_access_reset_thread();
    runtime_dem_startup_reset();
    app_session_complete(&session,
        machine_result.execution_status);
    app_session_reset(&session);
    app_bop_composition_reset();
    if (machine_result.begin_status != RUNTIME_MACHINE_STAGE_OK ||
        machine_result.entry_status != RUNTIME_MACHINE_STAGE_OK ||
        machine_result.reset_status != RUNTIME_MACHINE_STAGE_OK)
        return runtime_engine_result_set(result,
            RUNTIME_ENGINE_TERMINAL_MACHINE_FAILURE,
            machine_result.begin_status != RUNTIME_MACHINE_STAGE_OK ? machine_result.begin_status :
            machine_result.entry_status != RUNTIME_MACHINE_STAGE_OK ?
                machine_result.entry_status : machine_result.reset_status);
    if (machine_result.execution_status == RUNTIME_MACHINE_STAGE_EXECUTION_FIRST_FAULT_STOP)
        return runtime_engine_result_set(result,
            RUNTIME_ENGINE_TERMINAL_MACHINE_FAILURE, machine_result.execution_status);
    if (machine_result.execution_status == RUNTIME_MACHINE_STAGE_EXECUTION_BUDGET)
        return runtime_engine_result_set(result,
            RUNTIME_ENGINE_TERMINAL_EXECUTION_BUDGET,
            machine_result.execution_status);
    if (machine_result.execution_status ==
        RUNTIME_MACHINE_STAGE_EXECUTION_CONTROLLED_STOP)
        return runtime_engine_result_set(result,
            ordinary_terminal ? RUNTIME_ENGINE_TERMINAL_ORDINARY_GUEST_COMPLETION :
            RUNTIME_ENGINE_TERMINAL_CONTROLLED_GUEST_TERMINAL,
            machine_result.execution_status);
    if (machine_result.execution_status ==
        RUNTIME_MACHINE_STAGE_EXECUTION_HOST_CANCELLATION)
        return runtime_engine_result_set(result,
            RUNTIME_ENGINE_TERMINAL_HOST_CANCELLATION,
            machine_result.cancellation_reason != 0u ? machine_result.cancellation_reason :
            APP_SESSION_CANCELLATION_USER_REQUEST);
    return runtime_engine_result_set(result,
        RUNTIME_ENGINE_TERMINAL_MACHINE_FAILURE,
        machine_result.execution_status);
}
