[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$BuildRoot,
    [string]$ManifestPath = '',
    [switch]$Refresh,
    [switch]$InstructionHistoryDiagnostic,
    [switch]$InstructionHistoryProvenanceDiagnostic,
    [switch]$SoftwareInterruptDiagnostic,
    [switch]$InterruptReturnDiagnostic,
    [switch]$PhysicalWriteDiagnostic
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
$rootBuildLeaks = @(Get-ChildItem -LiteralPath $root -File -Force | Where-Object {
    $_.Extension -in @('.obj', '.exe', '.pdb', '.ilk', '.map', '.lib', '.a') -or
    $_.Name -in @('stdout.txt', 'stderr.txt')
})
if ($rootBuildLeaks.Count -ne 0) {
    throw ('Remove root-level build products before generating Ninja: ' +
        (($rootBuildLeaks | Select-Object -ExpandProperty Name) -join ', ') +
        '. Use build/<task>/<run>/ for cached output.')
}
if ([string]::IsNullOrWhiteSpace($ManifestPath)) { $ManifestPath = Join-Path $root 'tools\build\t260-s8-component-manifest.json' }
$manifestPath = (Resolve-Path -LiteralPath $ManifestPath).Path
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$ninja = Get-Command ninja -ErrorAction Stop
$buildParent = [IO.Path]::GetFullPath((Join-Path $root 'build'))
if (Test-Path -LiteralPath $build) {
    if (!$Refresh) { throw "Refusing to overwrite existing Ninja build root: $build" }
    if (!$build.StartsWith($buildParent + [IO.Path]::DirectorySeparatorChar,
            [StringComparison]::OrdinalIgnoreCase) -or
        !(Test-Path -LiteralPath (Join-Path $build 'build.ninja') -PathType Leaf) -or
        !(Test-Path -LiteralPath (Join-Path $build 't260-s8-component-graph.json') -PathType Leaf)) {
        throw "Refresh accepts only an existing formal T260 build root below ${buildParent}: $build"
    }
    Write-Host "Refreshing formal Ninja graph in-place; existing objects remain available for Ninja dependency reuse: $build"
}
foreach ($input in @($manifestPath, $vs, (Join-Path $root 'tools\build\Project-BochsConfig.ps1'))) {
    if (!(Test-Path -LiteralPath $input -PathType Leaf)) { throw "Required graph input missing: $input" }
}
$manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
$requiredPlatformLibraries = @('advapi32.lib', 'comctl32.lib', 'gdi32.lib', 'ntdll.lib')
# The app callback uses this selector-blind storage in every graph. Diagnostic
# switches enable observation only; they do not own the implementation object.
$softpc = @($manifest.modules | Where-Object { $_.name -eq 'adapter-softpc' })
if ($softpc.Count -ne 1) { throw 'Formal graph requires one adapter-softpc module.' }
$softpc[0].sources = @($softpc[0].sources) + 'src/adapter-softpc/instruction_history.cc'
if ($manifest.schema -ne 'runner.t260.s8.component-manifest.v1' -or $manifest.architecture -ne 'x64' -or $manifest.runtimeLibrary -ne '/MT') {
    throw 'Unsupported T260 S8 component manifest.'
}
if (@($manifest.modules).Count -ne 9) { throw 'T261 S7 requires exactly nine linkable component modules.' }
foreach ($module in @($manifest.modules)) {
    if ($module.name -notin @('bochs-core', 'adapter-bochs', 'adapter-bop', 'adapter-softpc', 'adapter-win32', 'opennt-host', 'opennt-bop', 'session', 'app') -or @($module.sources).Count -eq 0) {
        throw 'Module ownership or source list is invalid.'
    }
    foreach ($source in @($module.sources)) {
        if ($source -notmatch '^src/(?:bochs-core(?:-overlay)?|adapter-bochs|adapter-bop|adapter-softpc|adapter-win32|opennt-bop|opennt-host|session|app)/.+\.(c|cc)$' -or
            !(Test-Path -LiteralPath (Join-Path $root $source) -PathType Leaf)) {
            throw "Invalid or missing manifest source: $source"
        }
        if ($source -like 'src/bochs-core-overlay/*' -and $module.name -ne 'bochs-core') {
            throw "Private bochs-core overlay source may be compiled only by bochs-core: $source"
        }
    }
}
foreach ($entry in @($manifest.fixtures) + @($manifest.targets)) {
    if ($null -eq $entry -or [string]::IsNullOrWhiteSpace($entry.name) -or
        [string]::IsNullOrWhiteSpace($entry.source) -or
        !(Test-Path -LiteralPath (Join-Path $root $entry.source) -PathType Leaf)) {
        throw 'Fixture or target admission is invalid.'
    }
    $entrySupportSources = if ($null -ne $entry.PSObject.Properties['supportSources']) {
        @($entry.supportSources)
    } else {
        @()
    }
    foreach ($supportSource in $entrySupportSources) {
        if ([string]::IsNullOrWhiteSpace($supportSource) -or
            $supportSource -notmatch '^tests/.+\.(c|cc)$' -or
            !(Test-Path -LiteralPath (Join-Path $root $supportSource) -PathType Leaf)) {
            throw "Target $($entry.name) has an invalid test-only support source."
        }
    }
}
New-Item -ItemType Directory -Force -Path $build, (Join-Path $build 'obj'), (Join-Path $build 'lib'), (Join-Path $build 'bin') | Out-Null
foreach ($module in @($manifest.modules)) { New-Item -ItemType Directory -Force -Path (Join-Path $build ('obj\' + $module.name)) | Out-Null }
New-Item -ItemType Directory -Force -Path (Join-Path $build 'obj\targets') | Out-Null

$projectionTool = Join-Path $root $manifest.configProjection.tool
$projectionManifest = Join-Path $root $manifest.configProjection.projection
if ($Refresh) {
    & powershell.exe -ExecutionPolicy Bypass -File $projectionTool -ManifestPath $projectionManifest -AnalysisRoot (Join-Path $build 'config') -InputConfigPath (Join-Path $root 'src\bochs-core\config.h') -Refresh
} else {
    & powershell.exe -ExecutionPolicy Bypass -File $projectionTool -ManifestPath $projectionManifest -AnalysisRoot (Join-Path $build 'config') -InputConfigPath (Join-Path $root 'src\bochs-core\config.h')
}
if ($LASTEXITCODE -ne 0) { throw 'CPU5 config projection failed.' }
$config = Join-Path $build 'config\config.h'
if (!(Test-Path -LiteralPath $config -PathType Leaf)) { throw 'CPU5 config projection emitted no config.h.' }
$compiler = & cmd.exe /d /s /c ('call "' + $vs + '" -arch=x64 -host_arch=x64 >nul && (cl.exe /Bv 2>&1 & exit /b 0)') 2>&1
if ($LASTEXITCODE -ne 0) { throw 'Unable to query MSVC identity.' }
$environment = Join-Path $build 'msvc-x64-mt.cmd'
@('@echo off', 'set "NTVDM64_NINJA_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ntvdm64_msvc_ready', ('call "' + $vs + '" -arch=x64 -host_arch=x64 >nul'), 'if errorlevel 1 exit /b %errorlevel%', ':ntvdm64_msvc_ready', 'cd /d "%NTVDM64_NINJA_CALLER_CWD%"', '%*') | Set-Content -LiteralPath $environment -Encoding ascii
$manifestHash = Get-FileSha256 $manifestPath
$configHash = Get-FileSha256 $config
$toolchainHash = Get-TextSha256 (($compiler | Out-String).Trim())
$diagnosticDefineParts = [Collections.Generic.List[string]]::new()
if ($InstructionHistoryDiagnostic -or $InstructionHistoryProvenanceDiagnostic) {
    $diagnosticDefineParts.Add('/DRUNTIME_ENABLE_MACHINE_INSTRUCTION_HISTORY=1')
}
if ($InstructionHistoryProvenanceDiagnostic) {
    $diagnosticDefineParts.Add('/DRUNTIME_ENABLE_MACHINE_INSTRUCTION_HISTORY_PROVENANCE=1')
}
if ($SoftwareInterruptDiagnostic) {
    $diagnosticDefineParts.Add('/DRUNTIME_ENABLE_MACHINE_SOFTWARE_INTERRUPT_OBSERVATION=1')
}
if ($InterruptReturnDiagnostic) {
    $diagnosticDefineParts.Add('/DRUNTIME_ENABLE_MACHINE_INTERRUPT_RETURN_OBSERVATION=1')
}
if ($PhysicalWriteDiagnostic) {
    $diagnosticDefineParts.Add('/DRUNTIME_ENABLE_MACHINE_PHYSICAL_WRITE_OBSERVATION=1')
}
$diagnosticDefines = $diagnosticDefineParts -join ' '
$buildManifest = [ordered]@{
    schema = 'runner.t260.s8.ninja-component-graph.v1'
    sourceManifest = $manifestPath.Substring($root.Length + 1).Replace('\','/')
    sourceManifestSha256 = $manifestHash
    projectedConfigSha256 = $configHash
    msvcIdentitySha256 = $toolchainHash
    ninja = [ordered]@{ path = $ninja.Source; version = (& $ninja.Source --version).Trim() }
    architecture = 'x64'; runtimeLibrary = '/MT'; cpuConfiguration = $manifest.cpuConfiguration
    modules = $manifest.modules; fixtures = $manifest.fixtures; targets = $manifest.targets
    requiredPlatformLibraries = $requiredPlatformLibraries
    forbiddenInputs = $manifest.forbiddenInputs
    instructionHistoryDiagnostic = [bool]$InstructionHistoryDiagnostic
    instructionHistoryProvenanceDiagnostic = [bool]$InstructionHistoryProvenanceDiagnostic
    softwareInterruptDiagnostic = [bool]$SoftwareInterruptDiagnostic
    interruptReturnDiagnostic = [bool]$InterruptReturnDiagnostic
    physicalWriteDiagnostic = [bool]$PhysicalWriteDiagnostic
}
$configurationHash = Get-TextSha256 ($buildManifest | ConvertTo-Json -Depth 10)
$buildManifest.configurationSha256 = $configurationHash
$buildManifest | ConvertTo-Json -Depth 10 | Set-Content -LiteralPath (Join-Path $build 'module-manifest.json') -Encoding utf8

$includeArgs = @($manifest.includeRoots | ForEach-Object { '/I "' + (NinjaPath (Join-Path $root $_)) + '"' }) -join ' '
$envNinja = NinjaPath $environment
$configNinja = NinjaPath $config
$buildNinja = NinjaPath $build
$graph = [Collections.Generic.List[string]]::new()
$graph.Add('ninja_required_version = 1.10')
$graph.Add('configuration_sha256 = ' + $configurationHash)
$graph.Add('build_root = ' + $buildNinja)
$graph.Add('')
$graph.Add('rule cc')
$graph.Add('  command = cmd.exe /d /s /c call ' + $envNinja + ' cl.exe /nologo /TC /c /std:c11 /W4 /WX /MT /DWIN32 /D_CRT_SECURE_NO_WARNINGS /showIncludes /FI"' + $configNinja + '" ' + $includeArgs + ' ' + $diagnosticDefines + ' /Fo$out $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file:')
$graph.Add('  description = CC $out')
$graph.Add('')
$graph.Add('rule cxx')
$graph.Add('  command = cmd.exe /d /s /c call ' + $envNinja + ' cl.exe /nologo /TP /c /std:c++14 /EHsc /MT /Gy /DWIN32 /showIncludes /FI"' + $configNinja + '" ' + $includeArgs + ' ' + $diagnosticDefines + ' /Fo$out $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file:')
$graph.Add('  description = CXX $out')
$graph.Add('')
$graph.Add('rule lib')
$graph.Add('  command = cmd.exe /d /s /c cd /d ' + $buildNinja + ' && call ' + $envNinja + ' lib.exe /nologo /OUT:$build_root/$out @$out.rsp')
$graph.Add('  description = LIB $out')
$graph.Add('')
$graph.Add('rule link')
$graph.Add('  command = cmd.exe /d /s /c cd /d ' + $buildNinja + ' && call ' + $envNinja + ' link.exe /nologo /OPT:REF /OUT:$build_root/$out @$out.rsp')
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
    $libraryResponse = Join-Path $build ($library + '.rsp')
    [IO.File]::WriteAllText($libraryResponse, (($objects -join [Environment]::NewLine) + [Environment]::NewLine), [Text.UTF8Encoding]::new($false))
    $graph.Add('build ' + $library + ': lib ' + ($objects -join ' '))
    $graph.Add('')
    $moduleLibraries[$module.name] = $library
}
$outputs = [Collections.Generic.List[string]]::new()
foreach ($entry in @($manifest.fixtures) + @($manifest.targets)) {
    $entrySupportSources = if ($null -ne $entry.PSObject.Properties['supportSources']) {
        @($entry.supportSources)
    } else {
        @()
    }
    $targetSources = @($entry.source) + $entrySupportSources
    $objects = [Collections.Generic.List[string]]::new()
    foreach ($targetSource in $targetSources) {
        $source = NinjaPath (Join-Path $root $targetSource)
        $object = 'obj/targets/' + (ObjectName $targetSource)
        $rule = if ($targetSource.EndsWith('.cc')) { 'cxx' } else { 'cc' }
        $graph.Add('build ' + $object + ': ' + $rule + ' ' + $source)
        $objects.Add($object)
    }
    $output = 'bin/' + $entry.name + '.exe'
    $libraries = @($entry.libraries | ForEach-Object { if (!$moduleLibraries.ContainsKey($_)) { throw "Target $($entry.name) references unknown module $_" }; $moduleLibraries[$_] })
    $linkResponse = Join-Path $build ($output + '.rsp')
    $platformLibraries = @($entry.platformLibraries + $requiredPlatformLibraries | Select-Object -Unique)
    $targetObjects = @($objects)
    $linkInputs = $targetObjects + $libraries + $platformLibraries
    [IO.File]::WriteAllText($linkResponse, (($linkInputs -join [Environment]::NewLine) + [Environment]::NewLine), [Text.UTF8Encoding]::new($false))
    $graph.Add('build ' + $output + ': link ' + (($targetObjects + $libraries) -join ' '))
    $graph.Add('  platform = ' + ($platformLibraries -join ' '))
    $graph.Add('')
    $outputs.Add($output)
}
$graph.Add('build all: phony ' + ($outputs -join ' '))
$graph.Add('default all')
$graph | Set-Content -LiteralPath (Join-Path $build 'build.ninja') -Encoding ascii
[ordered]@{ schema='runner.t260.s8.ninja-component-graph-record.v1'; buildRoot=$build; graph='build.ninja'; manifest='module-manifest.json'; configurationSha256=$configurationHash; formalCacheExecutor='Ninja'; customCacheDisposition='P1 implementation retired; historical evidence retained' } | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $build 't260-s8-component-graph.json') -Encoding utf8
Write-Host "Generated T260 S8 component Ninja graph: $build"
