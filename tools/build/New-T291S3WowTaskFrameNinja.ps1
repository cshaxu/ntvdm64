[CmdletBinding()]
param(
    [ValidateSet('x86', 'x64')]
    [string]$Architecture,
    [string]$RepositoryRoot = ''
)

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot) }
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path.Replace('\', '/')
$build = Join-Path $root ("build/M0-T291/S3/wow-task-frame-{0}" -f $Architecture)
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
New-Item -ItemType Directory -Force -Path $build | Out-Null
$environment = Join-Path $build ("msvc-{0}.cmd" -f $Architecture)
@('@echo off', 'set "MVDM_T291_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready', ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'), 'if errorlevel 1 exit /b %errorlevel%', ':ready', 'cd /d "%MVDM_T291_CALLER_CWD%"', '%*') | Set-Content -LiteralPath $environment -Encoding ascii
$cflags = '/nologo /std:c11 /MT /W4 /WX /showIncludes /I ' + $root + '/src /I ' + $root + '/src/session /I ' + $root + '/src/adapter-mvdm-host-out/monitor/include'
$content = @"
ninja_required_version = 1.10
root = $root
cflags = $cflags
environment = $($environment.Replace('\', '/'))
rule cc
  command = cmd /c "`$environment cl `$cflags /c /Fo`$out `$in"
  deps = msvc
  description = CC `$in
rule link
  command = cmd /c "`$environment link /nologo /out:`$out `$in"
  description = LINK `$out
build obj/mapping_manager.obj: cc `$root/src/session/mapping_manager.c
build obj/guest_memory_lease.obj: cc `$root/src/session/guest_memory_lease.c
build obj/session.obj: cc `$root/src/session/session.c
build obj/task_frame.obj: cc `$root/src/adapter-mvdm-host-out/monitor/mvdm_wow_task_frame.c
build obj/fixture.obj: cc `$root/tests/adapter-mvdm-host-out/monitor/t291_s3_wow_task_frame_fixture.c
build bin/t291-s3-wow-task-frame-fixture.exe: link obj/mapping_manager.obj obj/guest_memory_lease.obj obj/session.obj obj/task_frame.obj obj/fixture.obj
default bin/t291-s3-wow-task-frame-fixture.exe
"@
[System.IO.File]::WriteAllText((Join-Path $build 'build.ninja'), $content + [Environment]::NewLine, (New-Object System.Text.UTF8Encoding($false)))
Write-Host "Generated T291 S3 $Architecture WOW task-frame graph: $build"
