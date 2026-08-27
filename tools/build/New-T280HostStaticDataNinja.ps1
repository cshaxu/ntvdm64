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
$build = Join-Path $root ("build/M0-T280/{0}" -f $Architecture)
New-Item -ItemType Directory -Force $build | Out-Null

$cflags = '/nologo /std:c11 /MT /W4 /showIncludes /FI ' + $root + '/src/adapter-win32/include/nt.h /FI ' + $root + '/src/adapter-softpc/include/error_abi.h /I ' + $root + '/src/adapter-win32/include /I ' + $root + '/src/adapter-softpc/include /I ' + $root + '/src/mvdm-host/softpc.new/host/inc /I ' + $root + '/src/mvdm-host/softpc.new/base/inc'
$content = @"
ninja_required_version = 1.10
root = $root
cflags = $cflags

rule cc
  command = cl `$cflags /Fo`$out /c `$in
  deps = msvc
  description = CC `$out
rule lib
  command = lib /nologo /out:`$out `$in
  description = LIB `$out

build obj/nt_ertbl.obj: cc `$root/src/mvdm-host/softpc.new/host/src/nt_ertbl.c
build obj/nt_mess.obj: cc `$root/src/mvdm-host/softpc.new/host/src/nt_mess.c
build host-static-data.lib: lib obj/nt_ertbl.obj obj/nt_mess.obj
default host-static-data.lib
"@
[System.IO.File]::WriteAllText((Join-Path $build 'build.ninja'), $content + [Environment]::NewLine,
    (New-Object System.Text.UTF8Encoding($false)))
Write-Host "Wrote T280 $Architecture host static-data Ninja graph: $build/build.ninja"
