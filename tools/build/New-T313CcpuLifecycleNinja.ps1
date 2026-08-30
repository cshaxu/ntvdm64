[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [ValidateSet('x86')] [string]$Architecture,
    [string]$RepositoryRoot = '',
    [string]$BuildRoot = '',
    [string]$NodeExecutable = ''
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function NinjaPath([string]$Path) {
    $value = $Path.Replace('\\', '/')
    if ($value.Length -ge 2 -and $value[1] -eq ':') { return $value.Substring(0, 1) + '$' + ':' + $value.Substring(2) }
    return $value
}

function Get-OriginalSources([string]$Manifest) {
    $raw = Get-Content -LiteralPath $Manifest -Raw
    $match = [regex]::Match($raw, '(?s)SOURCES\s*=\s*(.*?)(?=\r?\n\s*(?:!INCLUDE|UMTYPE|i386_SOURCES|MIPS_SOURCES|ALPHA_SOURCES))')
    if (!$match.Success) { throw "Cannot isolate original SOURCES block: $Manifest" }
    $block = [regex]::Replace($match.Groups[1].Value, '(?m)^\s*#.*(?:\r?\n|$)', '')
    return @([regex]::Matches($block, '\b([A-Za-z0-9_]+\.c)\b') | ForEach-Object { $_.Groups[1].Value } | Select-Object -Unique)
}

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot) }
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
if ([string]::IsNullOrWhiteSpace($BuildRoot)) { $build = Join-Path $root 'build/M0-T313/S2/ccpu-lifecycle/x86' } else { $build = [IO.Path]::GetFullPath($BuildRoot) }
if ([string]::IsNullOrWhiteSpace($NodeExecutable)) { $NodeExecutable = 'O:\.nvm\versions\node\v22.22.1\bin\node.exe' }
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (!(Test-Path -LiteralPath $vs -PathType Leaf) -or !(Get-Command ninja -ErrorAction SilentlyContinue)) { throw 'MSVC Build Tools and Ninja are required.' }
if (!(Test-Path -LiteralPath $NodeExecutable -PathType Leaf)) { throw "Node 22 is required: $NodeExecutable" }

$ccpuRoot = Join-Path $root 'src/mvdm-host/softpc.new/base/ccpu386'
$hostRoot = Join-Path $root 'src/mvdm-host/softpc.new/host/src'
$patchRoot = Join-Path $root 'src/mvdm-softpc-patch/patches/common'
$gdpOverlayRoot = Join-Path $root 'src/mvdm-host-overlay/softpc.new/base/cvidc'
$gdpGenerator = Join-Path $root 'tools/build/Generate-T310GdpSlots.mjs'
$ccpu = @(Get-OriginalSources (Join-Path $ccpuRoot 'sources') | Where-Object { $_ -ne 'ntstubs.c' })
$hostSources = @('nt_cprgs.c', 'nt_cpu.c', 'sim32.c', 'nt_mem.c')
$controllerSources = @('at_dma.c', 'ica.c')
$supportSources = @('ios.c')
$adapterSources = @('src/adapter-mvdm-host-out/softpc/mvdm_softpc_execution.c', 'src/adapter-mvdm-host-out/softpc/mvdm_softpc_termination.c', 'src/adapter-mvdm-host-out/softpc/mvdm_softpc_effective_address.c', 'src/adapter-mvdm-host-out/softpc/mvdm_softpc_physical_mapping.c', 'src/adapter-mvdm-host-out/softpc/mvdm_a20.c', 'src/session/session.c', 'src/session/mapping_manager.c', 'src/session/guest_memory_lease.c')
$testSources = @('tests/mvdm-host/ccpu_bounded_execution_fixture.c', 'tests/mvdm-host/ccpu_bounded_execution_fixture_seams.c')
$overlaySources = @('mvdm_gdp_state.c')
foreach ($name in $ccpu) { if (!(Test-Path -LiteralPath (Join-Path $ccpuRoot $name))) { throw "Missing original CCPU source: $name" } }
foreach ($name in $hostSources) { if (!(Test-Path -LiteralPath (Join-Path $hostRoot $name))) { throw "Missing original host source: $name" } }
foreach ($name in $controllerSources) { if (!(Test-Path -LiteralPath (Join-Path (Join-Path $root 'src/mvdm-host/softpc.new/base/system') $name))) { throw "Missing original controller source: $name" } }
foreach ($name in $supportSources) { if (!(Test-Path -LiteralPath (Join-Path (Join-Path $root 'src/mvdm-host/softpc.new/base/support') $name))) { throw "Missing original support source: $name" } }
foreach ($name in $adapterSources + $testSources) { if (!(Test-Path -LiteralPath (Join-Path $root $name))) { throw "Missing selected lifecycle source: $name" } }
foreach ($name in $overlaySources) { if (!(Test-Path -LiteralPath (Join-Path $gdpOverlayRoot $name))) { throw "Missing GDP overlay: $name" } }
if (!(Test-Path -LiteralPath (Join-Path $patchRoot 'fmstubs.c')) -or !(Test-Path -LiteralPath $gdpGenerator)) { throw 'Required selected source is missing.' }

New-Item -ItemType Directory -Force $build, (Join-Path $build 'generated/gdp'), (Join-Path $build 'obj/ccpu'), (Join-Path $build 'obj/host'), (Join-Path $build 'obj/controller'), (Join-Path $build 'obj/support'), (Join-Path $build 'obj/adapter'), (Join-Path $build 'obj/test'), (Join-Path $build 'obj/overlay'), (Join-Path $build 'obj/patch') | Out-Null
& $NodeExecutable $gdpGenerator $root (Join-Path $build 'generated/gdp') | Out-Null
$environment = Join-Path $build 'msvc-x86.cmd'
@('@echo off', 'set "MVDM_T313_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready', ('call "' + $vs + '" -arch=x86 -host_arch=x64 >nul'), 'if errorlevel 1 exit /b %errorlevel%', ':ready', 'cd /d "%MVDM_T313_CALLER_CWD%"', '%*') | Set-Content -LiteralPath $environment -Encoding ascii
$includeRoots = @('src', 'src/adapter-mvdm-host-out/win32/include', 'src/opennt-host/public/sdk/inc', 'src/opennt-abi/source/public/sdk/inc', 'src/opennt-abi/source/public/internal/base/inc', 'src/opennt-abi/source/public/internal/windows/inc', 'src/opennt-abi/source/public/ddk/inc', 'src/mvdm-support/inc', 'src/mvdm-softpc-patch/x86/prod', 'src/mvdm-host/softpc.new/base/ccpu386', 'src/mvdm-host/softpc.new/host/inc', 'src/mvdm-host/softpc.new/base/cvidc', 'src/mvdm-host/softpc.new/base/inc', 'src/adapter-mvdm-host-out/softpc/include', 'src/session') | ForEach-Object { Join-Path $root $_ }
$includes = @($includeRoots + @($gdpOverlayRoot, (Join-Path $build 'generated/gdp'))) | ForEach-Object { '/I "' + (NinjaPath $_) + '"' }
$cflags = '/nologo /TC /c /MT /W4 /showIncludes /DWIN32 /DWINNT /DNTVDM /DCPU_40_STYLE /DNEW_CPU /DCCPU /DSPC386 /DSIM32 /DANSI /DPROD /FI "' + (NinjaPath (Join-Path $root 'src/adapter-mvdm-host-out/win32/include/nt.h')) + '" ' + ($includes -join ' ')
$graph = [Collections.Generic.List[string]]::new()
$graph.Add('ninja_required_version = 1.10'); $graph.Add('cflags = ' + $cflags); $graph.Add('')
$graph.Add('rule cc'); $graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' cl.exe $cflags /Fo$out $in'); $graph.Add('  deps = msvc'); $graph.Add('  msvc_deps_prefix = Note: including file:')
$graph.Add('rule cc_patch'); $graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' cl.exe $cflags /DMVDM_SOFTPC_PATCH_EDL_FAST_BOP_ONLY /Fo$out $in'); $graph.Add('  deps = msvc'); $graph.Add('  msvc_deps_prefix = Note: including file:')
$graph.Add('rule lib'); $graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' lib.exe /nologo /out:$out $in')
$graph.Add('rule link'); $graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' link.exe /nologo /out:$out $in kernel32.lib user32.lib advapi32.lib ntdll.lib legacy_stdio_definitions.lib')
function Add-Objects([string]$Group, [string]$Rule, [string]$Directory, [string[]]$Names) { $objects = @(); foreach ($name in $Names) { $object = 'obj/' + $Group + '/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'; $graph.Add('build ' + $object + ': ' + $Rule + ' ' + (NinjaPath (Join-Path $Directory $name))); $objects += $object }; return $objects }
$ccpuObjects = Add-Objects 'ccpu' 'cc' $ccpuRoot $ccpu
$hostObjects = Add-Objects 'host' 'cc' $hostRoot $hostSources
$controllerObjects = Add-Objects 'controller' 'cc' (Join-Path $root 'src/mvdm-host/softpc.new/base/system') $controllerSources
$supportObjects = Add-Objects 'support' 'cc' (Join-Path $root 'src/mvdm-host/softpc.new/base/support') $supportSources
$adapterObjects = Add-Objects 'adapter' 'cc' $root $adapterSources
$testObjects = Add-Objects 'test' 'cc' $root $testSources
$overlayObjects = Add-Objects 'overlay' 'cc' $gdpOverlayRoot $overlaySources
$patchObject = 'obj/patch/fmstubs_edl_fast_bop.obj'; $graph.Add('build ' + $patchObject + ': cc_patch ' + (NinjaPath (Join-Path $patchRoot 'fmstubs.c')))
$graph.Add('build original-ccpu40.lib: lib ' + ($ccpuObjects -join ' '))
$graph.Add('build original-host-lifecycle.lib: lib ' + ($hostObjects -join ' '))
$graph.Add('build lifecycle-adapter.lib: lib ' + ($adapterObjects -join ' '))
$graph.Add('build ccpu-lifecycle.exe: link ' + (($testObjects + $overlayObjects + $controllerObjects + $supportObjects + @($patchObject, 'original-ccpu40.lib', 'original-host-lifecycle.lib', 'lifecycle-adapter.lib')) -join ' '))
$graph.Add('build ccpu-lifecycle: phony ccpu-lifecycle.exe'); $graph.Add('default ccpu-lifecycle')
[IO.File]::WriteAllText((Join-Path $build 'build.ninja'), (($graph -join [Environment]::NewLine) + [Environment]::NewLine), [Text.UTF8Encoding]::new($false))
[ordered]@{ schema='m0.t313.s3.ccpu40-controller-lifecycle.v1'; architecture='x86'; cpuProfile='CCPU40'; executor='original softpc.new/base/ccpu386 c_cpu_simulate'; selectedCcpu=$ccpu; hostSources=$hostSources; controllerSources=$controllerSources; supportSources=$supportSources; adapterSources=$adapterSources; testSources=$testSources; forbiddenInputs=@('src.old','bochs-core','adapter-bochs','CPU30','MONITOR','V86') } | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $build 'source-manifest.json') -Encoding utf8
Write-Host "Generated T313 S2 CCPU40 lifecycle graph: $build"
