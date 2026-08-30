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
    $RepositoryRoot = Split-Path -Parent (Split-Path $PSScriptRoot)
}
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = Join-Path $root ("build/M0-T310/S8/p2-heartbeat-source/{0}" -f $Architecture)
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (!(Test-Path -LiteralPath $vs -PathType Leaf)) { throw 'MSVC Build Tools are required.' }
New-Item -ItemType Directory -Force $build, (Join-Path $build 'obj') | Out-Null

$environment = Join-Path $build 'msvc-mt.cmd'
@('@echo off', 'set "MVDM_T310_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready',
  ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'),
  'if errorlevel 1 exit /b %errorlevel%', ':ready', 'cd /d "%MVDM_T310_CALLER_CWD%"', '%*') |
    Set-Content -LiteralPath $environment -Encoding ascii

$includes = @(
    'src',
    'src/adapter-mvdm-host-out/win32/include',
    'src/opennt-host/public/sdk/inc',
    'src/opennt-abi/source/public/sdk/inc',
    'src/opennt-abi/source/public/internal/base/inc',
    'src/opennt-abi/source/public/internal/windows/inc',
    'src/opennt-abi/source/private/windows/inc',
    'src/opennt-abi/source/public/ddk/inc',
    'src/mvdm-support/inc',
    'src/mvdm-softpc-patch/x86/prod',
    'src/mvdm-host/softpc.new/base/ccpu386',
    'src/mvdm-host/softpc.new/host/inc',
    'src/mvdm-host/softpc.new/base/cvidc',
    'src/mvdm-host/softpc.new/base/inc',
    'src/adapter-mvdm-host-out/softpc/include',
    'src/adapter-mvdm-host-out/monitor/include'
) | ForEach-Object { '/I "' + (NinjaPath (Join-Path $root $_)) + '"' }
$nt = NinjaPath (Join-Path $root 'src/adapter-mvdm-host-out/win32/include/nt.h')
$ntexapi = NinjaPath (Join-Path $root 'src/opennt-host/public/sdk/inc/ntexapi.h')
$threadCompat = NinjaPath (Join-Path $root 'src/adapter-mvdm-host-out/win32/include/thread_start_compat.h')
$cflags = '/nologo /TC /c /MT /Gy /W4 /showIncludes /DWIN32 /DWINNT /DOPENNT_ADAPTER_NT_ALERT_THREAD /DNTVDM /DCPU_40_STYLE /DNEW_CPU /DCCPU /DSPC386 /DSIM32 /DANSI /DPROD /FI "' + $nt + '" /FI "' + $ntexapi + '" /FI "' + $threadCompat + '" ' + ($includes -join ' ')
$graph = [Collections.Generic.List[string]]::new()
$graph.Add('ninja_required_version = 1.10')
$graph.Add('cflags = ' + $cflags)
$graph.Add('rule cc')
$graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' cl.exe $cflags /Fo$out $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file:')
$graph.Add('rule link')
$graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' link.exe /nologo /OPT:REF /out:$out $in kernel32.lib ntdll.lib')
$graph.Add('rule run')
$graph.Add('  command = $in')
$graph.Add('build obj/nt_timer.obj: cc ' + (NinjaPath (Join-Path $root 'src/mvdm-host/softpc.new/host/src/nt_timer.c')))
$graph.Add('build obj/softpc_delay_heartbeat_fixture.obj: cc ' + (NinjaPath (Join-Path $root 'tests/mvdm-host/softpc_delay_heartbeat_fixture.c')))
$graph.Add('build softpc-delay-heartbeat-fixture.exe: link obj/nt_timer.obj obj/softpc_delay_heartbeat_fixture.obj')
$graph.Add('build verify: run softpc-delay-heartbeat-fixture.exe')
$graph.Add('default verify')
[IO.File]::WriteAllText((Join-Path $build 'build.ninja'), (($graph -join [Environment]::NewLine) + [Environment]::NewLine), [Text.UTF8Encoding]::new($false))
Write-Host "Generated T310 S8 P2 original heartbeat-source graph: $build"
