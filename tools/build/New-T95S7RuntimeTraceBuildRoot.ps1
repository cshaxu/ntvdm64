[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$R4Root = '',
    [Parameter(Mandatory = $true)][string]$BuildRoot,
    [switch]$DeferredStartupPlan
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
if ([string]::IsNullOrWhiteSpace($R4Root)) {
    $R4Root = Join-Path $repository 'artifacts\build\bochs-2.6-native-intact-msvc-r4'
}
$r4 = [IO.Path]::GetFullPath($R4Root)
$build = [IO.Path]::GetFullPath($BuildRoot)
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build directory: $build" }
$executionPlanMacro = if ($DeferredStartupPlan) { 0 } else { 1 }
$deferredPlanMacro = if ($DeferredStartupPlan) { 1 } else { 0 }

function Hash([string]$path) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing required input: $path" }
    return (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash
}
function Copy-Verified([string]$source, [string]$destination) {
    $hash = Hash $source
    Copy-Item -LiteralPath $source -Destination $destination -Force
    if ((Hash $destination) -ne $hash) { throw "Copy hash mismatch: $source" }
    return $hash
}

foreach ($path in @('ntdos64-native-container.exe','Makefile','config.h','main.cc',
    'main.o','cpu\exception.cc','cpu\exception.o','cpu\data_xfer16.cc',
    'cpu\ctrl_xfer16.cc','cpu\ctrl_xfer32.cc','cpu\libcpu.a')) {
    [void](Hash (Join-Path $r4 $path))
}

$adapterSources = @(
    'bx_ntvdm_cpu_state_abi.c','bx_ntvdm_instruction_window_abi.c',
    'bx_ntvdm_guest_range.c','bx_ntvdm_guest_write_abi.c',
    'bx_ntvdm_multi_write_abi.c','bx_ntvdm_multi_write_transaction.c',
    'bx_ntvdm_observation_transaction_abi.c','bx_ntvdm_startup_snapshot_abi.c',
    'bx_ntvdm_startup_plan_abi.c','bx_ntvdm_startup_snapshot_evidence.c',
    'bx_ntvdm_machine_profile_abi.c','bx_ntvdm_host_service_catalog.c',
    'bx_ntvdm_host_transaction_abi.c','bx_ntvdm_session_resource_abi.c',
    'bx_ntvdm_host_service_plane.c','bx_ntvdm_host_session.c',
    'bx_ntvdm_bulk_result_transaction.c','bx_ntvdm_readonly_namespace.c',
    'bx_ntvdm_dem_readonly_file_service.c','bx_ntvdm_exception_abi.c',
    'bx_ntvdm_cpu_delta_abi.c','bx_ntvdm_cpu_result_v2.c',
    'bx_ntvdm_guest_read_action_v1.c','bx_ntvdm_guest_gather_read_action_v1.c',
    'bx_ntvdm_controlled_stop_service.c',
    'bx_ntvdm_bop_catalog_v1.c',
    'bx_ntvdm_bop_ingress_v1.c','bx_ntvdm_bop_provider_registry_v1.c',
    'bx_ntvdm_dem_plane_v1.c','bx_ntvdm_command_plane_v1.c',
    'bx_ntvdm_xms_dpmi_plane_v1.c','bx_ntvdm_system_plane_v1.c',
    'bx_ntvdm_legacy_plane_gate_v1.c',
    'bx_ntvdm_adapter_runtime.c','bx_ntvdm_bios_memory_service.c',
    'bx_ntvdm_dem_boot_drive_service.c','bx_ntvdm_dem_debug_service.c',
    'bx_ntvdm_emm_unavailable_service.c',
    'bx_ntvdm_mouse_install1_mapping_service.c',
    'bx_ntvdm_printer_unavailable_service.c',
    'bx_ntvdm_config_done_service.c',
    'bx_ntvdm_cmd_comspec_bootstrap_service.c',
    'bx_ntvdm_vdd_create_user_notify_service.c',
    'bx_ntvdm_spckbd_init_service.c',
    'bx_ntvdm_wait_if_idle_service.c',
    'bx_ntvdm_dem_drive_service.c','bx_ntvdm_dem_ioctl_changeable_service.c',
    'bx_ntvdm_dem_dta_service.c','bx_ntvdm_dem_hard_error_service.c',
    'bx_ntvdm_cmd_boot_file_service.c','bx_ntvdm_cmd_current_dir_service.c',
    'bx_ntvdm_cmd_set_info_service.c',
    'bx_ntvdm_dem_dpb_service.c','bx_ntvdm_host_drive_policy.c',
    'bx_ntvdm_cmdinfo_v1.c','bx_ntvdm_cmd_get_next_service.c',
    'bx_ntvdm_cmd_keyboard_layout_service.c'
)
$cliSources = @('byob_profile.c','byob_identity.c','byob_image.c',
    'byob_launch_declaration_v1.c')

& robocopy $r4 $build /E /COPY:DAT /DCOPY:DAT /R:1 /W:1 /XJ /NFL /NDL /NJH /NJS | Out-Null
if ($LASTEXITCODE -gt 7) { throw "r4 copy failed: $LASTEXITCODE" }

$manifest = [ordered]@{
    schema = 'ntdos64.t95-s7.runtime-trace-build-root.v1'
    mode = 'generator-only-no-compiler-linker-archive-or-executable-invocation'
    r4Root = $r4
    r4ContainerSha256 = Hash (Join-Path $r4 'ntdos64-native-container.exe')
    buildRoot = $build
    macros = @(("BX_NTVDM_ENABLE_EXECUTION_PLAN={0}" -f $executionPlanMacro),'BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT=1',
        'BX_NTVDM_ENABLE_BOP_CATALOG_LISTENER=1',
        'BX_NTVDM_ENABLE_STARTUP_TRANSACTION=0',
        ("BX_NTVDM_ENABLE_DEFERRED_STARTUP_PLAN={0}" -f $deferredPlanMacro),
        'BX_NTVDM_ENABLE_CPU_RESULT_BRIDGE=0')
    bochsReplacements = @()
    adapterSources = @()
    cliSources = @()
}
foreach ($entry in @(@{ source='refs\bochs\main.cc'; destination='main.cc'; stale='main.o' },
        @{ source='src\bx-core\cpu\exception.cc'; destination='cpu\exception.cc'; stale='cpu\exception.o' },
        @{ source='src\bx-core\cpu\bx_ntvdm_exception_intercept.h'; destination='cpu\bx_ntvdm_exception_intercept.h'; stale='' })) {
    $source = Join-Path $repository $entry.source
    $destination = Join-Path $build $entry.destination
    $hash = Copy-Verified $source $destination
    if ($entry.stale -ne '' -and (Test-Path -LiteralPath (Join-Path $build $entry.stale))) {
        Remove-Item -LiteralPath (Join-Path $build $entry.stale) -Force
    }
    if ($entry.stale -ne '' -and (Test-Path -LiteralPath (Join-Path $build $entry.stale))) { throw "Stale object remains: $($entry.stale)" }
    $manifest.bochsReplacements += [ordered]@{ path=$entry.source; sha256=$hash }
}

New-Item -ItemType Directory -Path (Join-Path $build 'adapter'), (Join-Path $build 'cli') -Force | Out-Null
foreach ($header in Get-ChildItem -LiteralPath (Join-Path $repository 'src\bx-vdm') -Filter *.h -File) {
    [void](Copy-Verified $header.FullName (Join-Path $build ('adapter\' + $header.Name)))
}
foreach ($header in Get-ChildItem -LiteralPath (Join-Path $repository 'src\cli') -Filter *.h -File) {
    [void](Copy-Verified $header.FullName (Join-Path $build ('cli\' + $header.Name)))
}
foreach ($name in $adapterSources) {
    $hash = Copy-Verified (Join-Path $repository ('src\bx-vdm\' + $name)) (Join-Path $build ('adapter\' + $name))
    $manifest.adapterSources += [ordered]@{ name=$name; sha256=$hash }
}
foreach ($name in $cliSources) {
    $hash = Copy-Verified (Join-Path $repository ('src\cli\' + $name)) (Join-Path $build ('cli\' + $name))
    $manifest.cliSources += [ordered]@{ name=$name; sha256=$hash }
}

$adapterObjects = @($adapterSources | ForEach-Object { 'adapter\' + ($_ -replace '\.c$', '.obj') })
$cliObjects = @($cliSources | ForEach-Object { 'cli\' + ($_ -replace '\.c$', '.obj') })
$make = @(
    '# Generated T95 S7 runtime trace shim. Generator-only tool; do not use an old startup root.',
    '!INCLUDE Makefile','',
    ('ADAPTER_OBJS = ' + (($adapterObjects + $cliObjects) -join ' ')),'',
    'main.o: main.cc',
    ("`t`$(CXX) /c `$(BX_INCDIRS) `$(CXXFLAGS) /DBX_NTVDM_ENABLE_EXECUTION_PLAN={0} /DBX_NTVDM_ENABLE_DEFERRED_STARTUP_PLAN={1} /Iadapter /Icli /Tpmain.cc /Fomain.o" -f $executionPlanMacro,$deferredPlanMacro),'',
    'cpu\exception.o: cpu\exception.cc',
    ("`t`$(CXX) /c `$(BX_INCDIRS) `$(CXXFLAGS) /DBX_NTVDM_ENABLE_EXCEPTION_INTERCEPT=1 /DBX_NTVDM_ENABLE_BOP_CATALOG_LISTENER=1 /DBX_NTVDM_ENABLE_STARTUP_TRANSACTION=0 /DBX_NTVDM_ENABLE_CPU_RESULT_BRIDGE=0 /DBX_NTVDM_ENABLE_DEFERRED_STARTUP_PLAN={0} /Iadapter /Icli /Tpcpu\exception.cc /Focpu\exception.o" -f $deferredPlanMacro),'',
    '# The original CPU archive remains a link input; no member is extracted or repacked by this shim.',
    '# Rebuilt exception object resolves before that archive on the final link.',
    '# No original CPU object is extracted or repacked.', ''
)
foreach ($object in @($adapterObjects + $cliObjects)) {
    $source = $object -replace '\.obj$', '.c'
    $make += "${object}: $source"
    $make += "`tcl.exe /nologo /c /MT /W3 /DWIN32 /D_WINDOWS /D_CRT_SECURE_NO_WARNINGS /Iadapter /Icli /Fo$object $source"
    $make += ''
}
$make += @(
    'ntdos64-s7-runtime-trace.exe: main.o cpu\exception.o cpu\data_xfer16_trace.obj cpu\ctrl_xfer16_trace.obj cpu\ctrl_xfer32_trace.obj $(ADAPTER_OBJS) iodev/libiodev.a iodev/hdimage/libhdimage.a iodev/usb/libusb.a iodev/network/libnetwork.a iodev/sound/libsound.a cpu/libcpu.a cpu/cpudb/libcpudb.a memory/libmemory.a gui/libgui.a $(DISASM_LIB) $(BX_OBJS) $(SIMX86_OBJS) $(FPU_LIB)',
    "`tlink /nologo /subsystem:console /incremental:no /opt:ref /map:ntdos64-s7-runtime-trace.map /out:`$@ `$(BX_OBJS) `$(SIMX86_OBJS) cpu\exception.o cpu\data_xfer16_trace.obj cpu\ctrl_xfer16_trace.obj cpu\ctrl_xfer32_trace.obj iodev/libiodev.a iodev/hdimage/libhdimage.a iodev/usb/libusb.a iodev/network/libnetwork.a iodev/sound/libsound.a cpu/libcpu.a cpu/cpudb/libcpudb.a memory/libmemory.a gui/libgui.a `$(DISASM_LIB) `$(FPU_LIB) `$(GUI_LINK_OPTS) `$(MCH_LINK_FLAGS) `$(SIMX86_LINK_FLAGS) `$(READLINE_LIB) `$(EXTRA_LINK_OPTS) `$(LIBS) `$(ADAPTER_OBJS) kernel32.lib bcrypt.lib",''
)
$shim = Join-Path $build 'ntdos64-s7-runtime-trace.mak'
[IO.File]::WriteAllText($shim, ($make -join "`r`n"), [Text.UTF8Encoding]::new($false))
$manifest.cpuArchiveSha256 = Hash (Join-Path $build 'cpu\libcpu.a')
$manifest.shimSha256 = Hash $shim
$manifest.permittedBuild = 'nmake /f ntdos64-s7-runtime-trace.mak ntdos64-s7-runtime-trace.exe'
[IO.File]::WriteAllText((Join-Path $build 't95-s7-runtime-trace-build-root.json'), ($manifest | ConvertTo-Json -Depth 6), [Text.UTF8Encoding]::new($false))
Write-Host "Prepared fresh T95 S7 runtime trace root: $build"
