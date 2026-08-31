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
$build = Join-Path $root ("build/M0-T290/S3/vrnmpipe-name-{0}" -f $Architecture)
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
New-Item -ItemType Directory -Force -Path $build | Out-Null
$environment = Join-Path $build ("msvc-{0}.cmd" -f $Architecture)
@('@echo off', 'set "MVDM_T290_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready', ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'), 'if errorlevel 1 exit /b %errorlevel%', ':ready', 'cd /d "%MVDM_T290_CALLER_CWD%"', '%*') |
    Set-Content -LiteralPath $environment -Encoding ascii
$includes = @('src', 'src/adapter-mvdm-host-out/redir/include',
    'src/adapter-mvdm-host-out/win32/include',
    'src/adapter-mvdm-host-out/softpc/include', 'src/mvdm-support/inc',
    'src/mvdm-host/vdmredir', 'src/mvdm-host/softpc.new/base/inc',
    'src/mvdm-host/softpc.new/host/inc',
    'src/opennt-abi/source/public/internal/base/inc',
    'src/opennt-abi/source/public/ddk/inc',
    'src/opennt-abi/source/private/inc',
    'src/opennt-host/public/sdk/inc') |
    ForEach-Object { '/I "' + (Join-Path $root $_).Replace('\', '/') + '"' }
$cflags = '/nologo /TC /c /std:c11 /MT /W4 /Gy /showIncludes /DWIN_32 /DVDMREDIR_DLL /FI "' + (Join-Path $root 'src/adapter-mvdm-host-out/win32/include/mvdm_redirector_thread.h').Replace('\', '/') + '" ' + ($includes -join ' ')
$content = @"
ninja_required_version = 1.10
root = $root
cflags = $cflags
environment = $($environment.Replace('\', '/'))

rule cc
  command = cmd /c "`$environment cl `$cflags /Fo`$out `$in"
  deps = msvc
  description = CC `$in
rule link
  command = cmd /c "`$environment link /nologo /OPT:REF /out:`$out `$in"
  description = LINK `$out

build obj/vrnmpipe.obj: cc `$root/src/mvdm-host/vdmredir/vrnmpipe.c
build obj/vrputil.obj: cc `$root/src/mvdm-host/vdmredir/vrputil.c
build obj/mvdm_redirector_thread.obj: cc `$root/src/adapter-mvdm-host-out/win32/source/mvdm_redirector_thread.c
build obj/mvdm_redirector_pointer_scope.obj: cc `$root/src/adapter-mvdm-host-out/softpc/mvdm_redirector_pointer_scope.c
build obj/mapping_manager.obj: cc `$root/src/session/mapping_manager.c
build obj/guest_memory_lease.obj: cc `$root/src/session/guest_memory_lease.c
build obj/session.obj: cc `$root/src/session/session.c
build obj/mvdm_host_identity.obj: cc `$root/src/adapter-mvdm-host-out/softpc/mvdm_host_identity.c
build obj/mvdm_redirector_handle.obj: cc `$root/src/adapter-mvdm-host-out/redir/mvdm_redirector_handle.c
build obj/fixture.obj: cc `$root/tests/mvdm-host/vdmredir/t290_s3_vrnmpipe_name_fixture.c
build bin/t290-s3-vrnmpipe-name-fixture.exe: link obj/vrnmpipe.obj obj/vrputil.obj obj/mvdm_redirector_thread.obj obj/mvdm_redirector_pointer_scope.obj obj/mapping_manager.obj obj/guest_memory_lease.obj obj/session.obj obj/mvdm_host_identity.obj obj/mvdm_redirector_handle.obj obj/fixture.obj
default bin/t290-s3-vrnmpipe-name-fixture.exe
"@
[System.IO.File]::WriteAllText((Join-Path $build 'build.ninja'), $content + [Environment]::NewLine, (New-Object System.Text.UTF8Encoding($false)))
Write-Host "Generated T290 S3 $Architecture original VrNamedPipeName graph: $build"
