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
$build = Join-Path $root ("build/M0-T282/S11/{0}" -f $Architecture)
New-Item -ItemType Directory -Force $build | Out-Null
$cflags = '/nologo /std:c11 /MT /W4 /DWIN32 /showIncludes /I ' + $root +
    '/src /I ' + $root + '/src/adapter-win32/include /I ' + $root +
    '/src/mvdm-support/inc /I ' + $root +
    '/src/mvdm-platform-abi/source/public/sdk/inc'
$content = @"
ninja_required_version = 1.10
root = $root
cflags = $cflags

rule cc
  command = cl `$cflags /Fo`$out /c `$in
  deps = msvc
  description = CC `$out
rule link
  command = link /nologo /out:`$out `$in kernel32.lib gdi32.lib
  description = LINK `$out
rule run
  command = `$in
  description = RUN `$in

build obj/fixture.obj: cc `$root/tests/mvdm-support/t282_s11_oem_support_fixture.c
build obj/process.obj: cc `$root/src/mvdm-support/oemuni/process.c
build obj/file.obj: cc `$root/src/mvdm-support/oemuni/file.c
build obj/opennt_support_rtl.obj: cc `$root/src/adapter-win32/source/opennt_support_rtl.c
build oem_support_fixture.exe: link obj/fixture.obj obj/process.obj obj/file.obj obj/opennt_support_rtl.obj
build test: run oem_support_fixture.exe
default oem_support_fixture.exe
"@
[System.IO.File]::WriteAllText((Join-Path $build 'build.ninja'),
    $content + [Environment]::NewLine, (New-Object System.Text.UTF8Encoding($false)))
Write-Host "Wrote T282 $Architecture COMMAND OEM support Ninja graph: $build/build.ninja"
