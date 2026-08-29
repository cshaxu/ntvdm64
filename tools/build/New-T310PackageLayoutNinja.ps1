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
$build = Join-Path $root ("build/M0-T310/S8/p1-package-layout/{0}" -f $Architecture)
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (!(Test-Path -LiteralPath $vs -PathType Leaf)) { throw 'MSVC Build Tools are required.' }
New-Item -ItemType Directory -Force $build, (Join-Path $build 'obj') | Out-Null
$environment = Join-Path $build 'msvc-mt.cmd'
@('@echo off', 'set "MVDM_T310_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready',
  ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'),
  'if errorlevel 1 exit /b %errorlevel%', ':ready', 'cd /d "%MVDM_T310_CALLER_CWD%"', '%*') |
    Set-Content -LiteralPath $environment -Encoding ascii

$sources = @('src/session/mapping_manager.c', 'src/session/guest_memory_lease.c',
    'src/session/session.c', 'src/app/package_layout.c',
    'tests/app/package_layout_fixture.c')
$graph = [Collections.Generic.List[string]]::new()
$graph.Add('ninja_required_version = 1.10')
$graph.Add('cflags = /nologo /TC /c /MT /W4 /showIncludes /I "' + (NinjaPath (Join-Path $root 'src')) + '"')
$graph.Add('rule cc')
$graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' cl.exe $cflags /Fo$out $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file:')
$graph.Add('rule link')
$graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' link.exe /nologo /out:$out $in kernel32.lib')
$graph.Add('rule run')
$graph.Add('  command = $in')
$objects = @()
for ($index = 0; $index -lt $sources.Count; ++$index) {
    $object = 'obj/' + $index + '-' + ([IO.Path]::GetFileNameWithoutExtension($sources[$index])) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $root $sources[$index])))
    $objects += $object
}
$graph.Add('build package-layout-fixture.exe: link ' + ($objects -join ' '))
$graph.Add('build verify: run package-layout-fixture.exe')
$graph.Add('default verify')
[IO.File]::WriteAllText((Join-Path $build 'build.ninja'), (($graph -join [Environment]::NewLine) + [Environment]::NewLine), [Text.UTF8Encoding]::new($false))
Write-Host "Generated T310 S8 package-layout graph: $build"
