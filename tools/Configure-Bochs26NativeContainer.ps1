[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$BuildRoot = '',
    [ValidateSet(3, 5)]
    [int]$CpuLevel = 3,
    [switch]$CompileContainer
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $scriptPath = $MyInvocation.MyCommand.Path
    if ([string]::IsNullOrWhiteSpace($scriptPath)) { throw 'RepositoryRoot was not supplied and script path is unavailable.' }
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $scriptPath)
}
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
$source = Join-Path $repository 'src\bochs'
if ([string]::IsNullOrWhiteSpace($BuildRoot)) {
    $BuildRoot = Join-Path $repository 'artifacts\build\bochs-2.6-native-nogui-ucrt-r2'
}
$build = [IO.Path]::GetFullPath($BuildRoot)
$bash = 'C:\msys64\usr\bin\bash.exe'

if (-not (Test-Path -LiteralPath $source -PathType Container)) { throw "Missing imported Bochs source: $source" }
if (-not (Test-Path -LiteralPath $bash -PathType Leaf)) { throw "Missing MSYS bash: $bash" }
$existingBuild = Test-Path -LiteralPath $build
if ($existingBuild -and -not $CompileContainer) { throw "Refusing to overwrite existing build directory: $build" }
if ($existingBuild -and $CompileContainer -and -not (Test-Path -LiteralPath (Join-Path $build 'ntdos64-native-container.json') -PathType Leaf)) {
    throw "Refusing to compile an unrecorded existing build directory: $build"
}

if (-not $existingBuild) {
    New-Item -ItemType Directory -Path $build -Force | Out-Null
    & robocopy $source $build /E /COPY:DAT /DCOPY:DAT /R:1 /W:1 /XJ /NFL /NDL /NJH /NJS | Out-Null
    if ($LASTEXITCODE -gt 7) { throw "robocopy failed with exit code $LASTEXITCODE" }
    $adapterSource = Join-Path $repository 'src\bx-ntvdm-adapter'
    $adapterBuild = Join-Path $build 'ntdos64_adapter'
    New-Item -ItemType Directory -Path $adapterBuild -Force | Out-Null
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_exception_abi.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_exception_abi.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_cpu_state_abi.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_cpu_state_abi.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_cpu_delta_abi.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_cpu_delta_abi.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_cpu_result_v2.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_cpu_result_v2.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_guest_read_action_v1.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_guest_read_action_v1.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_guest_gather_read_action_v1.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_guest_gather_read_action_v1.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_instruction_window_abi.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_instruction_window_abi.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_guest_range.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_guest_range.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_guest_write_abi.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_guest_write_abi.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_multi_write_abi.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_multi_write_abi.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_multi_write_transaction.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_multi_write_transaction.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_observation_transaction_abi.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_observation_transaction_abi.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_startup_snapshot_abi.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_startup_snapshot_abi.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_startup_snapshot_evidence.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_startup_snapshot_evidence.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_startup_plan_abi.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_startup_plan_abi.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_machine_profile_abi.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_machine_profile_abi.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_host_service_catalog.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_host_service_catalog.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_host_transaction_abi.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_host_transaction_abi.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_session_resource_abi.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_session_resource_abi.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_host_service_plane.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_host_service_plane.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_host_session.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_host_session.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_adapter_runtime.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_adapter_runtime.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_bios_memory_service.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_bios_memory_service.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_dem_debug_service.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_dem_debug_service.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_emm_unavailable_service.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_emm_unavailable_service.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_mouse_install1_mapping_service.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_mouse_install1_mapping_service.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_spckbd_init_service.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_spckbd_init_service.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_wait_if_idle_service.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_wait_if_idle_service.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_dem_drive_service.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_dem_drive_service.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_dem_dta_service.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_dem_dta_service.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_dem_hard_error_service.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_dem_hard_error_service.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_cmd_set_info_service.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_cmd_set_info_service.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_dem_dpb_service.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_dem_dpb_service.c') -Destination $adapterBuild
    # CLI owns BYOB admission.  Keep the private Bochs fixture copy aligned
    # with the published component boundary rather than the retired test name.
    $runnerSource = Join-Path $repository 'src\cli'
    Copy-Item -LiteralPath (Join-Path $runnerSource 'byob_profile.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $runnerSource 'byob_profile.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $runnerSource 'byob_identity.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $runnerSource 'byob_identity.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $runnerSource 'byob_image.h') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $runnerSource 'byob_image.c') -Destination $adapterBuild
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_exception_abi.h') -Destination (Join-Path $build 'cpu')
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_cpu_state_abi.h') -Destination (Join-Path $build 'cpu')
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_cpu_delta_abi.h') -Destination (Join-Path $build 'cpu')
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_cpu_result_v2.h') -Destination (Join-Path $build 'cpu')
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_guest_read_action_v1.h') -Destination (Join-Path $build 'cpu')
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_guest_gather_read_action_v1.h') -Destination (Join-Path $build 'cpu')
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_instruction_window_abi.h') -Destination (Join-Path $build 'cpu')
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_guest_range.h') -Destination (Join-Path $build 'cpu')
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_guest_write_abi.h') -Destination (Join-Path $build 'cpu')
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_multi_write_abi.h') -Destination (Join-Path $build 'cpu')
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_multi_write_transaction.h') -Destination (Join-Path $build 'cpu')
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_observation_transaction_abi.h') -Destination (Join-Path $build 'cpu')
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_startup_snapshot_abi.h') -Destination (Join-Path $build 'cpu')
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_startup_plan_abi.h') -Destination (Join-Path $build 'cpu')
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_machine_profile_abi.h') -Destination (Join-Path $build 'cpu')
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_host_service_catalog.h') -Destination (Join-Path $build 'cpu')
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_host_transaction_abi.h') -Destination (Join-Path $build 'cpu')
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_session_resource_abi.h') -Destination (Join-Path $build 'cpu')
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_host_service_plane.h') -Destination (Join-Path $build 'cpu')
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_host_session.h') -Destination (Join-Path $build 'cpu')
    Copy-Item -LiteralPath (Join-Path $adapterSource 'bx_ntvdm_adapter_runtime.h') -Destination (Join-Path $build 'cpu')
}

$drive = $build.Substring(0, 1).ToLowerInvariant()
$msysBuild = '/' + $drive + $build.Substring(2).Replace('\', '/')
$options = @(
    "--enable-cpu-level=$CpuLevel", '--disable-plugins', '--disable-debugger',
    '--disable-disasm', '--disable-gdb-stub', '--disable-iodebug',
    '--disable-smp', '--disable-x86-64', '--disable-long-phy-address',
    '--disable-ne2000', '--disable-pci', '--disable-usb', '--disable-usb-ohci',
    '--disable-usb-xhci', '--disable-pnic', '--disable-e1000', '--disable-sb16',
    '--disable-es1370', '--disable-gameport', '--disable-clgd54xx',
    '--disable-readline', '--without-x', '--with-nogui'
) -join ' '
$configure = "set -e; export PATH=/ucrt64/bin:/usr/bin; export CC=/ucrt64/bin/gcc CXX=/ucrt64/bin/g++; cd '$msysBuild'; mkdir -p tmp; export TMPDIR='$msysBuild/tmp' TMP='$msysBuild/tmp' TEMP='$msysBuild/tmp'; ./configure $options > configure.log 2>&1"
if (-not $existingBuild) {
    & $bash -lc $configure
    if ($LASTEXITCODE -ne 0) { throw "Native Bochs configure failed; see $build\configure.log" }

    $metadata = [ordered]@{
        schema = 'ntdos64.bochs-native-container.v1'
        source = $source
        buildRoot = $build
        toolchain = 'MSYS2 UCRT64 gcc/g++'
        configuration = "native Bochs SIM/init/exception lifecycle; nogui CPU-level-$CpuLevel profile"
        configureOptions = $options
        sourcePatches = @()
    }
    [IO.File]::WriteAllText((Join-Path $build 'ntdos64-native-container.json'), ($metadata | ConvertTo-Json), [Text.UTF8Encoding]::new($false))
}

if ($CompileContainer) {
    # Do not invoke the historical default/all graph. This declared fixture is
    # the current minimum mechanics closure and intentionally excludes bochs,
    # bximage, bxcommit, and any additional product/device target.
    # @EXE@ expands to .exe in the declared MSYS2/UCRT Windows container.
    # This is the exact custom target in the generated Makefile, not `all`.
    $make = "set -e; export PATH=/ucrt64/bin:/usr/bin; export TMPDIR='$msysBuild/tmp' TMP='$msysBuild/tmp' TEMP='$msysBuild/tmp'; cd '$msysBuild'; make ntdos64-observation-fixture.exe > build.log 2>&1"
    & $bash -lc $make
    if ($LASTEXITCODE -ne 0) { throw "Native Bochs build failed; see $build\build.log" }
}

Write-Host "Configured native Bochs container at $build"
