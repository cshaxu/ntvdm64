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

function Get-Sha256([string]$Path) {
    (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant()
}
function Get-NinjaPath([string]$Path) {
    $value = $Path.Replace('\', '/')
    if ($value.Length -ge 2 -and $value[1] -eq ':') {
        return $value.Substring(0, 1) + '$' + ':' + $value.Substring(2)
    }
    return $value
}
function Get-ObjectName([string]$Relative) {
    ($Relative -replace '[^A-Za-z0-9_]', '_') + '.obj'
}

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
if ([string]::IsNullOrWhiteSpace($BuildRoot)) {
    $build = Join-Path $root ("build/M0-T286/S6/{0}" -f $Architecture)
} elseif ([IO.Path]::IsPathRooted($BuildRoot)) {
    $build = $BuildRoot
} else {
    $build = Join-Path $root $BuildRoot
}
$build = [IO.Path]::GetFullPath($build)
$buildParent = [IO.Path]::GetFullPath((Join-Path $root 'build'))
if (!$build.StartsWith($buildParent + [IO.Path]::DirectorySeparatorChar,
        [StringComparison]::OrdinalIgnoreCase)) {
    throw "T286 build root must stay beneath $buildParent"
}

$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (!(Test-Path -LiteralPath $vs -PathType Leaf)) {
    throw "Required MSVC environment script is missing: $vs"
}
if (!(Get-Command ninja -ErrorAction SilentlyContinue)) {
    throw 'Ninja is required for the formal T286 graph.'
}

$coreRoot = Join-Path $root 'src\bochs-core'
$overlayRoot = Join-Path $root 'src\bochs-core-overlay'
$adapterRoot = Join-Path $root 'src\adapter-bochs'
$fixture = Join-Path $root 'tests\adapter-bochs\t286_memory_boundary_fixture.cc'
foreach ($path in @($coreRoot, $overlayRoot, $adapterRoot, $fixture)) {
    if (!(Test-Path -LiteralPath $path)) { throw "Missing admitted T286 input: $path" }
}

$coreSources = @(Get-ChildItem -LiteralPath $coreRoot -Recurse -File |
    Where-Object { $_.Extension -in @('.c', '.cc') } |
    ForEach-Object { $_.FullName.Substring($root.Length + 1).Replace('\', '/') } |
    Sort-Object)
$overlaySources = @(Get-ChildItem -LiteralPath $overlayRoot -Recurse -File |
    Where-Object { $_.Extension -in @('.c', '.cc') } |
    ForEach-Object { $_.FullName.Substring($root.Length + 1).Replace('\', '/') } |
    Sort-Object)
$adapterSources = @(
    'src/adapter-bochs/headless_8042.cc',
    'src/adapter-bochs/machine_facade.cc',
    'src/adapter-bochs/minimal_machine.cc',
    'src/adapter-bochs/minimal_pic.cc',
    'src/adapter-bochs/minimal_sim.cc'
)
foreach ($relative in @($coreSources + $overlaySources + $adapterSources)) {
    if (!(Test-Path -LiteralPath (Join-Path $root $relative) -PathType Leaf)) {
        throw "Missing selected T286 source: $relative"
    }
    if ($relative -match '(^|/)src\.old(/|$)') { throw "Forbidden src.old input: $relative" }
}

New-Item -ItemType Directory -Force -Path $build, (Join-Path $build 'obj\bochs-core'),
    (Join-Path $build 'obj\adapter-bochs'), (Join-Path $build 'obj\fixture'),
    (Join-Path $build 'lib'), (Join-Path $build 'bin') | Out-Null

$environment = Join-Path $build 'msvc-mt.cmd'
@('@echo off', 'set "MVDM_T286_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto mvdm_t286_ready',
    ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'),
    'if errorlevel 1 exit /b %errorlevel%', ':mvdm_t286_ready',
    'cd /d "%MVDM_T286_CALLER_CWD%"', '%*') |
    Set-Content -LiteralPath $environment -Encoding ascii

$sourceManifest = [ordered]@{
    schema = 'm0.t286.bochs-boundary-source-manifest.v1'
    architecture = $Architecture
    runtimeLibrary = '/MT'
    coreSources = @($coreSources | ForEach-Object { [ordered]@{ path = $_; sha256 = Get-Sha256 (Join-Path $root $_) } })
    overlaySources = @($overlaySources | ForEach-Object { [ordered]@{ path = $_; sha256 = Get-Sha256 (Join-Path $root $_) } })
    adapterSources = @($adapterSources | ForEach-Object { [ordered]@{ path = $_; sha256 = Get-Sha256 (Join-Path $root $_) } })
    fixture = [ordered]@{ path = 'tests/adapter-bochs/t286_memory_boundary_fixture.cc'; sha256 = Get-Sha256 $fixture }
    forbiddenInputs = @('src.old', 'prebuilt Bochs archives', 'product GUI/plugin/device archives')
}
$sourceManifest | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath (Join-Path $build 'source-manifest.json') -Encoding utf8

$rootNinja = Get-NinjaPath $root
$buildNinja = Get-NinjaPath $build
$envNinja = Get-NinjaPath $environment
$includes = @('src', 'src/bochs-core', 'src/bochs-core/cpu', 'src/bochs-core/iodev',
    'src/bochs-core/instrument/stubs', 'src/adapter-bochs') |
    ForEach-Object { '/I "' + (Get-NinjaPath (Join-Path $root $_)) + '"' }
$cflags = '/nologo /TP /c /std:c++14 /EHsc /MT /W4 /showIncludes /DWIN32 ' + ($includes -join ' ')
$graph = [Collections.Generic.List[string]]::new()
$graph.Add('ninja_required_version = 1.10')
$graph.Add('build_root = ' + $buildNinja)
$graph.Add('cflags = ' + $cflags)
$graph.Add('')
$graph.Add('rule cxx')
$graph.Add('  command = cmd.exe /d /s /c call ' + $envNinja + ' cl.exe $cflags /Fo$out $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file:')
$graph.Add('  description = CXX $out')
$graph.Add('')
$graph.Add('rule lib')
$graph.Add('  command = cmd.exe /d /s /c cd /d $build_root && call ' + $envNinja + ' lib.exe /nologo /OUT:$out @$out.rsp')
$graph.Add('  description = LIB $out')
$graph.Add('')
$graph.Add('rule link')
$graph.Add('  command = cmd.exe /d /s /c cd /d $build_root && call ' + $envNinja + ' link.exe /nologo /OUT:$out @$out.rsp')
$graph.Add('  description = LINK $out')
$graph.Add('')

function Add-Library([string]$Name, [string[]]$Sources) {
    $objects = [Collections.Generic.List[string]]::new()
    foreach ($relative in $Sources) {
        $object = 'obj/' + $Name + '/' + (Get-ObjectName $relative)
        $graph.Add('build ' + $object + ': cxx ' + (Get-NinjaPath (Join-Path $root $relative)))
        $objects.Add($object)
    }
    $library = 'lib/' + $Name + '.lib'
    [IO.File]::WriteAllText((Join-Path $build ($library + '.rsp')),
        (($objects -join [Environment]::NewLine) + [Environment]::NewLine),
        [Text.UTF8Encoding]::new($false))
    $graph.Add('build ' + $library + ': lib ' + ($objects -join ' '))
    $graph.Add('')
    return $library
}

$coreLibrary = Add-Library 'bochs-core' @($coreSources + $overlaySources)
$adapterLibrary = Add-Library 'adapter-bochs' $adapterSources
$fixtureObject = 'obj/fixture/t286_memory_boundary_fixture_cc.obj'
$graph.Add('build ' + $fixtureObject + ': cxx ' + (Get-NinjaPath $fixture))
$fixtureOutput = 'bin/t286-memory-boundary-fixture.exe'
[IO.File]::WriteAllText((Join-Path $build ($fixtureOutput + '.rsp')),
    (($fixtureObject + [Environment]::NewLine + $adapterLibrary + [Environment]::NewLine +
      $coreLibrary + [Environment]::NewLine + 'kernel32.lib' + [Environment]::NewLine +
      'user32.lib' + [Environment]::NewLine)), [Text.UTF8Encoding]::new($false))
$graph.Add('build ' + $fixtureOutput + ': link ' + $fixtureObject + ' ' + $adapterLibrary + ' ' + $coreLibrary)
$graph.Add('build all: phony ' + $fixtureOutput)
$graph.Add('default all')
[IO.File]::WriteAllLines((Join-Path $build 'build.ninja'), $graph, [Text.UTF8Encoding]::new($false))
Write-Host "Generated T286 $Architecture Bochs boundary Ninja graph: $build"
