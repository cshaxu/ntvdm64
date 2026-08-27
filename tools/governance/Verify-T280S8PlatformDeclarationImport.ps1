[CmdletBinding()]
param([string]$RepositoryRoot = '')

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$plan = Join-Path $root 'docs/etc/operations/m0-t280-s8-dem-platform-declaration-import-plan-001.md'
$ledger = Join-Path $root 'docs/etc/operations/m0-t280-s8-platform-declaration-import-ledger.tsv'
$manifest = Join-Path $root 'docs/etc/operations/m0-t280-s8-platform-declaration-import-manifest.tsv'
foreach ($path in @($plan, $ledger, $manifest)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing T280 S8 artifact: $path" }
}
$rows = Import-Csv -LiteralPath $manifest -Delimiter "`t"
$expected = @('ntdddisk.h','ntddstor.h','ntioapi.h')
if ($rows.Count -ne $expected.Count) { throw "Expected three imported declaration headers; found $($rows.Count)." }
foreach ($name in $expected) {
    $row = @($rows | Where-Object include -eq $name)
    if ($row.Count -ne 1) { throw "Expected one manifest row for $name." }
    if ($row[0].action -notin @('copied-byte-identical','already-identical')) { throw "$name has invalid import action: $($row[0].action)" }
    $sourceHash = (Get-FileHash -LiteralPath $row[0].source_path -Algorithm SHA256).Hash.ToLowerInvariant()
    $target = Join-Path $root ('src/mvdm-platform-abi/' + $row[0].target_path)
    if (-not (Test-Path -LiteralPath $target -PathType Leaf)) { throw "Missing destination: $target" }
    $targetHash = (Get-FileHash -LiteralPath $target -Algorithm SHA256).Hash.ToLowerInvariant()
    if ($sourceHash -ne $row[0].source_sha256 -or $targetHash -ne $row[0].source_sha256) { throw "$name is not byte-identical to its manifest source." }
}
$planText = Get-Content -LiteralPath $plan -Raw
foreach ($token in @('does not enable a DEM body','mapping-manager','without modification')) {
    if ($planText -notmatch [regex]::Escape($token)) { throw "T280 S8 plan misses boundary token: $token" }
}
Write-Host 'PASS: T280 S8 imports exactly three byte-identical OpenNT declaration carriers.'
