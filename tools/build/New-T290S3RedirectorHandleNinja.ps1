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
$build = Join-Path $root ("build/M0-T290/S3/redirector-handle-{0}" -f $Architecture)
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
New-Item -ItemType Directory -Force -Path $build | Out-Null
$environment = Join-Path $build ("msvc-{0}.cmd" -f $Architecture)
@('@echo off', 'set "MVDM_T290_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready', ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'), 'if errorlevel 1 exit /b %errorlevel%', ':ready', 'cd /d "%MVDM_T290_CALLER_CWD%"', '%*') |
    Set-Content -LiteralPath $environment -Encoding ascii
$includes = @('src', 'src/session', 'src/adapter-mvdm-host-out/redir/include',
    'src/adapter-mvdm-host-out/softpc/include') |
    ForEach-Object { '/I "' + (Join-Path $root $_).Replace('\', '/') + '"' }
$cflags = '/nologo /TC /c /std:c11 /MT /W4 /WX /showIncludes ' + ($includes -join ' ')
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
  command = cmd /c "`$environment link /nologo /out:`$out `$in"
  description = LINK `$out

build obj/mapping_manager.obj: cc `$root/src/session/mapping_manager.c
build obj/guest_memory_lease.obj: cc `$root/src/session/guest_memory_lease.c
build obj/session.obj: cc `$root/src/session/session.c
build obj/mvdm_host_identity.obj: cc `$root/src/adapter-mvdm-host-out/softpc/mvdm_host_identity.c
build obj/mvdm_redirector_handle.obj: cc `$root/src/adapter-mvdm-host-out/redir/mvdm_redirector_handle.c
build obj/mvdm_redirector_mailslot.obj: cc `$root/src/adapter-mvdm-host-out/redir/mvdm_redirector_mailslot.c
build obj/fixture.obj: cc `$root/tests/adapter-mvdm-host-out/redir/t290_s3_redirector_handle_fixture.c
build bin/t290-s3-redirector-handle-fixture.exe: link obj/mapping_manager.obj obj/guest_memory_lease.obj obj/session.obj obj/mvdm_host_identity.obj obj/mvdm_redirector_handle.obj obj/mvdm_redirector_mailslot.obj obj/fixture.obj
default bin/t290-s3-redirector-handle-fixture.exe
"@
[System.IO.File]::WriteAllText((Join-Path $build 'build.ninja'), $content + [Environment]::NewLine, (New-Object System.Text.UTF8Encoding($false)))
Write-Host "Generated T290 S3 $Architecture Redirector handle graph: $build"
