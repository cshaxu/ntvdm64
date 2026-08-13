[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$BuildRoot = ''
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

$seed = Join-Path $repository 'tools\Invoke-T197S6MinimalMachineLinkProbe.ps1'
$vsDevCmd = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
foreach ($input in @($seed, $vsDevCmd)) {
    if (-not (Test-Path -LiteralPath $input)) { throw "Required input missing: $input" }
}

$nativeCore = Join-Path $build 'native-core'
& powershell -ExecutionPolicy Bypass -File $seed -RepositoryRoot $repository -BuildRoot $nativeCore -WholeCpu5Core
if ($LASTEXITCODE -ne 0) { throw "CPU5 native-core seed build failed: $LASTEXITCODE" }

$config = Join-Path $nativeCore 'config.h'
$includeRoots = @('src', 'src\bochs', 'src\bochs\instrument\stubs', 'src\bx-core', 'src\bochs\iodev')
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
$probe = Join-Path $build 'finite_native_run_probe.cc'
@'
#include "bochs.h"
#include "bx-mantle/bx_ntvdm_finite_run.h"

int main()
{
  static const Bit8u bytes[] = { 0xf4 };
  bx_ntvdm_finite_run_request request;
  request.entry_bytes = bytes;
  request.entry_byte_count = sizeof(bytes);
  request.entry_physical_address = 0x1000;
  request.entry_cs = 0x0100;
  request.entry_eip = 0;
  request.instruction_tick_budget = 64;
  request.ips = 1000000;
  request.stop_on_ud_fixture = 0;
  return (int) bx_ntvdm_run_finite_bare_bytes(&request);
}
'@ | Set-Content -LiteralPath $probe -Encoding ascii
$probeObject = Join-Path $build 'finite_native_run_probe.obj'
$finiteCompile = 'call "' + $vsDevCmd + '" -arch=x86 -host_arch=x64 >nul && ' +
    (New-MsvcCompileCommand $finiteRunSource $finiteRunObject)
& cmd.exe /d /s /c $finiteCompile
if ($LASTEXITCODE -ne 0) { throw "MSVC compilation failed for finite-run helper: $LASTEXITCODE" }
$bridgeCompile = 'call "' + $vsDevCmd + '" -arch=x86 -host_arch=x64 >nul && ' +
    (New-MsvcCompileCommand $bridgeSource $bridgeObject)
& cmd.exe /d /s /c $bridgeCompile
if ($LASTEXITCODE -ne 0) { throw "MSVC compilation failed for generic #UD bridge: $LASTEXITCODE" }
$probeCompile = 'call "' + $vsDevCmd + '" -arch=x86 -host_arch=x64 >nul && ' +
    (New-MsvcCompileCommand $probe $probeObject)
& cmd.exe /d /s /c $probeCompile
if ($LASTEXITCODE -ne 0) { throw "MSVC compilation failed for finite-run fixture: $LASTEXITCODE" }

$seedObjects = Get-ChildItem -LiteralPath $nativeCore -Filter '*.obj' -File |
    Where-Object { $_.Name -ne 'minimal_machine_link_probe.obj' } |
    ForEach-Object { $_.FullName }
if ($seedObjects.Count -eq 0) { throw 'The seed build produced no reusable CPU5 objects' }
$exe = Join-Path $build 't198-s3-finite-native-run-probe.exe'
$map = Join-Path $build 'link.map'
$log = Join-Path $build 'link.log'
$response = Join-Path $build 'link.rsp'
$quotedObjects = @($probeObject, $finiteRunObject, $bridgeObject) + $seedObjects |
    ForEach-Object { '"' + $_ + '"' }
@('/nologo', ('/OUT:"' + $exe + '"'), ('/MAP:"' + $map + '"'), '/OPT:REF') + $quotedObjects |
    Set-Content -LiteralPath $response -Encoding ascii
& cmd.exe /d /s /c ('call "' + $vsDevCmd + '" -arch=x86 -host_arch=x64 >nul && link.exe @"' + $response + '"') 2>&1 |
    Tee-Object -LiteralPath $log
$linkExit = $LASTEXITCODE
if ($linkExit -ne 0) { throw "Finite native-run link failed: $linkExit" }

$runLog = Join-Path $build 'run.log'
$runCommand = '"' + $exe + '" > "' + $runLog + '" 2>&1'
& cmd.exe /d /s /c $runCommand
$runExit = $LASTEXITCODE
$record = [ordered]@{
    schema = 'ntdos64.t198.s3.finite-native-run-probe.v1'
    architecture = 'x86'
    profile = 'CPU5/Pentium-MMX, non-x86-64'
    seedBuild = 'tools/Invoke-T197S6MinimalMachineLinkProbe.ps1 -WholeCpu5Core'
    fixture = [ordered]@{
        entryBytes = 'f4 (HLT)'
        physicalAddress = '0x1000'
        cs = '0x0100'
        eip = '0x00000000'
        instructionTickBudget = 64
        ips = 1000000
    }
    forbiddenInputs = @('main.cc', 'config.cc', 'gui/siminterface.cc', 'bochs.exe', 'device archives', 'adapter', 'OpenNT', 'BOP', 'CLI')
    linkExitCode = $linkExit
    runExitCode = $runExit
    expectedRunExitCode = 0
    passed = ($linkExit -eq 0 -and $runExit -eq 0)
    linkLog = 'link.log'
    runLog = 'run.log'
    linkMap = 'link.map'
}
$record | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $build 't198-s3-finite-native-run-probe.json') -Encoding utf8
if ($runExit -ne 0) { throw "Finite native-run fixture failed: $runExit" }
Write-Host "Built and ran finite CPU5/Pentium-MMX native fixture: $exe"
