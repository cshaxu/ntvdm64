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
if ($bop.Count -ne 203) { throw "Td S2 P2 expected 203 BOP rows, found $($bop.Count)" }
if ($dependency.Count -ne 117) {
    throw "Td S2 P2 expected 117 dependency rows, found $($dependency.Count)"
}

$dependencyIds = @($dependency | ForEach-Object {
    if ($_ -match '^\| `BOP-DEPENDENCY-(\d{3})`') { [int]$matches[1] }
})
if ((Compare-Object $dependencyIds (1..117))) {
    throw 'Td S2 P2 dependency IDs must be exactly 001 through 117'
}

$text = Get-Content -LiteralPath $trackerPath -Raw
foreach ($heading in @('## Td S2 P2 Interface-Conformance Overlay',
        'Interface disposition', 'Original interface evidence', 'Divergence',
        'Exception', 'Migration conclusion')) {
    if (-not $text.Contains($heading)) { throw "Td S2 P2 missing overlay field: $heading" }
}

Write-Output 'Td S2 P2 interface-audit baseline: PASS (203 BOP + 117 dependency rows; schema present).'
