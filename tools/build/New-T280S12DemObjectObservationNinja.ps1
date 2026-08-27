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
$build = Join-Path $root ("build/M0-T280/{0}-dem-object-observation" -f $Architecture)
New-Item -ItemType Directory -Force -Path $build | Out-Null
$cflags = '/nologo /std:c11 /MT /W4 /DWIN_32 /DDEVL=1 /Zs /showIncludes /I ' + $root + '/src /I ' + $root + '/src/adapter-mvdm-host-out/monitor/include /I ' + $root + '/src/adapter-mvdm-host-out/win32/include /I ' + $root + '/src/adapter-mvdm-host-out/softpc/include /I ' + $root + '/src/mvdm-host/dos/dem /I ' + $root + '/src/mvdm-support/inc /I ' + $root + '/src/mvdm-platform-abi/source/public/sdk/inc /I ' + $root + '/src/mvdm-platform-abi/source/public/ddk/inc /I ' + $root + '/src/mvdm-host/softpc.new/base/inc /I ' + $root + '/src/mvdm-host/softpc.new/host/inc'
$content = @"
ninja_required_version = 1.10
root = $root
cflags = $cflags

rule syntax
  command = cl `$cflags /TC `$in
  deps = msvc
  description = SYNTAX `$in

build demsrch: syntax `$root/src/mvdm-host/dos/dem/demsrch.c
build demerror: syntax `$root/src/mvdm-host/dos/dem/demerror.c
build demgset: syntax `$root/src/mvdm-host/dos/dem/demgset.c
build demhndl: syntax `$root/src/mvdm-host/dos/dem/demhndl.c
build demfcb: syntax `$root/src/mvdm-host/dos/dem/demfcb.c
build all: phony demsrch demerror demgset demhndl demfcb
default all
"@
[System.IO.File]::WriteAllText((Join-Path $build 'build.ninja'), $content + [Environment]::NewLine, (New-Object System.Text.UTF8Encoding($false)))
Write-Host "Wrote T280 S12 $Architecture DEM observation Ninja graph: $build/build.ninja"
