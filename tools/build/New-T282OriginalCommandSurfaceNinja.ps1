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
$build = Join-Path $root ("build/M0-T282/S1/{0}" -f $Architecture)
New-Item -ItemType Directory -Force $build | Out-Null

$cflags = '/nologo /std:c11 /MT /W4 /showIncludes /DWIN_32 /DDEVL /DNTVDM ' +
    '/FI ' + $root + '/src/adapter-win32/include/nt.h ' +
    '/FI ' + $root + '/src/adapter-softpc/include/error_abi.h ' +
    '/FI ' + $root + '/src/adapter-vdm-monitor/include/vdm.h ' +
    '/I ' + $root + '/src ' +
    '/I ' + $root + '/src/adapter-win32/include ' +
    '/I ' + $root + '/src/adapter-softpc/include ' +
    '/I ' + $root + '/src/adapter-vdm-monitor/include ' +
    '/I ' + $root + '/src/opennt-mvdm-support/inc ' +
    '/I ' + $root + '/src/opennt-platform-abi/source/opennt/public/sdk/inc ' +
    '/I ' + $root + '/src/opennt-platform-abi/source/opennt/public/internal/base/inc ' +
    '/I ' + $root + '/src/opennt-platform-abi/source/opennt/public/ddk/inc ' +
    '/I ' + $root + '/src/opennt-mvdm-host/dos/command ' +
    '/I ' + $root + '/src/opennt-mvdm-host/dos/dem ' +
    '/I ' + $root + '/src/opennt-mvdm-host/softpc.new/host/inc ' +
    '/I ' + $root + '/src/opennt-mvdm-host/softpc.new/base/inc'

$units = @('cmd', 'cmddata', 'cmddisp', 'cmdexec', 'cmdexit', 'cmdmisc',
    'cmdpif', 'cmdredir', 'cmdconf', 'cmdkeyb', 'cmdenv')
$buildLines = foreach ($unit in $units) {
    "build obj/$unit.obj: cc `$root/src/opennt-mvdm-host/dos/command/$unit.c"
}
$objects = ($units | ForEach-Object { "obj/$_.obj" }) -join ' '
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

$($buildLines -join "`n")
build original-command-surface.lib: lib $objects
default original-command-surface.lib
"@
[System.IO.File]::WriteAllText((Join-Path $build 'build.ninja'),
    $content + [Environment]::NewLine, (New-Object System.Text.UTF8Encoding($false)))
Write-Host "Wrote T282 $Architecture original COMMAND surface graph: $build/build.ninja"
