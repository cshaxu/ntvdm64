[CmdletBinding()]
param([string]$RepositoryRoot = '')

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$ops = Join-Path $root 'docs/etc/operations'
$ledger = @(Import-Csv (Join-Path $ops 'm0-t278-destination-source-identity-ledger.tsv') -Delimiter "`t")
if ($ledger.Count -ne 1689) { throw "Expected 1689 frozen paths; found $($ledger.Count)." }

foreach ($row in $ledger) {
    $target = Join-Path $root ('src/' + $row.destination_component + '/' + $row.destination_relative_path)
    if (-not (Test-Path -LiteralPath $target -PathType Leaf)) { throw "Missing frozen target path: $($row.source_path) -> $($row.destination_component)" }
    $hash = (Get-FileHash -LiteralPath $target -Algorithm SHA256).Hash.ToLowerInvariant()
    if ($hash -ne $row.source_sha256) { throw "Hash changed during move: $($row.source_path)" }
    $old = Join-Path $root ('src/mvdm-host/' + $row.source_path)
    if ($row.destination_component -ne 'mvdm-host' -and (Test-Path -LiteralPath $old -PathType Leaf)) { throw "Moved path still exists in host component: $($row.source_path)" }
}

$expectedCounts = @{
    'mvdm-host' = 1491
    'mvdm-support' = 112
    'mvdm-tools' = 28
    'mvdm-softpc-firmware' = 58
}
foreach ($component in $expectedCounts.Keys) {
    $actual = @($ledger | Where-Object destination_component -eq $component).Count
    if ($actual -ne $expectedCounts[$component]) { throw "Unexpected ledger count for ${component}: $actual" }
}
Write-Host 'PASS: T278 S2 exact mirror movement: host=1491; support=112; tools=28; firmware=58; all frozen SHA-256 values retained.'
