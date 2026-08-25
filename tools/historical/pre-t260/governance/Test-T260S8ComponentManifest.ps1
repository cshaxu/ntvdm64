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
$manifestFile = Join-Path $root $ManifestPath
$manifest = Get-Content -LiteralPath $manifestFile -Raw | ConvertFrom-Json
$owners = @('bx-core','bx-machine','adapter-bop','adapter-softpc','adapter-win32',
    'opennt-host','opennt-bop','session','app')

if ($manifest.schema -ne 'runner.t260.s8.component-manifest.v1' -or
    @($manifest.modules).Count -ne $owners.Count) {
    throw 'T260 S8 component manifest schema or module count is invalid.'
}
foreach ($owner in $owners) {
    $module = @($manifest.modules | Where-Object { $_.name -eq $owner })
    if ($module.Count -ne 1 -or @($module[0].sources).Count -eq 0) {
        throw "Missing or empty component module: $owner"
    }
}
foreach ($module in @($manifest.modules)) {
    foreach ($source in @($module.sources)) {
        if ($source -notmatch ('^src/' + [regex]::Escape($module.name) + '/.+\.(c|cc)$') -or
            !(Test-Path -LiteralPath (Join-Path $root $source) -PathType Leaf)) {
            throw "Invalid component-owned source: $source"
        }
    }
}
foreach ($includeRoot in @($manifest.includeRoots)) {
    if ($includeRoot -match '^src/(opennt|bx-vdm|cli)(/|$)' -or
        !(Test-Path -LiteralPath (Join-Path $root $includeRoot) -PathType Container)) {
        throw "Invalid or legacy include root: $includeRoot"
    }
}
if ($manifest.guestArtifactComponent.name -ne 'opennt-guest' -or
    $manifest.guestArtifactComponent.linkable -ne $false -or
    !(Test-Path -LiteralPath (Join-Path $root $manifest.guestArtifactComponent.root) -PathType Container)) {
    throw 'OpenNT guest artifact component declaration is invalid.'
}
foreach ($entry in @($manifest.fixtures) + @($manifest.targets)) {
    if (!(Test-Path -LiteralPath (Join-Path $root $entry.source) -PathType Leaf)) {
        throw "Missing fixture or target source: $($entry.source)"
    }
    foreach ($library in @($entry.libraries)) {
        if ($library -notin $owners) { throw "Legacy or unknown library: $library" }
    }
}
foreach ($legacyRoot in @('src/opennt','src/bx-vdm','src/cli')) {
    if (Test-Path -LiteralPath (Join-Path $root $legacyRoot)) {
        throw "Legacy source root remains: $legacyRoot"
    }
}
Write-Host "T260 S8 component manifest passed: $(@($manifest.modules | ForEach-Object { $_.sources }).Count) sources, $(@($manifest.fixtures).Count) fixtures, $(@($manifest.targets).Count) targets."
