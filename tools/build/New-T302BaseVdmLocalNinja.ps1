[CmdletBinding()]
param(
    [ValidateSet('x86', 'x64')]
    [string]$Architecture,
    [string]$RepositoryRoot = ''
)

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path.Replace('\', '/')
$build = Join-Path $root ("build/M0-T302/S2/{0}" -f $Architecture)
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$abi = Join-Path $root 'src/opennt-abi/source/public/internal/base/inc'
$hostSdk = Join-Path $root 'src/opennt-host/public/sdk/inc'
$abi = $abi.Replace('\', '/')
$hostSdk = $hostSdk.Replace('\', '/')
New-Item -ItemType Directory -Force $build | Out-Null
$environment = Join-Path $build ("msvc-{0}.cmd" -f $Architecture)
@('@echo off', 'set "MVDM_T302_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready', ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'), 'if errorlevel 1 exit /b %errorlevel%', ':ready', 'cd /d "%MVDM_T302_CALLER_CWD%"', '%*') |
    Set-Content -LiteralPath $environment -Encoding ascii
$cflags = '/nologo /std:c11 /MT /W4 /showIncludes /I ' + $root +
    '/src /I ' + $abi + ' /I ' + $root +
    '/src/adapter-mvdm-host-out/basesrv/include /I ' + $root +
    '/src/adapter-mvdm-host-out/win32/include /I ' + $root + '/src/session /I ' + $hostSdk
$content = @"
ninja_required_version = 1.10
root = $root
cflags = $cflags
environment = $($environment.Replace('\', '/'))

rule cc
  command = cmd /c "`$environment cl `$cflags /Fo`$out /c `$in"
  deps = msvc
  description = CC `$out
rule link
  command = cmd /c "`$environment link /nologo /out:`$out `$in kernel32.lib"
  description = LINK `$out
rule run
  command = `$in
  description = RUN `$in

build obj/fixture.obj: cc `$root/tests/adapter-mvdm-host-out/basesrv/base_vdm_local_fixture.c
build obj/base_vdm_client.obj: cc `$root/src/adapter-mvdm-host-out/basesrv/source/base_vdm_client.c
build obj/base_vdm_local.obj: cc `$root/src/adapter-mvdm-host-out/basesrv/source/base_vdm_local.c
build obj/launch_declaration.obj: cc `$root/src/app/launch_declaration.c
build obj/thread_start_compat.obj: cc `$root/src/adapter-mvdm-host-out/win32/source/thread_start_compat.c
build obj/session.obj: cc `$root/src/session/session.c
build obj/mapping_manager.obj: cc `$root/src/session/mapping_manager.c
build obj/guest_memory_lease.obj: cc `$root/src/session/guest_memory_lease.c
build base_vdm_local_fixture.exe: link obj/fixture.obj obj/base_vdm_client.obj obj/base_vdm_local.obj obj/launch_declaration.obj obj/thread_start_compat.obj obj/session.obj obj/mapping_manager.obj obj/guest_memory_lease.obj
build test: run base_vdm_local_fixture.exe
default base_vdm_local_fixture.exe
"@
[System.IO.File]::WriteAllText((Join-Path $build 'build.ninja'),
    $content + [Environment]::NewLine, (New-Object System.Text.UTF8Encoding($false)))
Write-Host "Wrote T302 $Architecture Base VDM Ninja graph: $build/build.ninja"
