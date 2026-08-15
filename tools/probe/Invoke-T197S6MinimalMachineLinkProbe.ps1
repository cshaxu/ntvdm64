[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$BuildRoot = '',
    [switch]$WholeCpu5Core,
    [ValidateSet('x64', 'x86')]
    [string]$HostArchitecture = 'x64',
    [switch]$InstructionHistory,
    [switch]$RunLifecycle,
    [switch]$A20CapabilityFixture,
    [switch]$XmsPackageFixture
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
if ([string]::IsNullOrWhiteSpace($BuildRoot)) {
    $BuildRoot = Join-Path $repository 'artifacts\build\t197-s6-minimal-machine-link-probe'
}
$build = [IO.Path]::GetFullPath($BuildRoot)
if (Test-Path -LiteralPath $build) {
    throw "Refusing to overwrite existing build root: $build"
}

$projection = Join-Path $repository 'tools\build\Project-BochsConfig.ps1'
$projectionManifest = Join-Path $repository 'tools\build\t197-s6-cpu5-mantle-config-projection.json'
$vsDevCmd = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
foreach ($input in @($projection, $projectionManifest, $vsDevCmd)) {
    if (-not (Test-Path -LiteralPath $input)) { throw "Required input missing: $input" }
}

& powershell -ExecutionPolicy Bypass -File $projection -ManifestPath $projectionManifest -AnalysisRoot $build
if ($LASTEXITCODE -ne 0) { throw "CPU5 configuration projection failed: $LASTEXITCODE" }
$config = Join-Path $build 'config.h'

# This is the first native lifecycle candidate, not a product archive.  Every
# source is named here so the emitted manifest can prove the exact closure
# being tested.  CPU instruction/decode expansion is intentionally deferred to
# the linker's first unresolved native edge.
$sources = @(
    @{ Name = 'minimal_sim'; Path = 'src\bx-mantle\bx_ntvdm_minimal_sim.cc'; ExtraIncludes = @() },
    @{ Name = 'minimal_machine'; Path = 'src\bx-mantle\bx_ntvdm_minimal_machine.cc'; ExtraIncludes = @('refs\bochs\iodev') },
    @{ Name = 'a20_capability'; Path = 'src\bx-mantle\bx_ntvdm_a20_capability_v1.cc'; ExtraIncludes = @('src\bx-core', 'refs\bochs\iodev') },
    @{ Name = 'extended_memory'; Path = 'src\bx-mantle\bx_ntvdm_extended_memory_v1.cc'; ExtraIncludes = @('src\bx-core', 'refs\bochs\iodev') },
    @{ Name = 'minimal_port_space'; Path = 'src\bx-mantle\minimal_port_space.cc'; ExtraIncludes = @('refs\bochs\iodev') },
    @{ Name = 'minimal_product_shell'; Path = 'src\bx-mantle\minimal_product_shell.cc'; ExtraIncludes = @('src\bx-core', 'refs\bochs\iodev') },
    @{ Name = 'paramtree'; Path = 'src\bx-mantle\paramtree.cc'; ExtraIncludes = @() },
    @{ Name = 'logio'; Path = 'src\bx-mantle\logio.cc'; ExtraIncludes = @('src\bx-core', 'refs\bochs\iodev') },
    @{ Name = 'pc_system'; Path = 'src\bx-mantle\pc_system.cc'; ExtraIncludes = @('src\bx-core', 'refs\bochs\iodev') },
    @{ Name = 'memory'; Path = 'src\bx-core\memory\memory.cc'; ExtraIncludes = @('src\bx-core', 'refs\bochs\iodev') },
    @{ Name = 'misc_mem'; Path = 'src\bx-core\memory\misc_mem.cc'; ExtraIncludes = @('src\bx-core', 'refs\bochs\iodev') },
    @{ Name = 'cpu_init'; Path = 'src\bx-core\cpu\init.cc'; ExtraIncludes = @('src\bx-core', 'refs\bochs\iodev') },
    @{ Name = 'cpu_access'; Path = 'src\bx-core\cpu\access.cc'; ExtraIncludes = @('src\bx-core', 'refs\bochs\iodev') },
    @{ Name = 'cpu_apic'; Path = 'src\bx-core\cpu\apic.cc'; ExtraIncludes = @('src\bx-core', 'refs\bochs\iodev') },
    @{ Name = 'cpu_crregs'; Path = 'src\bx-core\cpu\crregs.cc'; ExtraIncludes = @('src\bx-core', 'refs\bochs\iodev') },
    @{ Name = 'cpu_debugstuff'; Path = 'src\bx-core\cpu\debugstuff.cc'; ExtraIncludes = @('src\bx-core', 'refs\bochs\iodev') },
    @{ Name = 'cpu_event'; Path = 'src\bx-core\cpu\event.cc'; ExtraIncludes = @('src\bx-core', 'refs\bochs\iodev') },
    @{ Name = 'cpu_fetchdecode'; Path = 'src\bx-core\cpu\fetchdecode.cc'; ExtraIncludes = @('src\bx-core', 'refs\bochs\iodev') },
    @{ Name = 'cpu_flag_ctrl_pro'; Path = 'src\bx-core\cpu\flag_ctrl_pro.cc'; ExtraIncludes = @('src\bx-core', 'refs\bochs\iodev') },
    @{ Name = 'cpu_generic_cpuid'; Path = 'src\bx-core\cpu\generic_cpuid.cc'; ExtraIncludes = @('src\bx-core', 'refs\bochs\iodev') },
    @{ Name = 'cpu_icache'; Path = 'src\bx-core\cpu\icache.cc'; ExtraIncludes = @('src\bx-core', 'refs\bochs\iodev') },
    @{ Name = 'cpu_msr'; Path = 'src\bx-core\cpu\msr.cc'; ExtraIncludes = @('src\bx-core', 'refs\bochs\iodev') },
    @{ Name = 'cpu_paging'; Path = 'src\bx-core\cpu\paging.cc'; ExtraIncludes = @('src\bx-core', 'refs\bochs\iodev') },
    @{ Name = 'cpu_proc_ctrl'; Path = 'src\bx-core\cpu\proc_ctrl.cc'; ExtraIncludes = @('src\bx-core', 'refs\bochs\iodev') },
    @{ Name = 'cpu_segment_ctrl_pro'; Path = 'src\bx-core\cpu\segment_ctrl_pro.cc'; ExtraIncludes = @('src\bx-core', 'refs\bochs\iodev') },
    @{ Name = 'cpu_smm'; Path = 'src\bx-core\cpu\smm.cc'; ExtraIncludes = @('src\bx-core', 'refs\bochs\iodev') },
    @{ Name = 'cpudb_pentium_mmx'; Path = 'src\bx-core\cpu\cpudb\pentium_mmx.cc'; ExtraIncludes = @('src\bx-core', 'refs\bochs\iodev', 'src\bx-core\cpu') },
    @{ Name = 'cpudb_amd_k6_2_chomper'; Path = 'src\bx-core\cpu\cpudb\amd_k6_2_chomper.cc'; ExtraIncludes = @('src\bx-core', 'refs\bochs\iodev', 'src\bx-core\cpu') }
)

if ($InstructionHistory) {
    $sources += @{ Name = 'instruction_history'; Path = 'src\bx-mantle\bx_ntvdm_instruction_history.cc'; ExtraIncludes = @('src\bx-core', 'refs\bochs\iodev') }
}

if ($XmsPackageFixture) {
    $sources += @(
        @{ Name = 'adapter_instruction_window'; Path = 'src\bx-vdm\bx_ntvdm_instruction_window_abi.c'; ExtraIncludes = @('src\bx-vdm', 'src\bx-mantle') },
        @{ Name = 'adapter_cpu_delta'; Path = 'src\bx-vdm\bx_ntvdm_cpu_delta_abi.c'; ExtraIncludes = @('src\bx-vdm', 'src\bx-mantle') },
        @{ Name = 'adapter_cpu_state'; Path = 'src\bx-vdm\bx_ntvdm_cpu_state_abi.c'; ExtraIncludes = @('src\bx-vdm', 'src\bx-mantle') },
        @{ Name = 'adapter_exception'; Path = 'src\bx-vdm\bx_ntvdm_exception_abi.c'; ExtraIncludes = @('src\bx-vdm', 'src\bx-mantle') },
        @{ Name = 'adapter_cpu_result'; Path = 'src\bx-vdm\bx_ntvdm_cpu_result_v2.c'; ExtraIncludes = @('src\bx-vdm', 'src\bx-mantle') },
        @{ Name = 'adapter_bop_ingress'; Path = 'src\bx-vdm\bx_ntvdm_bop_ingress_v1.c'; ExtraIncludes = @('src\bx-vdm', 'src\bx-mantle') },
        @{ Name = 'adapter_bop_registry'; Path = 'src\bx-vdm\bx_ntvdm_bop_provider_registry_v1.c'; ExtraIncludes = @('src\bx-vdm', 'src\bx-mantle') },
        @{ Name = 'adapter_xms_plane'; Path = 'src\bx-vdm\bx_ntvdm_xms_dpmi_plane_v1.c'; ExtraIncludes = @('src\bx-vdm', 'src\bx-mantle') },
        @{ Name = 'adapter_xms_session'; Path = 'src\bx-vdm\bx_ntvdm_xms_package_session_v1.c'; ExtraIncludes = @('src\bx-vdm', 'src\bx-mantle') }
    )
}

if ($WholeCpu5Core) {
    # The adopted tree is the authoritative source inventory.  The retained
    # VS2008 projects omit post-project CPU support units (stack/task/vm) and
    # the sibling FPU/disassembler directories, while CPU5 decode still links
    # those native methods.  Enumerate each adopted component directory as a
    # whole; CPU5 projection decides preprocessor reachability, never a trace.
    foreach ($component in @('cpu', 'fpu', 'memory', 'disasm')) {
        $directory = Join-Path $repository ('src\bx-core\' + $component)
        if (-not (Test-Path -LiteralPath $directory -PathType Container)) { throw "Adopted core directory missing: $directory" }
        $relativeSources = Get-ChildItem -LiteralPath $directory -File -Filter *.cc |
            Sort-Object -Property Name | ForEach-Object { $_.Name }
        foreach ($relative in $relativeSources) {
            $path = Join-Path ('src\bx-core\' + $component) $relative
            if ($sources.Path -contains $path) { continue }
            $name = 'whole_' + $component + '_' + ([IO.Path]::GetFileNameWithoutExtension($path))
            $extra = @('src\bx-core', 'refs\bochs\iodev')
            if ($component -eq 'cpu') { $extra += 'src\bx-core\cpu' }
            $sources += @{ Name = $name; Path = $path; ExtraIncludes = $extra }
        }
    }
}

$includeRoots = @('src', 'refs\bochs', 'refs\bochs\instrument\stubs')
function New-MsvcCompileCommand([string]$source, [string]$object, [string[]]$extraIncludes) {
    $includes = @($includeRoots + $extraIncludes | Select-Object -Unique | ForEach-Object {
        '/I "' + (Join-Path $repository $_) + '"'
    }) -join ' '
    if ([IO.Path]::GetExtension($source) -ieq '.c') {
        return 'cl.exe /nologo /TC /c /std:c11 /W4 /WX /MT /DWIN32 ' + $includes + ' /Fo"' + $object + '" "' + $source + '"'
    }
    return 'cl.exe /nologo /c /std:c++14 /EHsc /MT /Gy /DWIN32 ' + $includes + ' /FI "' + $config + '" /Fo"' + $object + '" "' + $source + '"'
}

$objects = @()
$compileCommands = @()
foreach ($entry in $sources) {
    $source = Join-Path $repository $entry.Path
    if (-not (Test-Path -LiteralPath $source)) { throw "Declared source missing: $source" }
    $object = Join-Path $build ($entry.Name + '.obj')
    $compileCommands += New-MsvcCompileCommand $source $object $entry.ExtraIncludes
    $objects += $object
}

$compileBatch = Join-Path $build 'compile-objects.cmd'
$compileLog = Join-Path $build 'compile.log'
@('call "' + $vsDevCmd + '" -arch=' + $HostArchitecture + ' -host_arch=x64 >nul') + $compileCommands |
    Set-Content -LiteralPath $compileBatch -Encoding ascii
& cmd.exe /d /s /c ('"' + $compileBatch + '"') 2>&1 | Tee-Object -LiteralPath $compileLog
$compileExit = $LASTEXITCODE
if ($compileExit -ne 0) { throw "MSVC compilation failed for the declared minimal native object set: $compileExit" }

$probe = Join-Path $build 'minimal_machine_link_probe.cc'
if ($XmsPackageFixture) {
@'
#include "bochs.h"
#include "bx-mantle/bx_ntvdm_minimal_machine.h"
#include "bx-vdm/bx_ntvdm_xms_package_session_v1.h"
#include <string.h>

static int call(bx_ntvdm_xms_package_session_v1 *session, unsigned service,
  unsigned ax, unsigned bx, unsigned dx, bx_ntvdm_cpu_result_v2 *result)
{
  Bit8u bytes[4] = { 0xc4, 0xc4, 0x52, (Bit8u)service };
  bx_ntvdm_instruction_window_v1 window;
  bx_ntvdm_bop_ingress_v1 ingress;
  bx_ntvdm_bop_provider_selection_v1 selection;
  bx_ntvdm_exception_event_v1 event = {};
  bx_ntvdm_cpu_state_v1 cpu;
  bx_ntvdm_instruction_window_v1_capture(&window, bytes, 4);
  if (!bx_ntvdm_bop_ingress_v1_classify(&window, &ingress) ||
      !bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection)) return 0;
  event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
  event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
  event.struct_bytes = sizeof(event);
  event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
  event.cpu_id = 1; event.vector = 6; event.fault_rip = 0x100;
  bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
  cpu.eax = ax; cpu.ebx = bx; cpu.edx = dx;
  return bx_ntvdm_xms_package_session_v1_dispatch(session, &ingress,
    &selection, &event, &cpu, &window, result);
}

int main()
{
  bx_ntvdm_minimal_machine_c machine;
  bx_ntvdm_xms_package_session_v1 session;
  bx_ntvdm_cpu_result_v2 result;
  if (machine.initialize(0x400000, 0x400000) != BX_NTVDM_MINIMAL_MACHINE_OK) return 1;
  if (!bx_ntvdm_xms_package_session_v1_initialize(&session)) return 2;
  if (!call(&session, 0, 0, 0, 0, &result) || result.cpu_delta.gpr16_values[0] != 1) return 3;
  if (!call(&session, 0, 2, 0, 0, &result) || result.cpu_delta.gpr16_values[0] != 0) return 4;
  if (!call(&session, 2, 0, 0, 64, &result) || result.cpu_delta.gpr16_values[0] != 1088) return 5;
  if (!call(&session, 11, 1088, 96, 64, &result) || result.cpu_delta.gpr16_values[1] != 1152) return 6;
  if (!call(&session, 3, 1152, 0, 96, &result) || result.cpu_delta.gpr16_values[0] != 1) return 7;
  if (!call(&session, 5, 0, 0, 0, &result) || result.cpu_delta.gpr16_values[0] != 3072) return 8;
  if (!call(&session, 1, 0, 0, 0, &result) || result.disposition != BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH) return 9;
  return machine.cleanup() == BX_NTVDM_MINIMAL_MACHINE_OK ? 0 : 10;
}
'@ | Set-Content -LiteralPath $probe -Encoding ascii
} elseif ($A20CapabilityFixture) {
@'
#include "bochs.h"
#include "bx-mantle/bx_ntvdm_minimal_machine.h"
#include "bx-mantle/bx_ntvdm_a20_capability_v1.h"

static int call(unsigned op, unsigned value, unsigned expected_status, unsigned expected_enabled)
{
  bx_ntvdm_a20_capability_request_v1 request = { BX_NTVDM_A20_CAPABILITY_V1_VERSION, op, value };
  bx_ntvdm_a20_capability_result_v1 result;
  bx_ntvdm_a20_capability_v1_dispatch(&request, &result);
  return result.status == expected_status && result.enabled == expected_enabled;
}

int main()
{
  bx_ntvdm_a20_capability_request_v1 request = { BX_NTVDM_A20_CAPABILITY_V1_VERSION, BX_NTVDM_A20_CAPABILITY_QUERY, 0 };
  bx_ntvdm_a20_capability_result_v1 result;
  bx_ntvdm_a20_capability_v1_dispatch(&request, &result);
  if (result.status != BX_NTVDM_A20_CAPABILITY_REJECTED_LIFECYCLE) return 1;
  bx_ntvdm_minimal_machine_c machine;
  if (machine.initialize(0x100000, 0x100000) != BX_NTVDM_MINIMAL_MACHINE_OK) return 2;
  if (!call(BX_NTVDM_A20_CAPABILITY_QUERY, 0, BX_NTVDM_A20_CAPABILITY_OK, 1)) return 3;
  if (!call(BX_NTVDM_A20_CAPABILITY_SET, 0, BX_NTVDM_A20_CAPABILITY_OK, 0)) return 4;
  if (!call(BX_NTVDM_A20_CAPABILITY_SET, 2, BX_NTVDM_A20_CAPABILITY_REJECTED_VALUE, 0)) return 5;
  if (!call(BX_NTVDM_A20_CAPABILITY_SET, 1, BX_NTVDM_A20_CAPABILITY_OK, 1)) return 6;
  if (machine.cleanup() != BX_NTVDM_MINIMAL_MACHINE_OK) return 7;
  bx_ntvdm_a20_capability_v1_dispatch(&request, &result);
  return result.status == BX_NTVDM_A20_CAPABILITY_REJECTED_LIFECYCLE ? 0 : 8;
}
'@ | Set-Content -LiteralPath $probe -Encoding ascii
} else {
@'
#include "bochs.h"
#include "bx-mantle/bx_ntvdm_minimal_machine.h"

int main()
{
  bx_ntvdm_minimal_machine_c machine;
  int status = (int) machine.initialize(0x100000, 0x100000);
  if (status != BX_NTVDM_MINIMAL_MACHINE_OK) return status;
  return (int) machine.cleanup();
}
'@ | Set-Content -LiteralPath $probe -Encoding ascii
}
$probeObject = Join-Path $build 'minimal_machine_link_probe.obj'
$probeCompile = 'call "' + $vsDevCmd + '" -arch=' + $HostArchitecture + ' -host_arch=x64 >nul && ' + (New-MsvcCompileCommand $probe $probeObject @())
& cmd.exe /d /s /c $probeCompile
if ($LASTEXITCODE -ne 0) { throw "MSVC compilation failed for generated link probe: $LASTEXITCODE" }

$exe = Join-Path $build 't197-s6-minimal-machine-link-probe.exe'
$map = Join-Path $build 'link.map'
$log = Join-Path $build 'link.log'
$response = Join-Path $build 'link.rsp'
$quotedObjects = @($probeObject) + $objects | ForEach-Object { '"' + $_ + '"' }
$linkResponse = @(
    '/nologo',
    ('/OUT:"' + $exe + '"'),
    ('/MAP:"' + $map + '"'),
    '/OPT:REF'
) + $quotedObjects
$linkResponse | Set-Content -LiteralPath $response -Encoding ascii
$link = 'call "' + $vsDevCmd + '" -arch=' + $HostArchitecture + ' -host_arch=x64 >nul && link.exe @"' + $response + '"'
& cmd.exe /d /s /c $link 2>&1 | Tee-Object -LiteralPath $log
$linkExit = $LASTEXITCODE

$runExit = $null
$headers = Join-Path $build 'headers.txt'
if ($linkExit -eq 0) {
    $headerCommand = 'call "' + $vsDevCmd + '" -arch=' + $HostArchitecture + ' -host_arch=x64 >nul && dumpbin.exe /headers "' + $exe + '"'
    & cmd.exe /d /s /c $headerCommand 2>&1 | Tee-Object -LiteralPath $headers
    if ($RunLifecycle) {
        & cmd.exe /d /s /c ('"' + $exe + '"')
        $runExit = $LASTEXITCODE
    }
}

$record = [ordered]@{
    schema = 'ntdos64.t197.s6.minimal-machine-link-probe.v1'
    architecture = $HostArchitecture
    profile = 'CPU5/Pentium-MMX, non-x86-64'
    wholeCpu5Core = [bool]$WholeCpu5Core
    instructionHistory = [bool]$InstructionHistory
    a20CapabilityFixture = [bool]$A20CapabilityFixture
    xmsPackageFixture = [bool]$XmsPackageFixture
    compiler = 'MSVC cl.exe/link.exe via VsDevCmd'
    configuration = 'tools/build/t197-s6-cpu5-mantle-config-projection.json'
    sources = @($sources | ForEach-Object { $_.Path })
    generatedProbe = 'minimal_machine_link_probe.cc'
    objects = @($objects | ForEach-Object { Split-Path -Leaf $_ }) + 'minimal_machine_link_probe.obj'
    forbiddenInputs = @('main.cc', 'config.cc', 'gui/siminterface.cc', 'bochs.exe', 'device archives', 'adapter', 'OpenNT')
    compileExitCode = $compileExit
    compileLog = 'compile.log'
    linkExitCode = $linkExit
    linkSucceeded = ($linkExit -eq 0)
    linkLog = 'link.log'
    linkMap = 'link.map'
    linkResponse = 'link.rsp'
    headers = if ($linkExit -eq 0) { 'headers.txt' } else { '' }
    runLifecycle = [bool]$RunLifecycle
    runExitCode = $runExit
}
$record | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $build 't197-s6-minimal-machine-link-probe.json') -Encoding utf8
if ($linkExit -ne 0) {
    Write-Host "Link did not close; retained exact linker diagnostics: $log"
    exit $linkExit
}
if ($RunLifecycle -and $runExit -ne 0) {
    throw "Minimal CPU5 lifecycle probe failed: $runExit"
}
Write-Host "Built minimal CPU5/Pentium-MMX machine link probe: $exe"
