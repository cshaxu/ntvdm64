[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [ValidateSet('x86', 'x64')] [string]$Architecture,
    [string]$RepositoryRoot = ''
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function NinjaPath([string]$Path) {
    $value = $Path.Replace('\', '/')
    if ($value.Length -ge 2 -and $value[1] -eq ':') {
        return $value.Substring(0, 1) + '$' + ':' + $value.Substring(2)
    }
    return $value
}

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = Join-Path $root ("build/M0-T309/S1/dispatchers/{0}" -f $Architecture)
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (!(Test-Path -LiteralPath $vs -PathType Leaf) -or !(Get-Command ninja -ErrorAction SilentlyContinue)) {
    throw 'MSVC Build Tools and Ninja are required.'
}

# This graph proves current-source object closure only.  The original tables
# deliberately retain unresolved provider references until their complete
# owner packages are composed; linking a hand-picked provider subset would be
# a second dispatcher rather than source recovery.
$inputs = @(
    'src/mvdm-host/dos/dem/demdisp.c',
    'src/mvdm-host/dos/command/cmddisp.c'
)
foreach ($input in $inputs) {
    if (!(Test-Path -LiteralPath (Join-Path $root $input)) -or $input -match '(^|/)src\.old(/|$)') {
        throw "Invalid dispatcher input: $input"
    }
}
New-Item -ItemType Directory -Force $build, (Join-Path $build 'obj') | Out-Null
$environment = Join-Path $build 'msvc-mt.cmd'
@('@echo off', 'set "MVDM_T309_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready',
  ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'),
  'if errorlevel 1 exit /b %errorlevel%', ':ready', 'cd /d "%MVDM_T309_CALLER_CWD%"', '%*') |
    Set-Content -LiteralPath $environment -Encoding ascii

$includes = @(
    'src',
    'src/adapter-mvdm-host-out/win32/include',
    'src/adapter-mvdm-host-out/softpc/include',
    'src/adapter-mvdm-host-out/monitor/include',
    'src/mvdm-support/inc',
    'src/opennt-abi/source/public/sdk/inc',
    'src/opennt-abi/source/public/internal/base/inc',
    'src/opennt-abi/source/public/internal/windows/inc',
    'src/opennt-abi/source/public/ddk/inc',
    'src/mvdm-host/dos/dem',
    'src/mvdm-host/dos/command',
    'src/mvdm-host/softpc.new/host/inc',
    'src/mvdm-host/softpc.new/base/inc'
) | ForEach-Object { '/I "' + (NinjaPath (Join-Path $root $_)) + '"' }
$flags = '/nologo /TC /c /std:c11 /MT /W4 /showIncludes /DWIN_32 /DDEVL /Di386 /DNTVDM ' +
    '/FI "' + (NinjaPath (Join-Path $root 'src/adapter-mvdm-host-out/win32/include/nt.h')) + '" ' +
    '/FI "' + (NinjaPath (Join-Path $root 'src/adapter-mvdm-host-out/softpc/include/error_abi.h')) + '" ' +
    ($includes -join ' ')

$graph = @"
ninja_required_version = 1.10
build_root = $(NinjaPath $build)
cflags = $flags

rule cc
  command = cmd.exe /d /s /c call $(NinjaPath $environment) cl.exe `$cflags /Fo`$out `$in
  deps = msvc
  msvc_deps_prefix = Note: including file:

build obj/demdisp.obj: cc $(NinjaPath (Join-Path $root 'src/mvdm-host/dos/dem/demdisp.c'))
build obj/cmddisp.obj: cc $(NinjaPath (Join-Path $root 'src/mvdm-host/dos/command/cmddisp.c'))
build dispatchers: phony obj/demdisp.obj obj/cmddisp.obj
default dispatchers
"@
[IO.File]::WriteAllText((Join-Path $build 'build.ninja'), $graph + [Environment]::NewLine,
    [Text.UTF8Encoding]::new($false))
Write-Host "Generated T309 S1 original-dispatcher object graph: $build"
