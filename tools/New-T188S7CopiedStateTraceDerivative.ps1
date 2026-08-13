[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$BaseRoot = '',
    [Parameter(Mandatory = $true)][string]$BuildRoot
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent $PSScriptRoot }
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
if ([string]::IsNullOrWhiteSpace($BaseRoot)) { $BaseRoot = Join-Path $repository 'artifacts\build\current\t187-passive-listener-engine-r5' }
$base = [IO.Path]::GetFullPath($BaseRoot)
$build = [IO.Path]::GetFullPath($BuildRoot)
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build directory: $build" }
function Hash([string]$Path) { if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) { throw "Missing input: $Path" }; (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash }
function Copy-Verified([string]$Source, [string]$Destination) { $hash = Hash $Source; Copy-Item -LiteralPath $Source -Destination $Destination -Force; if ((Hash $Destination) -ne $hash) { throw "Copy hash mismatch: $Source" }; $hash }

$baseEngine = Join-Path $base 'ntdos64-t187-passive-listener.exe'
if ((Hash $baseEngine) -ne 'EB55EFA8CBBB4E9B640D3AC2BFAF054AE551EB0EB793A597733E660F329CCA57') { throw 'Base root is not the accepted T187 r5 listener engine.' }
& robocopy $base $build /E /COPY:DAT /DCOPY:DAT /R:1 /W:1 /XJ /NFL /NDL /NJH /NJS | Out-Null
if ($LASTEXITCODE -gt 7) { throw "Base copy failed: $LASTEXITCODE" }

$exceptionHash = Copy-Verified (Join-Path $repository 'src\bochs\cpu\exception.cc') (Join-Path $build 'cpu\exception.cc')
$runtimeSourceHash = Copy-Verified (Join-Path $repository 'src\bx-ntvdm-adapter\bx_ntvdm_adapter_runtime.c') (Join-Path $build 'adapter\bx_ntvdm_adapter_runtime.c')
$runtimeHeaderHash = Copy-Verified (Join-Path $repository 'src\bx-ntvdm-adapter\bx_ntvdm_adapter_runtime.h') (Join-Path $build 'adapter\bx_ntvdm_adapter_runtime.h')
$make = @(
    '# Generated T188 S7 derivative: two object targets and final link.', '!INCLUDE Makefile', '',
    'cpu\exception.o: cpu\exception.cc',
    "`t`$(CXX) /c `$(BX_INCDIRS) `$(CXXFLAGS) /DBX_NTVDM_ENABLE_EXCEPTION_INTERCEPT=1 /DBX_NTVDM_ENABLE_ADAPTER_STATE_DIAGNOSTIC=1 /DBX_NTVDM_ENABLE_BOP_CATALOG_LISTENER=1 /DBX_NTVDM_ENABLE_STARTUP_TRANSACTION=0 /DBX_NTVDM_ENABLE_CPU_RESULT_BRIDGE=0 /DBX_NTVDM_ENABLE_DEFERRED_STARTUP_PLAN=1 /DBX_NTVDM_ENABLE_MACHINE_COMPOSITION=1 /Iadapter /Icli /Imachine /Tpcpu\exception.cc /Focpu\exception.o", '',
    'adapter\bx_ntvdm_adapter_runtime.obj: adapter\bx_ntvdm_adapter_runtime.c adapter\bx_ntvdm_adapter_runtime.h',
    "`tcl.exe /nologo /c /MT /W3 /DWIN32 /D_WINDOWS /D_CRT_SECURE_NO_WARNINGS /Iadapter /Icli /Foadapter\bx_ntvdm_adapter_runtime.obj adapter\bx_ntvdm_adapter_runtime.c", '',
    'ntdos64-t188-copied-state.exe: cpu\exception.o adapter\bx_ntvdm_adapter_runtime.obj',
    "`tlink /nologo /subsystem:console /incremental:no /opt:ref /map:ntdos64-t188-copied-state.map /out:`$@ `$(BX_OBJS) `$(SIMX86_OBJS) main.o cpu\exception.o iodev/libiodev.a iodev/hdimage/libhdimage.a iodev/usb/libusb.a iodev/network/libnetwork.a iodev/sound/libsound.a cpu/libcpu.a cpu/cpudb/libcpudb.a memory/libmemory.a gui/libgui.a `$(DISASM_LIB) `$(FPU_LIB) `$(GUI_LINK_OPTS) `$(MCH_LINK_FLAGS) `$(SIMX86_LINK_FLAGS) `$(READLINE_LIB) `$(EXTRA_LINK_OPTS) `$(LIBS) machine\bx_ntvdm_machine_bop_v1.obj machine\unexp_nt.c.obj machine\illegalp.c.obj vcruntime.lib adapter\bx_ntvdm_cpu_state_abi.obj adapter\bx_ntvdm_instruction_window_abi.obj adapter\bx_ntvdm_guest_range.obj adapter\bx_ntvdm_guest_write_abi.obj adapter\bx_ntvdm_multi_write_abi.obj adapter\bx_ntvdm_multi_write_transaction.obj adapter\bx_ntvdm_observation_transaction_abi.obj adapter\bx_ntvdm_startup_snapshot_abi.obj adapter\bx_ntvdm_startup_plan_abi.obj adapter\bx_ntvdm_startup_snapshot_evidence.obj adapter\bx_ntvdm_machine_profile_abi.obj adapter\bx_ntvdm_host_service_catalog.obj adapter\bx_ntvdm_host_transaction_abi.obj adapter\bx_ntvdm_session_resource_abi.obj adapter\bx_ntvdm_host_service_plane.obj adapter\bx_ntvdm_host_session.obj adapter\bx_ntvdm_bulk_result_transaction.obj adapter\bx_ntvdm_readonly_namespace.obj adapter\bx_ntvdm_dem_readonly_file_service.obj adapter\bx_ntvdm_exception_abi.obj adapter\bx_ntvdm_cpu_delta_abi.obj adapter\bx_ntvdm_cpu_result_v2.obj adapter\bx_ntvdm_guest_read_action_v1.obj adapter\bx_ntvdm_guest_gather_read_action_v1.obj adapter\bx_ntvdm_adapter_runtime.obj adapter\bx_ntvdm_boot_namespace_provider_v1.obj adapter\bx_ntvdm_profile_search_snapshot_v1.obj adapter\bx_ntvdm_search_session.obj adapter\bx_ntvdm_search_result_v1.obj adapter\bx_ntvdm_search_plan_v1.obj adapter\bx_ntvdm_search_request_v1.obj adapter\bx_ntvdm_search_transaction.obj adapter\bx_ntvdm_dem_path_search_service.obj adapter\bx_ntvdm_bop_catalog_v1.obj adapter\bx_ntvdm_bop_ingress_v1.obj adapter\bx_ntvdm_bop_provider_registry_v1.obj adapter\bx_ntvdm_dem_plane_v1.obj adapter\bx_ntvdm_dem_provider_v1.obj adapter\bx_ntvdm_dem_session_lifecycle_provider_v1.obj adapter\bx_ntvdm_dem_fastio_provider_v1.obj adapter\bx_ntvdm_command_plane_v1.obj adapter\bx_ntvdm_redir_unavailable_provider_v1.obj adapter\bx_ntvdm_legacy_plane_gate_v1.obj adapter\bx_ntvdm_bios_memory_service.obj adapter\bx_ntvdm_dem_boot_drive_service.obj adapter\bx_ntvdm_dem_debug_service.obj adapter\bx_ntvdm_emm_unavailable_service.obj adapter\bx_ntvdm_mouse_install1_mapping_service.obj adapter\bx_ntvdm_printer_unavailable_service.obj adapter\bx_ntvdm_config_done_service.obj adapter\bx_ntvdm_cmd_comspec_bootstrap_service.obj adapter\bx_ntvdm_cmd_get_next_service.obj adapter\bx_ntvdm_cmdinfo_v1.obj adapter\bx_ntvdm_cmd_keyboard_layout_service.obj adapter\bx_ntvdm_controlled_stop_service.obj adapter\bx_ntvdm_vdd_create_user_notify_service.obj adapter\bx_ntvdm_spckbd_init_service.obj adapter\bx_ntvdm_wait_if_idle_service.obj adapter\bx_ntvdm_dem_drive_service.obj adapter\bx_ntvdm_dem_ioctl_changeable_service.obj adapter\bx_ntvdm_dem_dta_service.obj adapter\bx_ntvdm_dem_hard_error_service.obj adapter\bx_ntvdm_cmd_boot_file_service.obj adapter\bx_ntvdm_cmd_current_dir_service.obj adapter\bx_ntvdm_cmd_set_info_service.obj adapter\bx_ntvdm_dem_dpb_service.obj adapter\bx_ntvdm_host_drive_policy.obj cli\byob_profile.obj cli\byob_identity.obj cli\byob_image.obj cli\byob_launch_declaration_v1.obj cli\byob_launch_plan_v2.obj kernel32.lib bcrypt.lib", ''
)
$make = $make -replace 'search_transaction\.obj', 'search_transaction_v1.obj' `
    -replace 'dem_path_search_service\.obj', 'dem_path_search_service_v1.obj'
$makePath = Join-Path $build 'ntdos64-t188-copied-state.mak'
[IO.File]::WriteAllText($makePath, ($make -join "`r`n"), [Text.UTF8Encoding]::new($false))
$text = Get-Content -LiteralPath $makePath -Raw
if (($text | Select-String -AllMatches -Pattern '(?m)^.*\.(?:o|obj):').Matches.Count -ne 2) { throw 'Unauthorized derivative target.' }
$record = [ordered]@{ schema='ntdos64.t188.s7.copied-state-derivative.v1'; baseRoot=$base; baseEngineSha256=Hash $baseEngine; objectTargets=@('cpu\exception.o','adapter\bx_ntvdm_adapter_runtime.obj'); retainedAdapterCliObjectCount=71; exceptionSourceSha256=$exceptionHash; adapterRuntimeSourceSha256=$runtimeSourceHash; adapterRuntimeHeaderSha256=$runtimeHeaderHash; makefileSha256=Hash $makePath; permittedBuild='nmake /f ntdos64-t188-copied-state.mak ntdos64-t188-copied-state.exe' }
[IO.File]::WriteAllText((Join-Path $build 't188-s7-copied-state-derivative.json'), ($record | ConvertTo-Json -Depth 4), [Text.UTF8Encoding]::new($false))
Write-Host "Prepared T188 S7 derivative: $build"
