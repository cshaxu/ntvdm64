[CmdletBinding()]
param(
    [ValidateSet('x86')]
    [string]$Architecture = 'x86',
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
    $BuildRoot = Join-Path $root 'build/M0-T310/S20/host-input-contract-x86'
}
$build = [IO.Path]::GetFullPath($BuildRoot)
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
New-Item -ItemType Directory -Force $build, (Join-Path $build 'obj') | Out-Null
$environment = Join-Path $build 'msvc-mt.cmd'
@('@echo off', 'set "MVDM_T310_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready',
  ('call "' + $vs + '" -arch=x86 -host_arch=x64 >nul'),
  'if errorlevel 1 exit /b %errorlevel%', ':ready', 'cd /d "%MVDM_T310_CALLER_CWD%"', '%*') |
    Set-Content -LiteralPath $environment -Encoding ascii
$cflags = '/nologo /TC /MT /W4 /showIncludes /I ' + $root +
    '/src /I ' + $root + '/src/adapter-mvdm-host-out/win32/include'
$content = @"
ninja_required_version = 1.10
root = $root
cflags = $cflags
environment = $($environment.Replace('\', '/'))

rule cc
  command = cmd /d /s /c call `$environment cl `$cflags /Fo`$out /c `$in
  deps = msvc
  description = CC `$out
rule link
  command = cmd /d /s /c call `$environment link /nologo /out:`$out `$in kernel32.lib user32.lib
  description = LINK `$out
rule run
  command = `$in
  description = RUN `$in

build obj/fixture.obj: cc `$root/tests/adapter-mvdm-host-out/win32/console_input_contract_fixture.c
build obj/console_compat.obj: cc `$root/src/adapter-mvdm-host-out/win32/source/console_compat.c
build console_input_contract_fixture.exe: link obj/fixture.obj obj/console_compat.obj
build test: run console_input_contract_fixture.exe
default console_input_contract_fixture.exe
"@
[System.IO.File]::WriteAllText((Join-Path $build 'build.ninja'),
    $content + [Environment]::NewLine, (New-Object System.Text.UTF8Encoding($false)))
Write-Host "Wrote T310 host-input x86 Ninja graph: $build/build.ninja"
