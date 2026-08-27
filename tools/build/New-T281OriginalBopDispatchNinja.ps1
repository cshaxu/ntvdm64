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
$build = Join-Path $root ("build/M0-T281/S1/{0}" -f $Architecture)
New-Item -ItemType Directory -Force $build | Out-Null

# The x86 values are the original host/src retail monitor build form from
# softpc.new/obj.vdm/CDEFINE.INC.  This selector-disabled compilation island
# applies the same source form to both architecture graphs; it does not select
# a provider or create a runtime route.
$cflags = '/nologo /std:c11 /MT /W4 /showIncludes /DPROD /DMONITOR /DC_VID /DX86GFX /DANSI ' +
    '/FI ' + $root + '/src/adapter-mvdm-host-out/win32/include/nt.h ' +
    '/FI ' + $root + '/src/adapter-mvdm-host-out/softpc/include/error_abi.h ' +
    '/I ' + $root + '/src/mvdm-platform-abi/source/public/internal/base/inc ' +
    '/I ' + $root + '/src/mvdm-support/inc ' +
    '/I ' + $root + '/src/adapter-mvdm-host-out/win32/include ' +
    '/I ' + $root + '/src/adapter-mvdm-host-out/softpc/include ' +
    '/I ' + $root + '/src/mvdm-host/softpc.new/host/inc ' +
    '/I ' + $root + '/src/mvdm-host/softpc.new/base/inc ' +
    '/I ' + $root + '/src/mvdm-host/softpc.new/base/cvidc'

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

build obj/nt_bop.obj: cc `$root/src/mvdm-host/softpc.new/host/src/nt_bop.c
build original-bop-dispatch.lib: lib obj/nt_bop.obj
default original-bop-dispatch.lib
"@

[System.IO.File]::WriteAllText(
    (Join-Path $build 'build.ninja'),
    $content + [Environment]::NewLine,
    (New-Object System.Text.UTF8Encoding($false)))
Write-Host "Wrote T281 $Architecture original-BOP dispatcher graph: $build/build.ninja"
