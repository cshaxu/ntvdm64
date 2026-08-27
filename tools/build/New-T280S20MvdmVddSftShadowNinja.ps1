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
$build = Join-Path $root ("build/M0-T280/{0}-vdd-sft-shadow" -f $Architecture)
New-Item -ItemType Directory -Force -Path $build | Out-Null
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (!(Test-Path -LiteralPath $vs -PathType Leaf)) { throw "MSVC environment entry point missing: $vs" }
$cflags = '/nologo /std:c11 /MT /W4 /showIncludes /I ' + $root + '/src /I ' + $root + '/src/session /I ' + $root + '/src/adapter-softpc/include /I ' + $root + '/src/opennt-mvdm-support/inc'
$content = @"
ninja_required_version = 1.10
root = $root
cflags = $cflags

rule cc
  command = cl `$cflags /c /Fo`$out `$in
  deps = msvc
  description = CC `$in
rule link
  command = link /nologo /out:`$out `$in
  description = LINK `$out

build obj/mapping_manager.obj: cc `$root/src/session/mapping_manager.c
build obj/guest_memory_lease.obj: cc `$root/src/session/guest_memory_lease.c
build obj/session.obj: cc `$root/src/session/session.c
build obj/mvdm_host_identity.obj: cc `$root/src/adapter-softpc/mvdm_host_identity.c
build obj/mvdm_guest_location.obj: cc `$root/src/adapter-softpc/mvdm_guest_location.c
build obj/mvdm_vdd_sft_shadow.obj: cc `$root/src/adapter-softpc/mvdm_vdd_sft_shadow.c
build obj/fixture.obj: cc `$root/tests/adapter-softpc/t280_s20_mvdm_vdd_sft_shadow_fixture.c
build mvdm_vdd_sft_shadow_fixture.exe: link obj/mapping_manager.obj obj/guest_memory_lease.obj obj/session.obj obj/mvdm_host_identity.obj obj/mvdm_guest_location.obj obj/mvdm_vdd_sft_shadow.obj obj/fixture.obj
default mvdm_vdd_sft_shadow_fixture.exe
"@
[System.IO.File]::WriteAllText((Join-Path $build 'build.ninja'), $content + [Environment]::NewLine, (New-Object System.Text.UTF8Encoding($false)))
$environment = Join-Path $build ("msvc-{0}.cmd" -f $Architecture)
@('@echo off', 'set "NTVDM64_NINJA_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto msvc_ready', ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'), 'if errorlevel 1 exit /b %errorlevel%', ':msvc_ready', 'cd /d "%NTVDM64_NINJA_CALLER_CWD%"', '%*') | Set-Content -LiteralPath $environment -Encoding ascii
Write-Host "Wrote T280 S20 $Architecture VDD SFT/JFT shadow Ninja graph: $build/build.ninja"
Write-Host "Use $environment ninja -C $build"
