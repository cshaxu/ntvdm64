[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [ValidateSet('x86', 'x64')] [string]$Architecture,
    [string]$RepositoryRoot = ''
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function NinjaPath([string]$Path) {
    $value = $Path.Replace('\', '/')
    if ($value.Length -ge 2 -and $value[1] -eq ':') {
        return $value.Substring(0, 1) + '$' + ':' + $value.Substring(2)
    }
    return $value
}

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = Join-Path $root ("build/M0-T310/S8/p2-heartbeat/{0}" -f $Architecture)
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (!(Test-Path -LiteralPath $vs -PathType Leaf)) { throw 'MSVC Build Tools are required.' }
New-Item -ItemType Directory -Force $build, (Join-Path $build 'obj') | Out-Null

$environment = Join-Path $build 'msvc-mt.cmd'
@('@echo off', 'set "MVDM_T310_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready',
  ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'),
  'if errorlevel 1 exit /b %errorlevel%', ':ready', 'cd /d "%MVDM_T310_CALLER_CWD%"', '%*') |
    Set-Content -LiteralPath $environment -Encoding ascii

$sources = @(
    'src/session/mapping_manager.c',
    'src/session/guest_memory_lease.c',
    'src/session/session.c',
    'src/adapter-mvdm-host-out/win32/source/thread_start_compat.c',
    'src/adapter-mvdm-host-out/win32/source/nt_thread_alert_compat.c',
    'src/adapter-mvdm-host-out/softpc/mvdm_softpc_event_thread.c',
    'tests/adapter-mvdm-host-out/nt_thread_alert_compat_fixture.c',
    'tests/adapter-mvdm-host-out/thread_start_session_fixture.c',
    'tests/adapter-mvdm-host-out/softpc/softpc_event_thread_shutdown_fixture.c'
)
$graph = [Collections.Generic.List[string]]::new()
$graph.Add('ninja_required_version = 1.10')
$graph.Add('cflags = /nologo /TC /c /MT /W4 /showIncludes /I "' +
    (NinjaPath (Join-Path $root 'src')) + '" /I "' +
    (NinjaPath (Join-Path $root 'src/adapter-mvdm-host-out/win32/include')) + '" /I "' +
    (NinjaPath (Join-Path $root 'src/adapter-mvdm-host-out/softpc/include')) + '"')
$graph.Add('rule cc')
$graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' cl.exe $cflags /Fo$out $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file:')
$graph.Add('rule link')
$graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' link.exe /nologo /out:$out $in kernel32.lib')
$graph.Add('rule run')
$graph.Add('  command = $in')
for ($index = 0; $index -lt $sources.Count; ++$index) {
    $object = 'obj/' + $index + '-' + ([IO.Path]::GetFileNameWithoutExtension($sources[$index])) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $root $sources[$index])))
}
$graph.Add('build nt-thread-alert-fixture.exe: link obj/4-nt_thread_alert_compat.obj obj/6-nt_thread_alert_compat_fixture.obj')
$graph.Add('build thread-start-session-fixture.exe: link obj/0-mapping_manager.obj obj/1-guest_memory_lease.obj obj/2-session.obj obj/3-thread_start_compat.obj obj/7-thread_start_session_fixture.obj')
$graph.Add('build softpc-event-thread-shutdown-fixture.exe: link obj/4-nt_thread_alert_compat.obj obj/5-mvdm_softpc_event_thread.obj obj/8-softpc_event_thread_shutdown_fixture.obj')
$graph.Add('build verify: run nt-thread-alert-fixture.exe')
$graph.Add('build verify-thread-session: run thread-start-session-fixture.exe')
$graph.Add('build verify-event-thread-shutdown: run softpc-event-thread-shutdown-fixture.exe')
$graph.Add('build all-verify: phony verify verify-thread-session verify-event-thread-shutdown')
$graph.Add('default all-verify')
[IO.File]::WriteAllText((Join-Path $build 'build.ninja'), (($graph -join [Environment]::NewLine) + [Environment]::NewLine), [Text.UTF8Encoding]::new($false))
Write-Host "Generated T310 S8 P2 heartbeat graph: $build"
