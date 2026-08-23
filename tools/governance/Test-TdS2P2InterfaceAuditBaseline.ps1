param(
    [string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..'))
)

$ErrorActionPreference = 'Stop'
$trackerPath = Join-Path $RepositoryRoot 'docs\etc\bop-list.md'
$planPath = Join-Path $RepositoryRoot 'docs\etc\operations\td-interface-conformance-audit-plan-001.md'

foreach ($path in @($trackerPath, $planPath)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) {
        throw "Td S2 P2 missing input: $path"
    }
}

$lines = Get-Content -LiteralPath $trackerPath
$bop = @($lines | Where-Object { $_ -match '^\| `BOP-(?!DEPENDENCY-)' })
$dependency = @($lines | Where-Object { $_ -match '^\| `BOP-DEPENDENCY-' })
$bopIds = @($bop | ForEach-Object {
    if ($_ -match '^\| `(BOP-(?!DEPENDENCY-)[^`]+)`') { $matches[1] }
} | Sort-Object -Unique)
if ($bopIds.Count -ne 203) { throw "Td S2 P2 expected 203 unique BOP rows, found $($bopIds.Count)" }
$dependencyIds = @($dependency | ForEach-Object {
    if ($_ -match '^\| `BOP-DEPENDENCY-(\d{3})`') { [int]$matches[1] }
} | Sort-Object -Unique)
if ($dependencyIds.Count -ne 118) {
    throw "Td S2 P2 expected 118 unique dependency IDs, found $($dependencyIds.Count)"
}
if ((Compare-Object $dependencyIds (1..118))) {
    throw 'Td S2 P2 dependency IDs must be exactly 001 through 118'
}

$text = Get-Content -LiteralPath $trackerPath -Raw
foreach ($heading in @('## Td S2 P2 Interface-Conformance Overlay',
        'Interface disposition', 'Original interface evidence', 'Divergence',
        'Exception', 'Migration conclusion')) {
    if (-not $text.Contains($heading)) { throw "Td S2 P2 missing overlay field: $heading" }
}

Write-Output 'Td S2 P2 interface-audit baseline: PASS (203 BOP + 118 dependency rows; schema present).'
