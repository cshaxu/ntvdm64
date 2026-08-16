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
$sources = @(
    'src\bx-vdm\bx_ntvdm_dem_overlay_store_v1.c',
    'src\bx-vdm\bx_ntvdm_mutation_profile_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_profile_consumer_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_file_session_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_virtual_namespace_view_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_overlay_file_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_virtual_namespace_backend_v1.c'
    ,'src\bx-vdm\bx_ntvdm_dem_virtual_mutation_backend_v1.c'
) | ForEach-Object { Join-Path $root $_ }
$tests = @('bx_ntvdm_dem_virtual_namespace_view_v1_test.c',
    'bx_ntvdm_dem_virtual_namespace_backend_v1_test.c',
    'bx_ntvdm_dem_virtual_mutation_backend_v1_test.c')
foreach ($test in $tests) {
    $stem = [IO.Path]::GetFileNameWithoutExtension($test)
    $exe = Join-Path $build ($stem + '.exe')
    $compile = 'call "' + $vs + '" -arch=x64 -host_arch=x64 >nul && cl.exe /nologo /TC /std:c11 /W4 /WX /MT /I "' +
        (Join-Path $root 'src\bx-vdm') + '" /Fe:"' + $exe + '" ' + ($sources -join ' ') + ' "' +
        (Join-Path $root ('tests\bx-vdm\' + $test)) + '"'
    & cmd.exe /d /s /c $compile 2>&1 | Tee-Object -FilePath (Join-Path $build ($stem + '.compile.log'))
    if ($LASTEXITCODE -ne 0) { throw "T221 S3 DEM Virtual namespace compilation failed: $LASTEXITCODE" }
    & $exe 2>&1 | Tee-Object -FilePath (Join-Path $build ($stem + '.run.log'))
    if ($LASTEXITCODE -ne 0) { throw "T221 S3 DEM Virtual namespace probe failed: $LASTEXITCODE" }
}
[ordered]@{ schema='ntdos64.t221.s3.dem-virtual-namespace.v2'; architecture='x64'; runtimeLibrary='/MT'; hostIo=$false; guestExecution=$false; passed=$true } | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $build 't221-s3-dem-virtual-namespace.json') -Encoding utf8
Write-Host "T221 S3 DEM Virtual namespace probe passed: $build"
