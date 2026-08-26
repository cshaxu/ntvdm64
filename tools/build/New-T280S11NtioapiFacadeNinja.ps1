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
$build = Join-Path $root ("build/M0-T280/{0}-ntioapi-facade" -f $Architecture)
New-Item -ItemType Directory -Force -Path $build | Out-Null
$cflags = '/nologo /std:c11 /MT /W4 /DWIN_32 /DDEVL=1 /showIncludes /I ' + $root + '/src/adapter-win32/include /I ' + $root + '/src/opennt-mvdm-host/dos/dem /I ' + $root + '/src/opennt-mvdm-support/inc /I ' + $root + '/src/opennt-platform-abi/source/opennt/public/sdk/inc /I ' + $root + '/src/opennt-platform-abi/source/opennt/public/ddk/inc /I ' + $root + '/src/opennt-mvdm-host/softpc.new/base/inc /I ' + $root + '/src/opennt-mvdm-host/softpc.new/host/inc'
$content = @"
ninja_required_version = 1.10
root = $root
cflags = $cflags

rule cc
  command = cl `$cflags /c /Fo`$out `$in
  deps = msvc
  description = CC `$in
rule link
  command = link /nologo /out:`$out `$in kernel32.lib
  description = LINK `$out

build obj/ntioapi_facade.obj: cc `$root/src/adapter-win32/source/ntioapi_facade.c
build obj/ntioapi_fixture.obj: cc `$root/tests/adapter-win32/t280_s11_ntioapi_facade_fixture.c
build ntioapi_facade_fixture.exe: link obj/ntioapi_facade.obj obj/ntioapi_fixture.obj
default ntioapi_facade_fixture.exe
"@
[System.IO.File]::WriteAllText((Join-Path $build 'build.ninja'), $content + [Environment]::NewLine, (New-Object System.Text.UTF8Encoding($false)))
Write-Host "Wrote T280 S11 $Architecture NT I/O facade Ninja graph: $build/build.ninja"
