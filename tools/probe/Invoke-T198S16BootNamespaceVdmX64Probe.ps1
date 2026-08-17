param(
    [string]$RepositoryRoot = '',
    [string]$BuildRoot = '',
    [ValidateSet('boot-namespace', 'boot-namespace-provider', 'profile-search-snapshot', 'dem-package', 'dem-drive-view-provider', 'dem-drive-view-session', 'dem-whole-provider-session', 'dem-loader-family', 'dem-lifecycle', 'dem-profile', 'readonly-file', 'global-bop', 'command-entry', 'command-bootstrap', 'command-launch-execution', 'command-console-keyboard', 'command-lifecycle', 'command-package', 'command-bootstrap-profile-session')]
    [string]$Fixture = 'boot-namespace',
    [ValidateSet('x64')]
    [string]$HostArchitecture = 'x64'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
if ([string]::IsNullOrWhiteSpace($BuildRoot)) {
    $BuildRoot = Join-Path $repository 'artifacts\build\t198-s16-bx-vdm-x64-r1'
}
$build = [IO.Path]::GetFullPath($BuildRoot)
if (Test-Path -LiteralPath $build) {
    throw "Refusing to overwrite existing build root: $build"
}

$vsDevCmd = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (-not (Test-Path -LiteralPath $vsDevCmd -PathType Leaf)) {
    throw "Required MSVC environment entry point missing: $vsDevCmd"
}

# This is the exact source closure retained by the former i686 composition
# witness.  It deliberately contains only the boot-namespace composition,
# its generic ingress/session bridge, and direct adapter dependencies.
$sourceRelatives = @(
    'src\bx-vdm\bx_ntvdm_boot_namespace_composition_v1.c',
    'src\bx-vdm\bx_ntvdm_bios_memory_service.c',
    'src\bx-vdm\bx_ntvdm_config_done_service.c',
    'src\bx-vdm\bx_ntvdm_boot_namespace_plane_v1.c',
    'src\bx-vdm\bx_ntvdm_boot_namespace_provider_v1.c',
    'src\bx-vdm\bx_ntvdm_bop_sequence_observation_v1.c',
    'src\bx-vdm\bx_ntvdm_bop_ingress_v1.c',
    'src\bx-vdm\bx_ntvdm_bop_provider_registry_v1.c',
    'src\bx-vdm\bx_ntvdm_bulk_result_transaction.c',
    'src\bx-vdm\bx_ntvdm_cmd_boot_file_service.c',
    'src\bx-vdm\bx_ntvdm_command_boot_input_v1.c',
    'src\bx-vdm\bx_ntvdm_command_bootstrap_provider_v1.c',
    'src\bx-vdm\bx_ntvdm_command_launch_execution_provider_v1.c',
    'src\bx-vdm\bx_ntvdm_command_lifecycle_provider_v1.c',
    'src\bx-vdm\bx_ntvdm_command_execution_lifecycle_v1.c',
    'src\bx-vdm\bx_ntvdm_command_stream_session_v1.c',
    'src\bx-vdm\bx_ntvdm_cmd_comspec_bootstrap_service.c',
    'src\bx-vdm\bx_ntvdm_cmd_get_next_service.c',
    'src\bx-vdm\bx_ntvdm_cmd_current_dir_service.c',
    'src\bx-vdm\bx_ntvdm_command_host_context_v1.c',
    'src\bx-vdm\bx_ntvdm_command_console_capability_v1.c',
    'src\bx-vdm\bx_ntvdm_command_console_keyboard_provider_v1.c',
    'src\bx-vdm\bx_ntvdm_cmd_keyboard_layout_service.c',
    'src\bx-vdm\bx_ntvdm_command_profile_consumer_v1.c',
    'src\bx-vdm\bx_ntvdm_cmdinfo_v1.c',
    'src\bx-vdm\bx_ntvdm_cmd_set_info_service.c',
    'src\bx-vdm\bx_ntvdm_command_launch_plane_v1.c',
    'src\bx-vdm\bx_ntvdm_command_plane_v1.c',
    'src\bx-vdm\bx_ntvdm_command_package_facade_v1.c',
    'src\bx-vdm\bx_ntvdm_command_package_session_v1.c',
    'src\bx-vdm\bx_ntvdm_command_provider_v1.c',
    'src\bx-vdm\bx_ntvdm_redir_package_facade_v1.c',
    'src\bx-vdm\bx_ntvdm_debugger_package_facade_v1.c',
    'src\bx-vdm\bx_ntvdm_top_level_package_facade_v1.c',
    'src\bx-vdm\bx_ntvdm_machine_bop_facade_v1.c',
    'src\bx-vdm\bx_ntvdm_system_plane_v1.c',
    'src\bx-vdm\bx_ntvdm_cpu_delta_abi.c',
    'src\bx-vdm\bx_ntvdm_cpu_result_v2.c',
    'src\bx-vdm\bx_ntvdm_cpu_state_abi.c',
    'src\bx-vdm\bx_ntvdm_dem_path_search_service_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_dta_service.c',
    'src\bx-vdm\bx_ntvdm_dem_load_dos_service.c',
    'src\bx-vdm\bx_ntvdm_dem_debug_service.c',
    'src\bx-vdm\bx_ntvdm_dem_boot_drive_service.c',
    'src\bx-vdm\bx_ntvdm_dem_cli_unavailable_provider_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_dpb_service.c',
    'src\bx-vdm\bx_ntvdm_dem_clock_service_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_check_path_service_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_drive_provider_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_error_lock_plane_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_fastio_provider_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_fcb_provider_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_fcb_search_service_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_computer_name_service_v1.c',
    'src\bx-vdm\bx_ntvdm_session_host_context_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_cwd_context_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_path_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_file_view_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_file_session_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_overlay_store_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_overlay_file_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_overlay_namespace_view_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_overlay_mutation_backend_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_overlay_metadata_backend_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_overlay_resolver_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_overlay_namespace_backend_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_overlay_handle_backend_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_virtual_namespace_view_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_virtual_namespace_backend_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_virtual_mutation_backend_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_virtual_metadata_backend_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_fcb_overlay_backend_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_local_file_backend_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_cwd_service_v2.c',
    'src\bx-vdm\bx_ntvdm_dem_full_dpb_service_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_gset_plane_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_drive_view_provider_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_media_id_service_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_volume_provider_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_hard_error_service.c',
    'src\bx-vdm\bx_ntvdm_dem_ioctl_metadata_provider_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_raw_media_provider_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_misc_plane_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_package_facade_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_profile_consumer_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_package_session_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_process_owner_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_whole_provider_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_handle_partition_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_handle_route_partition_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_fcb_handle_partition_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_fcb_io_route_partition_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_fcb_wildcard_partition_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_fcb_path_route_partition_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_namespace_partition_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_namespace_identity_observation_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_namespace_route_partition_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_search_partition_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_plane_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_provider_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_readonly_file_service.c',
    'src\bx-vdm\bx_ntvdm_dem_readonly_namespace_failure_provider_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_open_observation_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_session_lifecycle_provider_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_system_symbol_service.c',
    'src\bx-vdm\bx_ntvdm_emm_unavailable_service.c',
    'src\bx-vdm\bx_ntvdm_exception_abi.c',
    'src\bx-vdm\bx_ntvdm_guest_gather_read_action_v1.c',
    'src\bx-vdm\bx_ntvdm_guest_range.c',
    'src\bx-vdm\bx_ntvdm_guest_read_action_v1.c',
    'src\bx-vdm\bx_ntvdm_guest_write_abi.c',
    'src\bx-vdm\bx_ntvdm_host_drive_policy.c',
    'src\bx-vdm\bx_ntvdm_host_namespace.c',
    'src\bx-vdm\bx_ntvdm_host_volume_snapshot_v1.c',
    'src\bx-vdm\bx_ntvdm_instruction_window_abi.c',
    'src\bx-vdm\bx_ntvdm_multi_write_abi.c',
    'src\bx-vdm\bx_ntvdm_multi_write_transaction.c',
    'src\bx-vdm\bx_ntvdm_mutation_profile_v1.c',
    'src\bx-vdm\bx_ntvdm_mutation_overlay_v1.c',
    'src\bx-vdm\bx_ntvdm_startup_configuration_policy_v1.c',
    'src\bx-vdm\bx_ntvdm_startup_configuration_provider_v1.c',
    'src\bx-vdm\bx_ntvdm_mouse_install1_mapping_service.c',
    'src\bx-vdm\bx_ntvdm_profile_search_snapshot_v1.c',
    'src\bx-vdm\bx_ntvdm_printer_unavailable_service.c',
    'src\bx-vdm\bx_ntvdm_readonly_namespace.c',
    'src\bx-vdm\bx_ntvdm_search_plan_v1.c',
    'src\bx-vdm\bx_ntvdm_search_request_v1.c',
    'src\bx-vdm\bx_ntvdm_search_result_v1.c',
    'src\bx-vdm\bx_ntvdm_search_session.c',
    'src\bx-vdm\bx_ntvdm_search_transaction_v1.c',
    'src\bx-vdm\bx_ntvdm_spckbd_init_service.c',
    'src\bx-vdm\bx_ntvdm_terminal_observation_v1.c',
    'src\bx-vdm\bx_ntvdm_normal_return_observation_v1.c',
    'src\bx-vdm\bx_ntvdm_normal_terminal_sequence_observation_v1.c',
    'src\bx-vdm\bx_ntvdm_vdm_generic_ud_bridge_v1.c',
    'src\bx-vdm\bx_ntvdm_vdd_create_user_notify_service.c',
    'src\cli\byob_identity.c',
    'src\cli\byob_image.c',
    'src\cli\byob_launch_plan_v2.c',
    'src\cli\byob_launch_declaration_v1.c',
    'src\cli\byob_profile.c'
)
if ($Fixture -eq 'dem-package') {
    $sourceRelatives += 'tests\bx-vdm\bx_ntvdm_dem_package_family_v1_test.c'
} elseif ($Fixture -eq 'dem-drive-view-provider') {
    $sourceRelatives += @(
        'tests\bx-vdm\bx_ntvdm_mantle_mechanical_action_decline_stub.c',
        'tests\bx-vdm\bx_ntvdm_dem_drive_view_provider_v1_test.c'
    )
} elseif ($Fixture -eq 'dem-drive-view-session') {
    $sourceRelatives += 'tests\bx-vdm\bx_ntvdm_dem_drive_view_session_v1_test.c'
} elseif ($Fixture -eq 'dem-whole-provider-session') {
    $sourceRelatives += 'tests\bx-vdm\bx_ntvdm_dem_whole_provider_session_v1_test.c'
} elseif ($Fixture -eq 'dem-loader-family') {
    $sourceRelatives += 'tests\bx-vdm\bx_ntvdm_dem_loader_family_v1_test.c'
} elseif ($Fixture -eq 'boot-namespace-provider') {
    $sourceRelatives += @(
        'tests\bx-vdm\bx_ntvdm_mantle_mechanical_action_decline_stub.c',
        'tests\bx-vdm\bx_ntvdm_boot_namespace_provider_v1_test.c'
    )
} elseif ($Fixture -eq 'profile-search-snapshot') {
    $sourceRelatives += @(
        'tests\bx-vdm\bx_ntvdm_mantle_mechanical_action_decline_stub.c',
        'tests\bx-vdm\bx_ntvdm_profile_search_snapshot_v1_test.c'
    )
} elseif ($Fixture -eq 'dem-lifecycle') {
    $sourceRelatives += 'tests\bx-vdm\bx_ntvdm_dem_session_lifecycle_provider_v1_test.c'
} elseif ($Fixture -eq 'dem-profile') {
    $sourceRelatives += @(
        'tests\bx-vdm\bx_ntvdm_mantle_mechanical_action_decline_stub.c',
        'tests\bx-vdm\bx_ntvdm_dem_package_profile_v1_test.c'
    )
} elseif ($Fixture -eq 'readonly-file') {
    $sourceRelatives += 'tests\bx-vdm\bx_ntvdm_dem_readonly_file_service_test.c'
    $sourceRelatives += 'tests\bx-vdm\bx_ntvdm_mantle_mechanical_action_decline_stub.c'
} elseif ($Fixture -eq 'global-bop') {
    $sourceRelatives += @(
        'src\bx-vdm\bx_ntvdm_native_bop_composition_v1.c',
        'src\bx-vdm\bx_ntvdm_xms_package_session_v1.c',
        'src\bx-vdm\bx_ntvdm_dpmi_package_session_v1.c',
        'src\bx-vdm\bx_ntvdm_xms_dpmi_plane_v1.c',
        'tests\bx-vdm\bx_ntvdm_mantle_mechanical_action_decline_stub.c',
        'tests\bx-vdm\bx_ntvdm_global_bop_composition_v1_test.c'
    )
} elseif ($Fixture -eq 'command-entry') {
    $sourceRelatives += @(
        'tests\bx-vdm\bx_ntvdm_mantle_mechanical_action_decline_stub.c',
        'tests\bx-vdm\bx_ntvdm_cmd_get_next_service_test.c'
    )
} elseif ($Fixture -eq 'command-bootstrap') {
    $sourceRelatives += @(
        'tests\bx-vdm\bx_ntvdm_mantle_mechanical_action_decline_stub.c',
        'tests\bx-vdm\bx_ntvdm_command_bootstrap_provider_v1_test.c'
    )
} elseif ($Fixture -eq 'command-launch-execution') {
    $sourceRelatives += @(
        'tests\bx-vdm\bx_ntvdm_mantle_mechanical_action_decline_stub.c',
        'tests\bx-vdm\bx_ntvdm_command_launch_execution_provider_v1_test.c'
    )
} elseif ($Fixture -eq 'command-console-keyboard') {
    $sourceRelatives += @(
        'tests\bx-vdm\bx_ntvdm_mantle_mechanical_action_decline_stub.c',
        'tests\bx-vdm\bx_ntvdm_command_console_keyboard_provider_v1_test.c'
    )
} elseif ($Fixture -eq 'command-lifecycle') {
    $sourceRelatives += @(
        'tests\bx-vdm\bx_ntvdm_mantle_mechanical_action_decline_stub.c',
        'tests\bx-vdm\bx_ntvdm_command_lifecycle_provider_v1_test.c'
    )
} elseif ($Fixture -eq 'command-package') {
    $sourceRelatives += @(
        'tests\bx-vdm\bx_ntvdm_mantle_mechanical_action_decline_stub.c',
        'tests\bx-vdm\bx_ntvdm_command_package_family_v1_test.c'
    )
} elseif ($Fixture -eq 'command-bootstrap-profile-session') {
    $sourceRelatives += @(
        'tests\bx-vdm\bx_ntvdm_command_bootstrap_profile_session_v1_test.c'
    )
} else {
    $sourceRelatives += 'tests\bx-vdm\bx_ntvdm_boot_namespace_composition_v1_test.c'
}
if ($Fixture -ne 'global-bop') {
    $sourceRelatives += 'tests\bx-vdm\bx_ntvdm_native_bop_composition_decline_stub.c'
}
$sources = @($sourceRelatives | ForEach-Object { Join-Path $repository $_ })
foreach ($source in $sources) {
    if (-not (Test-Path -LiteralPath $source -PathType Leaf)) {
        throw "Required source missing: $source"
    }
}

$forbidden = @('src\bx-core', 'src\bx-mantle', 'refs\bochs', 'CLI engine', 'MinGW',
    'OpenNT runtime', 'main.cc', 'bochs.exe', 'device archive')
New-Item -ItemType Directory -Path $build | Out-Null
$objectsDirectory = Join-Path $build 'obj'
New-Item -ItemType Directory -Path $objectsDirectory | Out-Null
$compileLog = Join-Path $build 'compile.log'
$includeRoots = @('src', 'src\cli', 'src\bx-vdm', 'src\bx-mantle') |
    ForEach-Object { '/I "' + (Join-Path $repository $_) + '"' }
$includeArguments = $includeRoots -join ' '
$objects = @()
$compileCommands = @('@echo off', ('call "' + $vsDevCmd + '" -arch=' +
    $HostArchitecture + ' -host_arch=x64 >nul'), 'if errorlevel 1 exit /b %errorlevel%')

foreach ($source in $sources) {
    $object = Join-Path $objectsDirectory (([IO.Path]::GetFileNameWithoutExtension($source)) + '.obj')
    $command = 'cl.exe /nologo /TC /c /std:c11 /W4 /WX /MT /DWIN32 /D_CRT_SECURE_NO_WARNINGS ' +
        $includeArguments + ' /Fo"' + $object + '" "' + $source + '"'
    $compileCommands += $command
    $compileCommands += 'if errorlevel 1 exit /b %errorlevel%'
    $objects += $object
}
$compileBatch = Join-Path $build 'compile.cmd'
$compileCommands | Set-Content -LiteralPath $compileBatch -Encoding ascii
$compileOutput = & cmd.exe /d /s /c $compileBatch 2>&1
$compileExit = $LASTEXITCODE
$compileOutput | Out-File -LiteralPath $compileLog
if ($compileExit -ne 0) { throw "MSVC C source closure compilation failed: $compileExit" }

$exe = Join-Path $build ('t198-s16-bx-vdm-' + $Fixture + '.exe')
$map = Join-Path $build 'link.map'
$response = Join-Path $build 'link.rsp'
$linkLog = Join-Path $build 'link.log'
@('/nologo', ('/OUT:"' + $exe + '"'), ('/MAP:"' + $map + '"'), '/OPT:REF',
    '/STACK:8388608') +
    @($objects | ForEach-Object { '"' + $_ + '"' }) + @('bcrypt.lib', 'ntdll.lib') |
    Set-Content -LiteralPath $response -Encoding ascii
& cmd.exe /d /s /c ('call "' + $vsDevCmd + '" -arch=' + $HostArchitecture +
    ' -host_arch=x64 >nul && link.exe @"' + $response + '"') 2>&1 |
    Tee-Object -LiteralPath $linkLog
$linkExit = $LASTEXITCODE
if ($linkExit -ne 0) { throw "MSVC x64 bx-vdm fixture link failed: $linkExit" }

$headers = Join-Path $build 'headers.txt'
& cmd.exe /d /s /c ('call "' + $vsDevCmd + '" -arch=' + $HostArchitecture +
    ' -host_arch=x64 >nul && dumpbin.exe /headers "' + $exe + '"') 2>&1 |
    Tee-Object -LiteralPath $headers
$runLog = Join-Path $build 'run.log'
& cmd.exe /d /s /c ('"' + $exe + '" > "' + $runLog + '" 2>&1')
$runExit = $LASTEXITCODE
$record = [ordered]@{
    schema = 'ntdos64.t198.s16.bx-vdm-x64-probe.v2'
    architecture = $HostArchitecture
    compiler = 'MSVC cl.exe/link.exe via VsDevCmd'
    runtime = '/MT'
    stackReserveBytes = 8388608
    platformLibraries = @('bcrypt.lib', 'ntdll.lib')
    sourceClosure = $sourceRelatives
    fixture = if ($Fixture -eq 'dem-package') {
        'tests/bx-vdm/bx_ntvdm_dem_package_family_v1_test.c'
    } elseif ($Fixture -eq 'dem-drive-view-provider') {
        'tests/bx-vdm/bx_ntvdm_dem_drive_view_provider_v1_test.c'
    } elseif ($Fixture -eq 'dem-drive-view-session') {
        'tests/bx-vdm/bx_ntvdm_dem_drive_view_session_v1_test.c'
    } elseif ($Fixture -eq 'dem-whole-provider-session') {
        'tests/bx-vdm/bx_ntvdm_dem_whole_provider_session_v1_test.c'
    } elseif ($Fixture -eq 'dem-loader-family') {
        'tests/bx-vdm/bx_ntvdm_dem_loader_family_v1_test.c'
    } elseif ($Fixture -eq 'boot-namespace-provider') {
        'tests/bx-vdm/bx_ntvdm_boot_namespace_provider_v1_test.c'
    } elseif ($Fixture -eq 'profile-search-snapshot') {
        'tests/bx-vdm/bx_ntvdm_profile_search_snapshot_v1_test.c'
    } elseif ($Fixture -eq 'dem-lifecycle') {
        'tests/bx-vdm/bx_ntvdm_dem_session_lifecycle_provider_v1_test.c'
    } elseif ($Fixture -eq 'readonly-file') {
        'tests/bx-vdm/bx_ntvdm_dem_readonly_file_service_test.c'
    } elseif ($Fixture -eq 'global-bop') {
        'tests/bx-vdm/bx_ntvdm_global_bop_composition_v1_test.c'
    } elseif ($Fixture -eq 'command-entry') {
        'tests/bx-vdm/bx_ntvdm_cmd_get_next_service_test.c'
    } elseif ($Fixture -eq 'command-bootstrap') {
        'tests/bx-vdm/bx_ntvdm_command_bootstrap_provider_v1_test.c'
    } elseif ($Fixture -eq 'command-launch-execution') {
        'tests/bx-vdm/bx_ntvdm_command_launch_execution_provider_v1_test.c'
    } elseif ($Fixture -eq 'command-console-keyboard') {
        'tests/bx-vdm/bx_ntvdm_command_console_keyboard_provider_v1_test.c'
    } elseif ($Fixture -eq 'command-lifecycle') {
        'tests/bx-vdm/bx_ntvdm_command_lifecycle_provider_v1_test.c'
    } elseif ($Fixture -eq 'command-package') {
        'tests/bx-vdm/bx_ntvdm_command_package_family_v1_test.c'
    } elseif ($Fixture -eq 'command-bootstrap-profile-session') {
        'tests/bx-vdm/bx_ntvdm_command_bootstrap_profile_session_v1_test.c'
    } else {
        'tests/bx-vdm/bx_ntvdm_boot_namespace_composition_v1_test.c'
    }
    forbiddenInputs = $forbidden
    linkExitCode = $linkExit
    runExitCode = $runExit
    expectedRunExitCode = 0
    passed = ($linkExit -eq 0 -and $runExit -eq 0)
    compileLog = 'compile.log'
    linkLog = 'link.log'
    linkMap = 'link.map'
    headers = 'headers.txt'
    runLog = 'run.log'
}
$record | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $build 't198-s16-bx-vdm-boot-namespace.json') -Encoding utf8
if ($runExit -ne 0) { throw "MSVC x64 bx-vdm fixture failed: $runExit" }
Write-Host "Built and ran MSVC x64 bx-vdm fixture ($Fixture): $exe"
