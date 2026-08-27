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
$build = Join-Path $root ("build/M0-T282/S6/{0}" -f $Architecture)
New-Item -ItemType Directory -Force $build | Out-Null
$cflags = '/nologo /std:c11 /MT /W4 /showIncludes /I ' + $root +
    '/src /I ' + $root + '/src/adapter-vdm-monitor/include /I ' + $root +
    '/src/adapter-win32/include /I ' + $root + '/src/session'
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

build obj/vdm_command_fixture.obj: cc `$root/tests/adapter-vdm-monitor/vdm_command_fixture.c
build obj/vdm_command.obj: cc `$root/src/adapter-vdm-monitor/source/vdm_command.c
build obj/command_source.obj: cc `$root/src/app/command_source.c
build obj/session.obj: cc `$root/src/session/session.c
build obj/mapping_manager.obj: cc `$root/src/session/mapping_manager.c
build obj/guest_memory_lease.obj: cc `$root/src/session/guest_memory_lease.c
build vdm_command_fixture.exe: link obj/vdm_command_fixture.obj obj/vdm_command.obj obj/command_source.obj obj/session.obj obj/mapping_manager.obj obj/guest_memory_lease.obj
build test: run vdm_command_fixture.exe
default vdm_command_fixture.exe
"@
[System.IO.File]::WriteAllText((Join-Path $build 'build.ninja'),
    $content + [Environment]::NewLine, (New-Object System.Text.UTF8Encoding($false)))
Write-Host "Wrote T282 $Architecture local command Ninja graph: $build/build.ninja"
