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
$build = Join-Path $root ("build/M0-T290/S4/vrmslot-{0}" -f $Architecture)
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
New-Item -ItemType Directory -Force -Path $build | Out-Null
$environment = Join-Path $build ("msvc-{0}.cmd" -f $Architecture)
@('@echo off', 'set "MVDM_T290_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready', ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'), 'if errorlevel 1 exit /b %errorlevel%', ':ready', 'cd /d "%MVDM_T290_CALLER_CWD%"', '%*') | Set-Content -LiteralPath $environment -Encoding ascii
$includes = @('src', 'src/adapter-mvdm-host-out/redir/include', 'src/adapter-mvdm-host-out/win32/include', 'src/adapter-mvdm-host-out/softpc/include', 'src/mvdm-support/inc', 'src/mvdm-host/vdmredir', 'src/mvdm-host/softpc.new/base/inc', 'src/mvdm-host/softpc.new/host/inc', 'src/mvdm-platform-abi/source/public/internal/base/inc', 'src/mvdm-platform-abi/source/public/ddk/inc', 'src/mvdm-platform-abi/source/private/inc') | ForEach-Object { '/I "' + (Join-Path $root $_).Replace('\', '/') + '"' }
$cflags = '/nologo /TC /c /std:c11 /MT /W4 /Gy /showIncludes /DWIN_32 /DVDMREDIR_DLL ' + ($includes -join ' ')
$content = @"
ninja_required_version = 1.10
root = $root
cflags = $cflags
environment = $($environment.Replace('\', '/'))
rule cc
  command = cmd /c "`$environment cl `$cflags /Fo`$out `$in"
  deps = msvc
rule link
  command = cmd /c "`$environment link /nologo /OPT:REF /out:`$out `$in"
build obj/vrmslot.obj: cc `$root/src/mvdm-host/vdmredir/vrmslot.c
build obj/vrputil.obj: cc `$root/src/mvdm-host/vdmredir/vrputil.c
build obj/mailslot.obj: cc `$root/src/adapter-mvdm-host-out/redir/mvdm_redirector_mailslot.c
build obj/pointer_scope.obj: cc `$root/src/adapter-mvdm-host-out/softpc/mvdm_redirector_pointer_scope.c
build obj/identity.obj: cc `$root/src/adapter-mvdm-host-out/softpc/mvdm_host_identity.c
build obj/mapping.obj: cc `$root/src/session/mapping_manager.c
build obj/lease.obj: cc `$root/src/session/guest_memory_lease.c
build obj/session.obj: cc `$root/src/session/session.c
build obj/fixture.obj: cc `$root/tests/mvdm-host/vdmredir/t290_s4_vrmslot_fixture.c
build bin/t290-s4-vrmslot-fixture.exe: link obj/vrmslot.obj obj/vrputil.obj obj/mailslot.obj obj/pointer_scope.obj obj/identity.obj obj/mapping.obj obj/lease.obj obj/session.obj obj/fixture.obj
default bin/t290-s4-vrmslot-fixture.exe
"@
[System.IO.File]::WriteAllText((Join-Path $build 'build.ninja'), $content + [Environment]::NewLine, (New-Object System.Text.UTF8Encoding($false)))
Write-Host "Generated T290 S4 original VrMailslot graph: $build"
