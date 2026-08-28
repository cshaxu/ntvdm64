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
$build = Join-Path $root ("build/M0-T310/S3/backend-selection/{0}" -f $Architecture)
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (!(Test-Path -LiteralPath $vs -PathType Leaf)) {
    throw 'MSVC Build Tools are required.'
}
New-Item -ItemType Directory -Force $build, (Join-Path $build 'obj') | Out-Null

$environment = Join-Path $build 'msvc-mt.cmd'
@('@echo off', 'set "MVDM_T310_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready',
  ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'),
  'if errorlevel 1 exit /b %errorlevel%', ':ready', 'cd /d "%MVDM_T310_CALLER_CWD%"', '%*') |
    Set-Content -LiteralPath $environment -Encoding ascii

$sources = @(
    'src/session/mapping_manager.c',
    'src/session/guest_memory_lease.c',
    'src/session/session.c',
    'src/app/machine_shell.c',
    'tests/session/backend_selection_test.c',
    'tests/app/backend_selection_test.c',
    'src/adapter-mvdm-host-out/softpc/mvdm_vdm_stack.c',
    'tests/adapter-mvdm-host-out/softpc_backend_selection_test.c'
)
$graph = [Collections.Generic.List[string]]::new()
$graph.Add('ninja_required_version = 1.10')
$graph.Add('cflags = /nologo /TC /c /MT /W4 /showIncludes /I "' + (NinjaPath (Join-Path $root 'src')) + '" /I "' + (NinjaPath (Join-Path $root 'src/adapter-mvdm-host-out/softpc/include')) + '"')
$graph.Add('rule cc')
$graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' cl.exe $cflags /Fo$out $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file:')
$graph.Add('rule link')
$graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' link.exe /nologo /out:$out $in')
$graph.Add('rule run')
$graph.Add('  command = $in')
$objects = @()
for ($index = 0; $index -lt $sources.Count; ++$index) {
    $source = $sources[$index]
    $object = 'obj/' + $index + '-' + ([IO.Path]::GetFileNameWithoutExtension($source)) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $root $source)))
    $objects += $object
}
$graph.Add('build session-backend-selection-test.exe: link obj/0-mapping_manager.obj obj/1-guest_memory_lease.obj obj/2-session.obj obj/4-backend_selection_test.obj')
$graph.Add('build app-backend-selection-test.exe: link obj/0-mapping_manager.obj obj/1-guest_memory_lease.obj obj/2-session.obj obj/3-machine_shell.obj obj/5-backend_selection_test.obj')
$graph.Add('build softpc-backend-selection-test.exe: link obj/0-mapping_manager.obj obj/1-guest_memory_lease.obj obj/2-session.obj obj/6-mvdm_vdm_stack.obj obj/7-softpc_backend_selection_test.obj')
$graph.Add('build verify: run session-backend-selection-test.exe')
$graph.Add('build verify-app: run app-backend-selection-test.exe')
$graph.Add('build verify-softpc: run softpc-backend-selection-test.exe')
$graph.Add('build all-verify: phony verify verify-app verify-softpc')
$graph.Add('default all-verify')
[IO.File]::WriteAllText((Join-Path $build 'build.ninja'),
    (($graph -join [Environment]::NewLine) + [Environment]::NewLine),
    [Text.UTF8Encoding]::new($false))
Write-Host "Generated T310 S3 selected-backend graph: $build"
