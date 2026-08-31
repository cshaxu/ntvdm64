[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidateSet('x86', 'x64')]
    [string]$Architecture,
    [string]$RepositoryRoot = '',
    [string]$BuildRoot = ''
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path.Replace('\', '/')
if ([string]::IsNullOrWhiteSpace($BuildRoot)) {
    $build = Join-Path $root ("build/M0-T322/S2/xms-mapped-memory/{0}" -f $Architecture)
} else {
    $build = Join-Path $BuildRoot $Architecture
}
New-Item -ItemType Directory -Force $build, (Join-Path $build 'obj') | Out-Null

$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (-not (Test-Path -LiteralPath $vs -PathType Leaf)) { throw "MSVC environment script missing: $vs" }
$environment = Join-Path $build 'msvc-mt.cmd'
@('@echo off', 'set "MVDM_NINJA_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready',
    ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'),
    'if errorlevel 1 exit /b %errorlevel%', ':ready',
    'cd /d "%MVDM_NINJA_CALLER_CWD%"', '%*') |
    Set-Content -LiteralPath $environment -Encoding ascii
$environmentNinja = $environment.Replace('\', '/')

$sources = @(
    'tests/adapter-mvdm-host-out/softpc/xms_mapped_memory_fixture.c',
    'src/adapter-mvdm-host-out/softpc/mvdm_xms_memory.c',
    'src/session/mapping_manager.c',
    'src/session/guest_memory_lease.c',
    'src/session/session.c'
)
$cflags = '/nologo /std:c11 /MT /W4 /DWIN_32 /DCPU_40_STYLE ' +
    '/FI ' + $root + '/src/adapter-mvdm-host-out/win32/include/nt.h ' +
    '/I ' + $root + '/src ' +
    '/I ' + $root + '/src/adapter-mvdm-host-out/win32/include ' +
    '/I ' + $root + '/src/adapter-mvdm-host-out/softpc/include ' +
    '/I ' + $root + '/src/opennt-host/public/sdk/inc ' +
    '/I ' + $root + '/src/mvdm-host/xms.486 ' +
    '/I ' + $root + '/src/mvdm-host/inc ' +
    '/I ' + $root + '/src/opennt-abi/source/public/sdk/inc ' +
    '/I ' + $root + '/src/opennt-abi/source/public/internal/base/inc ' +
    '/I ' + $root + '/src/opennt-abi/source/public/ddk/inc ' +
    '/I ' + $root + '/src/mvdm-host/softpc.new/host/inc ' +
    '/I ' + $root + '/src/mvdm-host/softpc.new/base/inc '
$objects = @()
$buildLines = @()
foreach ($source in $sources) {
    $object = 'obj/' + ([IO.Path]::GetFileNameWithoutExtension($source)) + '.obj'
    $objects += $object
    $buildLines += "build ${object}: cc `$root/$source"
}
$content = @"
ninja_required_version = 1.10
root = $root
cflags = $cflags

rule cc
  command = cmd.exe /d /s /c call $environmentNinja cl.exe `$cflags /Fo`$out /c `$in
  deps = msvc
  description = CC `$out
rule link
  command = cmd.exe /d /s /c call $environmentNinja link.exe /nologo /out:`$out `$in
  description = LINK `$out

$($buildLines -join "`n")
build xms-mapped-memory-fixture.exe: link $($objects -join ' ')
default xms-mapped-memory-fixture.exe
"@
[IO.File]::WriteAllText((Join-Path $build 'build.ninja'), $content + [Environment]::NewLine,
    (New-Object System.Text.UTF8Encoding($false)))
Write-Host "Wrote T322 S2 $Architecture mapped-XMS fixture graph: $build/build.ninja"
