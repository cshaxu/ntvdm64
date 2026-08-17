[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$CacheRoot,
    [Parameter(Mandatory = $true)][string]$BuildRoot,
    [Parameter(Mandatory = $true)][string]$ModuleName,
    [Parameter(Mandatory = $true)][string[]]$ModuleSource,
    [Parameter(Mandatory = $true)][string]$FixtureSource,
    [string[]]$IncludeRoot = @('src', 'src/bx-vdm', 'src/cli'),
    [ValidateSet('x64')][string]$HostArchitecture = 'x64'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Get-Sha256([string]$Path) {
    return (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant()
}

function Get-TextSha256([string]$Text) {
    $sha = [Security.Cryptography.SHA256]::Create()
    try {
        return [BitConverter]::ToString($sha.ComputeHash([Text.Encoding]::UTF8.GetBytes($Text))).Replace('-', '').ToLowerInvariant()
    } finally { $sha.Dispose() }
}

function Resolve-LocalDependencies([string]$Source, [string[]]$Roots) {
    $visited = [System.Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
    $result = [System.Collections.Generic.List[object]]::new()
    function Visit([string]$Path) {
        $full = [IO.Path]::GetFullPath($Path)
        if (!$visited.Add($full)) { return }
        $result.Add([ordered]@{ path = $full; sha256 = Get-Sha256 $full })
        foreach ($line in [IO.File]::ReadLines($full)) {
            if ($line -notmatch '^\s*#\s*include\s*[<\"]([^>\"]+)[>\"]') { continue }
            $include = $Matches[1]
            $candidate = Join-Path ([IO.Path]::GetDirectoryName($full)) $include
            if (!(Test-Path -LiteralPath $candidate -PathType Leaf)) {
                foreach ($root in $Roots) {
                    $candidate = Join-Path $root $include
                    if (Test-Path -LiteralPath $candidate -PathType Leaf) { break }
                }
            }
            if (Test-Path -LiteralPath $candidate -PathType Leaf) { Visit $candidate }
        }
    }
    Visit $Source
    return @($result | Sort-Object path)
}

$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$cache = [IO.Path]::GetFullPath($CacheRoot)
$build = [IO.Path]::GetFullPath($BuildRoot)
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (!(Test-Path -LiteralPath $vs -PathType Leaf)) { throw "MSVC environment entry point missing: $vs" }
$roots = @($IncludeRoot | ForEach-Object { (Resolve-Path -LiteralPath (Join-Path $root $_)).Path })
$includeArgs = @($roots | ForEach-Object { '/I "' + $_ + '"' }) -join ' '
# cl /Bv intentionally returns a nonzero status when it has no input.  Capture
# its identity text but normalize that probe result without normalizing any
# actual compilation or link command below.
$compilerOutput = & cmd.exe /d /s /c ('call "' + $vs + '" -arch=' + $HostArchitecture + ' -host_arch=x64 >nul && (cl.exe /Bv 2>&1 & exit /b 0)') 2>&1
if ($LASTEXITCODE -ne 0) { throw 'Unable to identify MSVC compiler.' }
$compilerIdentity = ($compilerOutput | Out-String).Trim()
if ([string]::IsNullOrWhiteSpace($compilerIdentity)) { throw 'MSVC compiler identity is empty.' }
$toolchain = [ordered]@{
    compilerSha256 = Get-TextSha256 $compilerIdentity
    architecture = $HostArchitecture
    runtimeLibrary = '/MT'
    language = 'C11'
    defines = @('WIN32', '_CRT_SECURE_NO_WARNINGS')
    includeRoots = $roots
}
New-Item -ItemType Directory -Force -Path $cache, $build | Out-Null
$moduleCache = Join-Path $cache ('modules\' + $ModuleName)
$moduleBuild = Join-Path $build ('modules\' + $ModuleName)
New-Item -ItemType Directory -Force -Path $moduleCache, $moduleBuild | Out-Null
$entries = [System.Collections.Generic.List[object]]::new()
$objects = [System.Collections.Generic.List[string]]::new()
$hits = 0; $misses = 0
foreach ($relative in $ModuleSource) {
    $source = Join-Path $root $relative
    if (!(Test-Path -LiteralPath $source -PathType Leaf)) { throw "Module source missing: $relative" }
    $deps = Resolve-LocalDependencies $source $roots
    $identity = [ordered]@{ schema = 'ntdos64.module-object-cache.v1'; module = $ModuleName; source = $relative.Replace('\','/'); dependencies = $deps; toolchain = $toolchain }
    $identityJson = $identity | ConvertTo-Json -Depth 8 -Compress
    $key = Get-TextSha256 $identityJson
    $entryRoot = Join-Path $moduleCache $key
    $cachedObject = Join-Path $entryRoot 'module.obj'
    $manifest = Join-Path $entryRoot 'manifest.json'
    $hit = (Test-Path -LiteralPath $cachedObject -PathType Leaf) -and (Test-Path -LiteralPath $manifest -PathType Leaf) -and ((Get-Content -LiteralPath $manifest -Raw) -eq ($identity | ConvertTo-Json -Depth 8))
    if (!$hit) {
        New-Item -ItemType Directory -Force -Path $entryRoot | Out-Null
        $compile = 'call "' + $vs + '" -arch=' + $HostArchitecture + ' -host_arch=x64 >nul && cl.exe /nologo /TC /c /std:c11 /W4 /WX /MT /DWIN32 /D_CRT_SECURE_NO_WARNINGS ' + $includeArgs + ' /Fo"' + $cachedObject + '" "' + $source + '"'
        $output = & cmd.exe /d /s /c $compile 2>&1
        if ($LASTEXITCODE -ne 0) { throw "Module object compilation failed: $relative`n$($output | Out-String)" }
        [IO.File]::WriteAllText($manifest, ($identity | ConvertTo-Json -Depth 8), [Text.UTF8Encoding]::new($false))
        $misses++
    } else { $hits++ }
    $staged = Join-Path $moduleBuild (([IO.Path]::GetFileNameWithoutExtension($relative)) + '-' + $key.Substring(0,16) + '.obj')
    Copy-Item -LiteralPath $cachedObject -Destination $staged -Force
    $objects.Add($staged)
    $entries.Add([ordered]@{ source = $relative.Replace('\','/'); cacheKey = $key; cacheHit = $hit; dependencies = $deps })
}
$library = Join-Path $build ($ModuleName + '.lib')
$libraryResponse = Join-Path $build ($ModuleName + '.rsp')
@('/nologo', ('/OUT:"' + $library + '"')) + @($objects | ForEach-Object { '"' + $_ + '"' }) | Set-Content -LiteralPath $libraryResponse -Encoding ascii
$libOutput = & cmd.exe /d /s /c ('call "' + $vs + '" -arch=' + $HostArchitecture + ' -host_arch=x64 >nul && lib.exe @"' + $libraryResponse + '"') 2>&1
if ($LASTEXITCODE -ne 0) { throw "Module archive failed: $ModuleName`n$($libOutput | Out-String)" }
$fixture = Join-Path $root $FixtureSource
if (!(Test-Path -LiteralPath $fixture -PathType Leaf)) { throw "Fixture source missing: $FixtureSource" }
$fixtureObject = Join-Path $build 'fixture.obj'
$fixtureCompile = 'call "' + $vs + '" -arch=' + $HostArchitecture + ' -host_arch=x64 >nul && cl.exe /nologo /TC /c /std:c11 /W4 /WX /MT /DWIN32 /D_CRT_SECURE_NO_WARNINGS ' + $includeArgs + ' /Fo"' + $fixtureObject + '" "' + $fixture + '"'
$fixtureOutput = & cmd.exe /d /s /c $fixtureCompile 2>&1
if ($LASTEXITCODE -ne 0) { throw "Fixture compilation failed: $FixtureSource`n$($fixtureOutput | Out-String)" }
$exe = Join-Path $build ($ModuleName + '-fixture.exe')
$linkResponse = Join-Path $build 'link.rsp'
@('/nologo', ('/OUT:"' + $exe + '"'), '/OPT:REF', ('"' + $fixtureObject + '"'), ('"' + $library + '"')) | Set-Content -LiteralPath $linkResponse -Encoding ascii
$linkOutput = & cmd.exe /d /s /c ('call "' + $vs + '" -arch=' + $HostArchitecture + ' -host_arch=x64 >nul && link.exe @"' + $linkResponse + '"') 2>&1
if ($LASTEXITCODE -ne 0) { throw "Fresh fixture link failed: $ModuleName`n$($linkOutput | Out-String)" }
& $exe
$runExit = $LASTEXITCODE
if ($runExit -ne 0) { throw "Fixture execution failed: $runExit" }
$record = [ordered]@{ schema = 'ntdos64.module-object-cache-run.v1'; module = $ModuleName; cacheRoot = $cache; buildRoot = $build; toolchain = $toolchain; entries = $entries; cacheHits = $hits; cacheMisses = $misses; library = $library; fixture = $FixtureSource.Replace('\','/'); freshLink = $true; runExitCode = $runExit }
$record | ConvertTo-Json -Depth 10 | Set-Content -LiteralPath (Join-Path $build 'module-object-cache-run.json') -Encoding utf8
Write-Host "Module cache run completed: module=$ModuleName hits=$hits misses=$misses build=$build"
exit 0