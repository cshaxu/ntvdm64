[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [ValidatePattern('^[a-z0-9-]+$')][string]$RunId = 'r1'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$driver = Join-Path $root 'tools\build\Invoke-ModuleObjectCache.ps1'
$base = Join-Path $root ('build\t225-s7\' + $RunId)
$cache = Join-Path $base 'cache'
$fixture = 'tests/bx-vdm/bx_ntvdm_cpu_state_abi_layout_test.c'
$source = 'src/bx-vdm/bx_ntvdm_cpu_state_abi.c'
foreach ($path in @($driver, (Join-Path $root $fixture), (Join-Path $root $source))) {
    if (!(Test-Path -LiteralPath $path -PathType Leaf)) { throw "Required S7 input missing: $path" }
}
foreach ($name in @('cache','r1-cold','r2-hot','r3-invalidation','sandbox')) {
    $path = Join-Path $base $name
    if (Test-Path -LiteralPath $path) { throw "Refusing to overwrite existing S7 verification root: $path" }
}
New-Item -ItemType Directory -Force -Path $base | Out-Null
function Invoke-CacheRun([string]$RunRoot, [string]$RunSource) {
    & powershell.exe -NoProfile -ExecutionPolicy Bypass -File $driver `
        -RepositoryRoot $root -CacheRoot $cache -BuildRoot $RunRoot `
        -ModuleName 'bx-vdm-cpu-state-abi' -ModuleSource $RunSource `
        -FixtureSource $fixture `
        -HostArchitecture x64
    if ($LASTEXITCODE -ne 0) { throw "S7 cache run failed: $RunRoot ($LASTEXITCODE)" }
}
Invoke-CacheRun (Join-Path $base 'r1-cold') $source
Invoke-CacheRun (Join-Path $base 'r2-hot') $source
$sandbox = Join-Path $base 'sandbox\bx_ntvdm_cpu_state_abi.c'
New-Item -ItemType Directory -Force -Path (Split-Path -Parent $sandbox) | Out-Null
[IO.File]::WriteAllText($sandbox, ([IO.File]::ReadAllText((Join-Path $root $source)) + "`r`n/* T225 S7 disposable invalidation witness. */`r`n"), [Text.UTF8Encoding]::new($false))
Invoke-CacheRun (Join-Path $base 'r3-invalidation') ('build/t225-s7/' + $RunId + '/sandbox/bx_ntvdm_cpu_state_abi.c')
Write-Host 'T225 S7 cache phases completed; validating records.'
$cold = Get-Content -LiteralPath (Join-Path $base 'r1-cold\module-object-cache-run.json') -Raw | ConvertFrom-Json
$hot = Get-Content -LiteralPath (Join-Path $base 'r2-hot\module-object-cache-run.json') -Raw | ConvertFrom-Json
$changed = Get-Content -LiteralPath (Join-Path $base 'r3-invalidation\module-object-cache-run.json') -Raw | ConvertFrom-Json
if ($cold.cacheMisses -ne 1 -or $cold.cacheHits -ne 0 -or !$cold.freshLink -or $cold.runExitCode -ne 0) { throw 'S7 cold-build cache contract failed.' }
if ($hot.cacheHits -ne 1 -or $hot.cacheMisses -ne 0 -or !$hot.freshLink -or $hot.runExitCode -ne 0) { throw 'S7 hot-build cache contract failed.' }
if ($changed.cacheMisses -ne 1 -or $changed.cacheHits -ne 0 -or !$changed.freshLink -or $changed.runExitCode -ne 0) { throw 'S7 invalidation cache contract failed.' }
$record = [ordered]@{ schema = 'ntdos64.t225.s7.incremental-cache-probe.v1'; architecture = 'x64'; runtimeLibrary = '/MT'; module = 'bx-vdm-cpu-state-abi'; fixture = $fixture; cache = @{ cold = @{ hits = $cold.cacheHits; misses = $cold.cacheMisses }; hot = @{ hits = $hot.cacheHits; misses = $hot.cacheMisses }; invalidated = @{ hits = $changed.cacheHits; misses = $changed.cacheMisses } }; targetLink = 'fresh per run'; runtimeSemanticChange = $false; cachePublication = 'none; local build/t225-s7 only' }
$record | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath (Join-Path $base 't225-s7-incremental-cache-probe.json') -Encoding utf8
Write-Host "T225 S7 incremental-cache probe passed: $base"