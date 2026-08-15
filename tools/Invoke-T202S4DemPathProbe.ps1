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
$exe = Join-Path $build 't202-s4-dem-path.exe'
$sources = @(
    'bx_ntvdm_mutation_profile_v1.c', 'bx_ntvdm_mutation_overlay_v1.c',
    'bx_ntvdm_dem_profile_consumer_v1.c', 'bx_ntvdm_host_drive_policy.c',
    'bx_ntvdm_host_namespace.c', 'bx_ntvdm_dem_cwd_context_v1.c',
    'bx_ntvdm_dem_path_v1.c' | ForEach-Object { Join-Path $root ('src\bx-vdm\' + $_) }
)
$sources += Join-Path $root 'tests\bx-vdm\bx_ntvdm_dem_path_v1_test.c'
$compile = 'call "' + $vs + '" -arch=x64 -host_arch=x64 >nul && cl.exe /nologo /TC /std:c11 /W4 /WX /MT /I "' +
    (Join-Path $root 'src\bx-vdm') + '" /Fe:"' + $exe + '" ' + ($sources -join ' ') + ' /link ntdll.lib'
& cmd.exe /d /s /c $compile 2>&1 | Tee-Object -FilePath (Join-Path $build 'compile.log')
if ($LASTEXITCODE -ne 0) { throw "T202 S4 DEM path compilation failed: $LASTEXITCODE" }
& $exe 2>&1 | Tee-Object -FilePath (Join-Path $build 'run.log')
$runExit = $LASTEXITCODE
[ordered]@{ schema='ntdos64.t202.s4.dem-path.v1'; architecture='x64'; runtimeLibrary='/MT'; hostIo=$false; guestExecution=$false; runExitCode=$runExit; passed=($runExit -eq 0) } | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $build 't202-s4-dem-path.json') -Encoding utf8
if ($runExit -ne 0) { throw "T202 S4 DEM path probe failed: $runExit" }
Write-Host "T202 S4 DEM path probe passed: $build"
