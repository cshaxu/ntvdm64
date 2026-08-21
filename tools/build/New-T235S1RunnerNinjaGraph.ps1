[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$BuildRoot
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function NinjaPath([string]$Path) {
    $normal = $Path.Replace('\', '/')
    if ($normal.Length -ge 2 -and $normal[1] -eq ':') {
        return $normal.Substring(0, 1) + '$' + ':' + $normal.Substring(2)
    }
    return $normal
}

$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = [IO.Path]::GetFullPath($BuildRoot)
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$ninja = Get-Command ninja -ErrorAction Stop
$runnerSources = @(
    'src/cli/ntdos64_run.c',
    'src/cli/ntdos64_config.c',
    'src/cli/byob_identity.c',
    'src/cli/byob_target_selection.c',
    'src/cli/byob_launch_declaration_v1.c',
    'src/cli/byob_launch_plan_v2.c'
)
$probeSource = 'tests/runner/runner_engine_probe.c'

if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing Ninja build root: $build" }
foreach ($input in @($vs, $probeSource) + $runnerSources) {
    $candidate = if ([IO.Path]::IsPathRooted($input)) { $input } else { Join-Path $root $input }
    if (!(Test-Path -LiteralPath $candidate -PathType Leaf)) {
        throw "Required T235 S1 runner input is missing: $input"
    }
}

New-Item -ItemType Directory -Force -Path $build, (Join-Path $build 'obj') | Out-Null
$environment = Join-Path $build 'msvc-x64-mt.cmd'
@('@echo off', 'set "NTDOS64_NINJA_CALLER_CWD=%CD%"',
  ('call "' + $vs + '" -arch=x64 -host_arch=x64 >nul'),
  'if errorlevel 1 exit /b %errorlevel%',
  'cd /d "%NTDOS64_NINJA_CALLER_CWD%"', '%*') |
    Set-Content -LiteralPath $environment -Encoding ascii

$manifest = [ordered]@{
    schema = 'ntdos64.t235.s1.runner-ninja-graph.v1'
    task = 'M0 T235 S1'
    sourceRoot = $root
    buildRoot = $build
    architecture = 'x64'
    runtimeLibrary = '/MT'
    compiler = 'MSVC Build Tools 2022'
    executor = [ordered]@{ path = $ninja.Source; version = (& $ninja.Source --version).Trim() }
    targets = [ordered]@{ runner = $runnerSources; probe = $probeSource }
    forbiddenInputs = @('OpenNT runtime objects', 'Bochs objects', 'guest media', 'artifacts/build')
}
$manifest | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath (Join-Path $build 'module-manifest.json') -Encoding utf8

$envNinja = NinjaPath $environment
$include = '/I "' + (NinjaPath (Join-Path $root 'src/cli')) + '"'
$graph = [Collections.Generic.List[string]]::new()
$graph.Add('ninja_required_version = 1.10')
$graph.Add('build_root = ' + (NinjaPath $build))
$graph.Add('')
$graph.Add('rule cc')
$graph.Add('  command = cmd.exe /d /s /c call ' + $envNinja + ' cl.exe /nologo /TC /c /std:c11 /W4 /WX /MT /DUNICODE /D_UNICODE /DWIN32_LEAN_AND_MEAN /D_CRT_SECURE_NO_WARNINGS ' + $include + ' /Fo$out $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file:')
$graph.Add('  description = CC $out')
$graph.Add('')
$graph.Add('rule link_runner')
$graph.Add('  command = cmd.exe /d /s /c call ' + $envNinja + ' link.exe /nologo /OUT:$out $in shell32.lib bcrypt.lib')
$graph.Add('  description = LINK $out')
$graph.Add('')
$graph.Add('rule link_probe')
$graph.Add('  command = cmd.exe /d /s /c call ' + $envNinja + ' link.exe /nologo /OUT:$out $in')
$graph.Add('  description = LINK $out')
$graph.Add('')
$runnerObjects = [Collections.Generic.List[string]]::new()
foreach ($relative in $runnerSources) {
    $object = 'obj/' + (($relative -replace '[^A-Za-z0-9_]', '_') + '.obj')
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $root $relative)))
    $runnerObjects.Add($object)
}
$probeObject = 'obj/tests_runner_runner_engine_probe_c.obj'
$graph.Add('build ' + $probeObject + ': cc ' + (NinjaPath (Join-Path $root $probeSource)))
$graph.Add('build ntvdm64-0235.exe: link_runner ' + ($runnerObjects -join ' '))
$graph.Add('build runner-engine-probe.exe: link_probe ' + $probeObject)
$graph.Add('build all: phony ntvdm64-0235.exe runner-engine-probe.exe')
$graph.Add('default all')
$graph | Set-Content -LiteralPath (Join-Path $build 'build.ninja') -Encoding ascii
Write-Host "Generated M0 T235 S1 runner Ninja graph: $build"
