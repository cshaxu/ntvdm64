[CmdletBinding()]
param([ValidateSet('x86', 'x64')][string]$Architecture,[string]$RepositoryRoot = '')
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot) }
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path.Replace('\', '/')
$build = Join-Path $root ("build/M0-T282/S12/{0}" -f $Architecture)
New-Item -ItemType Directory -Force $build | Out-Null
$cflags = '/nologo /std:c11 /MT /W4 /showIncludes /I ' + $root + '/src /I ' + $root + '/src/session /I ' + $root + '/src/adapter-softpc/include'
$content = @"
ninja_required_version = 1.10
root = $root
cflags = $cflags
rule cc
  command = cl `$cflags /Fo`$out /c `$in
  deps = msvc
rule link
  command = link /nologo /out:`$out `$in
rule run
  command = `$in
build obj/mapping_manager.obj: cc `$root/src/session/mapping_manager.c
build obj/guest_memory_lease.obj: cc `$root/src/session/guest_memory_lease.c
build obj/session.obj: cc `$root/src/session/session.c
build obj/location.obj: cc `$root/src/adapter-softpc/mvdm_guest_location.c
build obj/fixture.obj: cc `$root/tests/adapter-softpc/t280_s20_mvdm_guest_location_fixture.c
build bounded_copy_fixture.exe: link obj/mapping_manager.obj obj/guest_memory_lease.obj obj/session.obj obj/location.obj obj/fixture.obj
build test: run bounded_copy_fixture.exe
default bounded_copy_fixture.exe
"@
[System.IO.File]::WriteAllText((Join-Path $build 'build.ninja'), $content + [Environment]::NewLine, (New-Object System.Text.UTF8Encoding($false)))
