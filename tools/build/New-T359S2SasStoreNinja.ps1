[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidateSet('x86')]
    [string]$Architecture,
    [string]$RepositoryRoot = '',
    [string]$BuildRoot = ''
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function NinjaPath([string]$Path) {
    $result = $Path.Replace('\', '/')
    if ($result.Length -ge 2 -and $result[1] -eq ':') {
        return $result.Substring(0, 1) + '$' + ':' + $result.Substring(2)
    }
    return $result
}

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
if ([string]::IsNullOrWhiteSpace($BuildRoot)) {
    $BuildRoot = Join-Path $root 'build/M0-T359/S2/sas-store-observer-x86'
}
$build = [IO.Path]::GetFullPath($BuildRoot)
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (!(Test-Path -LiteralPath $vs -PathType Leaf)) { throw "Missing MSVC: $vs" }
if (!(Get-Command ninja -ErrorAction SilentlyContinue)) { throw 'Ninja is required.' }

$sources = @(
    'src/adapter-mvdm-host-out/softpc/mvdm_softpc_termination.c',
    'tests/adapter-mvdm-host-out/softpc/t355_s6_config_command_store_observation_fixture.c',
    'tests/adapter-mvdm-host-out/softpc/t359_s2_sas_store_fixture_stubs.c'
)
foreach ($path in $sources) {
    $fullPath = Join-Path $root $path
    if (!(Test-Path -LiteralPath $fullPath -PathType Leaf) -or
        $path -match '(^|/)src\.old(/|$)') { throw "Invalid T359 S2 input: $path" }
}

New-Item -ItemType Directory -Force $build, (Join-Path $build 'obj'),
    (Join-Path $build 'bin') | Out-Null
$environment = Join-Path $build 'msvc-mt.cmd'
@('@echo off', 'set "MVDM_T359_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready',
    ('call "' + $vs + '" -arch=x86 -host_arch=x86 >nul'),
    'if errorlevel 1 exit /b %errorlevel%', ':ready',
    'cd /d "%MVDM_T359_CALLER_CWD%"', '%*') |
    Set-Content -LiteralPath $environment -Encoding ascii

$manifest = [ordered]@{
    schema = 'm0.t359.s2.sas-store-observer.v1'
    architecture = $Architecture
    runtimeLibrary = '/MT'
    sources = $sources
    purpose = 'default-off SAS byte/word/dword store observation contract'
    forbiddenInputs = @('src.old', 'guest media', 'BOP routing', 'CPU semantic replacement')
}
$manifest | ConvertTo-Json -Depth 4 |
    Set-Content -LiteralPath (Join-Path $build 'source-manifest.json') -Encoding utf8

$includes = @('src', 'src/adapter-mvdm-host-out/softpc/include') |
    ForEach-Object { '/I "' + (NinjaPath (Join-Path $root $_)) + '"' }
$flags = '/nologo /TC /c /std:c11 /MT /W4 /Gy /showIncludes ' +
    ($includes -join ' ')
$graph = [Collections.Generic.List[string]]::new()
$graph.Add('ninja_required_version = 1.10')
$graph.Add('cflags = ' + $flags)
$graph.Add('')
$graph.Add('rule cc')
$graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) +
    ' cl.exe $cflags /Fo$out $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file:')
$graph.Add('rule link')
$graph.Add('  command = cmd.exe /d /s /c cd /d ' + (NinjaPath $build) +
    ' && call ' + (NinjaPath $environment) +
    ' link.exe /nologo /opt:ref /OUT:$out $in kernel32.lib')
$graph.Add('rule run')
$graph.Add('  command = $in')
$objects = @()
foreach ($source in $sources) {
    $object = 'obj/' + ([IO.Path]::GetFileNameWithoutExtension($source)) + '.obj'
    $objects += $object
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $root $source)))
}
$graph.Add('build bin/sas-store-observer-fixture.exe: link ' + ($objects -join ' '))
$graph.Add('build test: run bin/sas-store-observer-fixture.exe')
$graph.Add('default test')
$graph | Set-Content -LiteralPath (Join-Path $build 'build.ninja') -Encoding ascii
Write-Output "Generated T359 S2 SAS-store observer graph: $build"
