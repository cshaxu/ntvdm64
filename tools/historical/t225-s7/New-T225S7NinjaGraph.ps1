[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$BuildRoot
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Get-TextSha256([string]$Text) {
    $sha = [Security.Cryptography.SHA256]::Create()
    try { return [BitConverter]::ToString($sha.ComputeHash([Text.Encoding]::UTF8.GetBytes($Text))).Replace('-', '').ToLowerInvariant() }
    finally { $sha.Dispose() }
}

$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = [IO.Path]::GetFullPath($BuildRoot)
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$ninja = Get-Command ninja -ErrorAction Stop
foreach ($input in @($vs, (Join-Path $root 'src\bx-vdm\bx_ntvdm_cpu_state_abi.c'),
        (Join-Path $root 'tests\bx-vdm\bx_ntvdm_cpu_state_abi_layout_test.c'))) {
    if (!(Test-Path -LiteralPath $input -PathType Leaf)) { throw "Required Ninja graph input missing: $input" }
}
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing Ninja build root: $build" }
New-Item -ItemType Directory -Force -Path $build, (Join-Path $build 'obj') | Out-Null
$compiler = & cmd.exe /d /s /c ('call "' + $vs + '" -arch=x64 -host_arch=x64 >nul && (cl.exe /Bv 2>&1 & exit /b 0)') 2>&1
if ($LASTEXITCODE -ne 0) { throw 'Unable to query MSVC identity.' }
$module = [ordered]@{
    schema = 'runner.t225.s7.ninja-module-manifest.v1'
    architecture = 'x64'
    runtimeLibrary = '/MT'
    cpuConfiguration = 'CPU5/Pentium-MMX, non-x86-64'
    includeRoots = @('src','src/bx-vdm','src/cli')
    modules = @(
        [ordered]@{ name = 'bx-vdm'; sources = @('src/bx-vdm/bx_ntvdm_cpu_state_abi.c'); disposition = 'admitted focused source closure' },
        [ordered]@{ name = 'bx-core'; sources = @(); disposition = 'awaiting extraction from audited CPU5 seed manifest' },
        [ordered]@{ name = 'bx-machine'; sources = @(); disposition = 'awaiting extraction from audited CPU5 seed manifest' },
        [ordered]@{ name = 'opennt-host'; sources = @(); disposition = 'awaiting independently composable provider closure' },
        [ordered]@{ name = 'cli'; sources = @(); disposition = 'awaiting CLI target admission' }
    )
    fixture = [ordered]@{ source = 'tests/bx-vdm/bx_ntvdm_cpu_state_abi_layout_test.c'; target = 'bx-vdm-cpu-state-abi-fixture.exe' }
    forbiddenInputs = @('bochs.exe','main.cc','config.cc','GUI/plugin/device archives','artifacts/build')
    ninja = [ordered]@{ path = $ninja.Source; version = (& $ninja.Source --version).Trim() }
    msvcIdentitySha256 = Get-TextSha256 (($compiler | Out-String).Trim())
}
$manifestJson = $module | ConvertTo-Json -Depth 8
$configurationHash = Get-TextSha256 $manifestJson
$module.configurationSha256 = $configurationHash
$manifestPath = Join-Path $build 'module-manifest.json'
$module | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $manifestPath -Encoding utf8
$environment = Join-Path $build 'msvc-x64-mt.cmd'
@('@echo off', ('call "' + $vs + '" -arch=x64 -host_arch=x64 >nul'), 'if errorlevel 1 exit /b %errorlevel%', '%*') | Set-Content -LiteralPath $environment -Encoding ascii
function NinjaPath([string]$Path) {
    $normal = $Path.Replace('\','/')
    if ($normal.Length -ge 2 -and $normal[1] -eq ':') {
        return $normal.Substring(0, 1) + '$' + ':' + $normal.Substring(2)
    }
    return $normal
}
$rootNinja = NinjaPath $root
$buildNinja = NinjaPath $build
$includeArgs = @($module.includeRoots | ForEach-Object { '/I "' + (NinjaPath (Join-Path $root $_)) + '"' }) -join ' '
$source = NinjaPath (Join-Path $root 'src/bx-vdm/bx_ntvdm_cpu_state_abi.c')
$fixture = NinjaPath (Join-Path $root 'tests/bx-vdm/bx_ntvdm_cpu_state_abi_layout_test.c')
$envNinja = NinjaPath $environment
$graph = @(
    'ninja_required_version = 1.10',
    ('configuration_sha256 = ' + $configurationHash),
    '',
    'rule cc',
    ('  command = cmd.exe /d /s /c call ' + $envNinja + ' cl.exe /nologo /TC /c /std:c11 /W4 /WX /MT /DWIN32 /D_CRT_SECURE_NO_WARNINGS /showIncludes ' + $includeArgs + ' /Fo$out $in'),
    '  deps = msvc',
    '  msvc_deps_prefix = Note: including file:',
    '  description = CC $out',
    '',
    'rule lib',
    ('  command = cmd.exe /d /s /c call ' + $envNinja + ' lib.exe /nologo /OUT:$out $in'),
    '  description = LIB $out',
    '',
    'rule link',
    ('  command = cmd.exe /d /s /c call ' + $envNinja + ' link.exe /nologo /OUT:$out /OPT:REF $in'),
    '  description = LINK $out',
    '',
    ('build obj/bx_ntvdm_cpu_state_abi.obj: cc ' + $source),
    'build bx-vdm.lib: lib obj/bx_ntvdm_cpu_state_abi.obj',
    ('build obj/bx_ntvdm_cpu_state_abi_layout_test.obj: cc ' + $fixture),
    'build bx-vdm-cpu-state-abi-fixture.exe: link obj/bx_ntvdm_cpu_state_abi_layout_test.obj bx-vdm.lib',
    '',
    'default bx-vdm-cpu-state-abi-fixture.exe'
)
$graph | Set-Content -LiteralPath (Join-Path $build 'build.ninja') -Encoding ascii
[ordered]@{ schema = 'runner.t225.s7.ninja-graph.v1'; buildRoot = $build; manifest = 'module-manifest.json'; configurationSha256 = $configurationHash; graph = 'build.ninja'; executor = $ninja.Source; architecture = 'x64'; runtimeLibrary = '/MT'; deps = 'msvc'; showIncludes = $true; fixture = 'bx-vdm-cpu-state-abi-fixture.exe'; formalCacheExecutor = 'Ninja'; customCacheDisposition = 'P1 evidence only' } | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $build 't225-s7-ninja-graph.json') -Encoding utf8
Write-Host "Generated T225 S7 Ninja graph: $build"