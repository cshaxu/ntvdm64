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
function ObjectName([string]$Path) { return (($Path -replace '[^A-Za-z0-9_]', '_') + '.obj') }
function Sha256([string]$Path) { return (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant() }
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot) }
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = Join-Path $root ("build/M0-T289/S5/audit-{0}" -f $Architecture)
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (!(Test-Path -LiteralPath $vs -PathType Leaf)) { throw "Missing MSVC: $vs" }
if (!(Get-Command ninja -ErrorAction SilentlyContinue)) { throw 'Ninja is required.' }
$sources = @('src/mvdm-host/dpmi32/data.c', 'src/mvdm-host/dpmi32/register.c',
    'src/mvdm-host/dpmi32/stack.c', 'src/mvdm-host/dpmi32/dpmiint.c',
    'src/mvdm-host/dpmi32/dpmimscr.c', 'src/mvdm-host/dpmi32/modesw.c',
    'src/mvdm-host/dpmi32/savestat.c',
    'src/mvdm-host-overlay/dpmi32/dpmi_interrupt_registration.c')
foreach ($path in $sources) {
    if (!(Test-Path -LiteralPath (Join-Path $root $path) -PathType Leaf) -or $path -match '(^|/)src\.old(/|$)') { throw "Invalid S5 source: $path" }
}
New-Item -ItemType Directory -Force $build, (Join-Path $build 'obj'), (Join-Path $build 'lib') | Out-Null
$environment = Join-Path $build 'msvc-mt.cmd'
@('@echo off', 'set "MVDM_T289_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready',
    ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'), 'if errorlevel 1 exit /b %errorlevel%', ':ready', 'cd /d "%MVDM_T289_CALLER_CWD%"', '%*') |
    Set-Content -LiteralPath $environment -Encoding ascii
$manifest = [ordered]@{ schema = 'm0.t289.s5.dpmi-interrupt-audit.v1'; architecture = $Architecture; runtimeLibrary = '/MT';
    sources = @($sources | ForEach-Object { [ordered]@{ path = $_; sha256 = Sha256 (Join-Path $root $_) } });
    forbiddenInputs = @('src.old', 'DOSX host translation unit', 'host LDT implementation', 'BOP ingress', 'prebuilt product archive') }
$manifest | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $build 'source-manifest.json') -Encoding utf8
$includes = @('src', 'src/adapter-mvdm-host-out/softpc/include', 'src/adapter-mvdm-host-out/win32/include',
    'src/adapter-mvdm-host-out/monitor/include', 'src/mvdm-host/dpmi32', 'src/mvdm-host-overlay/dpmi32',
    'src/mvdm-support/inc', 'src/mvdm-platform-abi/source/public/sdk/inc',
    'src/mvdm-platform-abi/source/public/internal/base/inc', 'src/mvdm-platform-abi/source/public/ddk/inc',
    'src/mvdm-host/softpc.new/host/inc', 'src/mvdm-host/softpc.new/base/inc') |
    ForEach-Object { '/I "' + (NinjaPath (Join-Path $root $_)) + '"' }
$flags = '/nologo /TC /c /std:c11 /MT /W4 /showIncludes /DWIN_32 /Di386 /DDEVL ' +
    '/FI "' + (NinjaPath (Join-Path $root 'src/adapter-mvdm-host-out/win32/include/nt.h')) + '" ' +
    '/FI "' + (NinjaPath (Join-Path $root 'src/adapter-mvdm-host-out/softpc/include/error_abi.h')) + '" ' +
    '/FI "' + (NinjaPath (Join-Path $root 'src/adapter-mvdm-host-out/monitor/include/monitor_context.h')) + '" ' + ($includes -join ' ')
$graph = [Collections.Generic.List[string]]::new()
$graph.Add('ninja_required_version = 1.10'); $graph.Add('build_root = ' + (NinjaPath $build)); $graph.Add('cflags = ' + $flags); $graph.Add('')
$graph.Add('rule cc'); $graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' cl.exe $cflags /Fo$out $in'); $graph.Add('  deps = msvc'); $graph.Add('  msvc_deps_prefix = Note: including file:')
$graph.Add('rule lib'); $graph.Add('  command = cmd.exe /d /s /c cd /d $build_root && call ' + (NinjaPath $environment) + ' lib.exe /nologo /OUT:$out @$out.rsp')
$objects = [Collections.Generic.List[string]]::new()
foreach ($source in $sources) { $object = 'obj/' + (ObjectName $source); $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $root $source))); $objects.Add($object) }
$library = 'lib/original-dpmi-s5-source-audit.lib'; [IO.File]::WriteAllLines((Join-Path $build ($library + '.rsp')), $objects, [Text.UTF8Encoding]::new($false)); $graph.Add('build ' + $library + ': lib ' + ($objects -join ' ')); $graph.Add('default ' + $library)
[IO.File]::WriteAllLines((Join-Path $build 'build.ninja'), $graph, [Text.UTF8Encoding]::new($false))
Write-Host "Generated T289 S5 original DPMI interrupt audit graph: $build"
