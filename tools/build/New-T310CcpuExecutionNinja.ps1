[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [ValidateSet('x86', 'x64')] [string]$Architecture,
    [string]$RepositoryRoot = '',
    [string]$BuildRoot = ''
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function NinjaPath([string]$Path) {
    $value = $Path.Replace('\', '/')
    if ($value.Length -ge 2 -and $value[1] -eq ':') { return $value.Substring(0, 1) + '$' + ':' + $value.Substring(2) }
    return $value
}
function GetOriginalSources([string]$Manifest) {
    $raw = Get-Content -LiteralPath $Manifest -Raw
    $match = [regex]::Match($raw, '(?s)SOURCES\s*=\s*(.*?)(?=\r?\n\s*(?:!INCLUDE|UMTYPE|i386_SOURCES|MIPS_SOURCES|ALPHA_SOURCES))')
    if (!$match.Success) { throw "Cannot isolate original SOURCES block: $Manifest" }
    return @([regex]::Matches($match.Groups[1].Value, '\b([A-Za-z0-9_]+\.c)\b') | ForEach-Object { $_.Groups[1].Value } | Select-Object -Unique)
}

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot) }
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
if ([string]::IsNullOrWhiteSpace($BuildRoot)) {
    $build = Join-Path $root ("build/M0-T310/S5/ccpu/$Architecture")
} else {
    $build = [IO.Path]::GetFullPath($BuildRoot)
}
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (!(Test-Path -LiteralPath $vs -PathType Leaf) -or !(Get-Command ninja -ErrorAction SilentlyContinue)) { throw 'MSVC Build Tools and Ninja are required.' }

$ccpuRoot = Join-Path $root 'src/mvdm-host/softpc.new/base/ccpu386'
$hostRoot = Join-Path $root 'src/mvdm-host/softpc.new/host/src'
$manifest = Join-Path $ccpuRoot 'sources'
$allCcpu = GetOriginalSources $manifest
# The original manifest carries both the real FPU implementation and the
# alternate no-FPU ntstubs.c carrier.  A normal static link chooses one by
# resolution order; this focused product link explicitly selects real fpu.c.
$ccpu = @($allCcpu | Where-Object { $_ -ne 'ntstubs.c' })
$hostSources = @('nt_cprgs.c', 'nt_cpu.c', 'sim32.c', 'nt_mem.c')
$adapterSources = @('src/adapter-mvdm-host-out/softpc/mvdm_softpc_execution.c', 'src/adapter-mvdm-host-out/softpc/mvdm_softpc_physical_mapping.c', 'src/session/session.c', 'src/session/mapping_manager.c', 'src/session/guest_memory_lease.c')
$test = @('tests/mvdm-host/ccpu_bounded_execution_fixture.c', 'tests/mvdm-host/ccpu_bounded_execution_fixture_seams.c')
foreach ($name in $ccpu) { if (!(Test-Path -LiteralPath (Join-Path $ccpuRoot $name))) { throw "Missing original CCPU source: $name" } }
foreach ($name in $hostSources) { if (!(Test-Path -LiteralPath (Join-Path $hostRoot $name))) { throw "Missing original host source: $name" } }
foreach ($name in $adapterSources) { if (!(Test-Path -LiteralPath (Join-Path $root $name))) { throw "Missing S5 adapter source: $name" } }
foreach ($name in $test) { if (!(Test-Path -LiteralPath (Join-Path $root $name))) { throw "Missing focused fixture source: $name" } }

New-Item -ItemType Directory -Force $build, (Join-Path $build 'obj/ccpu'), (Join-Path $build 'obj/host'), (Join-Path $build 'obj/adapter'), (Join-Path $build 'obj/test') | Out-Null
$environment = Join-Path $build 'msvc-mt.cmd'
@('@echo off', 'set "MVDM_T310_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready', ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'), 'if errorlevel 1 exit /b %errorlevel%', ':ready', 'cd /d "%MVDM_T310_CALLER_CWD%"', '%*') | Set-Content -LiteralPath $environment -Encoding ascii
$includes = @('src', 'src/adapter-mvdm-host-out/win32/include', 'src/opennt-host/public/sdk/inc', 'src/opennt-abi/source/public/sdk/inc', 'src/opennt-abi/source/public/internal/base/inc', 'src/opennt-abi/source/public/internal/windows/inc', 'src/opennt-abi/source/public/ddk/inc', 'src/mvdm-support/inc', 'src/mvdm-softpc-patch/x86/prod', 'src/mvdm-host/softpc.new/base/ccpu386', 'src/mvdm-host/softpc.new/host/inc', 'src/mvdm-host/softpc.new/base/cvidc', 'src/mvdm-host/softpc.new/base/inc', 'src/adapter-mvdm-host-out/softpc/include', 'src/session') | ForEach-Object { '/I "' + (NinjaPath (Join-Path $root $_)) + '"' }
# `i386` is never a product-wide host switch. Any original x86-only source
# unit needs a separately registered, target-local compilation exception.
$cflags = '/nologo /TC /c /MT /W4 /showIncludes /DWIN32 /DWINNT /DNTVDM /DCPU_30_STYLE /DCPU_40_STYLE /DNEW_CPU /DCCPU /DSPC386 /DSIM32 /DANSI /DPROD /FI "' + (NinjaPath (Join-Path $root 'src/adapter-mvdm-host-out/win32/include/nt.h')) + '" ' + ($includes -join ' ')
$graph = [Collections.Generic.List[string]]::new()
$graph.Add('ninja_required_version = 1.10'); $graph.Add('cflags = ' + $cflags); $graph.Add('')
$graph.Add('rule cc'); $graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' cl.exe $cflags /Fo$out $in'); $graph.Add('  deps = msvc'); $graph.Add('  msvc_deps_prefix = Note: including file:')
$graph.Add('rule lib'); $graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' lib.exe /nologo /out:$out $in')
$graph.Add('rule link'); $graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' link.exe /nologo /out:$out $in kernel32.lib user32.lib advapi32.lib ntdll.lib legacy_stdio_definitions.lib')
$ccpuObj = foreach ($name in $ccpu) { $obj = 'obj/ccpu/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'; $graph.Add('build ' + $obj + ': cc ' + (NinjaPath (Join-Path $ccpuRoot $name))); $obj }
$hostObj = foreach ($name in $hostSources) { $obj = 'obj/host/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'; $graph.Add('build ' + $obj + ': cc ' + (NinjaPath (Join-Path $hostRoot $name))); $obj }
$adapterObj = foreach ($name in $adapterSources) { $obj = 'obj/adapter/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'; $graph.Add('build ' + $obj + ': cc ' + (NinjaPath (Join-Path $root $name))); $obj }
$testObj = foreach ($name in $test) { $obj = 'obj/test/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'; $graph.Add('build ' + $obj + ': cc ' + (NinjaPath (Join-Path $root $name))); $obj }
$graph.Add('build original-ccpu-execution.lib: lib ' + ($ccpuObj -join ' '))
$graph.Add('build original-softpc-s5-host.lib: lib ' + ($hostObj -join ' '))
$graph.Add('build original-softpc-s5-bridge.lib: lib ' + ($adapterObj -join ' '))
$graph.Add('build ccpu-bounded-execution.exe: link ' + (($testObj + @('original-ccpu-execution.lib', 'original-softpc-s5-host.lib', 'original-softpc-s5-bridge.lib')) -join ' '))
$graph.Add('build ccpu-bounded-execution: phony ccpu-bounded-execution.exe'); $graph.Add('default ccpu-bounded-execution')
[IO.File]::WriteAllText((Join-Path $build 'build.ninja'), (($graph -join [Environment]::NewLine) + [Environment]::NewLine), [Text.UTF8Encoding]::new($false))
[ordered]@{ schema='m0.t310.s5.original-ccpu-bounded-execution.v2'; architecture=$Architecture; i386Define=$false; executor='original softpc.new/base/ccpu386 c_cpu_simulate'; selectedCcpu=$ccpu; excludedAlternateSource=@('ntstubs.c'); hostSources=$hostSources; adapterSources=$adapterSources; test=$test; forbiddenInputs=@('src.old','bochs-core','adapter-bochs','MONITOR','V86') } | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $build 'source-manifest.json') -Encoding utf8
Write-Host "Generated T310 S5 original CCPU execution graph: $build"
