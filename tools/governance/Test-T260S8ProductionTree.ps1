[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$ManifestPath = 'tools/build/t260-s8-component-manifest.json'
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$components = @(
    'bochs-core', 'bochs-core-overlay', 'adapter-bochs',
    'opennt-guest', 'opennt-host', 'opennt-host-overlay', 'opennt-bop',
    'opennt-softpc', 'opennt-utils', 'adapter-mvdm-host-in', 'adapter-win32',
    'adapter-softpc', 'session', 'app'
)
$nonProductNames = @(
    'original', 'mirror', 'overlay', 'capability', 'example', 'examples',
    'sample', 'samples', 'reference', 'references', 'fixture', 'fixtures',
    'test', 'tests'
)

foreach ($component in $components) {
    $path = Join-Path $root (Join-Path 'src' $component)
    if (!(Test-Path -LiteralPath $path -PathType Container)) {
        throw "Missing production component root: src/$component"
    }
}

$forbidden = Get-ChildItem -LiteralPath (Join-Path $root 'src') -Directory -Recurse |
    Where-Object { $_.Name.ToLowerInvariant() -in $nonProductNames }
if (@($forbidden).Count -ne 0) {
    $relative = @($forbidden | ForEach-Object {
        $_.FullName.Substring($root.Length + 1).Replace('\', '/')
    }) -join ', '
    throw "Non-production staging directory remains under src/: $relative"
}

$manifest = Get-Content -LiteralPath (Join-Path $root $ManifestPath) -Raw |
    ConvertFrom-Json
$declaredPaths = @(
    $manifest.modules | ForEach-Object { $_.sources }
) + @($manifest.fixtures | ForEach-Object { $_.source }) + @($manifest.targets.source) +
    @($manifest.includeRoots)
foreach ($path in $declaredPaths) {
    if ($path -match '(^|/)(docs/etc/legacy_code|tests/legacy)(/|$)') {
        throw "Formal build input is not production code: $path"
    }
}

$registers = @{
    'opennt-bop'    = 'README.md'
    'opennt-host'   = 'README.md'
    'adapter-softpc' = 'README.md'
    'adapter-win32' = 'README.md'
}
foreach ($component in $registers.Keys) {
    $componentRoot = Join-Path $root (Join-Path 'src' $component)
    $readme = Get-Content -LiteralPath (Join-Path $componentRoot $registers[$component]) -Raw
    $matches = Get-ChildItem -LiteralPath $componentRoot -File -Recurse |
        Select-String -Pattern 'DIVERGENCE\(([A-Z0-9]+-DIV-[0-9]+)\)' -AllMatches
    $ids = @($matches | ForEach-Object {
        $_.Matches | ForEach-Object { $_.Groups[1].Value }
    })
    foreach ($id in @($ids | Select-Object -Unique)) {
        if ($readme -notmatch [regex]::Escape("``$id``")) {
            throw "Divergence is absent from $component README register: $id"
        }
    }
}

$legacyRoot = Join-Path $root 'docs/etc/legacy_code'
if (!(Test-Path -LiteralPath $legacyRoot -PathType Container)) {
    throw 'Missing docs/etc/legacy_code evidence root.'
}

Write-Host "T260 S8 production-tree gate passed: $($components.Count) component roots; no reference/example/test staging directory or archived build input; all source divergence identifiers are README-registered."
