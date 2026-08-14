param(
    [string]$RepositoryRoot = '',
    [string]$BuildRoot = ''
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent $PSScriptRoot }
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
if ([string]::IsNullOrWhiteSpace($BuildRoot)) { $BuildRoot = Join-Path $repository 'artifacts\build\t199-s20-machine-facade-r1' }
$build = [IO.Path]::GetFullPath($BuildRoot)
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build root: $build" }
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (-not (Test-Path -LiteralPath $vs -PathType Leaf)) { throw "Required MSVC environment entry point missing: $vs" }
$relatives = @(
    'src\bx-vdm\bx_ntvdm_machine_bop_facade_v1.c',
    'src\bx-vdm\bx_ntvdm_bop_provider_registry_v1.c',
    'src\bx-vdm\bx_ntvdm_bop_ingress_v1.c',
    'src\bx-vdm\bx_ntvdm_cpu_result_v2.c',
    'src\bx-vdm\bx_ntvdm_cpu_delta_abi.c',
    'src\bx-vdm\bx_ntvdm_cpu_state_abi.c',
    'src\bx-vdm\bx_ntvdm_exception_abi.c',
    'src\bx-vdm\bx_ntvdm_instruction_window_abi.c',
    'tests\bx-vdm\bx_ntvdm_machine_bop_facade_v1_test.c'
)
$sources = @($relatives | ForEach-Object { Join-Path $repository $_ })
foreach ($source in $sources) { if (-not (Test-Path -LiteralPath $source -PathType Leaf)) { throw "Required source missing: $source" } }
New-Item -ItemType Directory -Path $build | Out-Null
$exe = Join-Path $build 't199-s20-machine-facade.exe'
$compile = 'call "' + $vs + '" -arch=x64 -host_arch=x64 >nul && cl.exe /nologo /TC /std:c11 /W4 /WX /MT /DWIN32 /I "' +
    (Join-Path $repository 'src\bx-vdm') + '" /Fe:"' + $exe + '" ' + ($sources -join ' ')
& cmd.exe /d /s /c $compile 2>&1 | Tee-Object -LiteralPath (Join-Path $build 'compile.log')
if ($LASTEXITCODE -ne 0) { throw "MSVC x64 machine facade compilation failed: $LASTEXITCODE" }
& $exe 2>&1 | Tee-Object -LiteralPath (Join-Path $build 'run.log')
$runExit = $LASTEXITCODE
[ordered]@{ schema = 'ntdos64.t199.s20.machine-facade.x64-probe.v1'; architecture = 'x64'; compiler = 'MSVC cl.exe via VsDevCmd'; runtime = '/MT'; sourceClosure = $relatives; runExitCode = $runExit; expectedRunExitCode = 0; passed = ($runExit -eq 0) } |
    ConvertTo-Json -Depth 3 | Set-Content -LiteralPath (Join-Path $build 't199-s20-machine-facade.json') -Encoding utf8
if ($runExit -ne 0) { throw "MSVC x64 machine facade fixture failed: $runExit" }
Write-Host "Built and ran MSVC x64 machine facade fixture: $exe"
