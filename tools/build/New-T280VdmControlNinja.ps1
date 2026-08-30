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
$build = Join-Path $root ("build/M0-T280/{0}-vdm-control" -f $Architecture)
New-Item -ItemType Directory -Force $build | Out-Null
# The monitor declaration carrier retains original OpenNT public subsets
# (`ntpsapi.h`, `ntexapi.h`) under opennt-host.  Keep this focused fixture on
# the same include boundary as the formal SoftPC graph rather than relying on
# an old aggregate tree.
$cflags = '/nologo /std:c11 /MT /W4 /showIncludes /I ' + $root + '/src/adapter-mvdm-host-out/monitor/include /I ' + $root + '/src/adapter-mvdm-host-out/win32/include /I ' + $root + '/src/opennt-host/public/sdk/inc /I ' + $root + '/src/opennt-abi/source/public/sdk/inc /I ' + $root + '/src/opennt-abi/source/public/internal/base/inc /I ' + $root + '/src/session'
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

build obj/vdm_control_fixture.obj: cc `$root/tests/adapter-mvdm-host-out/monitor/vdm_control_fixture.c
build obj/vdm_control.obj: cc `$root/src/adapter-mvdm-host-out/monitor/source/vdm_control.c
build obj/session.obj: cc `$root/src/session/session.c
build obj/mapping_manager.obj: cc `$root/src/session/mapping_manager.c
build obj/guest_memory_lease.obj: cc `$root/src/session/guest_memory_lease.c
build vdm_control_fixture.exe: link obj/vdm_control_fixture.obj obj/vdm_control.obj obj/session.obj obj/mapping_manager.obj obj/guest_memory_lease.obj
build test: run vdm_control_fixture.exe
default vdm_control_fixture.exe
"@
[System.IO.File]::WriteAllText((Join-Path $build 'build.ninja'), $content + [Environment]::NewLine,
    (New-Object System.Text.UTF8Encoding($false)))
Write-Host "Wrote T280 $Architecture VDM control Ninja graph: $build/build.ninja"
