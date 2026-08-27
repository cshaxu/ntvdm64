[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$InventoryPath = 'docs/etc/operations/m0-t283-s1-platform-abi-candidate-inventory-001.tsv',
    [string]$OutputPath = 'docs/etc/operations/m0-t283-s1-platform-abi-selection-ledger-001.tsv',
    [string]$CanonicalManifestPath = 'docs/etc/operations/m0-t283-s2-platform-abi-canonical-import-manifest-001.tsv'
)

$ErrorActionPreference = 'Stop'

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}

$repositoryRoot = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$inventoryFullPath = Join-Path $repositoryRoot $InventoryPath
$outputFullPath = Join-Path $repositoryRoot $OutputPath
$canonicalManifestFullPath = Join-Path $repositoryRoot $CanonicalManifestPath
$rows = Import-Csv -LiteralPath $inventoryFullPath -Delimiter "`t"

if ($rows.Count -ne 29) {
    throw "Expected 29 platform-ABI inventory rows; found $($rows.Count)."
}

$selection = foreach ($row in $rows) {
    $basis = switch ($row.candidate_class) {
        'one-sided' {
            'only corresponding OpenNT/OpenNT-4.5 logical candidate exists'
            break
        }
        'identical-dual-source' {
            'both logical candidates have the same SHA-256; retain dual provenance'
            break
        }
        'conflicting-content' {
            if ($row.selected_source -notmatch '\\OpenNT\\public\\sdk\\inc\\') {
                throw "Unexpected conflict selection root: $($row.logical_path) -> $($row.selected_source)"
            }
            'OpenNT public-SDK ABI cohort selected as one complete group: current carrier and direct MVDM consumer provenance select it; OpenNT-4.5 candidate is an older non-identical declaration set and is rejected without hybridization'
            break
        }
        default { throw "Unknown candidate class: $($row.candidate_class)" }
    }

    [pscustomobject][ordered]@{
        logical_path = $row.logical_path
        selected_source = $row.selected_source
        selected_sha256 = $row.selected_sha256
        rejected_source = $row.alternate_source
        rejected_sha256 = $row.alternate_sha256
        candidate_class = $row.candidate_class
        selection_basis = $basis
        first_mvdm_source = $row.first_mvdm_source
        direct_occurrences = $row.direct_occurrences
    }
}

$selection | Export-Csv -LiteralPath $outputFullPath -Delimiter "`t" -NoTypeInformation -Encoding utf8
$canonicalManifest = foreach ($row in $selection) {
    [pscustomobject][ordered]@{
        logical_path = $row.logical_path
        target_path = ('source/' + $row.logical_path)
        selected_source = $row.selected_source
        selected_sha256 = $row.selected_sha256
        rejected_source = $row.rejected_source
        rejected_sha256 = $row.rejected_sha256
        candidate_class = $row.candidate_class
        selection_basis = $row.selection_basis
        first_mvdm_source = $row.first_mvdm_source
        direct_occurrences = $row.direct_occurrences
    }
}
$canonicalManifest | Export-Csv -LiteralPath $canonicalManifestFullPath -Delimiter "`t" -NoTypeInformation -Encoding utf8
Write-Host "Wrote $($selection.Count) platform-ABI selection rows: $OutputPath"
Write-Host "Wrote $($canonicalManifest.Count) canonical platform-ABI import rows: $CanonicalManifestPath"
