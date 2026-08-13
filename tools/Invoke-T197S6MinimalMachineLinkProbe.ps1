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
    $BuildRoot = Join-Path $repository 'artifacts\build\t197-s6-minimal-machine-link-probe'
}
$build = [IO.Path]::GetFullPath($BuildRoot)
if (Test-Path -LiteralPath $build) {
    throw "Refusing to overwrite existing build root: $build"
}

$projection = Join-Path $repository 'tools\Project-BochsConfig.ps1'
$projectionManifest = Join-Path $repository 'tools\t197-s6-cpu5-mantle-config-projection.json'
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
    @{ Name = 'minimal_machine'; Path = 'src\bx-mantle\bx_ntvdm_minimal_machine.cc'; ExtraIncludes = @('src\bochs\iodev') },
    @{ Name = 'minimal_port_space'; Path = 'src\bx-mantle\minimal_port_space.cc'; ExtraIncludes = @('src\bochs\iodev') },
    @{ Name = 'paramtree'; Path = 'src\bx-mantle\paramtree.cc'; ExtraIncludes = @() },
    @{ Name = 'logio'; Path = 'src\bx-mantle\logio.cc'; ExtraIncludes = @('src\bx-core', 'src\bochs\iodev') },
    @{ Name = 'pc_system'; Path = 'src\bx-mantle\pc_system.cc'; ExtraIncludes = @('src\bx-core', 'src\bochs\iodev') },
    @{ Name = 'memory'; Path = 'src\bx-core\memory\memory.cc'; ExtraIncludes = @('src\bx-core', 'src\bochs\iodev') },
    @{ Name = 'misc_mem'; Path = 'src\bx-core\memory\misc_mem.cc'; ExtraIncludes = @('src\bx-core', 'src\bochs\iodev') },
    @{ Name = 'cpu_init'; Path = 'src\bx-core\cpu\init.cc'; ExtraIncludes = @('src\bx-core', 'src\bochs\iodev') }
)

$includeRoots = @('src', 'src\bochs', 'src\bochs\instrument\stubs')
function Invoke-MsvcCompile([string]$source, [string]$object, [string[]]$extraIncludes) {
    $includes = @($includeRoots + $extraIncludes | Select-Object -Unique | ForEach-Object {
        '/I "' + (Join-Path $repository $_) + '"'
    }) -join ' '
    $command = 'call "' + $vsDevCmd + '" -arch=x86 -host_arch=x64 >nul && cl.exe /nologo /c /std:c++14 /EHsc /MT /Gy /DWIN32 ' + $includes + ' /FI "' + $config + '" /Fo"' + $object + '" "' + $source + '"'
    & cmd.exe /d /s /c $command
    if ($LASTEXITCODE -ne 0) { throw "MSVC compile failed for ${source}: $LASTEXITCODE" }
}

$objects = @()
foreach ($entry in $sources) {
    $source = Join-Path $repository $entry.Path
    if (-not (Test-Path -LiteralPath $source)) { throw "Declared source missing: $source" }
    $object = Join-Path $build ($entry.Name + '.obj')
    Invoke-MsvcCompile $source $object $entry.ExtraIncludes
    $objects += $object
}

$probe = Join-Path $build 'minimal_machine_link_probe.cc'
@'
#include "bochs.h"
#include "bx-mantle/bx_ntvdm_minimal_machine.h"

int main()
{
  bx_ntvdm_minimal_machine_c machine;
  return (int) machine.initialize(0x100000, 0x100000);
}
'@ | Set-Content -LiteralPath $probe -Encoding ascii
$probeObject = Join-Path $build 'minimal_machine_link_probe.obj'
Invoke-MsvcCompile $probe $probeObject @()

$exe = Join-Path $build 't197-s6-minimal-machine-link-probe.exe'
$map = Join-Path $build 'link.map'
$log = Join-Path $build 'link.log'
$quotedObjects = @($probeObject) + $objects | ForEach-Object { '"' + $_ + '"' }
$link = 'call "' + $vsDevCmd + '" -arch=x86 -host_arch=x64 >nul && link.exe /nologo /OUT:"' + $exe + '" /MAP:"' + $map + '" /OPT:REF ' + ($quotedObjects -join ' ')
& cmd.exe /d /s /c $link 2>&1 | Tee-Object -LiteralPath $log
$linkExit = $LASTEXITCODE

$record = [ordered]@{
    schema = 'ntdos64.t197.s6.minimal-machine-link-probe.v1'
    architecture = 'x86'
    profile = 'CPU5/Pentium-MMX, non-x86-64'
    compiler = 'MSVC cl.exe/link.exe via VsDevCmd'
    configuration = 'tools/t197-s6-cpu5-mantle-config-projection.json'
    sources = @($sources | ForEach-Object { $_.Path })
    generatedProbe = 'minimal_machine_link_probe.cc'
    objects = @($objects | ForEach-Object { Split-Path -Leaf $_ }) + 'minimal_machine_link_probe.obj'
    forbiddenInputs = @('main.cc', 'config.cc', 'gui/siminterface.cc', 'bochs.exe', 'device archives', 'adapter', 'OpenNT')
    linkExitCode = $linkExit
    linkSucceeded = ($linkExit -eq 0)
    linkLog = 'link.log'
    linkMap = 'link.map'
}
$record | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $build 't197-s6-minimal-machine-link-probe.json') -Encoding utf8
if ($linkExit -ne 0) {
    Write-Host "Link did not close; retained exact linker diagnostics: $log"
    exit $linkExit
}
Write-Host "Built minimal CPU5/Pentium-MMX machine link probe: $exe"
