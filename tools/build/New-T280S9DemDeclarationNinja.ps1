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
$build = Join-Path $root ("build/M0-T280/{0}-dem-storage-declarations" -f $Architecture)
New-Item -ItemType Directory -Force -Path $build | Out-Null
$cflags = '/nologo /std:c11 /MT /W4 /DWIN_32 /DDEVL=1 /I ' + $root + '/src /I ' + $root + '/src/adapter-softpc/include /I ' + $root + '/src/adapter-win32/include /I ' + $root + '/src/mvdm-host/dos/dem /I ' + $root + '/src/mvdm-support/inc /I ' + $root + '/src/mvdm-platform-abi/source/public/sdk/inc /I ' + $root + '/src/mvdm-platform-abi/source/public/ddk/inc /I ' + $root + '/src/mvdm-host/softpc.new/base/inc /I ' + $root + '/src/mvdm-host/softpc.new/host/inc'
$content = @"
ninja_required_version = 1.10
root = $root
cflags = $cflags

rule syntax
  command = cl `$cflags /Zs /showIncludes /TC `$in
  deps = msvc
  description = SYNTAX `$in

build dem_storage_declarations: syntax `$root/tests/adapter-win32/t280_s9_dem_declaration_fixture.c
default dem_storage_declarations
"@
[System.IO.File]::WriteAllText((Join-Path $build 'build.ninja'), $content + [Environment]::NewLine, (New-Object System.Text.UTF8Encoding($false)))
Write-Host "Wrote T280 S9 $Architecture declaration Ninja graph: $build/build.ninja"
