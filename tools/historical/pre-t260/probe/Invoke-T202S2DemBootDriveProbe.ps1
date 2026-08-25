param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$BuildRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = [IO.Path]::GetFullPath($BuildRoot)
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build directory: $build" }
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (-not (Test-Path -LiteralPath $vs -PathType Leaf)) { throw "Required MSVC environment entry point missing: $vs" }
New-Item -ItemType Directory -Path $build | Out-Null
$exe = Join-Path $build 't202-s2-dem-boot-drive.exe'
$sources = @('bx_ntvdm_cpu_delta_abi.c','bx_ntvdm_cpu_state_abi.c',
    'bx_ntvdm_exception_abi.c','bx_ntvdm_instruction_window_abi.c',
    'bx_ntvdm_host_drive_policy.c','bx_ntvdm_dem_boot_drive_service.c') |
    ForEach-Object { Join-Path $root ('src\bx-vdm\' + $_) }
$sources += Join-Path $root 'tests\bx-vdm\bx_ntvdm_dem_boot_drive_service_test.c'
$compile = 'call "' + $vs + '" -arch=x64 -host_arch=x64 >nul && cl.exe /nologo /TC /std:c11 /W4 /WX /MT /I "' +
    (Join-Path $root 'src\bx-vdm') + '" /Fe:"' + $exe + '" ' + ($sources -join ' ')
& cmd.exe /d /s /c $compile 2>&1 | Tee-Object -FilePath (Join-Path $build 'compile.log')
if ($LASTEXITCODE -ne 0) { throw "T202 S2 DEM boot-drive compilation failed: $LASTEXITCODE" }
& $exe 2>&1 | Tee-Object -FilePath (Join-Path $build 'run.log')
$runExit = $LASTEXITCODE
[ordered]@{ schema='runner.t202.s2.dem-boot-drive.v2'; architecture='x64'; runtimeLibrary='/MT'; hostIo=$false; guestExecution=$false; runExitCode=$runExit; passed=($runExit -eq 0) } | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $build 't202-s2-dem-boot-drive.json') -Encoding utf8
if ($runExit -ne 0) { throw "T202 S2 DEM boot-drive fixture failed: $runExit" }
Write-Host "T202 S2 DEM boot-drive probe passed: $build"
