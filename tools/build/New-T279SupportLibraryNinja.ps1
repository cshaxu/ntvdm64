[CmdletBinding()]
param(
    [ValidateSet('x86')]
    [string]$Architecture,
    [string]$RepositoryRoot = ''
)

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path.Replace('\', '/')
$build = Join-Path $root ("build/M0-T279/{0}" -f $Architecture)
New-Item -ItemType Directory -Force $build | Out-Null

$common = '/nologo /std:c11 /MT /W4 /showIncludes /I ' + $root + '/src/adapter-mvdm-host-out/win32/include /I ' + $root + '/src/mvdm-host/inc /I ' + $root + '/src/mvdm-host/oemuni'

$content = @"
ninja_required_version = 1.10
root = $root
cflags = $common
suballoc_defines = /Di386

rule cc
  command = cl `$cflags `$defines /Fo`$out /c `$in
  deps = msvc
  description = CC `$out
rule cc_suballoc
  command = cl `$cflags `$suballoc_defines /Fo`$out /c `$in
  deps = msvc
  description = CC `$out
rule lib
  command = lib /nologo /out:`$out `$in
  description = LIB `$out
rule link
  command = link /nologo /out:`$out `$in kernel32.lib
  description = LINK `$out
rule run
  command = `$in
  description = RUN `$in

build obj/opennt_support_rtl.obj: cc `$root/src/adapter-mvdm-host-out/win32/source/opennt_support_rtl.c
build adapter-mvdm-host-out-win32.lib: lib obj/opennt_support_rtl.obj
build obj/file.obj: cc `$root/src/mvdm-host/oemuni/file.c
build obj/process.obj: cc `$root/src/mvdm-host/oemuni/process.c
build obj/suballoc.obj: cc_suballoc `$root/src/mvdm-host/suballoc/suballoc.c
"@
$content += "`n"
$content += "`nbuild oemuni.lib: lib obj/file.obj obj/process.obj`n"
$content += "`n"
$content += @"
build suballoc.lib: lib obj/suballoc.obj
build all: phony adapter-mvdm-host-out-win32.lib oemuni.lib suballoc.lib
default all
"@
$content += "`n"

[System.IO.File]::WriteAllText((Join-Path $build 'build.ninja'), $content,
    (New-Object System.Text.UTF8Encoding($false)))
Write-Host "Wrote T279 $Architecture Ninja graph: $build/build.ninja"
