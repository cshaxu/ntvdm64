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
$exe = Join-Path $build 't221-s3-dem-virtual-namespace.exe'
$sources = @(
    'src\bx-vdm\bx_ntvdm_dem_overlay_store_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_virtual_namespace_view_v1.c',
    'tests\bx-vdm\bx_ntvdm_dem_virtual_namespace_view_v1_test.c'
) | ForEach-Object { Join-Path $root $_ }
$compile = 'call "' + $vs + '" -arch=x64 -host_arch=x64 >nul && cl.exe /nologo /TC /std:c11 /W4 /WX /MT /I "' +
    (Join-Path $root 'src\bx-vdm') + '" /Fe:"' + $exe + '" ' + ($sources -join ' ')
& cmd.exe /d /s /c $compile 2>&1 | Tee-Object -FilePath (Join-Path $build 'compile.log')
if ($LASTEXITCODE -ne 0) { throw "T221 S3 DEM Virtual namespace compilation failed: $LASTEXITCODE" }
& $exe 2>&1 | Tee-Object -FilePath (Join-Path $build 'run.log')
$runExit = $LASTEXITCODE
[ordered]@{ schema='ntdos64.t221.s3.dem-virtual-namespace.v1'; architecture='x64'; runtimeLibrary='/MT'; hostIo=$false; guestExecution=$false; runExitCode=$runExit; passed=($runExit -eq 0) } | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $build 't221-s3-dem-virtual-namespace.json') -Encoding utf8
if ($runExit -ne 0) { throw "T221 S3 DEM Virtual namespace probe failed: $runExit" }
Write-Host "T221 S3 DEM Virtual namespace probe passed: $build"
