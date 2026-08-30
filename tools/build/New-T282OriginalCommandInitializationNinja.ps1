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
$build = Join-Path $root ("build/M0-T282/S7/{0}" -f $Architecture)
New-Item -ItemType Directory -Force $build | Out-Null
$cflags = '/nologo /std:c11 /MT /W4 /showIncludes /DWIN_32 /DDEVL /DNTVDM ' +
    '/FI ' + $root + '/src/adapter-mvdm-host-out/win32/include/nt.h ' +
    '/FI ' + $root + '/src/adapter-mvdm-host-out/softpc/include/error_abi.h ' +
    '/FI ' + $root + '/src/adapter-mvdm-host-out/monitor/include/monitor_context.h ' +
    '/I ' + $root + '/src ' +
    '/I ' + $root + '/src/adapter-mvdm-host-out/win32/include ' +
    '/I ' + $root + '/src/adapter-mvdm-host-out/softpc/include ' +
    '/I ' + $root + '/src/adapter-mvdm-host-out/monitor/include ' +
    '/I ' + $root + '/src/opennt-host/public/sdk/inc ' +
    '/I ' + $root + '/src/opennt-abi/source/public/sdk/inc ' +
    '/I ' + $root + '/src/opennt-abi/source/public/internal/base/inc ' +
    '/I ' + $root + '/src/opennt-abi/source/public/internal/windows/inc ' +
    '/I ' + $root + '/src/opennt-abi/source/private/windows/inc ' +
    '/I ' + $root + '/src/opennt-abi/source/public/ddk/inc ' +
    '/I ' + $root + '/src/mvdm-support/inc ' +
    '/I ' + $root + '/src/mvdm-host/dos/command ' +
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
rule link
  command = link /nologo /out:`$out `$in kernel32.lib
  description = LINK `$out
rule run
  command = `$in
  description = RUN `$in

build obj/command_initialization_fixture.obj: cc `$root/tests/mvdm-host/command_initialization_fixture.c
build obj/cmd.obj: cc `$root/src/mvdm-host/dos/command/cmd.c
build obj/cmddata.obj: cc `$root/src/mvdm-host/dos/command/cmddata.c
build command_initialization_fixture.exe: link obj/command_initialization_fixture.obj obj/cmd.obj obj/cmddata.obj
build test: run command_initialization_fixture.exe
default command_initialization_fixture.exe
"@
[System.IO.File]::WriteAllText((Join-Path $build 'build.ninja'),
    $content + [Environment]::NewLine, (New-Object System.Text.UTF8Encoding($false)))
Write-Host "Wrote T282 $Architecture original COMMAND initialization graph: $build/build.ninja"
