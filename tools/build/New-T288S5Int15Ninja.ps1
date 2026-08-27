[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidateSet('x86', 'x64')]
    [string]$Architecture,
    [string]$RepositoryRoot = ''
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
function NinjaPath([string]$Path) {
    $result = $Path.Replace('\', '/')
    if ($result.Length -ge 2 -and $result[1] -eq ':') { return $result.Substring(0, 1) + '$' + ':' + $result.Substring(2) }
    return $result
}
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot) }
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = Join-Path $root ("build/M0-T288/S5/{0}" -f $Architecture)
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (!(Test-Path -LiteralPath $vs -PathType Leaf)) { throw "Missing MSVC: $vs" }
if (!(Get-Command ninja -ErrorAction SilentlyContinue)) { throw 'Ninja is required.' }
$sources = @('src/adapter-mvdm-host-out/softpc/mvdm_sas.c', 'src/adapter-mvdm-host-out/softpc/mvdm_int15.c', 'tests/adapter-mvdm-host-out/softpc/int15_vector_fixture.c')
foreach ($path in $sources) {
    if (!(Test-Path -LiteralPath (Join-Path $root $path) -PathType Leaf) -or $path -match '(^|/)src\.old(/|$)') { throw "Invalid S5 input: $path" }
}
New-Item -ItemType Directory -Force $build, (Join-Path $build 'obj'), (Join-Path $build 'bin') | Out-Null
$environment = Join-Path $build 'msvc-mt.cmd'
@('@echo off', 'set "MVDM_T288_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready',
    ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'), 'if errorlevel 1 exit /b %errorlevel%', ':ready', 'cd /d "%MVDM_T288_CALLER_CWD%"', '%*') |
    Set-Content -LiteralPath $environment -Encoding ascii
$manifest = [ordered]@{ schema = 'm0.t288.s5.int15.v1'; architecture = $Architecture; runtimeLibrary = '/MT'; sources = $sources;
    forbiddenInputs = @('src.old', 'softpc.new/base/keymouse/keybd_io.c', 'prebuilt product archive') }
$manifest | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $build 'source-manifest.json') -Encoding utf8
$includes = @('src', 'src/adapter-mvdm-host-out/softpc/include') | ForEach-Object { '/I "' + (NinjaPath (Join-Path $root $_)) + '"' }
$flags = '/nologo /TC /c /std:c11 /MT /W4 /showIncludes ' + ($includes -join ' ')
$graph = [Collections.Generic.List[string]]::new()
$graph.Add('ninja_required_version = 1.10')
$graph.Add('cflags = ' + $flags)
$graph.Add('')
$graph.Add('rule cc')
$graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' cl.exe $cflags /Fo$out $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file:')
$graph.Add('rule link')
$graph.Add('  command = cmd.exe /d /s /c cd /d ' + (NinjaPath $build) + ' && call ' + (NinjaPath $environment) + ' link.exe /nologo /OUT:$out $in')
$objects = @()
foreach ($source in $sources) {
    $object = 'obj/' + ([IO.Path]::GetFileNameWithoutExtension($source)) + '.obj'
    $objects += $object
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $root $source)))
}
$graph.Add('build bin/int15-vector-fixture.exe: link ' + ($objects -join ' '))
$graph.Add('default bin/int15-vector-fixture.exe')
$graph | Set-Content -LiteralPath (Join-Path $build 'build.ninja') -Encoding ascii
Write-Output ("Generated T288 S5 INT15 graph: {0}" -f $build)
