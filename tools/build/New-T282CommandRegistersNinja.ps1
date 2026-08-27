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
$build = Join-Path $root ("build/M0-T282/S10/{0}" -f $Architecture)
New-Item -ItemType Directory -Force $build | Out-Null
$cflags = '/nologo /std:c11 /MT /W4 /showIncludes /I ' + $root +
    '/src /I ' + $root + '/src/adapter-softpc/include'
$content = @"
ninja_required_version = 1.10
root = $root
cflags = $cflags

rule cc
  command = cl `$cflags /Fo`$out /c `$in
  deps = msvc
  description = CC `$out
rule link
  command = link /nologo /out:`$out `$in
  description = LINK `$out
rule run
  command = `$in
  description = RUN `$in

build obj/fixture.obj: cc `$root/tests/adapter-softpc/t282_s10_command_registers_fixture.c
build obj/registers.obj: cc `$root/src/adapter-softpc/mvdm_command_registers.c
build command_registers_fixture.exe: link obj/fixture.obj obj/registers.obj
build test: run command_registers_fixture.exe
default command_registers_fixture.exe
"@
[System.IO.File]::WriteAllText((Join-Path $build 'build.ninja'),
    $content + [Environment]::NewLine, (New-Object System.Text.UTF8Encoding($false)))
Write-Host "Wrote T282 $Architecture COMMAND register Ninja graph: $build/build.ninja"
