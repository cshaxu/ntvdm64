[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$BuildRoot,
    [string]$ManifestPath = ''
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Get-FileSha256([string]$Path) {
    return (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant()
}
function Get-TextSha256([string]$Text) {
    $sha = [Security.Cryptography.SHA256]::Create()
    try { return [BitConverter]::ToString($sha.ComputeHash([Text.Encoding]::UTF8.GetBytes($Text))).Replace('-', '').ToLowerInvariant() }
    finally { $sha.Dispose() }
}
function NinjaPath([string]$Path) {
    $normal = $Path.Replace('\', '/')
    if ($normal.Length -ge 2 -and $normal[1] -eq ':') { return $normal.Substring(0, 1) + '$' + ':' + $normal.Substring(2) }
    return $normal
}
function ObjectName([string]$Relative) {
    return ($Relative -replace '[^A-Za-z0-9_]', '_') + '.obj'
}

$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = [IO.Path]::GetFullPath($BuildRoot)
if ([string]::IsNullOrWhiteSpace($ManifestPath)) { $ManifestPath = Join-Path $root 'tools\build\t225-s7-full-module-manifest.json' }
$manifestPath = (Resolve-Path -LiteralPath $ManifestPath).Path
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$ninja = Get-Command ninja -ErrorAction Stop
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing Ninja build root: $build" }
foreach ($input in @($manifestPath, $vs, (Join-Path $root 'tools\build\Project-BochsConfig.ps1'))) {
    if (!(Test-Path -LiteralPath $input -PathType Leaf)) { throw "Required graph input missing: $input" }
}
$manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
if ($manifest.schema -ne 'ntdos64.t225.s7.full-module-manifest.v1' -or $manifest.architecture -ne 'x64' -or $manifest.runtimeLibrary -ne '/MT') {
    throw 'Unsupported T225 S7 full-module manifest.'
}
if (@($manifest.modules).Count -ne 4) { throw 'Full module manifest must contain exactly four owned modules.' }
foreach ($module in @($manifest.modules)) {
    if ($module.name -notin @('bx-core', 'bx-mantle', 'bx-vdm', 'cli') -or @($module.sources).Count -eq 0) {
        throw 'Module ownership or source list is invalid.'
    }
    foreach ($source in @($module.sources)) {
        if ($source -notmatch '^src/(bx-core|bx-mantle|bx-vdm|cli)/.+\.(c|cc)$' -or
            !(Test-Path -LiteralPath (Join-Path $root $source) -PathType Leaf)) {
            throw "Invalid or missing manifest source: $source"
        }
    }
}
foreach ($entry in @($manifest.fixtures) + @($manifest.targets)) {
    if ($null -eq $entry -or [string]::IsNullOrWhiteSpace($entry.name) -or
        [string]::IsNullOrWhiteSpace($entry.source) -or
        !(Test-Path -LiteralPath (Join-Path $root $entry.source) -PathType Leaf)) {
        throw 'Fixture or target admission is invalid.'
    }
}
New-Item -ItemType Directory -Force -Path $build, (Join-Path $build 'obj'), (Join-Path $build 'lib'), (Join-Path $build 'bin') | Out-Null
foreach ($module in @($manifest.modules)) { New-Item -ItemType Directory -Force -Path (Join-Path $build ('obj\' + $module.name)) | Out-Null }
New-Item -ItemType Directory -Force -Path (Join-Path $build 'obj\targets') | Out-Null

$projectionTool = Join-Path $root $manifest.configProjection.tool
$projectionManifest = Join-Path $root $manifest.configProjection.projection
& powershell.exe -ExecutionPolicy Bypass -File $projectionTool -ManifestPath $projectionManifest -AnalysisRoot (Join-Path $build 'config') -InputConfigPath (Join-Path $root 'refs\bochs\config.h')
if ($LASTEXITCODE -ne 0) { throw 'CPU5 config projection failed.' }
$config = Join-Path $build 'config\config.h'
if (!(Test-Path -LiteralPath $config -PathType Leaf)) { throw 'CPU5 config projection emitted no config.h.' }
$compiler = & cmd.exe /d /s /c ('call "' + $vs + '" -arch=x64 -host_arch=x64 >nul && (cl.exe /Bv 2>&1 & exit /b 0)') 2>&1
if ($LASTEXITCODE -ne 0) { throw 'Unable to query MSVC identity.' }
$environment = Join-Path $build 'msvc-x64-mt.cmd'
@('@echo off', ('call "' + $vs + '" -arch=x64 -host_arch=x64 >nul'), 'if errorlevel 1 exit /b %errorlevel%', '%*') | Set-Content -LiteralPath $environment -Encoding ascii
$manifestHash = Get-FileSha256 $manifestPath
$configHash = Get-FileSha256 $config
$toolchainHash = Get-TextSha256 (($compiler | Out-String).Trim())
$buildManifest = [ordered]@{
    schema = 'ntdos64.t225.s7.ninja-full-graph.v1'
    sourceManifest = $manifestPath.Substring($root.Length + 1).Replace('\','/')
    sourceManifestSha256 = $manifestHash
    projectedConfigSha256 = $configHash
    msvcIdentitySha256 = $toolchainHash
    ninja = [ordered]@{ path = $ninja.Source; version = (& $ninja.Source --version).Trim() }
    architecture = 'x64'; runtimeLibrary = '/MT'; cpuConfiguration = $manifest.cpuConfiguration
    modules = $manifest.modules; fixtures = $manifest.fixtures; targets = $manifest.targets
    forbiddenInputs = $manifest.forbiddenInputs
}
$configurationHash = Get-TextSha256 ($buildManifest | ConvertTo-Json -Depth 10)
$buildManifest.configurationSha256 = $configurationHash
$buildManifest | ConvertTo-Json -Depth 10 | Set-Content -LiteralPath (Join-Path $build 'module-manifest.json') -Encoding utf8

$includeArgs = @($manifest.includeRoots | ForEach-Object { '/I "' + (NinjaPath (Join-Path $root $_)) + '"' }) -join ' '
$envNinja = NinjaPath $environment
$configNinja = NinjaPath $config
$graph = [Collections.Generic.List[string]]::new()
$graph.Add('ninja_required_version = 1.10')
$graph.Add('configuration_sha256 = ' + $configurationHash)
$graph.Add('')
$graph.Add('rule cc')
$graph.Add('  command = cmd.exe /d /s /c call ' + $envNinja + ' cl.exe /nologo /TC /c /std:c11 /W4 /WX /MT /DWIN32 /D_CRT_SECURE_NO_WARNINGS /showIncludes /FI "' + $configNinja + '" ' + $includeArgs + ' /Fo$out $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file:')
$graph.Add('  description = CC $out')
$graph.Add('')
$graph.Add('rule cxx')
$graph.Add('  command = cmd.exe /d /s /c call ' + $envNinja + ' cl.exe /nologo /TP /c /std:c++14 /EHsc /MT /Gy /DWIN32 /showIncludes /FI "' + $configNinja + '" ' + $includeArgs + ' /Fo$out $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file:')
$graph.Add('  description = CXX $out')
$graph.Add('')
$graph.Add('rule lib')
$graph.Add('  command = cmd.exe /d /s /c call ' + $envNinja + ' lib.exe /nologo /OUT:$out @$rspfile')
$graph.Add('  rspfile = $out.rsp')
$graph.Add('  rspfile_content = $in')
$graph.Add('  description = LIB $out')
$graph.Add('')
$graph.Add('rule link')
$graph.Add('  command = cmd.exe /d /s /c call ' + $envNinja + ' link.exe /nologo /OPT:REF /OUT:$out @$rspfile')
$graph.Add('  rspfile = $out.rsp')
$graph.Add('  rspfile_content = $in $platform')
$graph.Add('  description = LINK $out')
$graph.Add('')

$moduleLibraries = @{}
foreach ($module in @($manifest.modules)) {
    $objects = [Collections.Generic.List[string]]::new()
    foreach ($relative in @($module.sources)) {
        $source = NinjaPath (Join-Path $root $relative)
        $object = 'obj/' + $module.name + '/' + (ObjectName $relative)
        $rule = if ($relative.EndsWith('.cc')) { 'cxx' } else { 'cc' }
        $graph.Add('build ' + $object + ': ' + $rule + ' ' + $source)
        $objects.Add($object)
    }
    $library = 'lib/' + $module.name + '.lib'
    $graph.Add('build ' + $library + ': lib ' + ($objects -join ' '))
    $graph.Add('')
    $moduleLibraries[$module.name] = $library
}
$outputs = [Collections.Generic.List[string]]::new()
foreach ($entry in @($manifest.fixtures) + @($manifest.targets)) {
    $source = NinjaPath (Join-Path $root $entry.source)
    $object = 'obj/targets/' + (ObjectName $entry.source)
    $rule = if ($entry.source.EndsWith('.cc')) { 'cxx' } else { 'cc' }
    $output = 'bin/' + $entry.name + '.exe'
    $libraries = @($entry.libraries | ForEach-Object { if (!$moduleLibraries.ContainsKey($_)) { throw "Target $($entry.name) references unknown module $_" }; $moduleLibraries[$_] })
    $graph.Add('build ' + $object + ': ' + $rule + ' ' + $source)
    $graph.Add('build ' + $output + ': link ' + $object + ' ' + ($libraries -join ' '))
    $graph.Add('  platform = ' + (@($entry.platformLibraries) -join ' '))
    $graph.Add('')
    $outputs.Add($output)
}
$graph.Add('build all: phony ' + ($outputs -join ' '))
$graph.Add('default all')
$graph | Set-Content -LiteralPath (Join-Path $build 'build.ninja') -Encoding ascii
[ordered]@{ schema='ntdos64.t225.s7.ninja-full-graph-record.v1'; buildRoot=$build; graph='build.ninja'; manifest='module-manifest.json'; configurationSha256=$configurationHash; formalCacheExecutor='Ninja'; customCacheDisposition='P1 implementation pending deletion after full-graph verification' } | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $build 't225-s7-full-ninja-graph.json') -Encoding utf8
Write-Host "Generated T225 S7 full Ninja graph: $build"