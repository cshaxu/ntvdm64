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
$build = Join-Path $root ("build/M0-T290/S5/vrnetapi-local-{0}" -f $Architecture)
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
New-Item -ItemType Directory -Force -Path $build | Out-Null
$environment = Join-Path $build ("msvc-{0}.cmd" -f $Architecture)
@('@echo off', 'set "MVDM_T290_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready', ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'), 'if errorlevel 1 exit /b %errorlevel%', ':ready', 'cd /d "%MVDM_T290_CALLER_CWD%"', '%*') |
    Set-Content -LiteralPath $environment -Encoding ascii
$includes = @(
    'src',
    'src/adapter-mvdm-host-out/win32/include',
    'src/adapter-mvdm-host-out/softpc/include',
    'src/mvdm-support/inc',
    'src/mvdm-platform-abi/source/public/internal/ds/inc',
    'src/mvdm-platform-abi/source/public/internal/base/inc',
    'src/mvdm-platform-abi/source/public/internal/net/inc',
    'src/mvdm-host/vdmredir'
) | ForEach-Object { '/I "' + (Join-Path $root $_).Replace('\', '/') + '"' }
$cflags = '/nologo /TC /c /std:c11 /MT /W4 /WX /Gy /showIncludes /D_CRT_SECURE_NO_WARNINGS /wd4013 /wd4057 /wd4133 /wd4189 /wd4267 /wd4311 /wd4701 /wd4703 /DWIN_32 /DVDMREDIR_DLL ' + ($includes -join ' ')
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
  command = cmd /c "`$environment link /nologo /OPT:REF /FORCE:UNRESOLVED /out:`$out `$in"
  description = LINK `$out

build obj/vrnetapi.obj: cc `$root/src/mvdm-host/vdmredir/vrnetapi.c
build obj/fixture.obj: cc `$root/tests/mvdm-host/vdmredir/t290_s5_vrnetapi_local_fixture.c
build bin/t290-s5-vrnetapi-local-fixture.exe: link obj/vrnetapi.obj obj/fixture.obj
default bin/t290-s5-vrnetapi-local-fixture.exe
"@
[System.IO.File]::WriteAllText((Join-Path $build 'build.ninja'), $content + [Environment]::NewLine, (New-Object System.Text.UTF8Encoding($false)))
Write-Host "Generated T290 S5 $Architecture original VrNetApi local graph: $build"
