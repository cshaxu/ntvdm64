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
$exe = Join-Path $build 't200-s18-initial-state-abi.exe'
$command = 'call "' + $vs + '" -arch=x64 -host_arch=x64 >nul && cl.exe /nologo /TC /std:c11 /W4 /WX /MT /I "' +
    (Join-Path $root 'src\bx-vdm') + '" /Fe:"' + $exe + '" "' +
    (Join-Path $root 'src\bx-vdm\bx_ntvdm_guest_write_abi.c') + '" "' +
    (Join-Path $root 'src\bx-vdm\bx_ntvdm_guest_range.c') + '" "' +
    (Join-Path $root 'src\bx-vdm\bx_ntvdm_initial_state_abi.c') + '" "' +
    (Join-Path $root 'tests\bx-vdm\bx_ntvdm_initial_state_abi_test.c') + '"'
& cmd.exe /d /s /c $command 2>&1 | Tee-Object -FilePath (Join-Path $build 'compile.log')
if ($LASTEXITCODE -ne 0) { throw "T200 S18 compile failed: $LASTEXITCODE" }
& $exe 2>&1 | Tee-Object -FilePath (Join-Path $build 'run.log')
$runExit = $LASTEXITCODE
([ordered]@{ schema='ntdos64.t200.s18.initial-state-abi.v1'; architecture='x64'; runtimeLibrary='/MT'; machineInitialized=$false; guestExecution=$false; runExitCode=$runExit; passed=($runExit -eq 0) }) | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $build 't200-s18-initial-state-abi.json') -Encoding utf8
if ($runExit -ne 0) { throw "T200 S18 fixture failed: $runExit" }
Write-Host "T200 S18 initial-state ABI probe passed: $build"
