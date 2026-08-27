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
$build = Join-Path $root ("build/M0-T280/{0}-pdb-consumers" -f $Architecture)
New-Item -ItemType Directory -Force -Path $build | Out-Null
$cflags = '/nologo /std:c11 /MT /W4 /DWIN_32 /DDEVL=1 /Zs /showIncludes /I ' + $root + '/src /I ' + $root + '/src/adapter-softpc/include /I ' + $root + '/src/adapter-vdm-monitor/include /I ' + $root + '/src/adapter-win32/include /I ' + $root + '/src/opennt-mvdm-host/dos/dem /I ' + $root + '/src/opennt-mvdm-support/inc /I ' + $root + '/src/opennt-platform-abi/source/public/sdk/inc /I ' + $root + '/src/opennt-platform-abi/source/public/ddk/inc /I ' + $root + '/src/opennt-mvdm-host/softpc.new/base/inc /I ' + $root + '/src/opennt-mvdm-host/softpc.new/host/inc'
$content = @"
ninja_required_version = 1.10
root = $root
cflags = $cflags

rule syntax
  command = cl `$cflags /TC `$in
  deps = msvc
  description = SYNTAX `$in

build demfile: syntax `$root/src/opennt-mvdm-host/dos/dem/demfile.c
build nt_fdisk: syntax `$root/src/opennt-mvdm-host/softpc.new/host/src/nt_fdisk.c
build nt_rflop: syntax `$root/src/opennt-mvdm-host/softpc.new/host/src/nt_rflop.c
build all: phony demfile nt_fdisk nt_rflop
default all
"@
[System.IO.File]::WriteAllText((Join-Path $build 'build.ninja'), $content + [Environment]::NewLine, (New-Object System.Text.UTF8Encoding($false)))
Write-Host "Wrote T280 S20 $Architecture PDB-consumer Ninja graph: $build/build.ninja"
