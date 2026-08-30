[CmdletBinding()]
param(
    [ValidateSet('x86')]
    [string]$Architecture,
    [string]$RepositoryRoot = ''
)

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path.Replace('\', '/')
$build = Join-Path $root ("build/M0-T317/S3/{0}" -f $Architecture)
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
New-Item -ItemType Directory -Force $build | Out-Null
$environment = Join-Path $build 'msvc-mt.cmd'
@('@echo off', 'set "MVDM_T317_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready', ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'), 'if errorlevel 1 exit /b %errorlevel%', ':ready', 'cd /d "%MVDM_T317_CALLER_CWD%"', '%*') |
    Set-Content -LiteralPath $environment -Encoding ascii
$cflags = '/nologo /std:c11 /MT /W4 /WX /showIncludes /I ' + $root +
    '/src /I ' + $root + '/src/adapter-mvdm-host-out/win32/include'
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

build obj/fixture.obj: cc `$root/tests/adapter-mvdm-host-out/win32/command_process_compat_fixture.c
build obj/command_process_compat.obj: cc `$root/src/adapter-mvdm-host-out/win32/source/command_process_compat.c
build command_process_compat_fixture.exe: link obj/fixture.obj obj/command_process_compat.obj
build test: run command_process_compat_fixture.exe
default command_process_compat_fixture.exe
"@
[System.IO.File]::WriteAllText((Join-Path $build 'build.ninja'),
    $content + [Environment]::NewLine, (New-Object System.Text.UTF8Encoding($false)))
Write-Host "Wrote T317 S3 x86 COMMAND child binding graph: $build/build.ninja"
