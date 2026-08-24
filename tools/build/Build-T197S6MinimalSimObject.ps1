[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$BuildRoot = ''
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot) }
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
if ([string]::IsNullOrWhiteSpace($BuildRoot)) { $BuildRoot = Join-Path $repository 'artifacts\build\t197-s6-minimal-sim-object' }
$build = [IO.Path]::GetFullPath($BuildRoot)
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build root: $build" }
$projectionManifest = Join-Path $repository 'tools\build\t197-s6-cpu5-mantle-config-projection.json'
$projectionTool = Join-Path $repository 'tools\build\Project-BochsConfig.ps1'
$devCmd = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (-not (Test-Path -LiteralPath $devCmd)) { throw "Missing Visual Studio developer command: $devCmd" }
& powershell -ExecutionPolicy Bypass -File $projectionTool -ManifestPath $projectionManifest -AnalysisRoot $build
if ($LASTEXITCODE -ne 0) { throw "CPU5 configuration projection failed: $LASTEXITCODE" }
$config = Join-Path $build 'config.h'
$object = Join-Path $build 'bx_ntvdm_minimal_sim.obj'
$source = Join-Path $repository 'src\adapter-bochs\bx_ntvdm_minimal_sim.cc'
$command = 'call "' + $devCmd + '" -arch=x86 -host_arch=x64 >nul && cl.exe /nologo /c /std:c++14 /EHsc /MT /DWIN32 /I "' + $repository + '\src" /I "' + $repository + '\refs\bochs" /I "' + $repository + '\refs\bochs\instrument\stubs" /FI "' + $config + '" /Fo"' + $object + '" "' + $source + '"'
& cmd.exe /d /s /c $command
if ($LASTEXITCODE -ne 0) { throw "CPU5 minimal-SIM object compilation failed: $LASTEXITCODE" }
if (-not (Test-Path -LiteralPath $object -PathType Leaf)) { throw "Missing object after successful compiler run: $object" }
$record = [ordered]@{ schema = 'ntdos64.t197.s6.minimal-sim-object.v1'; source = 'src/adapter-bochs/bx_ntvdm_minimal_sim.cc'; configuration = 'tools/build/t197-s6-cpu5-mantle-config-projection.json'; architecture = 'x86'; compiler = 'MSVC cl.exe via VsDevCmd'; defines = @('WIN32'); includeRoots = @('src', 'refs/bochs', 'refs/bochs/instrument/stubs'); forbiddenInputs = @('main.cc', 'config.cc', 'gui/siminterface.cc', 'bochs.exe', 'device archives'); object = 'bx_ntvdm_minimal_sim.obj' }
$record | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $build 't197-s6-minimal-sim-object.json') -Encoding utf8
Write-Host "Built CPU5/Pentium-MMX minimal SIM object: $object"
