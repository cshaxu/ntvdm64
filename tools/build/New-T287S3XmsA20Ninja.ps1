[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidateSet('x86', 'x64')]
    [string]$Architecture,
    [string]$RepositoryRoot = ''
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
function NinjaPath([string]$Path) {
    $result = $Path.Replace('\', '/')
    if ($result.Length -ge 2 -and $result[1] -eq ':') {
        return $result.Substring(0, 1) + '$' + ':' + $result.Substring(2)
    }
    return $result
}
function ObjectName([string]$Path) { return (($Path -replace '[^A-Za-z0-9_]', '_') + '.obj') }

$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = Join-Path $root ("build/M0-T287/S3/{0}" -f $Architecture)
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (!(Test-Path -LiteralPath $vs -PathType Leaf)) { throw "Missing MSVC: $vs" }
$core = Join-Path $root 'src\bochs-core'
$overlay = Join-Path $root 'src\bochs-core-overlay'
$fixture = 'tests/adapter-bochs/t287_s3_xms_a20_lease_fixture.cc'
$coreSources = @(Get-ChildItem -LiteralPath $core -Recurse -File |
    Where-Object { $_.Extension -in @('.c', '.cc') } |
    ForEach-Object { $_.FullName.Substring($root.Length + 1).Replace('\', '/') } | Sort-Object)
$overlaySources = @(Get-ChildItem -LiteralPath $overlay -Recurse -File |
    Where-Object { $_.Extension -in @('.c', '.cc') } |
    ForEach-Object { $_.FullName.Substring($root.Length + 1).Replace('\', '/') } | Sort-Object)
$bochsSources = @($coreSources + $overlaySources)
$bochsAdapter = @('src/adapter-bochs/headless_8042.cc',
    'src/adapter-bochs/machine_facade.cc', 'src/adapter-bochs/minimal_machine.cc',
    'src/adapter-bochs/minimal_pic.cc', 'src/adapter-bochs/minimal_sim.cc')
$cSources = @('src/session/mapping_manager.c', 'src/session/guest_memory_lease.c',
    'src/session/session.c', 'src/adapter-mvdm-host-out/softpc/mvdm_guest_location.c',
    'src/adapter-mvdm-host-out/softpc/mvdm_command_registers.c',
    'src/adapter-mvdm-host-out/softpc/mvdm_a20.c',
    'src/mvdm-host-overlay/xms.486/xms_a20_state.c',
    'src/mvdm-host/xms.486/xmsa20.c', 'src/mvdm-host/xms.486/xmsumb.c')
foreach ($item in @($fixture) + $bochsSources + $bochsAdapter + $cSources) {
    if (!(Test-Path -LiteralPath (Join-Path $root $item) -PathType Leaf) -or
        $item -match '(^|/)src\.old(/|$)') { throw "Invalid S3 input: $item" }
}
New-Item -ItemType Directory -Force $build, (Join-Path $build 'obj'),
    (Join-Path $build 'lib'), (Join-Path $build 'bin') | Out-Null
$env = Join-Path $build 'msvc-mt.cmd'
@('@echo off', 'set "MVDM_T287_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready',
    ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'),
    'if errorlevel 1 exit /b %errorlevel%', ':ready', 'cd /d "%MVDM_T287_CALLER_CWD%"', '%*') |
    Set-Content -LiteralPath $env -Encoding ascii

$rootNinja = NinjaPath $root
$buildNinja = NinjaPath $build
$envNinja = NinjaPath $env
$cxxIncludes = @('src', 'src/bochs-core', 'src/bochs-core/cpu', 'src/bochs-core/iodev',
    'src/bochs-core/instrument/stubs', 'src/adapter-bochs') |
    ForEach-Object { '/I "' + (NinjaPath (Join-Path $root $_)) + '"' }
$cIncludes = @('src', 'src/adapter-mvdm-host-out/softpc/include',
    'src/adapter-mvdm-host-out/win32/include', 'src/adapter-mvdm-host-out/monitor/include',
    'src/mvdm-host/xms.486', 'src/mvdm-host-overlay/xms.486', 'src/mvdm-support/inc',
    'src/mvdm-platform-abi/source/public/sdk/inc',
    'src/mvdm-platform-abi/source/public/internal/base/inc',
    'src/mvdm-platform-abi/source/public/ddk/inc',
    'src/mvdm-host/softpc.new/host/inc', 'src/mvdm-host/softpc.new/base/inc') |
    ForEach-Object { '/I "' + (NinjaPath (Join-Path $root $_)) + '"' }
$cxxFlags = '/nologo /TP /c /std:c++14 /EHsc /MT /W4 /showIncludes /DWIN32 ' + ($cxxIncludes -join ' ')
$cFlags = '/nologo /TC /c /std:c11 /MT /W4 /showIncludes /DWIN_32 /Di386 /DDEVL ' +
    '/FI "' + (NinjaPath (Join-Path $root 'src/adapter-mvdm-host-out/win32/include/nt.h')) + '" ' +
    '/FI "' + (NinjaPath (Join-Path $root 'src/adapter-mvdm-host-out/softpc/include/error_abi.h')) + '" ' +
    '/FI "' + (NinjaPath (Join-Path $root 'src/adapter-mvdm-host-out/monitor/include/vdm.h')) + '" ' +
    ($cIncludes -join ' ')
$lines = [Collections.Generic.List[string]]::new()
$lines.Add('ninja_required_version = 1.10'); $lines.Add('build_root = ' + $buildNinja)
$lines.Add('cxxflags = ' + $cxxFlags); $lines.Add('cflags = ' + $cFlags); $lines.Add('')
$lines.Add('rule cxx'); $lines.Add('  command = cmd.exe /d /s /c call ' + $envNinja + ' cl.exe $cxxflags /Fo$out $in'); $lines.Add('  deps = msvc'); $lines.Add('  msvc_deps_prefix = Note: including file:')
$lines.Add('rule cc'); $lines.Add('  command = cmd.exe /d /s /c call ' + $envNinja + ' cl.exe $cflags /Fo$out $in'); $lines.Add('  deps = msvc'); $lines.Add('  msvc_deps_prefix = Note: including file:')
$lines.Add('rule lib'); $lines.Add('  command = cmd.exe /d /s /c cd /d $build_root && call ' + $envNinja + ' lib.exe /nologo /OUT:$out @$out.rsp')
$lines.Add('rule link'); $lines.Add('  command = cmd.exe /d /s /c cd /d $build_root && call ' + $envNinja + ' link.exe /nologo /OUT:$out @$out.rsp')
function Add-Library([string]$name, [string[]]$sources, [string]$rule) {
    $objects = [Collections.Generic.List[string]]::new()
    foreach ($source in $sources) {
        $object = 'obj/' + $name + '/' + (ObjectName $source)
        $lines.Add('build ' + $object + ': ' + $rule + ' ' + (NinjaPath (Join-Path $root $source)))
        $objects.Add($object)
    }
    $library = 'lib/' + $name + '.lib'
    [IO.File]::WriteAllLines((Join-Path $build ($library + '.rsp')),
        $objects, [Text.UTF8Encoding]::new($false))
    $lines.Add('build ' + $library + ': lib ' + ($objects -join ' '))
    return $library
}
$bochsLibrary = Add-Library 'bochs-core' @($bochsSources) 'cxx'
$bochsAdapterLibrary = Add-Library 'adapter-bochs' @($bochsAdapter) 'cxx'
$xmsLibrary = Add-Library 'xms-a20-binding' @($cSources) 'cc'
$fixtureObject = 'obj/fixture/' + (ObjectName $fixture); $lines.Add('build ' + $fixtureObject + ': cxx ' + (NinjaPath (Join-Path $root $fixture)))
$output = 'bin/t287-s3-xms-a20-lease-fixture.exe'
[IO.File]::WriteAllLines((Join-Path $build ($output + '.rsp')),
    @($fixtureObject, $xmsLibrary, $bochsAdapterLibrary, $bochsLibrary,
      'kernel32.lib', 'user32.lib'), [Text.UTF8Encoding]::new($false))
$lines.Add('build ' + $output + ': link ' + $fixtureObject + ' ' + $xmsLibrary + ' ' + $bochsAdapterLibrary + ' ' + $bochsLibrary); $lines.Add('default ' + $output)
[IO.File]::WriteAllLines((Join-Path $build 'build.ninja'), $lines, [Text.UTF8Encoding]::new($false))
Write-Host "Generated T287 S3 $Architecture XMS A20 graph: $build"
