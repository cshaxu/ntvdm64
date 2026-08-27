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
function Sha256([string]$Path) { return (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant() }
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot) }
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = Join-Path $root ("build/M0-T288/S2/{0}" -f $Architecture)
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (!(Test-Path -LiteralPath $vs -PathType Leaf)) { throw "Missing MSVC: $vs" }
if (!(Get-Command ninja -ErrorAction SilentlyContinue)) { throw 'Ninja is required.' }
$fixture = 'tests/adapter-mvdm-host-out/monitor/t288_s2_sim32_declaration_fixture.c'
$source = 'src/mvdm-host/sim32/sim32.h'
foreach ($path in @($fixture, $source)) {
    if (!(Test-Path -LiteralPath (Join-Path $root $path) -PathType Leaf) -or $path -match '(^|/)src\.old(/|$)') { throw "Invalid S2 input: $path" }
}
New-Item -ItemType Directory -Force $build, (Join-Path $build 'obj'), (Join-Path $build 'lib') | Out-Null
$environment = Join-Path $build 'msvc-mt.cmd'
@('@echo off', 'set "MVDM_T288_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready',
    ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'), 'if errorlevel 1 exit /b %errorlevel%', ':ready', 'cd /d "%MVDM_T288_CALLER_CWD%"', '%*') |
    Set-Content -LiteralPath $environment -Encoding ascii
$manifest = [ordered]@{ schema = 'm0.t288.s2.monitor-declaration.v1'; architecture = $Architecture; runtimeLibrary = '/MT';
    sources = @($fixture, $source | ForEach-Object { [ordered]@{ path = $_; sha256 = Sha256 (Join-Path $root $_) } });
    forbiddenInputs = @('src.old', 'sim32.c', 'v86 monitor body', 'prebuilt product archive') }
$manifest | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $build 'source-manifest.json') -Encoding utf8
$includes = @('src', 'src/mvdm-host/sim32', 'src/adapter-mvdm-host-out/win32/include',
    'src/mvdm-platform-abi/source/public/sdk/inc', 'src/mvdm-platform-abi/source/public/internal/base/inc',
    'src/mvdm-platform-abi/source/public/ddk/inc') | ForEach-Object { '/I "' + (NinjaPath (Join-Path $root $_)) + '"' }
$flags = '/nologo /TC /c /std:c11 /MT /W4 /showIncludes /DWIN_32 /Di386 ' + ($includes -join ' ')
$graph = [Collections.Generic.List[string]]::new()
$graph.Add('ninja_required_version = 1.10')
$graph.Add('cflags = ' + $flags)
$graph.Add('')
$graph.Add('rule cc')
$graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' cl.exe $cflags /Fo$out $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file:')
$graph.Add('rule lib')
$graph.Add('  command = cmd.exe /d /s /c cd /d ' + (NinjaPath $build) + ' && call ' + (NinjaPath $environment) + ' lib.exe /nologo /OUT:$out $in')
$fixtureInput = NinjaPath (Join-Path $root $fixture)
$graph.Add('build obj/monitor_declaration_fixture.obj: cc ' + $fixtureInput)
$graph.Add('build lib/original-monitor-declarations.lib: lib obj/monitor_declaration_fixture.obj')
$graph.Add('default lib/original-monitor-declarations.lib')
$graph | Set-Content -LiteralPath (Join-Path $build 'build.ninja') -Encoding ascii
Write-Output ("Generated T288 S2 {0} declaration graph: {1}" -f $Architecture, $build)
