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
$build = Join-Path $root ("build/M0-T290/S6/disabled-ingress-{0}" -f $Architecture)
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
New-Item -ItemType Directory -Force -Path $build | Out-Null
$environment = Join-Path $build ("msvc-{0}.cmd" -f $Architecture)
@('@echo off', 'set "MVDM_T290_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready', ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'), 'if errorlevel 1 exit /b %errorlevel%', ':ready', 'cd /d "%MVDM_T290_CALLER_CWD%"', '%*') |
    Set-Content -LiteralPath $environment -Encoding ascii
$includes = @('src', 'src/adapter-mvdm-host-in') |
    ForEach-Object { '/I "' + (Join-Path $root $_).Replace('\', '/') + '"' }
$cflags = '/nologo /TC /c /std:c11 /MT /W4 /WX /showIncludes ' + ($includes -join ' ')
$cxxflags = '/nologo /TP /c /std:c++17 /EHsc /MT /W4 /WX /showIncludes ' + ($includes -join ' ')
$content = @"
ninja_required_version = 1.10
root = $root
cflags = $cflags
cxxflags = $cxxflags
environment = $($environment.Replace('\', '/'))

rule cc
  command = cmd /c "`$environment cl `$cflags /Fo`$out `$in"
  deps = msvc
rule cxx
  command = cmd /c "`$environment cl `$cxxflags /Fo`$out `$in"
  deps = msvc
rule link
  command = cmd /c "`$environment link /nologo /out:`$out `$in"

build obj/bop_ingress.obj: cc `$root/src/adapter-mvdm-host-in/bop_ingress.c
build obj/generic_ud_bridge.obj: cxx `$root/src/adapter-mvdm-host-in/generic_ud_bridge.cc
build obj/fixture.obj: cc `$root/tests/adapter-mvdm-host-in/t290_s6_redirector_disabled_ingress_fixture.c
build bin/t290-s6-redirector-disabled-ingress-fixture.exe: link obj/bop_ingress.obj obj/generic_ud_bridge.obj obj/fixture.obj
default bin/t290-s6-redirector-disabled-ingress-fixture.exe
"@
[System.IO.File]::WriteAllText((Join-Path $build 'build.ninja'), $content + [Environment]::NewLine, (New-Object System.Text.UTF8Encoding($false)))
Write-Host "Generated T290 S6 $Architecture Redirector disabled-ingress graph: $build"
