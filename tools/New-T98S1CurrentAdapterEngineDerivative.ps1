[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$R5Root = '',
    [Parameter(Mandatory = $true)][string]$BuildRoot
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent $PSScriptRoot
}
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
if ([string]::IsNullOrWhiteSpace($R5Root)) {
    $R5Root = Join-Path $repository 'artifacts\build\bochs-2.6-native-s7-runtime-trace-msvc-r5'
}
$r5 = [IO.Path]::GetFullPath($R5Root)
$build = [IO.Path]::GetFullPath($BuildRoot)
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build directory: $build" }

function Get-Sha256([string]$Path) {
    if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) { throw "Missing required input: $Path" }
    return (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash
}
function Copy-Verified([string]$Source, [string]$Destination) {
    $hash = Get-Sha256 $Source
    Copy-Item -LiteralPath $Source -Destination $Destination -Force
    if ((Get-Sha256 $Destination) -ne $hash) { throw "Copy hash mismatch: $Source" }
    return $hash
}

# This is the source list compiled by the current adapter-runtime fixture.  It
# deliberately includes no host-namespace object and no Bochs source file.
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
    'bx_ntvdm_adapter_runtime.c','bx_ntvdm_profile_search_snapshot_v1.c',
    'bx_ntvdm_search_session.c','bx_ntvdm_search_result_v1.c',
    'bx_ntvdm_search_plan_v1.c','bx_ntvdm_search_request_v1.c',
    'bx_ntvdm_search_transaction_v1.c','bx_ntvdm_dem_path_search_service_v1.c',
    'bx_ntvdm_bop_catalog_v1.c',
    'bx_ntvdm_bop_ingress_v1.c','bx_ntvdm_bop_provider_registry_v1.c',
    'bx_ntvdm_dem_plane_v1.c','bx_ntvdm_command_plane_v1.c',
    'bx_ntvdm_legacy_plane_gate_v1.c','bx_ntvdm_bios_memory_service.c',
    'bx_ntvdm_dem_boot_drive_service.c','bx_ntvdm_dem_debug_service.c',
    'bx_ntvdm_emm_unavailable_service.c','bx_ntvdm_mouse_install1_mapping_service.c',
    'bx_ntvdm_printer_unavailable_service.c','bx_ntvdm_config_done_service.c',
    'bx_ntvdm_cmd_comspec_bootstrap_service.c','bx_ntvdm_cmd_get_next_service.c',
    'bx_ntvdm_cmdinfo_v1.c','bx_ntvdm_cmd_keyboard_layout_service.c',
    'bx_ntvdm_controlled_stop_service.c','bx_ntvdm_vdd_create_user_notify_service.c',
    'bx_ntvdm_spckbd_init_service.c','bx_ntvdm_wait_if_idle_service.c',
    'bx_ntvdm_dem_drive_service.c','bx_ntvdm_dem_ioctl_changeable_service.c',
    'bx_ntvdm_dem_dta_service.c','bx_ntvdm_dem_hard_error_service.c',
    'bx_ntvdm_cmd_boot_file_service.c','bx_ntvdm_cmd_current_dir_service.c',
    'bx_ntvdm_cmd_set_info_service.c','bx_ntvdm_dem_dpb_service.c',
    'bx_ntvdm_host_drive_policy.c'
)
$cliSources = @('byob_profile.c','byob_identity.c','byob_image.c','byob_launch_declaration_v1.c')
if (($adapterSources | Sort-Object -Unique).Count -ne $adapterSources.Count) { throw 'Duplicate adapter source.' }

foreach ($path in @('ntdos64-s7-runtime-trace.exe','Makefile','config.h','main.o',
        'cpu\exception.o','cpu\libcpu.a','iodev\libiodev.a','memory\libmemory.a',
        'gui\libgui.a','ntdos64-s7-runtime-trace.mak','t95-s7-runtime-trace-build-root.json')) {
    [void](Get-Sha256 (Join-Path $r5 $path))
}

& robocopy $r5 $build /E /COPY:DAT /DCOPY:DAT /R:1 /W:1 /XJ /NFL /NDL /NJH /NJS | Out-Null
if ($LASTEXITCODE -gt 7) { throw "r5 copy failed: $LASTEXITCODE" }
New-Item -ItemType Directory -Path (Join-Path $build 'adapter'), (Join-Path $build 'cli') -Force | Out-Null

$manifest = [ordered]@{
    schema = 'ntdos64.t98.s1.current-adapter-engine-derivative.v1'
    mode = 'generator-only-no-compiler-linker-archive-or-executable-invocation'
    r5Root = $r5
    r5BinarySha256 = Get-Sha256 (Join-Path $r5 'ntdos64-s7-runtime-trace.exe')
    buildRoot = $build
    bochsReplacementCount = 0
    adapterSources = @()
    cliSources = @()
    retainedEngineInputs = @()
}
foreach ($header in Get-ChildItem -LiteralPath (Join-Path $repository 'src\bx-ntvdm-adapter') -Filter *.h -File) {
    [void](Copy-Verified $header.FullName (Join-Path $build ('adapter\' + $header.Name)))
}
foreach ($header in Get-ChildItem -LiteralPath (Join-Path $repository 'src\cli') -Filter *.h -File) {
    [void](Copy-Verified $header.FullName (Join-Path $build ('cli\' + $header.Name)))
}
foreach ($name in $adapterSources) {
    $source = Join-Path $repository ('src\bx-ntvdm-adapter\' + $name)
    $hash = Copy-Verified $source (Join-Path $build ('adapter\' + $name))
    $object = Join-Path $build ('adapter\' + ($name -replace '\.c$', '.obj'))
    if (Test-Path -LiteralPath $object) { Remove-Item -LiteralPath $object -Force }
    $manifest.adapterSources += [ordered]@{ name=$name; sha256=$hash }
}
foreach ($name in $cliSources) {
    $source = Join-Path $repository ('src\cli\' + $name)
    $hash = Copy-Verified $source (Join-Path $build ('cli\' + $name))
    $object = Join-Path $build ('cli\' + ($name -replace '\.c$', '.obj'))
    if (Test-Path -LiteralPath $object) { Remove-Item -LiteralPath $object -Force }
    $manifest.cliSources += [ordered]@{ name=$name; sha256=$hash }
}

$adapterObjects = @($adapterSources | ForEach-Object { 'adapter\' + ($_ -replace '\.c$', '.obj') })
$cliObjects = @($cliSources | ForEach-Object { 'cli\' + ($_ -replace '\.c$', '.obj') })
$allObjects = @($adapterObjects + $cliObjects)
$make = @(
    '# Generated T98 S1 derivative: retains r5 Bochs/engine artifacts and rebuilds only the listed adapter/CLI objects.',
    '!INCLUDE Makefile','',
    ('ADAPTER_OBJS = ' + ($allObjects -join ' ')),'',
    '# No Bochs C/C++ compilation rule is present.  Existing r5 main, exception, CPU, device and simulator inputs remain unchanged.',''
)
foreach ($object in $allObjects) {
    $source = $object -replace '\.obj$', '.c'
    $make += "${object}: $source"
    $make += "`tcl.exe /nologo /c /MT /W3 /DWIN32 /D_WINDOWS /D_CRT_SECURE_NO_WARNINGS /Iadapter /Icli /Fo$object $source"
    $make += ''
}
$make += @(
    '# Deliberately no Bochs archive is a make prerequisite: inherited Makefile rules would recurse into devices.',
    'ntdos64-t98-current-adapter.exe: $(ADAPTER_OBJS)',
    "`tlink /nologo /subsystem:console /incremental:no /opt:ref /map:ntdos64-t98-current-adapter.map /out:`$@ `$(BX_OBJS) `$(SIMX86_OBJS) cpu\exception.o iodev/libiodev.a iodev/hdimage/libhdimage.a iodev/usb/libusb.a iodev/network/libnetwork.a iodev/sound/libsound.a cpu/libcpu.a cpu/cpudb/libcpudb.a memory/libmemory.a gui/libgui.a `$(DISASM_LIB) `$(FPU_LIB) `$(GUI_LINK_OPTS) `$(MCH_LINK_FLAGS) `$(SIMX86_LINK_FLAGS) `$(READLINE_LIB) `$(EXTRA_LINK_OPTS) `$(LIBS) `$(ADAPTER_OBJS) kernel32.lib bcrypt.lib",''
)
$shim = Join-Path $build 'ntdos64-t98-current-adapter.mak'
[IO.File]::WriteAllText($shim, ($make -join "`r`n"), [Text.UTF8Encoding]::new($false))

if ((Get-Content -LiteralPath $shim -Raw) -match '(^|\r?\n)(main\.o|cpu\\exception\.o):') { throw 'Derivative shim unexpectedly rebuilds a Bochs object.' }
if ((Get-Content -LiteralPath $shim -Raw) -match 'host_namespace') { throw 'Derivative shim unexpectedly admits host namespace.' }
if ((Get-Content -LiteralPath $shim -Raw) -match '^ntdos64-t98-current-adapter\.exe:.*(?:iodev|cpu\\libcpu|memory|gui)') { throw 'Derivative shim makes a retained Bochs archive a prerequisite.' }
foreach ($path in @('main.o','cpu\exception.o','cpu\libcpu.a','iodev\libiodev.a','memory\libmemory.a','gui\libgui.a')) {
    $manifest.retainedEngineInputs += [ordered]@{ path=$path; sha256=(Get-Sha256 (Join-Path $build $path)) }
}
$manifest.shimSha256 = Get-Sha256 $shim
$manifest.permittedBuild = 'nmake /f ntdos64-t98-current-adapter.mak ntdos64-t98-current-adapter.exe'
[IO.File]::WriteAllText((Join-Path $build 't98-s1-current-adapter-engine-derivative.json'), ($manifest | ConvertTo-Json -Depth 7), [Text.UTF8Encoding]::new($false))
Write-Host "Prepared fresh T98 S1 current-adapter derivative: $build"
