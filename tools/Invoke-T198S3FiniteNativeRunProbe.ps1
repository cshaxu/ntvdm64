[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$BuildRoot = '',
    [switch]$UdStopFixture,
    [switch]$MechanicalActionProbe,
    [string[]]$ExternalBridgeObjects = @(),
    [string[]]$ExternalLibraries = @(),
    [string]$ExternalFixtureSource = '',
    [ValidateSet('x64', 'x86')]
    [string]$HostArchitecture = 'x64'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent $PSScriptRoot
}
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
if ([string]::IsNullOrWhiteSpace($BuildRoot)) {
    $BuildRoot = Join-Path $repository 'artifacts\build\t198-s3-finite-native-run-probe'
}
$build = [IO.Path]::GetFullPath($BuildRoot)
if (Test-Path -LiteralPath $build) {
    throw "Refusing to overwrite existing build root: $build"
}
if ($MechanicalActionProbe -and ($UdStopFixture -or $ExternalBridgeObjects.Count -ne 0)) {
    throw 'Mechanical-action probe cannot be combined with a #UD fixture or external bridge.'
}
if ($MechanicalActionProbe -and ![string]::IsNullOrWhiteSpace($ExternalFixtureSource)) {
    throw 'Mechanical-action probe cannot be combined with an external fixture source.'
}

$seed = Join-Path $repository 'tools\Invoke-T197S6MinimalMachineLinkProbe.ps1'
$vsDevCmd = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
foreach ($input in @($seed, $vsDevCmd)) {
    if (-not (Test-Path -LiteralPath $input)) { throw "Required input missing: $input" }
}

$nativeCore = Join-Path $build 'native-core'
& powershell -ExecutionPolicy Bypass -File $seed -RepositoryRoot $repository -BuildRoot $nativeCore -WholeCpu5Core -HostArchitecture $HostArchitecture
if ($LASTEXITCODE -ne 0) { throw "CPU5 native-core seed build failed: $LASTEXITCODE" }

$config = Join-Path $nativeCore 'config.h'
$includeRoots = @('src', 'src\cli', 'src\bx-vdm', 'src\bx-mantle', 'src\bochs', 'src\bochs\instrument\stubs', 'src\bx-core', 'src\bochs\iodev')
function New-MsvcCompileCommand([string]$source, [string]$object) {
    $includes = @($includeRoots | Select-Object -Unique | ForEach-Object {
        '/I "' + (Join-Path $repository $_) + '"'
    }) -join ' '
    return 'cl.exe /nologo /c /std:c++14 /EHsc /MT /Gy /DWIN32 ' + $includes +
        ' /FI "' + $config + '" /Fo"' + $object + '" "' + $source + '"'
}

$finiteRunSource = Join-Path $repository 'src\bx-mantle\bx_ntvdm_finite_run.cc'
$finiteRunObject = Join-Path $build 'finite_run.obj'
$bridgeSource = Join-Path $repository 'src\bx-mantle\bx_ntvdm_generic_ud_bridge.cc'
$bridgeObject = Join-Path $build 'generic_ud_bridge.obj'
$mechanicalActionSource = Join-Path $repository 'src\bx-mantle\bx_ntvdm_mechanical_action_v1.cc'
$mechanicalActionObject = Join-Path $build 'mechanical_action.obj'
$externalBridge = $ExternalBridgeObjects.Count -ne 0
$expandedExternalBridgeObjects = @()
foreach ($object in $ExternalBridgeObjects) {
    $matches = @(Get-ChildItem -Path $object -File -ErrorAction SilentlyContinue)
    if ($matches.Count -ne 0) {
        $expandedExternalBridgeObjects += $matches.FullName
        continue
    }
    if (-not (Test-Path -LiteralPath $object -PathType Leaf)) {
        throw "External bridge object missing: $object"
    }
    $expandedExternalBridgeObjects += [IO.Path]::GetFullPath($object)
}
$ExternalBridgeObjects = $expandedExternalBridgeObjects
$fixtureBytes = if ($UdStopFixture) { '0x0f, 0x0b' } else { '0xf4' }
$fixtureStopOnUd = if ($UdStopFixture) { '1' } else { '0' }
$replacementExceptionObject = $null
if ($UdStopFixture -or $externalBridge) {
    $replacementExceptionObject = Join-Path $build 'exception_mantle_ud.obj'
    $exceptionCompile = 'call "' + $vsDevCmd + '" -arch=' + $HostArchitecture + ' -host_arch=x64 >nul && ' +
        (New-MsvcCompileCommand (Join-Path $repository 'src\bx-core\cpu\exception.cc') $replacementExceptionObject) +
        ' /DBX_NTVDM_ENABLE_MANTLE_UD_BRIDGE=1'
    & cmd.exe /d /s /c $exceptionCompile
    if ($LASTEXITCODE -ne 0) { throw "MSVC compilation failed for mantle #UD exception replacement: $LASTEXITCODE" }
}
$probe = Join-Path $build 'finite_native_run_probe.cc'
if (![string]::IsNullOrWhiteSpace($ExternalFixtureSource)) {
    $probe = [IO.Path]::GetFullPath($ExternalFixtureSource)
    if (-not (Test-Path -LiteralPath $probe -PathType Leaf)) {
        throw "External fixture source missing: $probe"
    }
}
if (![string]::IsNullOrWhiteSpace($ExternalFixtureSource)) {
    # The supplied fixture owns only its adapter session lifetime. It still
    # invokes the existing finite mantle runner and is compiled below.
} elseif ($MechanicalActionProbe) {
@"
#include "bochs.h"
#include "bx-mantle/bx_ntvdm_minimal_machine.h"
#include "bx-mantle/bx_ntvdm_mechanical_action_v1.h"

int main()
{
  bx_ntvdm_minimal_machine_c machine;
  struct bx_ntvdm_mechanical_action_v1 action;
  if (machine.initialize(0x100000, 0x100000) != BX_NTVDM_MINIMAL_MACHINE_OK) return 1;
  bx_ntvdm_mechanical_action_v1_clear(&action);
  action.action_id = 1; action.kind = BX_NTVDM_MECHANICAL_ACTION_V1_WRITE;
  action.range_count = 1; action.payload_bytes = 4;
  action.ranges[0].physical_address = 0x800; action.ranges[0].byte_count = 4;
  action.payload[0] = 0xaa; action.payload[1] = 0xbb;
  action.payload[2] = 0xcc; action.payload[3] = 0xdd;
  if (!bx_ntvdm_mantle_execute_mechanical_action_v1(&action)) return 2;
  bx_ntvdm_mechanical_action_v1_clear(&action);
  action.action_id = 2; action.kind = BX_NTVDM_MECHANICAL_ACTION_V1_WRITE;
  action.range_count = 2; action.payload_bytes = 4;
  action.ranges[0].physical_address = 0x800; action.ranges[0].byte_count = 2;
  action.ranges[1].physical_address = 0x100000; action.ranges[1].byte_count = 2;
  action.ranges[1].payload_offset = 2;
  action.payload[0] = 0x11; action.payload[1] = 0x22;
  action.payload[2] = 0x33; action.payload[3] = 0x44;
  if (bx_ntvdm_mantle_execute_mechanical_action_v1(&action)) return 3;
  bx_ntvdm_mechanical_action_v1_clear(&action);
  action.action_id = 3; action.kind = BX_NTVDM_MECHANICAL_ACTION_V1_READ;
  action.range_count = 1; action.payload_bytes = 4;
  action.ranges[0].physical_address = 0x800; action.ranges[0].byte_count = 4;
  if (!bx_ntvdm_mantle_execute_mechanical_action_v1(&action) ||
      action.payload[0] != 0xaa || action.payload[1] != 0xbb ||
      action.payload[2] != 0xcc || action.payload[3] != 0xdd) return 4;
  bx_ntvdm_mechanical_action_v1_clear(&action);
  action.action_id = 4; action.kind = BX_NTVDM_MECHANICAL_ACTION_V1_WRITE;
  action.range_count = 1; action.payload_bytes = 65535;
  action.ranges[0].physical_address = 0x10000; action.ranges[0].byte_count = 65535;
  action.payload[0] = 0x5a; action.payload[32767] = 0x3c;
  action.payload[65534] = 0xa5;
  if (!bx_ntvdm_mantle_execute_mechanical_action_v1(&action)) return 5;
  bx_ntvdm_mechanical_action_v1_clear(&action);
  action.action_id = 5; action.kind = BX_NTVDM_MECHANICAL_ACTION_V1_READ;
  action.range_count = 1; action.payload_bytes = 65535;
  action.ranges[0].physical_address = 0x10000; action.ranges[0].byte_count = 65535;
  if (!bx_ntvdm_mantle_execute_mechanical_action_v1(&action) ||
      action.payload[0] != 0x5a || action.payload[32767] != 0x3c ||
      action.payload[65534] != 0xa5) return 6;
  return machine.cleanup() == BX_NTVDM_MINIMAL_MACHINE_OK ? 0 : 7;
}
"@ | Set-Content -LiteralPath $probe -Encoding ascii
} else {
@"
#include "bochs.h"
#include "bx-mantle/bx_ntvdm_finite_run.h"

#include <string.h>

int main()
{
  static const Bit8u bytes[] = { $fixtureBytes };
  static bx_ntvdm_finite_run_request request;
  request.request_version = BX_NTVDM_FINITE_RUN_REQUEST_VERSION;
  memcpy(request.entry_bytes, bytes, sizeof(bytes));
  request.entry_byte_count = sizeof(bytes);
  request.entry_physical_address = 0x1000;
  request.entry_cs = 0x0100;
  request.entry_eip = 0;
  request.instruction_tick_budget = 64;
  request.ips = 1000000;
  request.stop_on_ud_fixture = $fixtureStopOnUd;
  request.preserve_physical_address = 0;
  request.preserve_byte_count = 0;
  int status = (int) bx_ntvdm_run_finite_bare_bytes(&request);
  return $fixtureStopOnUd ?
    (status == BX_NTVDM_FINITE_RUN_COMPLETED_UD_STOP ? 0 : status + 1) : status;
}
"@ | Set-Content -LiteralPath $probe -Encoding ascii
}
$probeObject = Join-Path $build 'finite_native_run_probe.obj'
if (!$MechanicalActionProbe) {
    $finiteCompile = 'call "' + $vsDevCmd + '" -arch=' + $HostArchitecture + ' -host_arch=x64 >nul && ' +
        (New-MsvcCompileCommand $finiteRunSource $finiteRunObject)
    & cmd.exe /d /s /c $finiteCompile
    if ($LASTEXITCODE -ne 0) { throw "MSVC compilation failed for finite-run helper: $LASTEXITCODE" }
}
if ($MechanicalActionProbe -or $externalBridge -or
    ![string]::IsNullOrWhiteSpace($ExternalFixtureSource)) {
    $actionCompile = 'call "' + $vsDevCmd + '" -arch=' + $HostArchitecture + ' -host_arch=x64 >nul && ' +
        (New-MsvcCompileCommand $mechanicalActionSource $mechanicalActionObject)
    & cmd.exe /d /s /c $actionCompile
    if ($LASTEXITCODE -ne 0) { throw "MSVC compilation failed for mechanical action: $LASTEXITCODE" }
}
if (!$MechanicalActionProbe) {
    $bridgeCompile = 'call "' + $vsDevCmd + '" -arch=' + $HostArchitecture + ' -host_arch=x64 >nul && ' +
        (New-MsvcCompileCommand $bridgeSource $bridgeObject) +
        $(if ($externalBridge) { ' /DBX_NTVDM_EXTERNAL_GENERIC_UD_BRIDGE=1' } else { '' })
    & cmd.exe /d /s /c $bridgeCompile
    if ($LASTEXITCODE -ne 0) { throw "MSVC compilation failed for generic #UD bridge: $LASTEXITCODE" }
}
$probeCompile = 'call "' + $vsDevCmd + '" -arch=' + $HostArchitecture + ' -host_arch=x64 >nul && ' +
    (New-MsvcCompileCommand $probe $probeObject)
& cmd.exe /d /s /c $probeCompile
if ($LASTEXITCODE -ne 0) { throw "MSVC compilation failed for finite-run fixture: $LASTEXITCODE" }

$seedObjects = Get-ChildItem -LiteralPath $nativeCore -Filter '*.obj' -File |
    Where-Object { $_.Name -ne 'minimal_machine_link_probe.obj' -and
        (!(($UdStopFixture -or $externalBridge)) -or $_.Name -ne 'whole_cpu_exception.obj') } |
    ForEach-Object { $_.FullName }
if ($seedObjects.Count -eq 0) { throw 'The seed build produced no reusable CPU5 objects' }
$exe = Join-Path $build 't198-s3-finite-native-run-probe.exe'
$map = Join-Path $build 'link.map'
$log = Join-Path $build 'link.log'
$response = Join-Path $build 'link.rsp'
$localObjects = @($probeObject)
if ($MechanicalActionProbe) {
    $localObjects += $mechanicalActionObject
} else {
    $localObjects += $finiteRunObject
    $localObjects += $bridgeObject
    if (! [string]::IsNullOrWhiteSpace($ExternalFixtureSource)) {
        $localObjects += $mechanicalActionObject
    }
    if ($externalBridge) { $localObjects += $mechanicalActionObject }
    if ($externalBridge) { $localObjects += $ExternalBridgeObjects }
    if ($UdStopFixture -or $externalBridge) { $localObjects += $replacementExceptionObject }
}
$quotedObjects = $localObjects + $seedObjects |
    ForEach-Object { '"' + $_ + '"' }
@('/nologo', ('/OUT:"' + $exe + '"'), ('/MAP:"' + $map + '"'), '/OPT:REF') + $quotedObjects + $ExternalLibraries |
    Set-Content -LiteralPath $response -Encoding ascii
& cmd.exe /d /s /c ('call "' + $vsDevCmd + '" -arch=' + $HostArchitecture + ' -host_arch=x64 >nul && link.exe @"' + $response + '"') 2>&1 |
    Tee-Object -LiteralPath $log
$linkExit = $LASTEXITCODE
if ($linkExit -ne 0) { throw "Finite native-run link failed: $linkExit" }

$headers = Join-Path $build 'headers.txt'
& cmd.exe /d /s /c ('call "' + $vsDevCmd + '" -arch=' + $HostArchitecture + ' -host_arch=x64 >nul && dumpbin.exe /headers "' + $exe + '"') 2>&1 |
    Tee-Object -LiteralPath $headers

$runLog = Join-Path $build 'run.log'
$runCommand = '"' + $exe + '" > "' + $runLog + '" 2>&1'
& cmd.exe /d /s /c $runCommand
$runExit = $LASTEXITCODE
$record = [ordered]@{
    schema = 'ntdos64.t198.s3.finite-native-run-probe.v2'
    architecture = $HostArchitecture
    profile = 'CPU5/Pentium-MMX, non-x86-64'
    seedBuild = 'tools/Invoke-T197S6MinimalMachineLinkProbe.ps1 -WholeCpu5Core -HostArchitecture ' + $HostArchitecture
    fixture = [ordered]@{
        entryBytes = if (![string]::IsNullOrWhiteSpace($ExternalFixtureSource)) {
            'external fixture; inspect externalFixtureSource'
        } elseif ($UdStopFixture) { '0f 0b (UD2)' } else { 'f4 (HLT)' }
        physicalAddress = '0x1000'
        cs = '0x0100'
        eip = '0x00000000'
        instructionTickBudget = 64
        udStopFixture = [bool]$UdStopFixture
        mechanicalActionProbe = [bool]$MechanicalActionProbe
        externalBridgeObjects = @($ExternalBridgeObjects | ForEach-Object { [IO.Path]::GetFileName($_) })
        externalLibraries = @($ExternalLibraries)
        externalFixtureSource = if ([string]::IsNullOrWhiteSpace($ExternalFixtureSource)) { '' } else { [IO.Path]::GetFileName($probe) }
        ips = 1000000
    }
    forbiddenInputs = if ($externalBridge) {
        @('main.cc', 'config.cc', 'gui/siminterface.cc', 'bochs.exe',
          'device archives', 'MinGW object', 'historical OpenNT runtime',
          'CLI engine')
    } else {
        @('main.cc', 'config.cc', 'gui/siminterface.cc', 'bochs.exe',
          'device archives', 'adapter', 'BOP', 'OpenNT', 'CLI')
    }
    linkExitCode = $linkExit
    runExitCode = $runExit
    expectedRunExitCode = 0
    passed = ($linkExit -eq 0 -and $runExit -eq 0)
    linkLog = 'link.log'
    runLog = 'run.log'
    linkMap = 'link.map'
    headers = 'headers.txt'
}
$record | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $build 't198-s3-finite-native-run-probe.json') -Encoding utf8
if ($runExit -ne 0) { throw "Finite native-run fixture failed: $runExit" }
Write-Host "Built and ran finite CPU5/Pentium-MMX native fixture: $exe"
