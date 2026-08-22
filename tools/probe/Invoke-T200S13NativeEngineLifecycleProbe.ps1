param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$BuildRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = [IO.Path]::GetFullPath($BuildRoot)
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build directory: $build" }
New-Item -ItemType Directory -Path $build | Out-Null
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (-not (Test-Path -LiteralPath $vs)) { throw "Missing MSVC environment: $vs" }
$contract = Join-Path $root 'src\bx-vdm\bx_ntvdm_engine_contract_v1.c'
$entry = Join-Path $root 'src\bx-vdm\bx_ntvdm_engine_run_v1.c'
$test = Join-Path $root 'tests\bx-mantle\bx_ntvdm_engine_run_v1_test.c'
$exe = Join-Path $build 't200-s13-native-engine-lifecycle.exe'
$command = 'call "' + $vs + '" -arch=x64 -host_arch=x64 >nul && cl.exe /nologo /TC /std:c11 /W4 /WX /MT /I "' +
    (Join-Path $root 'src\bx-mantle') + '" /I "' + (Join-Path $root 'src\bx-vdm') +
    '" /Fe:"' + $exe + '" "' + $contract + '" "' + $entry + '" "' + $test + '"'
& cmd.exe /d /s /c $command 2>&1 | Tee-Object -LiteralPath (Join-Path $build 'compile.log')
if ($LASTEXITCODE -ne 0) { throw "T200 S13 compile failed: $LASTEXITCODE" }
& $exe 2>&1 | Tee-Object -LiteralPath (Join-Path $build 'run.log')
$runExit = $LASTEXITCODE
$record = [ordered]@{
    schema = 'ntdos64.t200.s13.native-engine-lifecycle.v1'
    architecture = 'x64'
    runtimeLibrary = '/MT'
    sourceClosure = @('src/bx-mantle/bx_ntvdm_engine_contract_v1.c', 'src/bx-mantle/bx_ntvdm_engine_run_v1.c', 'tests/bx-mantle/bx_ntvdm_engine_run_v1_test.c')
    seam = 'bx_ntvdm_composition_runtime_v1_install_from_engine_request/reset (test substitute)'
    forbiddenDependencies = @('bochs.h', 'src/bx-core', 'guest execution')
    runExitCode = $runExit
    expectedRunExitCode = 0
    passed = ($runExit -eq 0)
}
$record | ConvertTo-Json -Depth 3 | Set-Content -LiteralPath (Join-Path $build 't200-s13-native-engine-lifecycle.json') -Encoding utf8
if ($runExit -ne 0) { throw "T200 S13 fixture failed: $runExit" }
Write-Host "T200 S13 native-engine lifecycle probe passed: $build"
