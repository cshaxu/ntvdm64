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
$build = Join-Path $root ("build/M0-T279/{0}" -f $Architecture)
New-Item -ItemType Directory -Force $build | Out-Null

$common = '/nologo /std:c11 /MT /W4 /showIncludes /I ' + $root + '/src/adapter-win32/include /I ' + $root + '/src/opennt-mvdm-support/inc /I ' + $root + '/src/opennt-mvdm-support/oemuni'
$unsafeFileDefines = ''
$unsafeProcessDefines = ''
$overlayObject = ''
if ($Architecture -eq 'x64') {
    $unsafeFileDefines = ' /DFindFirstFileOem=opennt_original_FindFirstFileOem /DFindNextFileOem=opennt_original_FindNextFileOem /DGetFullPathNameOem=opennt_original_GetFullPathNameOem'
    $unsafeProcessDefines = ' /DSearchPathOem=opennt_original_SearchPathOem'
    $overlayObject = ' obj/oemuni_pointer_width.obj'
}

$content = @"
ninja_required_version = 1.10
root = $root
cflags = $common
file_defines = $unsafeFileDefines
process_defines = $unsafeProcessDefines
suballoc_defines = $(if ($Architecture -eq 'x86') { '/Di386' } else { '' })

rule cc
  command = cl `$cflags `$defines /Fo`$out /c `$in
  deps = msvc
  description = CC `$out
rule cc_file
  command = cl `$cflags `$file_defines /Fo`$out /c `$in
  deps = msvc
  description = CC `$out
rule cc_process
  command = cl `$cflags `$process_defines /Fo`$out /c `$in
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

build obj/opennt_support_rtl.obj: cc `$root/src/adapter-win32/source/opennt_support_rtl.c
build adapter-win32.lib: lib obj/opennt_support_rtl.obj
build obj/file.obj: cc_file `$root/src/opennt-mvdm-support/oemuni/file.c
build obj/process.obj: cc_process `$root/src/opennt-mvdm-support/oemuni/process.c
build obj/suballoc.obj: cc_suballoc `$root/src/opennt-mvdm-support/suballoc/suballoc.c
"@
$content += "`n"
if ($Architecture -eq 'x64') {
    $content += @"
build obj/oemuni_pointer_width.obj: cc `$root/src/opennt-mvdm-support-overlay/source/oemuni_pointer_width.c
build oemuni.lib: lib obj/file.obj obj/process.obj obj/oemuni_pointer_width.obj
build obj/oemuni_pointer_width_fixture.obj: cc `$root/tests/mvdm-support/oemuni_pointer_width_fixture.c
build oemuni_pointer_width_fixture.exe: link obj/oemuni_pointer_width_fixture.obj oemuni.lib adapter-win32.lib
build test: run oemuni_pointer_width_fixture.exe
"@
} else {
    $content += "`nbuild oemuni.lib: lib obj/file.obj obj/process.obj`n"
}
$content += "`n"
$content += @"
build suballoc.lib: lib obj/suballoc.obj
build all: phony adapter-win32.lib oemuni.lib suballoc.lib$(if ($Architecture -eq 'x64') { ' oemuni_pointer_width_fixture.exe' } else { '' })
default all
"@
$content += "`n"

[System.IO.File]::WriteAllText((Join-Path $build 'build.ninja'), $content,
    (New-Object System.Text.UTF8Encoding($false)))
Write-Host "Wrote T279 $Architecture Ninja graph: $build/build.ninja"
