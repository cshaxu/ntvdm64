[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidateSet('x86', 'x64')]
    [string]$Architecture,
    [string]$RepositoryRoot = ''
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
function NinjaPath([string]$Path) {
    $result = $Path.Replace('\', '/')
    if ($result.Length -ge 2 -and $result[1] -eq ':') { return $result.Substring(0, 1) + '$' + ':' + $result.Substring(2) }
    return $result
}
function ObjectName([string]$Path) { return (($Path -replace '[^A-Za-z0-9_]', '_') + '.obj') }
function Sha256([string]$Path) { return (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant() }
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot) }
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = Join-Path $root ("build/M0-T289/S6/vdd-misc-{0}" -f $Architecture)
$bochs = Join-Path $root ("build/M0-T289/S5/realmode-frame-{0}/lib/bochs-core.lib" -f $Architecture)
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (!(Test-Path -LiteralPath $bochs -PathType Leaf)) { throw "Missing S5 prerequisite: $bochs" }
if (!(Test-Path -LiteralPath $vs -PathType Leaf)) { throw "Missing MSVC: $vs" }
if (!(Get-Command ninja -ErrorAction SilentlyContinue)) { throw 'Ninja is required.' }
$cSources = @('src/adapter-mvdm-host-out/softpc/mvdm_command_registers.c',
    'src/adapter-mvdm-host-out/softpc/mvdm_protected_frame_transaction.c',
    'src/mvdm-host/dpmi32/i386/dpmimisc.c', 'src/mvdm-host/dpmi32/vxd.c')
$cxxSources = @('src/adapter-bochs/headless_8042.cc',
    'src/adapter-bochs/machine_facade.cc', 'src/adapter-bochs/minimal_machine.cc',
    'src/adapter-bochs/minimal_pic.cc', 'src/adapter-bochs/minimal_sim.cc')
$fixture = 'tests/adapter-bochs/t289_s6_dpmi_vdd_misc_fixture.cc'
foreach ($path in $cSources + $cxxSources + $fixture) {
    if (!(Test-Path -LiteralPath (Join-Path $root $path) -PathType Leaf) -or $path -match '(^|/)src\.old(/|$)') { throw "Invalid S6 input: $path" }
}
New-Item -ItemType Directory -Force $build, (Join-Path $build 'obj'), (Join-Path $build 'lib'), (Join-Path $build 'bin') | Out-Null
$environment = Join-Path $build 'msvc-mt.cmd'
@('@echo off', 'set "MVDM_T289_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready',
    ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'), 'if errorlevel 1 exit /b %errorlevel%', ':ready', 'cd /d "%MVDM_T289_CALLER_CWD%"', '%*') |
    Set-Content -LiteralPath $environment -Encoding ascii
$manifest = [ordered]@{ schema = 'm0.t289.s6.dpmi-vdd-misc.v1'; architecture = $Architecture; runtimeLibrary = '/MT';
    sources = @($cSources + $cxxSources + $fixture | ForEach-Object { [ordered]@{ path = $_; sha256 = Sha256 (Join-Path $root $_) } });
    prerequisites = @([ordered]@{ path = $bochs.Substring($root.Length + 1).Replace('\', '/'); sha256 = Sha256 $bochs });
    forbiddenInputs = @('src.old', 'BOP ingress', 'host LDT implementation', 'VDD broker', 'prebuilt product archive') }
$manifest | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $build 'source-manifest.json') -Encoding utf8
$cIncludes = @('src', 'src/adapter-mvdm-host-out/softpc/include', 'src/adapter-mvdm-host-out/win32/include',
    'src/adapter-mvdm-host-out/monitor/include', 'src/mvdm-host/dpmi32', 'src/mvdm-host-overlay/dpmi32',
    'src/mvdm-support/inc', 'src/mvdm-platform-abi/source/public/sdk/inc',
    'src/mvdm-platform-abi/source/public/internal/base/inc', 'src/mvdm-platform-abi/source/public/ddk/inc',
    'src/mvdm-host/softpc.new/host/inc', 'src/mvdm-host/softpc.new/base/inc') | ForEach-Object { '/I "' + (NinjaPath (Join-Path $root $_)) + '"' }
$cxxIncludes = @('src', 'src/bochs-core', 'src/bochs-core/cpu', 'src/bochs-core/iodev',
    'src/bochs-core/instrument/stubs', 'src/adapter-bochs', 'src/adapter-mvdm-host-in',
    'src/adapter-mvdm-host-out/softpc/include') | ForEach-Object { '/I "' + (NinjaPath (Join-Path $root $_)) + '"' }
$cflags = '/nologo /TC /c /std:c11 /MT /W4 /showIncludes /DWIN_32 /Di386 /DDEVL ' +
    '/FI "' + (NinjaPath (Join-Path $root 'src/adapter-mvdm-host-out/win32/include/nt.h')) + '" ' +
    '/FI "' + (NinjaPath (Join-Path $root 'src/adapter-mvdm-host-out/softpc/include/error_abi.h')) + '" ' +
    '/FI "' + (NinjaPath (Join-Path $root 'src/adapter-mvdm-host-out/monitor/include/vdm.h')) + '" ' + ($cIncludes -join ' ')
$cxxflags = '/nologo /TP /c /std:c++14 /EHsc /MT /W4 /showIncludes /DWIN32 /DRUNTIME_ENABLE_MACHINE_UD_BRIDGE=1 ' + ($cxxIncludes -join ' ')
$graph = [Collections.Generic.List[string]]::new()
$graph.Add('ninja_required_version = 1.10'); $graph.Add('build_root = ' + (NinjaPath $build)); $graph.Add('cflags = ' + $cflags); $graph.Add('cxxflags = ' + $cxxflags); $graph.Add('')
$graph.Add('rule cc'); $graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' cl.exe $cflags /Fo$out $in'); $graph.Add('  deps = msvc'); $graph.Add('  msvc_deps_prefix = Note: including file:')
$graph.Add('rule cxx'); $graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' cl.exe $cxxflags /Fo$out $in'); $graph.Add('  deps = msvc'); $graph.Add('  msvc_deps_prefix = Note: including file:')
$graph.Add('rule lib'); $graph.Add('  command = cmd.exe /d /s /c cd /d $build_root && call ' + (NinjaPath $environment) + ' lib.exe /nologo /OUT:$out @$out.rsp')
$graph.Add('rule link'); $graph.Add('  command = cmd.exe /d /s /c cd /d $build_root && call ' + (NinjaPath $environment) + ' link.exe /nologo /OUT:$out @$out.rsp')
$objects = [Collections.Generic.List[string]]::new()
foreach ($source in $cSources) { $object = 'obj/' + (ObjectName $source); $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $root $source))); $objects.Add($object) }
$library = 'lib/original-dpmi-s6-vdd-misc.lib'; [IO.File]::WriteAllLines((Join-Path $build ($library + '.rsp')), $objects, [Text.UTF8Encoding]::new($false)); $graph.Add('build ' + $library + ': lib ' + ($objects -join ' '))
$fixtureObject = 'obj/' + (ObjectName $fixture); $graph.Add('build ' + $fixtureObject + ': cxx ' + (NinjaPath (Join-Path $root $fixture)))
$bridgeObjects = [Collections.Generic.List[string]]::new()
foreach ($source in $cxxSources) { $object = 'obj/' + (ObjectName $source); $graph.Add('build ' + $object + ': cxx ' + (NinjaPath (Join-Path $root $source))); $bridgeObjects.Add($object) }
$output = 'bin/dpmi-vdd-misc-fixture.exe'
[IO.File]::WriteAllLines((Join-Path $build ($output + '.rsp')), @($fixtureObject, $library) + @($bridgeObjects) + @($bochs, 'kernel32.lib', 'user32.lib', 'advapi32.lib', 'ntdll.lib'), [Text.UTF8Encoding]::new($false))
$graph.Add('build ' + $output + ': link ' + $fixtureObject + ' ' + $library + ' ' + ($bridgeObjects -join ' ') + ' ' + (NinjaPath $bochs)); $graph.Add('default ' + $output)
[IO.File]::WriteAllLines((Join-Path $build 'build.ninja'), $graph, [Text.UTF8Encoding]::new($false))
Write-Host "Generated T289 S6 original DPMI VDD/misc graph: $build"
