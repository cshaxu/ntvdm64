cmake_minimum_required(VERSION 3.20)

project(ntdos64_research LANGUAGES C)

if(NOT WIN32)
    message(FATAL_ERROR "ntdos64-run is a Windows research CLI.")
endif()

# The modern CLI, BYOB assembly, and future modern-host shim use the MinGW-w64
# GCC CRT.  Historical source trees are separate standalone CMake projects so
# a historical compiler and CRT never silently enter this runtime graph.
if(NOT CMAKE_C_COMPILER_ID STREQUAL "GNU")
    message(FATAL_ERROR
        "The modern ntdos64 research runner requires MinGW-w64 GCC. "
        "Configure OpenNT or NTVDMx64 historical input gates from their "
        "own source roots with their selected historical compiler.")
endif()

# T195's adapter machine-composition contract is a static, default-disabled
# test island.  It has no Bochs, OpenNT-object, #UD, or runner link edge.
add_executable(bx-ntvdm-machine-bop-v1-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_machine_composition_v2.c
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_result_v2.c
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    tests/bx-vdm/bx_ntvdm_machine_composition_v2_test.c
)
target_include_directories(bx-ntvdm-machine-bop-v1-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-machine-bop-v1-test PRIVATE c_std_11)
target_compile_definitions(bx-ntvdm-machine-bop-v1-test PRIVATE
    BX_NTVDM_ENABLE_MACHINE_COMPOSITION=1)

function(ntdos64_use_wide_console_entry target)
    # Modern CLI sources use wmain so paths and BYOB metadata stay UTF-16.
    # MinGW selects that console CRT entry point only when this flag is set.
    if(MINGW)
        target_link_options(${target} PRIVATE -municode)
    endif()
endfunction()

add_executable(ntdos64-run
    src/cli/ntdos64_run.c
    src/cli/byob_profile.c
    src/cli/byob_identity.c
    src/cli/byob_target_selection.c
    src/cli/byob_launch_declaration_v1.c
    src/cli/byob_launch_plan_v2.c
)

# Test-only external-engine argument verifier. It is never linked to the CLI.
add_executable(runner-engine-probe
    tests/runner/runner_engine_probe.c
)

# This process boundary owns only fixed bundle validation and native Bochs
# argv/config materialization. It contains no Bochs or adapter object.
add_executable(ntdos64-bochs-engine
    src/cli/ntdos64_bochs_engine.c
)

add_executable(bochs-engine-probe
    tests/runner/bochs_engine_probe.c
)

add_executable(byob-identity-test EXCLUDE_FROM_ALL
    src/cli/byob_identity.c
    tests/runner/byob_identity_test.c
)
target_include_directories(byob-identity-test PRIVATE src/cli)
target_compile_features(byob-identity-test PRIVATE c_std_11)
target_compile_definitions(byob-identity-test PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN)
target_link_libraries(byob-identity-test PRIVATE bcrypt)
ntdos64_use_wide_console_entry(byob-identity-test)

add_executable(byob-profile-test EXCLUDE_FROM_ALL
    src/cli/byob_profile.c
    src/cli/byob_identity.c
    src/cli/byob_image.c
    tests/runner/byob_profile_test.c
)

target_include_directories(byob-profile-test PRIVATE src/cli)
target_compile_features(byob-profile-test PRIVATE c_std_11)
target_compile_definitions(byob-profile-test PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN)
target_link_libraries(byob-profile-test PRIVATE bcrypt)

add_executable(byob-target-selection-test EXCLUDE_FROM_ALL
    src/cli/byob_target_selection.c
    tests/runner/byob_target_selection_test.c
)
target_include_directories(byob-target-selection-test PRIVATE src/cli)
target_compile_features(byob-target-selection-test PRIVATE c_std_11)
target_compile_definitions(byob-target-selection-test PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN)

add_executable(byob-launch-declaration-v1-test EXCLUDE_FROM_ALL
    src/cli/byob_launch_declaration_v1.c
    tests/runner/byob_launch_declaration_v1_test.c
)
target_include_directories(byob-launch-declaration-v1-test PRIVATE src/cli)
target_compile_features(byob-launch-declaration-v1-test PRIVATE c_std_11)
target_compile_definitions(byob-launch-declaration-v1-test PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN)

add_executable(byob-launch-plan-v2-test EXCLUDE_FROM_ALL
    src/cli/byob_launch_declaration_v1.c
    src/cli/byob_launch_plan_v2.c
    tests/runner/byob_launch_plan_v2_test.c
)
target_include_directories(byob-launch-plan-v2-test PRIVATE src/cli)
target_compile_features(byob-launch-plan-v2-test PRIVATE c_std_11)
target_compile_definitions(byob-launch-plan-v2-test PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN)

add_executable(bx-ntvdm-readonly-namespace-test EXCLUDE_FROM_ALL
    src/cli/byob_image.c
    src/cli/byob_identity.c
    src/bx-vdm/bx_ntvdm_readonly_namespace.c
    tests/bx-vdm/bx_ntvdm_readonly_namespace_test.c
)
target_include_directories(bx-ntvdm-readonly-namespace-test PRIVATE src/cli src/bx-vdm)
target_compile_features(bx-ntvdm-readonly-namespace-test PRIVATE c_std_11)
target_link_libraries(bx-ntvdm-readonly-namespace-test PRIVATE bcrypt)

add_executable(bx-ntvdm-dem-readonly-file-service-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_result_v2.c
    src/bx-vdm/bx_ntvdm_guest_range.c
    src/bx-vdm/bx_ntvdm_guest_write_abi.c
    src/bx-vdm/bx_ntvdm_guest_read_action_v1.c
    src/bx-vdm/bx_ntvdm_guest_gather_read_action_v1.c
    src/bx-vdm/bx_ntvdm_bulk_result_transaction.c
    src/bx-vdm/bx_ntvdm_readonly_namespace.c
    src/bx-vdm/bx_ntvdm_dem_readonly_file_service.c
    tests/bx-vdm/bx_ntvdm_dem_readonly_file_service_test.c
)
target_include_directories(bx-ntvdm-dem-readonly-file-service-test PRIVATE src/cli src/bx-vdm)
target_compile_features(bx-ntvdm-dem-readonly-file-service-test PRIVATE c_std_11)
target_link_libraries(bx-ntvdm-dem-readonly-file-service-test PRIVATE bcrypt)

add_executable(bx-ntvdm-boot-namespace-provider-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_result_v2.c
    src/bx-vdm/bx_ntvdm_guest_range.c
    src/bx-vdm/bx_ntvdm_guest_write_abi.c
    src/bx-vdm/bx_ntvdm_guest_read_action_v1.c
    src/bx-vdm/bx_ntvdm_guest_gather_read_action_v1.c
    src/bx-vdm/bx_ntvdm_bulk_result_transaction.c
    src/bx-vdm/bx_ntvdm_multi_write_abi.c
    src/bx-vdm/bx_ntvdm_multi_write_transaction.c
    src/bx-vdm/bx_ntvdm_readonly_namespace.c
    src/bx-vdm/bx_ntvdm_profile_search_snapshot_v1.c
    src/bx-vdm/bx_ntvdm_search_session.c
    src/bx-vdm/bx_ntvdm_search_result_v1.c
    src/bx-vdm/bx_ntvdm_search_plan_v1.c
    src/bx-vdm/bx_ntvdm_search_request_v1.c
    src/bx-vdm/bx_ntvdm_search_transaction_v1.c
    src/bx-vdm/bx_ntvdm_dem_path_search_service_v1.c
    src/bx-vdm/bx_ntvdm_dem_readonly_file_service.c
    src/bx-vdm/bx_ntvdm_cmd_boot_file_service.c
    src/bx-vdm/bx_ntvdm_boot_namespace_provider_v1.c
    tests/bx-vdm/bx_ntvdm_boot_namespace_provider_v1_test.c
)
target_include_directories(bx-ntvdm-boot-namespace-provider-test PRIVATE src/cli src/bx-vdm)
target_compile_features(bx-ntvdm-boot-namespace-provider-test PRIVATE c_std_11)
target_link_libraries(bx-ntvdm-boot-namespace-provider-test PRIVATE bcrypt)

add_executable(bx-ntvdm-dem-session-lifecycle-provider-v1-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_result_v2.c
    src/bx-vdm/bx_ntvdm_guest_range.c
    src/bx-vdm/bx_ntvdm_guest_write_abi.c
    src/bx-vdm/bx_ntvdm_guest_read_action_v1.c
    src/bx-vdm/bx_ntvdm_guest_gather_read_action_v1.c
    src/bx-vdm/bx_ntvdm_bulk_result_transaction.c
    src/bx-vdm/bx_ntvdm_multi_write_abi.c
    src/bx-vdm/bx_ntvdm_multi_write_transaction.c
    src/bx-vdm/bx_ntvdm_readonly_namespace.c
    src/bx-vdm/bx_ntvdm_profile_search_snapshot_v1.c
    src/bx-vdm/bx_ntvdm_search_session.c
    src/bx-vdm/bx_ntvdm_search_result_v1.c
    src/bx-vdm/bx_ntvdm_search_plan_v1.c
    src/bx-vdm/bx_ntvdm_search_request_v1.c
    src/bx-vdm/bx_ntvdm_search_transaction_v1.c
    src/bx-vdm/bx_ntvdm_dem_path_search_service_v1.c
    src/bx-vdm/bx_ntvdm_dem_readonly_file_service.c
    src/bx-vdm/bx_ntvdm_cmd_boot_file_service.c
    src/bx-vdm/bx_ntvdm_boot_namespace_provider_v1.c
    src/bx-vdm/bx_ntvdm_bop_catalog_v1.c
    src/bx-vdm/bx_ntvdm_bop_ingress_v1.c
    src/bx-vdm/bx_ntvdm_bop_provider_registry_v1.c
    src/bx-vdm/bx_ntvdm_dem_plane_v1.c
    src/bx-vdm/bx_ntvdm_dem_session_lifecycle_provider_v1.c
    tests/bx-vdm/bx_ntvdm_dem_session_lifecycle_provider_v1_test.c
)
target_include_directories(bx-ntvdm-dem-session-lifecycle-provider-v1-test PRIVATE src/cli src/bx-vdm)
target_compile_features(bx-ntvdm-dem-session-lifecycle-provider-v1-test PRIVATE c_std_11)
target_link_libraries(bx-ntvdm-dem-session-lifecycle-provider-v1-test PRIVATE bcrypt)

add_executable(bx-ntvdm-dem-provider-v1-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_result_v2.c
    src/bx-vdm/bx_ntvdm_bop_catalog_v1.c
    src/bx-vdm/bx_ntvdm_bop_ingress_v1.c
    src/bx-vdm/bx_ntvdm_bop_provider_registry_v1.c
    src/bx-vdm/bx_ntvdm_dem_plane_v1.c
    src/bx-vdm/bx_ntvdm_dem_provider_v1.c
    src/bx-vdm/bx_ntvdm_dem_session_lifecycle_provider_v1.c
    tests/bx-vdm/bx_ntvdm_dem_provider_v1_test.c
)
target_include_directories(bx-ntvdm-dem-provider-v1-test PRIVATE src/bx-vdm)
target_compile_features(bx-ntvdm-dem-provider-v1-test PRIVATE c_std_11)

add_executable(bx-ntvdm-dem-drive-provider-v1-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_result_v2.c
    src/bx-vdm/bx_ntvdm_bop_ingress_v1.c
    src/bx-vdm/bx_ntvdm_bop_provider_registry_v1.c
    src/bx-vdm/bx_ntvdm_dem_plane_v1.c
    src/bx-vdm/bx_ntvdm_host_drive_policy.c
    src/bx-vdm/bx_ntvdm_dem_drive_provider_v1.c
    tests/bx-vdm/bx_ntvdm_dem_drive_provider_v1_test.c
)
target_include_directories(bx-ntvdm-dem-drive-provider-v1-test PRIVATE src/bx-vdm)
target_compile_features(bx-ntvdm-dem-drive-provider-v1-test PRIVATE c_std_11)

add_executable(bx-ntvdm-redir-unavailable-provider-v1-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_result_v2.c
    src/bx-vdm/bx_ntvdm_bop_ingress_v1.c
    src/bx-vdm/bx_ntvdm_bop_provider_registry_v1.c
    src/bx-vdm/bx_ntvdm_redir_unavailable_provider_v1.c
    tests/bx-vdm/bx_ntvdm_redir_unavailable_provider_v1_test.c
)
target_include_directories(bx-ntvdm-redir-unavailable-provider-v1-test PRIVATE src/bx-vdm)
target_compile_features(bx-ntvdm-redir-unavailable-provider-v1-test PRIVATE c_std_11)

# S4 is a separately admitted, default-disabled software-monitor mechanics
# slice. It is not linked into ntdos64-run or registered in default CTest.
add_executable(ntdos64-reconstructed-monitor-s4-test EXCLUDE_FROM_ALL
    src/reconstruction/monitor/reconstructed_monitor_s4.c
    tests/reconstructed_monitor_s4_test.c
)
target_include_directories(ntdos64-reconstructed-monitor-s4-test PRIVATE
    src/reconstruction/monitor)
target_compile_features(ntdos64-reconstructed-monitor-s4-test PRIVATE c_std_11)

# This takes a user-selected, source-built NTIO.SYS as a test input, maps the
# entire image at 0070:0000, and records the first unimplemented CPU boundary.
# It is neither a runtime nor a device/BOP/DEM implementation.
add_executable(ntdos64-reconstructed-monitor-s4-ntio-strategy-fixture EXCLUDE_FROM_ALL
    src/reconstruction/monitor/reconstructed_monitor_s4.c
    tests/reconstructed_monitor_s4_ntio_strategy_fixture.c
)
target_include_directories(ntdos64-reconstructed-monitor-s4-ntio-strategy-fixture PRIVATE
    src/reconstruction/monitor)
target_compile_features(ntdos64-reconstructed-monitor-s4-ntio-strategy-fixture PRIVATE c_std_11)

# S5 v2 records are a default-disabled wire-format fixture only.  They do not
# start a helper, expose a historical entry point, or link the normal runner.
add_executable(ntdos64-historical-transport-v2-layout-test EXCLUDE_FROM_ALL
    tests/historical_transport_v2_layout_test.c
)
target_include_directories(ntdos64-historical-transport-v2-layout-test PRIVATE
    src/reconstruction/transport)
target_compile_features(ntdos64-historical-transport-v2-layout-test PRIVATE c_std_11)

# Fixed-width adapter ABI layout only. This target is deliberately not linked
# to Bochs or OpenNT and is excluded from the default product build.
add_executable(bx-ntvdm-exception-abi-layout-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    tests/bx-vdm/bx_ntvdm_exception_abi_layout_test.c
)
target_include_directories(bx-ntvdm-exception-abi-layout-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-exception-abi-layout-test PRIVATE c_std_11)

add_executable(bx-ntvdm-instruction-window-abi-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    tests/bx-vdm/bx_ntvdm_instruction_window_abi_test.c
)
target_include_directories(bx-ntvdm-instruction-window-abi-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-instruction-window-abi-test PRIVATE c_std_11)

add_executable(bx-ntvdm-guest-range-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_guest_range.c
    tests/bx-vdm/bx_ntvdm_guest_range_test.c
)
target_include_directories(bx-ntvdm-guest-range-test PRIVATE src/bx-vdm)
target_compile_features(bx-ntvdm-guest-range-test PRIVATE c_std_11)

add_executable(bx-ntvdm-cpu-state-abi-layout-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    tests/bx-vdm/bx_ntvdm_cpu_state_abi_layout_test.c
)
target_include_directories(bx-ntvdm-cpu-state-abi-layout-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-cpu-state-abi-layout-test PRIVATE c_std_11)

add_executable(bx-ntvdm-cpu-delta-abi-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    tests/bx-vdm/bx_ntvdm_cpu_delta_abi_test.c
)
target_include_directories(bx-ntvdm-cpu-delta-abi-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-cpu-delta-abi-test PRIVATE c_std_11)

add_executable(bx-ntvdm-guest-write-abi-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_guest_range.c
    src/bx-vdm/bx_ntvdm_guest_write_abi.c
    src/bx-vdm/bx_ntvdm_multi_write_abi.c
    src/bx-vdm/bx_ntvdm_multi_write_transaction.c
    src/bx-vdm/bx_ntvdm_multi_write_abi.c
    src/bx-vdm/bx_ntvdm_multi_write_transaction.c
    tests/bx-vdm/bx_ntvdm_guest_write_abi_test.c
)
target_include_directories(bx-ntvdm-guest-write-abi-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-guest-write-abi-test PRIVATE c_std_11)

add_executable(bx-ntvdm-observation-transaction-abi-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_guest_range.c
    src/bx-vdm/bx_ntvdm_observation_transaction_abi.c
    tests/bx-vdm/bx_ntvdm_observation_transaction_abi_test.c
)
target_include_directories(bx-ntvdm-observation-transaction-abi-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-observation-transaction-abi-test PRIVATE c_std_11)

add_executable(bx-ntvdm-startup-snapshot-abi-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_guest_range.c
    src/bx-vdm/bx_ntvdm_startup_snapshot_abi.c
    src/bx-vdm/bx_ntvdm_startup_plan_abi.c
    tests/bx-vdm/bx_ntvdm_startup_snapshot_abi_test.c
)
target_include_directories(bx-ntvdm-startup-snapshot-abi-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-startup-snapshot-abi-test PRIVATE c_std_11)

add_executable(bx-ntvdm-startup-plan-abi-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_guest_range.c
    src/bx-vdm/bx_ntvdm_guest_write_abi.c
    src/bx-vdm/bx_ntvdm_multi_write_abi.c
    src/bx-vdm/bx_ntvdm_multi_write_transaction.c
    src/bx-vdm/bx_ntvdm_startup_plan_abi.c
    tests/bx-vdm/bx_ntvdm_startup_plan_abi_test.c
)
target_include_directories(bx-ntvdm-startup-plan-abi-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-startup-plan-abi-test PRIVATE c_std_11)

add_executable(bx-ntvdm-startup-snapshot-evidence-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_guest_range.c
    src/bx-vdm/bx_ntvdm_startup_snapshot_abi.c
    src/bx-vdm/bx_ntvdm_startup_plan_abi.c
    src/bx-vdm/bx_ntvdm_startup_snapshot_evidence.c
    src/cli/byob_identity.c
    tests/bx-vdm/bx_ntvdm_startup_snapshot_evidence_test.c
)
target_include_directories(bx-ntvdm-startup-snapshot-evidence-test PRIVATE
    src/bx-vdm src/cli)
target_compile_features(bx-ntvdm-startup-snapshot-evidence-test PRIVATE c_std_11)
target_compile_definitions(bx-ntvdm-startup-snapshot-evidence-test PRIVATE
    UNICODE _UNICODE WIN32_LEAN_AND_MEAN)
target_link_libraries(bx-ntvdm-startup-snapshot-evidence-test PRIVATE bcrypt)

add_executable(bx-ntvdm-machine-profile-abi-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_guest_range.c
    src/bx-vdm/bx_ntvdm_observation_transaction_abi.c
    src/bx-vdm/bx_ntvdm_startup_snapshot_abi.c
    src/bx-vdm/bx_ntvdm_machine_profile_abi.c
    tests/bx-vdm/bx_ntvdm_machine_profile_abi_test.c
)
target_include_directories(bx-ntvdm-machine-profile-abi-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-machine-profile-abi-test PRIVATE c_std_11)

# Recovered historical owner selection only.  This target contains no BIOS
# function pointer, BOP decode, OpenNT source, or Bochs dependency.
add_executable(bx-ntvdm-bios-selector-map-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_bios_selector_map.c
    tests/bx-vdm/bx_ntvdm_bios_selector_map_test.c
)
target_include_directories(bx-ntvdm-bios-selector-map-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-bios-selector-map-test PRIVATE c_std_11)

add_executable(bx-ntvdm-bios-memory-service-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_bios_memory_service.c
    tests/bx-vdm/bx_ntvdm_bios_memory_service_test.c
)
target_include_directories(bx-ntvdm-bios-memory-service-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-bios-memory-service-test PRIVATE c_std_11)

add_executable(bx-ntvdm-wait-if-idle-service-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_wait_if_idle_service.c
    tests/bx-vdm/bx_ntvdm_wait_if_idle_service_test.c
)
target_include_directories(bx-ntvdm-wait-if-idle-service-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-wait-if-idle-service-test PRIVATE c_std_11)

add_executable(bx-ntvdm-spckbd-init-service-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_cpu_result_v2.c
    src/bx-vdm/bx_ntvdm_spckbd_init_service.c
    tests/bx-vdm/bx_ntvdm_spckbd_init_service_test.c
)
target_include_directories(bx-ntvdm-spckbd-init-service-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-spckbd-init-service-test PRIVATE c_std_11)

add_executable(bx-ntvdm-emm-unavailable-service-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_cpu_result_v2.c
    src/bx-vdm/bx_ntvdm_emm_unavailable_service.c
    tests/bx-vdm/bx_ntvdm_emm_unavailable_service_test.c
)
target_include_directories(bx-ntvdm-emm-unavailable-service-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-emm-unavailable-service-test PRIVATE c_std_11)

add_executable(bx-ntvdm-printer-unavailable-service-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_cpu_result_v2.c
    src/bx-vdm/bx_ntvdm_printer_unavailable_service.c
    tests/bx-vdm/bx_ntvdm_printer_unavailable_service_test.c
)
target_include_directories(bx-ntvdm-printer-unavailable-service-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-printer-unavailable-service-test PRIVATE c_std_11)

add_executable(bx-ntvdm-config-done-service-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_cpu_result_v2.c
    src/bx-vdm/bx_ntvdm_config_done_service.c
    tests/bx-vdm/bx_ntvdm_config_done_service_test.c)
target_include_directories(bx-ntvdm-config-done-service-test PRIVATE src/bx-vdm)
target_compile_features(bx-ntvdm-config-done-service-test PRIVATE c_std_11)

add_executable(bx-ntvdm-vdd-create-user-notify-service-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_cpu_result_v2.c
    src/bx-vdm/bx_ntvdm_vdd_create_user_notify_service.c
    tests/bx-vdm/bx_ntvdm_vdd_create_user_notify_service_test.c
)
target_include_directories(bx-ntvdm-vdd-create-user-notify-service-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-vdd-create-user-notify-service-test PRIVATE c_std_11)

add_executable(bx-ntvdm-cmd-comspec-bootstrap-service-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_cpu_result_v2.c
    src/bx-vdm/bx_ntvdm_guest_range.c
    src/bx-vdm/bx_ntvdm_guest_write_abi.c
    src/bx-vdm/bx_ntvdm_guest_gather_read_action_v1.c
    src/bx-vdm/bx_ntvdm_multi_write_abi.c
    src/bx-vdm/bx_ntvdm_multi_write_transaction.c
    src/bx-vdm/bx_ntvdm_cmd_comspec_bootstrap_service.c
    tests/bx-vdm/bx_ntvdm_cmd_comspec_bootstrap_service_test.c
)
target_include_directories(bx-ntvdm-cmd-comspec-bootstrap-service-test PRIVATE
    src/bx-vdm src/cli)
target_compile_features(bx-ntvdm-cmd-comspec-bootstrap-service-test PRIVATE c_std_11)

add_executable(bx-ntvdm-cmd-launch-envelope-v1-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cmd_launch_envelope_v1.c
    tests/bx-vdm/bx_ntvdm_cmd_launch_envelope_v1_test.c
)
target_include_directories(bx-ntvdm-cmd-launch-envelope-v1-test PRIVATE
    src/bx-vdm src/cli)
target_compile_features(bx-ntvdm-cmd-launch-envelope-v1-test PRIVATE c_std_11)

add_executable(bx-ntvdm-cmdinfo-v1-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cmdinfo_v1.c
    tests/bx-vdm/bx_ntvdm_cmdinfo_v1_test.c
)
target_include_directories(bx-ntvdm-cmdinfo-v1-test PRIVATE src/bx-vdm)
target_compile_features(bx-ntvdm-cmdinfo-v1-test PRIVATE c_std_11)

add_executable(bx-ntvdm-mouse-install1-mapping-service-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_cpu_result_v2.c
    src/bx-vdm/bx_ntvdm_guest_range.c
    src/bx-vdm/bx_ntvdm_guest_write_abi.c
    src/bx-vdm/bx_ntvdm_multi_write_abi.c
    src/bx-vdm/bx_ntvdm_multi_write_transaction.c
    src/bx-vdm/bx_ntvdm_guest_gather_read_action_v1.c
    src/bx-vdm/bx_ntvdm_mouse_install1_mapping_service.c
    tests/bx-vdm/bx_ntvdm_mouse_install1_mapping_service_test.c
)
target_include_directories(bx-ntvdm-mouse-install1-mapping-service-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-mouse-install1-mapping-service-test PRIVATE c_std_11)

add_executable(bx-ntvdm-cpu-result-bridge-v1-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_cpu_result_v2.c
    src/bx-vdm/bx_ntvdm_bios_memory_service.c
    src/bx-vdm/bx_ntvdm_cpu_result_bridge_v1.c
    tests/bx-vdm/bx_ntvdm_cpu_result_bridge_v1_test.c
)
target_include_directories(bx-ntvdm-cpu-result-bridge-v1-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-cpu-result-bridge-v1-test PRIVATE c_std_11)

add_executable(bx-ntvdm-bop-catalog-v1-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_bop_catalog_v1.c
    tests/bx-vdm/bx_ntvdm_bop_catalog_v1_test.c
)
target_include_directories(bx-ntvdm-bop-catalog-v1-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-bop-catalog-v1-test PRIVATE c_std_11)

add_executable(bx-ntvdm-dem-drive-service-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_result_v2.c
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_dem_drive_service.c
    tests/bx-vdm/bx_ntvdm_dem_drive_service_test.c
)
target_include_directories(bx-ntvdm-dem-drive-service-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-dem-drive-service-test PRIVATE c_std_11)

add_executable(bx-ntvdm-dem-ioctl-metadata-provider-v1-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_result_v2.c
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_bop_ingress_v1.c
    src/bx-vdm/bx_ntvdm_bop_provider_registry_v1.c
    src/bx-vdm/bx_ntvdm_dem_plane_v1.c
    src/bx-vdm/bx_ntvdm_host_drive_policy.c
    src/bx-vdm/bx_ntvdm_dem_ioctl_metadata_provider_v1.c
    tests/bx-vdm/bx_ntvdm_dem_ioctl_metadata_provider_v1_test.c
)
target_include_directories(bx-ntvdm-dem-ioctl-metadata-provider-v1-test PRIVATE src/bx-vdm)
target_compile_features(bx-ntvdm-dem-ioctl-metadata-provider-v1-test PRIVATE c_std_11)
target_compile_definitions(bx-ntvdm-dem-ioctl-metadata-provider-v1-test PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN)

add_executable(bx-ntvdm-dem-dta-service-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_result_v2.c
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_guest_range.c
    src/bx-vdm/bx_ntvdm_guest_read_action_v1.c
    src/bx-vdm/bx_ntvdm_dem_dta_service.c
    tests/bx-vdm/bx_ntvdm_dem_dta_service_test.c
)
target_include_directories(bx-ntvdm-dem-dta-service-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-dem-dta-service-test PRIVATE c_std_11)

add_executable(bx-ntvdm-dem-hard-error-service-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_result_v2.c
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_dem_hard_error_service.c
    tests/bx-vdm/bx_ntvdm_dem_hard_error_service_test.c
)
target_include_directories(bx-ntvdm-dem-hard-error-service-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-dem-hard-error-service-test PRIVATE c_std_11)

add_executable(bx-ntvdm-cmd-set-info-service-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_result_v2.c
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_cmd_set_info_service.c
    src/bx-vdm/bx_ntvdm_dem_dpb_service.c
    tests/bx-vdm/bx_ntvdm_cmd_set_info_service_test.c
)
target_include_directories(bx-ntvdm-cmd-set-info-service-test PRIVATE src/bx-vdm)
target_compile_features(bx-ntvdm-cmd-set-info-service-test PRIVATE c_std_11)

add_executable(bx-ntvdm-guest-read-action-v1-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_result_v2.c
    src/bx-vdm/bx_ntvdm_guest_read_action_v1.c
    tests/bx-vdm/bx_ntvdm_guest_read_action_v1_test.c
)
target_include_directories(bx-ntvdm-guest-read-action-v1-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-guest-read-action-v1-test PRIVATE c_std_11)

add_executable(bx-ntvdm-search-session-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_search_session.c
    tests/bx-vdm/bx_ntvdm_search_session_test.c
)
target_include_directories(bx-ntvdm-search-session-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-search-session-test PRIVATE c_std_11)
target_compile_definitions(bx-ntvdm-search-session-test PRIVATE
    UNICODE _UNICODE WIN32_LEAN_AND_MEAN)
target_link_libraries(bx-ntvdm-search-session-test PRIVATE bcrypt)

add_executable(bx-ntvdm-search-result-v1-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_result_v2.c
    src/bx-vdm/bx_ntvdm_guest_range.c
    src/bx-vdm/bx_ntvdm_guest_write_abi.c
    src/bx-vdm/bx_ntvdm_multi_write_abi.c
    src/bx-vdm/bx_ntvdm_multi_write_transaction.c
    src/bx-vdm/bx_ntvdm_search_result_v1.c
    tests/bx-vdm/bx_ntvdm_search_result_v1_test.c
)
target_include_directories(bx-ntvdm-search-result-v1-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-search-result-v1-test PRIVATE c_std_11)
target_compile_definitions(bx-ntvdm-search-result-v1-test PRIVATE
    UNICODE _UNICODE WIN32_LEAN_AND_MEAN)

add_executable(bx-ntvdm-search-plan-v1-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_host_drive_policy.c
    src/bx-vdm/bx_ntvdm_host_namespace.c
    src/bx-vdm/bx_ntvdm_search_session.c
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_result_v2.c
    src/bx-vdm/bx_ntvdm_guest_range.c
    src/bx-vdm/bx_ntvdm_guest_write_abi.c
    src/bx-vdm/bx_ntvdm_multi_write_abi.c
    src/bx-vdm/bx_ntvdm_multi_write_transaction.c
    src/bx-vdm/bx_ntvdm_search_result_v1.c
    src/bx-vdm/bx_ntvdm_search_plan_v1.c
    tests/bx-vdm/bx_ntvdm_search_plan_v1_test.c
)
target_include_directories(bx-ntvdm-search-plan-v1-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-search-plan-v1-test PRIVATE c_std_11)
target_compile_definitions(bx-ntvdm-search-plan-v1-test PRIVATE
    UNICODE _UNICODE WIN32_LEAN_AND_MEAN)
target_link_libraries(bx-ntvdm-search-plan-v1-test PRIVATE ntdll bcrypt)

add_executable(bx-ntvdm-search-request-v1-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_search_request_v1.c
    tests/bx-vdm/bx_ntvdm_search_request_v1_test.c
)
target_include_directories(bx-ntvdm-search-request-v1-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-search-request-v1-test PRIVATE c_std_11)
target_compile_definitions(bx-ntvdm-search-request-v1-test PRIVATE
    UNICODE _UNICODE WIN32_LEAN_AND_MEAN)

add_executable(bx-ntvdm-search-transaction-v1-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_host_drive_policy.c
    src/bx-vdm/bx_ntvdm_host_namespace.c
    src/bx-vdm/bx_ntvdm_search_session.c
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_result_v2.c
    src/bx-vdm/bx_ntvdm_guest_range.c
    src/bx-vdm/bx_ntvdm_guest_write_abi.c
    src/bx-vdm/bx_ntvdm_multi_write_abi.c
    src/bx-vdm/bx_ntvdm_multi_write_transaction.c
    src/bx-vdm/bx_ntvdm_search_result_v1.c
    src/bx-vdm/bx_ntvdm_search_plan_v1.c
    src/bx-vdm/bx_ntvdm_search_request_v1.c
    src/bx-vdm/bx_ntvdm_search_transaction_v1.c
    tests/bx-vdm/bx_ntvdm_search_transaction_v1_test.c
)
target_include_directories(bx-ntvdm-search-transaction-v1-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-search-transaction-v1-test PRIVATE c_std_11)
target_compile_definitions(bx-ntvdm-search-transaction-v1-test PRIVATE
    UNICODE _UNICODE WIN32_LEAN_AND_MEAN)
target_link_libraries(bx-ntvdm-search-transaction-v1-test PRIVATE ntdll bcrypt)

add_executable(bx-ntvdm-guest-gather-read-action-v1-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_result_v2.c
    src/bx-vdm/bx_ntvdm_guest_gather_read_action_v1.c
    tests/bx-vdm/bx_ntvdm_guest_gather_read_action_v1_test.c
)
target_include_directories(bx-ntvdm-guest-gather-read-action-v1-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-guest-gather-read-action-v1-test PRIVATE c_std_11)

add_executable(bx-ntvdm-multi-write-abi-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_guest_range.c
    src/bx-vdm/bx_ntvdm_guest_write_abi.c
    src/bx-vdm/bx_ntvdm_multi_write_abi.c
    tests/bx-vdm/bx_ntvdm_multi_write_abi_test.c
)
target_include_directories(bx-ntvdm-multi-write-abi-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-multi-write-abi-test PRIVATE c_std_11)

add_executable(bx-ntvdm-multi-write-transaction-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_guest_range.c
    src/bx-vdm/bx_ntvdm_guest_write_abi.c
    src/bx-vdm/bx_ntvdm_multi_write_abi.c
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_result_v2.c
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_multi_write_transaction.c
    tests/bx-vdm/bx_ntvdm_multi_write_transaction_test.c
)
target_include_directories(bx-ntvdm-multi-write-transaction-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-multi-write-transaction-test PRIVATE c_std_11)

add_executable(bx-ntvdm-bulk-result-transaction-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_result_v2.c
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_bulk_result_transaction.c
    tests/bx-vdm/bx_ntvdm_bulk_result_transaction_test.c
)
target_include_directories(bx-ntvdm-bulk-result-transaction-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-bulk-result-transaction-test PRIVATE c_std_11)

add_executable(bx-ntvdm-dem-dpb-service-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_guest_range.c
    src/bx-vdm/bx_ntvdm_guest_write_abi.c
    src/bx-vdm/bx_ntvdm_multi_write_abi.c
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_result_v2.c
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_multi_write_transaction.c
    src/bx-vdm/bx_ntvdm_dem_dpb_service.c
    tests/bx-vdm/bx_ntvdm_dem_dpb_service_test.c
)
target_include_directories(bx-ntvdm-dem-dpb-service-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-dem-dpb-service-test PRIVATE c_std_11)

add_executable(bx-ntvdm-bop-selector-probe-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_bios_selector_map.c
    src/bx-vdm/bx_ntvdm_bop_selector_probe.c
    tests/bx-vdm/bx_ntvdm_bop_selector_probe_test.c
)
target_include_directories(bx-ntvdm-bop-selector-probe-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-bop-selector-probe-test PRIVATE c_std_11)

add_executable(bx-ntvdm-host-service-catalog-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_host_service_catalog.c
    tests/bx-vdm/bx_ntvdm_host_service_catalog_test.c
)
target_include_directories(bx-ntvdm-host-service-catalog-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-host-service-catalog-test PRIVATE c_std_11)

add_executable(bx-ntvdm-host-transaction-abi-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_guest_range.c
    src/bx-vdm/bx_ntvdm_guest_write_abi.c
    src/bx-vdm/bx_ntvdm_host_transaction_abi.c
    tests/bx-vdm/bx_ntvdm_host_transaction_abi_test.c
)
target_include_directories(bx-ntvdm-host-transaction-abi-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-host-transaction-abi-test PRIVATE c_std_11)

add_executable(bx-ntvdm-session-resource-abi-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_session_resource_abi.c
    tests/bx-vdm/bx_ntvdm_session_resource_abi_test.c
)
target_include_directories(bx-ntvdm-session-resource-abi-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-session-resource-abi-test PRIVATE c_std_11)

add_executable(bx-ntvdm-host-service-plane-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_guest_range.c
    src/bx-vdm/bx_ntvdm_guest_write_abi.c
    src/bx-vdm/bx_ntvdm_host_transaction_abi.c
    src/bx-vdm/bx_ntvdm_session_resource_abi.c
    src/bx-vdm/bx_ntvdm_host_service_plane.c
    tests/bx-vdm/bx_ntvdm_host_service_plane_test.c
)
target_include_directories(bx-ntvdm-host-service-plane-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-host-service-plane-test PRIVATE c_std_11)

add_executable(bx-ntvdm-host-session-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_guest_range.c
    src/bx-vdm/bx_ntvdm_guest_write_abi.c
    src/bx-vdm/bx_ntvdm_multi_write_abi.c
    src/bx-vdm/bx_ntvdm_multi_write_transaction.c
    src/bx-vdm/bx_ntvdm_observation_transaction_abi.c
    src/bx-vdm/bx_ntvdm_startup_snapshot_abi.c
    src/bx-vdm/bx_ntvdm_startup_plan_abi.c
    src/bx-vdm/bx_ntvdm_host_service_catalog.c
    src/bx-vdm/bx_ntvdm_host_transaction_abi.c
    src/bx-vdm/bx_ntvdm_session_resource_abi.c
    src/bx-vdm/bx_ntvdm_host_service_plane.c
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_result_v2.c
    src/bx-vdm/bx_ntvdm_guest_read_action_v1.c
    src/bx-vdm/bx_ntvdm_guest_gather_read_action_v1.c
    src/bx-vdm/bx_ntvdm_bulk_result_transaction.c
    src/bx-vdm/bx_ntvdm_host_session.c
    src/bx-vdm/bx_ntvdm_bios_memory_service.c
    tests/bx-vdm/bx_ntvdm_host_session_test.c
)
target_include_directories(bx-ntvdm-host-session-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-host-session-test PRIVATE c_std_11)

add_executable(bx-ntvdm-host-drive-policy-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_host_drive_policy.c
    tests/bx-vdm/bx_ntvdm_host_drive_policy_test.c
)
target_include_directories(bx-ntvdm-host-drive-policy-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-host-drive-policy-test PRIVATE c_std_11)
target_compile_definitions(bx-ntvdm-host-drive-policy-test PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN)

add_executable(bx-ntvdm-cmd-get-next-service-test EXCLUDE_FROM_ALL
    src/cli/byob_launch_declaration_v1.c
    src/cli/byob_launch_plan_v2.c
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_result_v2.c
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_guest_gather_read_action_v1.c
    src/bx-vdm/bx_ntvdm_guest_range.c
    src/bx-vdm/bx_ntvdm_guest_write_abi.c
    src/bx-vdm/bx_ntvdm_host_drive_policy.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_multi_write_abi.c
    src/bx-vdm/bx_ntvdm_multi_write_transaction.c
    src/bx-vdm/bx_ntvdm_cmdinfo_v1.c
    src/bx-vdm/bx_ntvdm_cmd_get_next_service.c
    tests/bx-vdm/bx_ntvdm_cmd_get_next_service_test.c
)
target_include_directories(bx-ntvdm-cmd-get-next-service-test PRIVATE src/cli src/bx-vdm)
target_compile_features(bx-ntvdm-cmd-get-next-service-test PRIVATE c_std_11)
target_compile_definitions(bx-ntvdm-cmd-get-next-service-test PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN)

add_executable(bx-ntvdm-cmd-keyboard-layout-service-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_result_v2.c
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_cmd_keyboard_layout_service.c
    tests/bx-vdm/bx_ntvdm_cmd_keyboard_layout_service_test.c
)
target_include_directories(bx-ntvdm-cmd-keyboard-layout-service-test PRIVATE src/bx-vdm)
target_compile_features(bx-ntvdm-cmd-keyboard-layout-service-test PRIVATE c_std_11)

add_executable(bx-ntvdm-controlled-stop-service-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_result_v2.c
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_controlled_stop_service.c
    tests/bx-vdm/bx_ntvdm_controlled_stop_service_test.c
)
target_include_directories(bx-ntvdm-controlled-stop-service-test PRIVATE src/bx-vdm)
target_compile_features(bx-ntvdm-controlled-stop-service-test PRIVATE c_std_11)

add_executable(bx-ntvdm-host-namespace-test EXCLUDE_FROM_ALL
    src/bx-vdm/bx_ntvdm_host_drive_policy.c
    src/bx-vdm/bx_ntvdm_host_namespace.c
    tests/bx-vdm/bx_ntvdm_host_namespace_test.c
)
target_include_directories(bx-ntvdm-host-namespace-test PRIVATE
    src/bx-vdm)
target_compile_features(bx-ntvdm-host-namespace-test PRIVATE c_std_11)
target_compile_definitions(bx-ntvdm-host-namespace-test PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN)
target_link_libraries(bx-ntvdm-host-namespace-test PRIVATE ntdll)

add_executable(bx-vdm-runtime-test EXCLUDE_FROM_ALL
    src/cli/byob_profile.c
    src/cli/byob_identity.c
    src/cli/byob_image.c
    src/cli/byob_launch_declaration_v1.c
    src/cli/byob_launch_plan_v2.c
    src/bx-vdm/bx_ntvdm_cpu_state_abi.c
    src/bx-vdm/bx_ntvdm_instruction_window_abi.c
    src/bx-vdm/bx_ntvdm_guest_range.c
    src/bx-vdm/bx_ntvdm_guest_write_abi.c
    src/bx-vdm/bx_ntvdm_multi_write_abi.c
    src/bx-vdm/bx_ntvdm_multi_write_transaction.c
    src/bx-vdm/bx_ntvdm_observation_transaction_abi.c
    src/bx-vdm/bx_ntvdm_startup_snapshot_abi.c
    src/bx-vdm/bx_ntvdm_startup_plan_abi.c
    src/bx-vdm/bx_ntvdm_startup_snapshot_evidence.c
    src/bx-vdm/bx_ntvdm_machine_profile_abi.c
    src/bx-vdm/bx_ntvdm_host_service_catalog.c
    src/bx-vdm/bx_ntvdm_host_transaction_abi.c
    src/bx-vdm/bx_ntvdm_session_resource_abi.c
    src/bx-vdm/bx_ntvdm_host_service_plane.c
    src/bx-vdm/bx_ntvdm_host_session.c
    src/bx-vdm/bx_ntvdm_bulk_result_transaction.c
    src/bx-vdm/bx_ntvdm_readonly_namespace.c
    src/bx-vdm/bx_ntvdm_dem_readonly_file_service.c
    src/bx-vdm/bx_ntvdm_exception_abi.c
    src/bx-vdm/bx_ntvdm_cpu_delta_abi.c
    src/bx-vdm/bx_ntvdm_cpu_result_v2.c
    src/bx-vdm/bx_ntvdm_guest_read_action_v1.c
    src/bx-vdm/bx_ntvdm_guest_gather_read_action_v1.c
    src/bx-vdm/bx_ntvdm_adapter_runtime.c
    src/bx-vdm/bx_ntvdm_exception_observer_v1.c
    src/bx-vdm/bx_ntvdm_bop_catalog_v1.c
    src/bx-vdm/bx_ntvdm_boot_namespace_provider_v1.c
    src/bx-vdm/bx_ntvdm_profile_search_snapshot_v1.c
    src/bx-vdm/bx_ntvdm_search_session.c
    src/bx-vdm/bx_ntvdm_search_result_v1.c
    src/bx-vdm/bx_ntvdm_search_plan_v1.c
    src/bx-vdm/bx_ntvdm_search_request_v1.c
    src/bx-vdm/bx_ntvdm_search_transaction_v1.c
    src/bx-vdm/bx_ntvdm_dem_path_search_service_v1.c
    src/bx-vdm/bx_ntvdm_bop_ingress_v1.c
    src/bx-vdm/bx_ntvdm_bop_provider_registry_v1.c
    src/bx-vdm/bx_ntvdm_dem_plane_v1.c
    src/bx-vdm/bx_ntvdm_dem_provider_v1.c
    src/bx-vdm/bx_ntvdm_dem_session_lifecycle_provider_v1.c
    src/bx-vdm/bx_ntvdm_dem_drive_provider_v1.c
    src/bx-vdm/bx_ntvdm_dem_fastio_provider_v1.c
    src/bx-vdm/bx_ntvdm_command_plane_v1.c
    src/bx-vdm/bx_ntvdm_legacy_plane_gate_v1.c
    src/bx-vdm/bx_ntvdm_bios_memory_service.c
    src/bx-vdm/bx_ntvdm_dem_boot_drive_service.c
    src/bx-vdm/bx_ntvdm_dem_debug_service.c
    src/bx-vdm/bx_ntvdm_emm_unavailable_service.c
    src/bx-vdm/bx_ntvdm_mouse_install1_mapping_service.c
    src/bx-vdm/bx_ntvdm_printer_unavailable_service.c
    src/bx-vdm/bx_ntvdm_config_done_service.c
    src/bx-vdm/bx_ntvdm_cmd_comspec_bootstrap_service.c
    src/bx-vdm/bx_ntvdm_cmd_get_next_service.c
    src/bx-vdm/bx_ntvdm_cmdinfo_v1.c
    src/bx-vdm/bx_ntvdm_cmd_keyboard_layout_service.c
    src/bx-vdm/bx_ntvdm_controlled_stop_service.c
    src/bx-vdm/bx_ntvdm_vdd_create_user_notify_service.c
    src/bx-vdm/bx_ntvdm_spckbd_init_service.c
    src/bx-vdm/bx_ntvdm_wait_if_idle_service.c
    src/bx-vdm/bx_ntvdm_dem_drive_service.c
    src/bx-vdm/bx_ntvdm_dem_ioctl_metadata_provider_v1.c
    src/bx-vdm/bx_ntvdm_dem_dta_service.c
    src/bx-vdm/bx_ntvdm_dem_hard_error_service.c
    src/bx-vdm/bx_ntvdm_cmd_boot_file_service.c
    src/bx-vdm/bx_ntvdm_cmd_current_dir_service.c
    src/bx-vdm/bx_ntvdm_cmd_set_info_service.c
    src/bx-vdm/bx_ntvdm_dem_dpb_service.c
    src/bx-vdm/bx_ntvdm_host_drive_policy.c
    tests/bx-vdm/bx_ntvdm_adapter_runtime_test.c
)
target_include_directories(bx-vdm-runtime-test PRIVATE
    src/cli src/bx-vdm)
target_compile_features(bx-vdm-runtime-test PRIVATE c_std_11)
target_compile_definitions(bx-vdm-runtime-test PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN)
target_link_libraries(bx-vdm-runtime-test PRIVATE bcrypt)

add_executable(ntdos64-historical-transport-v2-pipe-test EXCLUDE_FROM_ALL
    tests/historical_transport_v2_pipe_test.c
)
target_include_directories(ntdos64-historical-transport-v2-pipe-test PRIVATE
    src/reconstruction/transport)
target_compile_features(ntdos64-historical-transport-v2-pipe-test PRIVATE c_std_11)

# This x64 half creates a named shared backing and launches an i386 helper
# explicitly supplied on its command line. It is not a transport or runtime.
add_executable(ntdos64-shared-aperture-v1-host-fixture EXCLUDE_FROM_ALL
    tests/shared_aperture_v1_host_fixture.c
)
target_include_directories(ntdos64-shared-aperture-v1-host-fixture PRIVATE
    src/reconstruction/transport)
target_compile_features(ntdos64-shared-aperture-v1-host-fixture PRIVATE c_std_11)
target_compile_definitions(ntdos64-shared-aperture-v1-host-fixture PRIVATE
    UNICODE _UNICODE WIN32_LEAN_AND_MEAN)
ntdos64_use_wide_console_entry(ntdos64-shared-aperture-v1-host-fixture)

# S4 fetches one ordinary instruction from the same section after the i386
# original SAS lifecycle helper releases its own mapping. No BOP is executed.
add_executable(ntdos64-shared-aperture-s4-post-sas-fixture EXCLUDE_FROM_ALL
    src/reconstruction/monitor/reconstructed_monitor_s4.c
    tests/shared_aperture_s4_post_sas_fixture.c
)
target_include_directories(ntdos64-shared-aperture-s4-post-sas-fixture PRIVATE
    src/reconstruction/monitor
    src/reconstruction/transport)
target_compile_features(ntdos64-shared-aperture-s4-post-sas-fixture PRIVATE c_std_11)
target_compile_definitions(ntdos64-shared-aperture-s4-post-sas-fixture PRIVATE
    UNICODE _UNICODE WIN32_LEAN_AND_MEAN)
ntdos64_use_wide_console_entry(ntdos64-shared-aperture-s4-post-sas-fixture)

target_compile_features(ntdos64-run PRIVATE c_std_11)
target_compile_definitions(ntdos64-run PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN)
target_link_libraries(ntdos64-run PRIVATE shell32 bcrypt)
ntdos64_use_wide_console_entry(ntdos64-run)
target_compile_features(runner-engine-probe PRIVATE c_std_11)
target_compile_definitions(runner-engine-probe PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN)
ntdos64_use_wide_console_entry(runner-engine-probe)
target_compile_features(ntdos64-bochs-engine PRIVATE c_std_11)
target_compile_definitions(ntdos64-bochs-engine PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN)
target_link_libraries(ntdos64-bochs-engine PRIVATE bcrypt)
ntdos64_use_wide_console_entry(ntdos64-bochs-engine)
target_compile_features(bochs-engine-probe PRIVATE c_std_11)
target_compile_definitions(bochs-engine-probe PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN)
ntdos64_use_wide_console_entry(bochs-engine-probe)

enable_testing()
add_test(NAME byob-identity COMMAND $<TARGET_FILE:byob-identity-test>)
add_test(NAME byob-profile COMMAND $<TARGET_FILE:byob-profile-test>)
add_test(NAME ntdos64-run-policy
    COMMAND ${CMAKE_COMMAND}
        -DRUNNER=$<TARGET_FILE:ntdos64-run>
        -DENGINE_PROBE=$<TARGET_FILE:runner-engine-probe>
        -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/ntdos64-run-policy.cmake)
add_test(NAME ntdos64-bochs-engine-policy
    COMMAND ${CMAKE_COMMAND}
        -DRUNNER=$<TARGET_FILE:ntdos64-run>
        -DSHIM=$<TARGET_FILE:ntdos64-bochs-engine>
        -DFAKE_BOCHS=$<TARGET_FILE:bochs-engine-probe>
        -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/ntdos64-bochs-engine-policy.cmake)
