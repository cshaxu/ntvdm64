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
$build = Join-Path $root ("build/M0-T283/S2/{0}" -f $Architecture)
New-Item -ItemType Directory -Force $build | Out-Null

$cflags = '/nologo /std:c11 /MT /W4 /showIncludes /DWIN_32 /DDEVL /DNTVDM ' +
    '/FI ' + $root + '/src/adapter-mvdm-host-out/win32/include/nt.h ' +
    '/FI ' + $root + '/src/adapter-mvdm-host-out/softpc/include/error_abi.h ' +
    '/FI ' + $root + '/src/adapter-mvdm-host-out/monitor/include/vdm.h ' +
    '/I ' + $root + '/src ' +
    '/I ' + $root + '/src/adapter-mvdm-host-out/win32/include ' +
    '/I ' + $root + '/src/adapter-mvdm-host-out/softpc/include ' +
    '/I ' + $root + '/src/adapter-mvdm-host-out/monitor/include ' +
    '/I ' + $root + '/src/mvdm-support/inc ' +
    '/I ' + $root + '/src/mvdm-platform-abi/source/public/sdk/inc ' +
    '/I ' + $root + '/src/mvdm-platform-abi/source/public/ddk/inc ' +
    '/I ' + $root + '/src/mvdm-platform-abi/source/public/internal/base/inc ' +
    '/I ' + $root + '/src/mvdm-platform-abi/source/public/internal/windows/inc ' +
    '/I ' + $root + '/src/mvdm-host/dos/dem ' +
    '/I ' + $root + '/src/mvdm-host/dos/command ' +
    '/I ' + $root + '/src/mvdm-host/softpc.new/host/inc ' +
    '/I ' + $root + '/src/mvdm-host/softpc.new/base/inc'

$commandUnits = @('cmd', 'cmddata', 'cmddisp', 'cmdexec', 'cmdexit', 'cmdmisc',
    'cmdpif', 'cmdredir', 'cmdconf', 'cmdkeyb', 'cmdenv')
$demUnits = @('dem', 'demdata', 'demmsg', 'demdisp', 'demdasd', 'demdir',
    'demerror', 'demfcb', 'demfile', 'demgset', 'demhndl', 'demioctl',
    'demlabel', 'demlock', 'demmisc', 'demsrch')
$commandLines = foreach ($unit in $commandUnits) {
    "build obj/command/$unit.obj: cc `$root/src/mvdm-host/dos/command/$unit.c"
}
$demLines = foreach ($unit in $demUnits) {
    "build obj/dem/$unit.obj: cc `$root/src/mvdm-host/dos/dem/$unit.c"
}
$commandObjects = ($commandUnits | ForEach-Object { "obj/command/$_.obj" }) -join ' '
$demObjects = ($demUnits | ForEach-Object { "obj/dem/$_.obj" }) -join ' '
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

$($commandLines -join "`n")
$($demLines -join "`n")
build original-command-carrier.lib: lib $commandObjects
build original-dem-carrier.lib: lib $demObjects
default original-command-carrier.lib original-dem-carrier.lib
"@

[System.IO.File]::WriteAllText((Join-Path $build 'build.ninja'),
    $content + [Environment]::NewLine, (New-Object System.Text.UTF8Encoding($false)))
Write-Host "Wrote T283 $Architecture platform-ABI declaration-carrier graph: $build/build.ninja"
