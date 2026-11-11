[CmdletBinding()]
param(
    [ValidateSet('x86')]
    [string]$Architecture,
    [string]$RepositoryRoot = ''
)

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path.Replace('\', '/')
$build = Join-Path $root ("build/M0-T280/{0}-dos-find-layout" -f $Architecture)
New-Item -ItemType Directory -Force -Path $build | Out-Null
$cflags = '/nologo /std:c11 /MT /W4 /showIncludes /FI ' + $root + '/src/compat/include/nt.h /I ' + $root + '/src/compat/include /I ' + $root + '/src/mvdm-host/dos/dem /I ' + $root + '/src/mvdm-host/inc /I ' + $root + '/src/non-mvdm/public/sdk/inc /I ' + $root + '/src/non-mvdm/public/ddk/inc /I ' + $root + '/src/mvdm-host/softpc.new/base/inc /I ' + $root + '/src/mvdm-host/softpc.new/host/inc'
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

build obj/layout.obj: cc `$root/test/mvdm-host/dos/dem/dos_find_layout_test.c
build dos_find_layout_fixture.exe: link obj/layout.obj
default dos_find_layout_fixture.exe
"@
[System.IO.File]::WriteAllText((Join-Path $build 'build.ninja'), $content + [Environment]::NewLine, (New-Object System.Text.UTF8Encoding($false)))
Write-Host "Wrote T280 S16 $Architecture DOS find-layout Ninja graph: $build/build.ninja"
