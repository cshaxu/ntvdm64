[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [ValidateSet('x86', 'x64')] [string]$Architecture,
    [string]$RepositoryRoot = '',
    [string]$BuildRoot = ''
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
if ([string]::IsNullOrWhiteSpace($BuildRoot)) {
    $BuildRoot = Join-Path $root ("build/M0-T324/S2/local-redir-{0}" -f $Architecture)
}
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (!(Test-Path -LiteralPath $vs -PathType Leaf) -or !(Get-Command ninja -ErrorAction SilentlyContinue)) {
    throw 'MSVC Build Tools and Ninja are required.'
}
New-Item -ItemType Directory -Force -Path $BuildRoot, (Join-Path $BuildRoot 'obj') | Out-Null
$environment = Join-Path $BuildRoot ("msvc-{0}.cmd" -f $Architecture)
@('@echo off', 'set "MVDM_T324_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready',
  ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'),
  'if errorlevel 1 exit /b %errorlevel%', ':ready', 'cd /d "%MVDM_T324_CALLER_CWD%"', '%*') |
    Set-Content -LiteralPath $environment -Encoding ascii
$includeRoots = @(
    'src', 'src/adapter-mvdm-host-out/redir/include', 'src/mvdm-host/inc', 'src/mvdm-host/vdmredir',
    'src/mvdm-host-overlay/vdmredir',
    'src/mvdm-host/dos/command', 'src/opennt-host/netapi/netlib',
    'src/mvdm-host/softpc.new/base/inc', 'src/mvdm-host/softpc.new/host/inc',
    'src/adapter-mvdm-host-out/softpc/include',
    'src/adapter-mvdm-host-out/win32/include',
    'src/opennt-abi/source/public/internal/base/inc',
    'src/opennt-abi/source/public/internal/ds/inc',
    'src/opennt-host/public/sdk/inc',
    'src/opennt-abi/source/public/sdk/inc', 'src/opennt-abi/source/public/ddk/inc',
    'src/opennt-abi/source/private/inc') | ForEach-Object {
        '/I "' + (Join-Path $root $_).Replace('\', '/') + '"'
    }
$threadHeader = (Join-Path $root 'src/adapter-mvdm-host-out/win32/include/thread_start_compat.h').Replace('\', '/')
$cflags = '/nologo /TC /c /std:c11 /MT /W4 /Gy /showIncludes /DWIN_32 /DVDMREDIR_DLL /DCPU_40_STYLE /FI "' +
    $threadHeader + '" ' + ($includeRoots -join ' ')
$ninjaRoot = $root.Replace('\', '/').Replace(':', '$:')
$ninjaEnvironment = $environment.Replace('\', '/').Replace(':', '$:')
$content = @"
ninja_required_version = 1.10
root = $ninjaRoot
cflags = $cflags
environment = $ninjaEnvironment

rule cc
  command = cmd /c "`$environment cl `$cflags /Fo`$out `$in"
  deps = msvc
  description = CC `$in
rule link
  command = cmd /c "`$environment link /nologo /OPT:REF /out:`$out `$in"
  description = LINK `$out

build obj/vrnmpipe.obj: cc `$root/src/mvdm-host/vdmredir/vrnmpipe.c
build obj/vrputil.obj: cc `$root/src/mvdm-host/vdmredir/vrputil.c
build obj/vrdisp.obj: cc `$root/src/mvdm-host/vdmredir/vrdisp.c
build obj/vrmisc.obj: cc `$root/src/mvdm-host/vdmredir/vrmisc.c
build obj/vrmslot.obj: cc `$root/src/mvdm-host/vdmredir/vrmslot.c
build obj/cmdredir.obj: cc `$root/src/mvdm-host/dos/command/cmdredir.c
build obj/ntstatus.obj: cc `$root/src/opennt-host/netapi/netlib/ntstatus.c
build obj/async.obj: cc `$root/src/mvdm-host-overlay/vdmredir/mvdm_redirector_async.c
build obj/location.obj: cc `$root/src/adapter-mvdm-host-out/softpc/mvdm_guest_location.c
build obj/thread.obj: cc `$root/src/adapter-mvdm-host-out/win32/source/thread_start_compat.c
build obj/session.obj: cc `$root/src/session/session.c
build obj/mapping.obj: cc `$root/src/session/mapping_manager.c
build obj/lease.obj: cc `$root/src/session/guest_memory_lease.c
build obj/fixture.obj: cc `$root/tests/mvdm-host/vdmredir/redirector_async_contract_fixture.c
build bin/redirector-async-contract-fixture.exe: link obj/async.obj obj/location.obj obj/session.obj obj/mapping.obj obj/lease.obj obj/fixture.obj
default obj/vrnmpipe.obj obj/vrputil.obj obj/vrdisp.obj obj/vrmisc.obj obj/vrmslot.obj obj/cmdredir.obj obj/ntstatus.obj bin/redirector-async-contract-fixture.exe
"@
[IO.File]::WriteAllText((Join-Path $BuildRoot 'build.ninja'), $content + [Environment]::NewLine,
    (New-Object Text.UTF8Encoding($false)))
Write-Host "Generated T324 S2 local Redirector x86/x64 source graph: $BuildRoot"
