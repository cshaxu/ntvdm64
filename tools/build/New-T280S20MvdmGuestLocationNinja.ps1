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
$build = Join-Path $root ("build/M0-T280/{0}-mvdm-guest-location" -f $Architecture)
New-Item -ItemType Directory -Force -Path $build | Out-Null
$cflags = '/nologo /std:c11 /MT /W4 /showIncludes /I ' + $root + '/src /I ' + $root + '/src/session /I ' + $root + '/src/adapter-softpc/include'
$content = @"
ninja_required_version = 1.10
root = $root
cflags = $cflags

rule cc
  command = cl `$cflags /c /Fo`$out `$in
  deps = msvc
  description = CC `$in
rule link
  command = link /nologo /out:`$out `$in
  description = LINK `$out

build obj/mapping_manager.obj: cc `$root/src/session/mapping_manager.c
build obj/guest_memory_lease.obj: cc `$root/src/session/guest_memory_lease.c
build obj/session.obj: cc `$root/src/session/session.c
build obj/mvdm_guest_location.obj: cc `$root/src/adapter-softpc/mvdm_guest_location.c
build obj/fixture.obj: cc `$root/tests/adapter-softpc/t280_s20_mvdm_guest_location_fixture.c
build mvdm_guest_location_fixture.exe: link obj/mapping_manager.obj obj/guest_memory_lease.obj obj/session.obj obj/mvdm_guest_location.obj obj/fixture.obj
default mvdm_guest_location_fixture.exe
"@
[System.IO.File]::WriteAllText((Join-Path $build 'build.ninja'), $content + [Environment]::NewLine, (New-Object System.Text.UTF8Encoding($false)))
Write-Host "Wrote T280 S20 $Architecture MVDM guest-location Ninja graph: $build/build.ninja"
