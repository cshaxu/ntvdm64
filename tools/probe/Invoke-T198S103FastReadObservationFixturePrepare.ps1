param(
    [string]$RepositoryRoot = '',
    [Parameter(Mandatory = $true)][string]$BuildRoot
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot) }
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
$build = [IO.Path]::GetFullPath($BuildRoot)
$vsDevCmd = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$config = Join-Path $repository 'artifacts\build\t198-s74-dem-pdb-termination-r1\native-core\config.h'
$bridge = Join-Path $repository 'tests\bx-vdm\t198_s23_native_ntio_boundary_bridge.c'
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build root: $build" }
foreach ($path in @($vsDevCmd, $config, $bridge,
    (Join-Path $repository 'tools\probe\Invoke-T198S93SourceBuiltNormalReturnFixturePrepare.ps1'))) {
    if (-not (Test-Path -LiteralPath $path)) { throw "Required S103 input missing: $path" }
}

New-Item -ItemType Directory -Path $build | Out-Null
$prepared = Join-Path $build 'prepared'
& (Join-Path $repository 'tools\probe\Invoke-T198S93SourceBuiltNormalReturnFixturePrepare.ps1') `
    -RepositoryRoot $repository -BuildRoot $prepared
if ($LASTEXITCODE -ne 0) { throw 'S103 fixture preparation failed.' }

$includes = @('src', 'refs\bochs', 'refs\bochs\instrument\stubs', 'src\bx-core', 'src\bx-core\cpu', 'src\bx-mantle', 'src\bx-vdm', 'src\cli') |
    ForEach-Object { '/I "' + (Join-Path $repository $_) + '"' }
$object = Join-Path $build 'native-ntio-boundary-bridge.obj'
$log = Join-Path $build 'bridge-compile.log'
$args = '/TC /c /std:c11 /W4 /WX /DWIN32 /MT ' + ($includes -join ' ') +
    ' /FI "' + $config + '" /Fo"' + $object + '" "' + $bridge + '"'
& cmd.exe /d /s /c ('call "' + $vsDevCmd + '" -arch=x64 -host_arch=x64 >nul && cl.exe /nologo ' + $args) 2>&1 |
    Tee-Object -FilePath $log
if ($LASTEXITCODE -ne 0) { throw "S103 bridge compile failed: $args" }

@{
    schema = 'ntdos64.t198.s103.fast-read-observation-fixture-prepare.v1'
    guestExecution = $false
    linked = $false
    compiled = @(
        'prepared/source-built-normal-return-fixture.obj',
        'prepared/command_bytes.obj',
        'prepared/share_bytes.obj',
        'native-ntio-boundary-bridge.obj'
    )
    excluded = @('guest execution', 'native fixture link', 'provider/routing/ABI change', 'Bochs core/mantle edit')
} | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $build 'outcome.json') -Encoding utf8
Write-Host "S103 fixture and bridge source compilation completed without link or guest execution: $build"
