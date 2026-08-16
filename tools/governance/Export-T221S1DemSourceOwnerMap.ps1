[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$OutputPath
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$root = [IO.Path]::GetFullPath($RepositoryRoot)
$routeAudit = Join-Path $root 'docs/etc/research/t221-s1-dem-current-bound-route-audit-001.json'
if (-not (Test-Path -LiteralPath $routeAudit -PathType Leaf)) {
    throw 'T221 S1 P1 route audit is required before the source-owner map'
}

$decoded = ConvertFrom-Json -InputObject (Get-Content -LiteralPath $routeAudit -Raw)
$auditEntries = @($decoded.entries)
if ($auditEntries.Count -ne 73) { throw "Expected 73 audited DEM entries, got $($auditEntries.Count)" }

$demRoot = Join-Path $root 'src/opennt/base/mvdm/dos/dem'
$sourceFiles = @(Get-ChildItem -LiteralPath $demRoot -Recurse -File -Filter '*.c')
if ($sourceFiles.Count -eq 0) { throw 'OpenNT DEM C sources are absent' }

$entries = foreach ($entry in ($auditEntries | Sort-Object { [Convert]::ToInt32($_.identity.Substring(3), 16) })) {
    $pattern = '\b' + [regex]::Escape($entry.originalHandler) + '\s*\('
    $candidates = @($sourceFiles | Where-Object {
        Select-String -LiteralPath $_.FullName -Pattern $pattern -Quiet
    } | ForEach-Object {
        $_.FullName.Substring($root.Length + 1).Replace('\', '/')
    })
    if ($candidates.Count -ne 1) {
        throw "Expected exactly one DEM C implementation candidate for $($entry.identity) $($entry.originalHandler), got $($candidates.Count): $($candidates -join '; ')"
    }
    [pscustomobject]@{
        identity = $entry.identity
        originalHandler = $entry.originalHandler
        originalDispatcher = $entry.originalDispatcher
        originalImplementation = $candidates[0]
        currentBoundRoute = $entry.currentBoundRoute
        currentRouteAudit = 'T221 S1 P1 current-source static route audit'
        recoveryRung = 'pending: source/ABI/failure analysis'
    }
}

$byImplementation = @($entries | Group-Object originalImplementation | Sort-Object Name |
    ForEach-Object {
        [pscustomobject]@{
            source = $_.Name
            serviceCount = $_.Count
            services = @($_.Group | ForEach-Object { $_.identity })
        }
    })
if ($entries.Count -ne 73 -or $byImplementation.Count -eq 0) {
    throw 'Source-owner map did not retain the full DEM inventory'
}

$output = [IO.Path]::GetFullPath($OutputPath)
$parent = Split-Path -Parent $output
if (-not (Test-Path -LiteralPath $parent)) { New-Item -ItemType Directory -Path $parent | Out-Null }
[ordered]@{
    schema = 'ntdos64.t221.s1.dem-source-owner-map.v1'
    generatedUtc = [DateTime]::UtcNow.ToString('o')
    sourceRoot = 'src/opennt/base/mvdm/dos/dem'
    sourceCount = $sourceFiles.Count
    serviceCount = $entries.Count
    staticOnly = $true
    byOriginalImplementation = $byImplementation
    entries = $entries
} | ConvertTo-Json -Depth 7 | Set-Content -LiteralPath $output -Encoding utf8
Write-Host "Exported $($entries.Count) DEM handler-to-source rows to $output"
