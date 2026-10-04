[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [ValidateSet('x86', 'x64')] [string]$Architecture,
    [string]$RepositoryRoot = '',
    [Parameter(Mandatory = $true)] [string]$RuntimeRoot,
    [string]$BuildRoot = ''
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
$runtime = (Resolve-Path -LiteralPath $RuntimeRoot).Path
$build = Join-Path $root ("build/M0-T310/S8/p1-firmware-resource/{0}" -f $Architecture)
if ($BuildRoot) { $build = [IO.Path]::GetFullPath($BuildRoot) }
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (!(Test-Path -LiteralPath $vs -PathType Leaf)) { throw 'MSVC Build Tools are required.' }
New-Item -ItemType Directory -Force $build, (Join-Path $build 'obj') | Out-Null

$environment = Join-Path $build 'msvc-mt.cmd'
@('@echo off', 'set "MVDM_T310_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready',
  ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'),
  'if errorlevel 1 exit /b %errorlevel%', ':ready', 'cd /d "%MVDM_T310_CALLER_CWD%"', '%*') |
    Set-Content -LiteralPath $environment -Encoding ascii

# `i386` is not a host-architecture switch in the product.  This focused
# resource graph is architecture-neutral; source-local exceptions, if ever
# required, must be separately audited and registered.
$sources = @(
    'src/ntvdm-exe/session/guest_memory_lease.c',
    'src/ntvdm-exe/session/session.c',
    'src/ntvdm-exe/softpc/mvdm_softpc_firmware.c',
    'tests/session/softpc_firmware_resource_fixture.c',
    'tests/session/softpc_media_resource_fixture.c',
    'src/common/system_root.c',
    'tests/session/softpc_product_loader_fixture.c',
    'src/common/guest_environment.c',
    'tests/session/softpc_wow_boot_environment_fixture.c'
)
$graph = [Collections.Generic.List[string]]::new()
$graph.Add('ninja_required_version = 1.10')
$graph.Add('cflags = /nologo /TC /c /MT /W4 /showIncludes' +
    ' /I "' + (NinjaPath (Join-Path $root 'src')) + '" /I "' +
    (NinjaPath (Join-Path $root 'src/ntvdm-exe/softpc/include')) + '"')
$graph.Add('rule cc')
$graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' cl.exe $cflags /Fo$out $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file:')
$graph.Add('rule link')
$graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' link.exe /nologo /out:$out $in kernel32.lib')
$graph.Add('rule run')
$graph.Add('  command = $in "' + (NinjaPath (Join-Path $root 'src/mvdm/softpc.new/roms')) + '"')
$graph.Add('rule run_dos')
$graph.Add('  command = $in "' + (NinjaPath $runtime) + '"')
$graph.Add('rule run_loader')
$graph.Add('  command = $in "' + (NinjaPath (Join-Path $build 'loader-shadow')) + '"')
$objects = @()
for ($index = 0; $index -lt $sources.Count; ++$index) {
    $object = 'obj/' + $index + '-' + ([IO.Path]::GetFileNameWithoutExtension($sources[$index])) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $root $sources[$index])))
    $objects += $object
}
$graph.Add('build softpc-firmware-resource-fixture.exe: link obj/0-guest_memory_lease.obj obj/1-session.obj obj/2-mvdm_softpc_firmware.obj obj/3-softpc_firmware_resource_fixture.obj obj/5-system_root.obj obj/7-guest_environment.obj')
$graph.Add('build softpc-media-resource-fixture.exe: link obj/0-guest_memory_lease.obj obj/1-session.obj obj/2-mvdm_softpc_firmware.obj obj/4-softpc_media_resource_fixture.obj obj/5-system_root.obj obj/7-guest_environment.obj')
$graph.Add('build softpc-product-loader-fixture.exe: link obj/0-guest_memory_lease.obj obj/1-session.obj obj/2-mvdm_softpc_firmware.obj obj/5-system_root.obj obj/6-softpc_product_loader_fixture.obj obj/7-guest_environment.obj')
$graph.Add('build softpc-wow-boot-environment-fixture.exe: link obj/0-guest_memory_lease.obj obj/1-session.obj obj/2-mvdm_softpc_firmware.obj obj/5-system_root.obj obj/7-guest_environment.obj obj/8-softpc_wow_boot_environment_fixture.obj')
$graph.Add('rule run_boot')
$graph.Add('  command = $in')
$graph.Add('build verify-boot: run_boot softpc-wow-boot-environment-fixture.exe')
$graph.Add('build verify: run softpc-firmware-resource-fixture.exe')
$graph.Add('build verify-media: run_dos softpc-media-resource-fixture.exe')
$graph.Add('build verify-loader: run_loader softpc-product-loader-fixture.exe')
$graph.Add('build all-verify: phony verify verify-media verify-loader verify-boot')
$graph.Add('default all-verify')
[IO.File]::WriteAllText((Join-Path $build 'build.ninja'), (($graph -join [Environment]::NewLine) + [Environment]::NewLine), [Text.UTF8Encoding]::new($false))
Write-Host "Generated T310 S8 firmware-resource graph: $build"
