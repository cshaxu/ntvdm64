[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidateSet('x86')]
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
    $build = Join-Path $root ("build/M0-T310/S15/xms-static/{0}-CPU40" -f $Architecture)
} elseif ([IO.Path]::IsPathRooted($BuildRoot)) {
    $build = Join-Path $BuildRoot ("{0}-CPU40" -f $Architecture)
} else {
    $build = Join-Path (Join-Path $root $BuildRoot) $Architecture
}
New-Item -ItemType Directory -Force $build, (Join-Path $build 'obj') | Out-Null

$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (-not (Test-Path -LiteralPath $vs -PathType Leaf)) {
    throw "Required MSVC environment script is missing: $vs"
}
$environment = Join-Path $build 'msvc-mt.cmd'
@('@echo off', 'set "MVDM_NINJA_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto mvdm_msvc_ready',
    ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'),
    'if errorlevel 1 exit /b %errorlevel%', ':mvdm_msvc_ready',
    'cd /d "%MVDM_NINJA_CALLER_CWD%"', '%*') |
    Set-Content -LiteralPath $environment -Encoding ascii
$environmentNinja = $environment.Replace('\', '/')

# This intentionally selects only OpenNT's six common XMS translation units.
# The i386 xmsmem86.c direct-host-pointer backend is a source witness, not a
# product object. The selected x86 session callback path never uses the
# historical i386 host-pointer condition.
$units = @('xms', 'xmsa20', 'xmsblock', 'xmsdisp', 'xmsmisc', 'xmsumb')
$sources = $units | ForEach-Object { "$root/src/mvdm-host/xms.486/$_.c" }
$adapterSources = @('src/adapter-mvdm-host-out/softpc/mvdm_xms_memory.c')
$adapterSourcePaths = $adapterSources | ForEach-Object { "$root/$_" }
$hashLines = @($sources + $adapterSourcePaths) | ForEach-Object {
    $path = $_.Replace('/', '\')
    $hash = (Get-FileHash -Algorithm SHA256 -LiteralPath $path).Hash.ToLowerInvariant()
    "$($_.Substring($root.Length + 1))`t$hash"
}
[IO.File]::WriteAllLines((Join-Path $build 'source-manifest.tsv'), $hashLines,
    (New-Object System.Text.UTF8Encoding($false)))

$cflags = '/nologo /std:c11 /MT /W4 /showIncludes /DWIN_32 /DMVDM_XMS_SESSION_BACKEND /DDEVL /DCPU_40_STYLE ' +
    '/FI ' + $root + '/src/adapter-mvdm-host-out/win32/include/nt.h ' +
    '/FI ' + $root + '/src/adapter-mvdm-host-out/softpc/include/error_abi.h ' +
    '/FI ' + $root + '/src/adapter-mvdm-host-out/monitor/include/monitor_context.h ' +
    '/I ' + $root + '/src ' +
    '/I ' + $root + '/src/adapter-mvdm-host-out/win32/include ' +
    '/I ' + $root + '/src/adapter-mvdm-host-out/softpc/include ' +
    '/I ' + $root + '/src/adapter-mvdm-host-out/monitor/include ' +
    '/I ' + $root + '/src/opennt-host/public/sdk/inc ' +
    '/I ' + $root + '/src/mvdm-host/xms.486 ' +
    '/I ' + $root + '/src/mvdm-host-overlay/softpc.new/host/src ' +
    '/I ' + $root + '/src/mvdm-host/inc ' +
    '/I ' + $root + '/src/opennt-abi/source/public/sdk/inc ' +
    '/I ' + $root + '/src/opennt-abi/source/public/internal/base/inc ' +
    '/I ' + $root + '/src/opennt-abi/source/public/ddk/inc ' +
    '/I ' + $root + '/src/mvdm-host/softpc.new/host/inc ' +
    '/I ' + $root + '/src/mvdm-host/softpc.new/base/inc '

$buildLines = foreach ($unit in $units) {
    "build obj/$unit.obj: cc `$root/src/mvdm-host/xms.486/$unit.c"
}
$adapterBuildLines = foreach ($source in $adapterSources) {
    $name = [IO.Path]::GetFileNameWithoutExtension($source)
    "build obj/adapter_$name.obj: cc `$root/$source"
}
$objects = (@($units | ForEach-Object { "obj/$_.obj" }) +
    @($adapterSources | ForEach-Object { 'obj/adapter_' +
        [IO.Path]::GetFileNameWithoutExtension($_) + '.obj' })) -join ' '
$content = @"
ninja_required_version = 1.10
root = $root
cflags = $cflags

rule cc
  command = cmd.exe /d /s /c call $environmentNinja cl.exe `$cflags /Fo`$out /c `$in
  deps = msvc
  description = CC `$out
rule lib
  command = cmd.exe /d /s /c call $environmentNinja lib.exe /nologo /out:`$out `$in
  description = LIB `$out

$($buildLines -join "`n")
$($adapterBuildLines -join "`n")
build original-xms-common.lib: lib $objects
default original-xms-common.lib
"@
[IO.File]::WriteAllText((Join-Path $build 'build.ninja'),
    $content + [Environment]::NewLine, (New-Object System.Text.UTF8Encoding($false)))
Write-Host "Wrote T310 S15 $Architecture CCPU40 original-XMS static graph: $build/build.ninja"
