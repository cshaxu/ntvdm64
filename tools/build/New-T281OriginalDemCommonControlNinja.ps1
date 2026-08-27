[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidateSet('x86', 'x64')]
    [string]$Architecture,
    [string]$RepositoryRoot = ''
)

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path.Replace('\', '/')
$build = Join-Path $root ("build/M0-T281/S2/{0}" -f $Architecture)
New-Item -ItemType Directory -Force $build | Out-Null

# WIN_32 and DEVL are the original DEM package declaration forms: sources
# specifies WIN_32 and demexp.h exposes its original debug declarations only
# under DEVL.  This archive contains no provider implementation or route.
$cflags = '/nologo /std:c11 /MT /W4 /showIncludes /DWIN_32 /DDEVL ' +
    '/FI ' + $root + '/src/adapter-mvdm-host-out/win32/include/nt.h ' +
    '/FI ' + $root + '/src/adapter-softpc/include/error_abi.h ' +
    '/I ' + $root + '/src ' +
    '/I ' + $root + '/src/adapter-mvdm-host-out/win32/include ' +
    '/I ' + $root + '/src/adapter-softpc/include ' +
    '/I ' + $root + '/src/mvdm-support/inc ' +
    '/I ' + $root + '/src/mvdm-platform-abi/source/public/sdk/inc ' +
    '/I ' + $root + '/src/mvdm-platform-abi/source/public/internal/base/inc ' +
    '/I ' + $root + '/src/mvdm-platform-abi/source/public/ddk/inc ' +
    '/I ' + $root + '/src/mvdm-host/dos/dem ' +
    '/I ' + $root + '/src/mvdm-host/softpc.new/host/inc ' +
    '/I ' + $root + '/src/mvdm-host/softpc.new/base/inc'

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

build obj/dem.obj: cc `$root/src/mvdm-host/dos/dem/dem.c
build obj/demdata.obj: cc `$root/src/mvdm-host/dos/dem/demdata.c
build obj/demmsg.obj: cc `$root/src/mvdm-host/dos/dem/demmsg.c
build obj/demdisp.obj: cc `$root/src/mvdm-host/dos/dem/demdisp.c
build original-dem-common-control.lib: lib obj/dem.obj obj/demdata.obj obj/demmsg.obj obj/demdisp.obj
default original-dem-common-control.lib
"@

[System.IO.File]::WriteAllText(
    (Join-Path $build 'build.ninja'),
    $content + [Environment]::NewLine,
    (New-Object System.Text.UTF8Encoding($false)))
Write-Host "Wrote T281 $Architecture original-DEM common-control graph: $build/build.ninja"
