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
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path.Replace('\', '/')
if ([string]::IsNullOrWhiteSpace($BuildRoot)) {
    $build = Join-Path $root ("build/M0-T335/S3/wow-callback-frame-{0}" -f $Architecture)
} else {
    $build = [IO.Path]::GetFullPath($BuildRoot)
}
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (!(Test-Path -LiteralPath $vs -PathType Leaf) -or !(Get-Command ninja -ErrorAction SilentlyContinue)) {
    throw 'MSVC Build Tools and Ninja are required.'
}
New-Item -ItemType Directory -Force -Path $build | Out-Null
$environment = Join-Path $build ("msvc-{0}.cmd" -f $Architecture)
@('@echo off', 'set "MVDM_T335_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready', ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'), 'if errorlevel 1 exit /b %errorlevel%', ':ready', 'cd /d "%MVDM_T335_CALLER_CWD%"', '%*') | Set-Content -LiteralPath $environment -Encoding ascii
$cflags = '/nologo /std:c11 /MT /W4 /WX /showIncludes /I ' + $root + '/src /I ' + $root + '/src/session /I ' + $root + '/src/adapter-mvdm-host-out/wow/include /I ' + $root + '/src/adapter-mvdm-host-out/win32/include /I ' + $root + '/src/opennt-host/public/sdk/inc'
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
build obj/wow_callback_frame_lease.obj: cc `$root/src/adapter-mvdm-host-out/wow/wow_callback_frame_lease.c
build obj/opennt_support_rtl.obj: cc `$root/src/adapter-mvdm-host-out/win32/source/opennt_support_rtl.c
build obj/fixture.obj: cc `$root/tests/adapter-mvdm-host-out/wow/t335_s3_wow_callback_frame_lease_fixture.c
build bin/t335-s3-wow-callback-frame-fixture.exe: link obj/mapping_manager.obj obj/guest_memory_lease.obj obj/session.obj obj/wow_callback_frame_lease.obj obj/opennt_support_rtl.obj obj/fixture.obj
default bin/t335-s3-wow-callback-frame-fixture.exe
"@
[System.IO.File]::WriteAllText((Join-Path $build 'build.ninja'), $content + [Environment]::NewLine, (New-Object System.Text.UTF8Encoding($false)))
Write-Host "Generated T335 S3 $Architecture WOW callback-frame graph: $build"
