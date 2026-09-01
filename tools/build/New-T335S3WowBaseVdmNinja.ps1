[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [ValidateSet('x86', 'x64')] [string]$Architecture,
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
    $build = Join-Path $root ("build/M0-T335/S3/wow-base-vdm-{0}" -f $Architecture)
} else {
    $build = [IO.Path]::GetFullPath($BuildRoot)
}
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (!(Test-Path -LiteralPath $vs -PathType Leaf) -or !(Get-Command ninja -ErrorAction SilentlyContinue)) {
    throw 'MSVC Build Tools and Ninja are required.'
}
New-Item -ItemType Directory -Force -Path $build | Out-Null
$environment = Join-Path $build ("msvc-{0}.cmd" -f $Architecture)
@('@echo off', 'set "MVDM_T335_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready', ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'), 'if errorlevel 1 exit /b %errorlevel%', ':ready', 'cd /d "%MVDM_T335_CALLER_CWD%"', '%*') | Set-Content -LiteralPath $environment -Encoding ascii
$abi = $root + '/src/opennt-abi/source/public/internal/base/inc'
$hostSdk = $root + '/src/opennt-host/public/sdk/inc'
$cflags = '/nologo /std:c11 /MT /W4 /showIncludes /I ' + $root + '/src /I ' + $root + '/src/session /I ' + $abi + ' /I ' + $root + '/src/adapter-mvdm-host-out/basesrv/include /I ' + $root + '/src/adapter-mvdm-host-out/win32/include /I ' + $hostSdk
$lines = @(
    'ninja_required_version = 1.10',
    ('root = ' + $root),
    ('cflags = ' + $cflags),
    ('environment = ' + $environment.Replace('\', '/')),
    'rule cc',
    '  command = cmd /c "$environment cl $cflags /Fo$out /c $in"',
    '  deps = msvc',
    '  description = CC $in',
    'rule link',
    '  command = cmd /c "$environment link /nologo /out:$out $in kernel32.lib"',
    '  description = LINK $out',
    'rule run',
    '  command = $in',
    '  description = RUN $in',
    'build obj/fixture.obj: cc $root/tests/adapter-mvdm-host-out/basesrv/base_vdm_local_fixture.c',
    'build obj/base_vdm_client.obj: cc $root/src/adapter-mvdm-host-out/basesrv/source/base_vdm_client.c',
    'build obj/base_vdm_local.obj: cc $root/src/adapter-mvdm-host-out/basesrv/source/base_vdm_local.c',
    'build obj/launch_declaration.obj: cc $root/src/app/launch_declaration.c',
    'build obj/thread_start_compat.obj: cc $root/src/adapter-mvdm-host-out/win32/source/thread_start_compat.c',
    'build obj/session.obj: cc $root/src/session/session.c',
    'build obj/mapping_manager.obj: cc $root/src/session/mapping_manager.c',
    'build obj/guest_memory_lease.obj: cc $root/src/session/guest_memory_lease.c',
    'build bin/t335-s3-wow-base-vdm-fixture.exe: link obj/fixture.obj obj/base_vdm_client.obj obj/base_vdm_local.obj obj/launch_declaration.obj obj/thread_start_compat.obj obj/session.obj obj/mapping_manager.obj obj/guest_memory_lease.obj',
    'build test: run bin/t335-s3-wow-base-vdm-fixture.exe',
    'default bin/t335-s3-wow-base-vdm-fixture.exe'
)
[System.IO.File]::WriteAllText((Join-Path $build 'build.ninja'), ($lines -join [Environment]::NewLine) + [Environment]::NewLine, (New-Object System.Text.UTF8Encoding($false)))
Write-Host "Generated T335 S3 $Architecture WOW/BaseVDM graph: $build"
