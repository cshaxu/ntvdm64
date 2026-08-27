[CmdletBinding()]
param(
    [string]$RepositoryRoot = ''
)

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}

$ErrorActionPreference = 'Stop'
$repositoryRoot = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$operationsRoot = Join-Path $repositoryRoot 'docs/etc/operations'
$unionPath = Join-Path $operationsRoot 'm0-t274-s1-non-guest-mvdm-union-ledger.tsv'
$fileLedgerPath = Join-Path $operationsRoot 'mvdm-file-recovery-ledger.tsv'
$packageLedgerPath = Join-Path $operationsRoot 'mvdm-package-dependency-ledger.tsv'
$evidencePath = Join-Path $operationsRoot 'm0-t276-s1-file-identity-taxonomy-001.md'
$mirrorRoot = Join-Path $repositoryRoot 'src/mvdm-host'

foreach ($path in @($unionPath, $fileLedgerPath, $packageLedgerPath)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) {
        throw "Missing canonical audit input: $path"
    }
}

$union = Import-Csv -LiteralPath $unionPath -Delimiter "`t" |
    Where-Object { $_.owner_root -eq 'mvdm-host' } |
    Sort-Object target_path
if ($union.Count -ne 1689) {
    throw "Expected 1,689 selected host paths; found $($union.Count)."
}

$existing = @{}
foreach ($row in (Import-Csv -LiteralPath $fileLedgerPath -Delimiter "`t")) {
    if ($existing.ContainsKey($row.target_path)) {
        throw "Duplicate existing file-ledger path: $($row.target_path)"
    }
    $existing[$row.target_path] = $row
}
if ($existing.Count -ne 1689) {
    throw "Expected 1,689 initialized file-ledger paths; found $($existing.Count)."
}

$failures = [System.Collections.Generic.List[string]]::new()
$kindCounts = @{}
$packageCounts = @{}
$output = [System.Collections.Generic.List[object]]::new()
$index = 0
foreach ($row in $union) {
    $index++
    if (-not $existing.ContainsKey($row.target_path)) {
        $failures.Add("Missing file-ledger path: $($row.target_path)")
        continue
    }
    $old = $existing[$row.target_path]
    $sourcePath = $row.primary_path
    $expectedHash = $row.primary_sha256
    if ([string]::IsNullOrWhiteSpace($sourcePath) -and
        $row.selection -match 'opennt-4\.5' -and
        -not [string]::IsNullOrWhiteSpace($row.secondary_path)) {
        $sourcePath = $row.secondary_path
        $expectedHash = $row.secondary_sha256
    }
    $targetPath = Join-Path $mirrorRoot ($row.target_path.Replace('/', '\'))
    if ([string]::IsNullOrWhiteSpace($sourcePath) -or
        -not (Test-Path -LiteralPath $sourcePath -PathType Leaf)) {
        $failures.Add("Missing selected source: $sourcePath")
        continue
    }
    if (-not (Test-Path -LiteralPath $targetPath -PathType Leaf)) {
        $failures.Add("Missing mirror path: $($row.target_path)")
        continue
    }
    $sourceHash = (Get-FileHash -LiteralPath $sourcePath -Algorithm SHA256).Hash.ToLowerInvariant()
    $targetHash = (Get-FileHash -LiteralPath $targetPath -Algorithm SHA256).Hash.ToLowerInvariant()
    if ($sourceHash -ne $expectedHash.ToLowerInvariant()) {
        $failures.Add("Canonical source hash mismatch: $($row.target_path)")
        continue
    }
    if ($targetHash -ne $sourceHash) {
        $failures.Add("Mirror hash mismatch: $($row.target_path)")
        continue
    }
    if (-not $kindCounts.ContainsKey($old.file_kind)) { $kindCounts[$old.file_kind] = 0 }
    if (-not $packageCounts.ContainsKey($row.package_root)) { $packageCounts[$row.package_root] = 0 }
    $kindCounts[$old.file_kind]++
    $packageCounts[$row.package_root]++
    $output.Add([pscustomobject]@{
        file_id = ('MVDM-FILE-{0:D4}' -f $index)
        target_path = $row.target_path
        package_root = $row.package_root
        file_kind = $old.file_kind
        selection = $row.selection
        selected_source_path = $sourcePath
        selected_source_sha256 = $sourceHash
        mirror_identity = 'byte-exact-verified'
        composition_class = $old.composition_class
        build_profile_state = $old.build_profile_state
        divergence_or_exclusion_id = $old.divergence_or_exclusion_id
        final_non_recovery_reason = $old.final_non_recovery_reason
        evidence = 'm0-t276-s1-file-identity-taxonomy-001.md'
    })
}

if ($packageCounts.Count -ne 23) {
    $failures.Add("Expected 23 package roots; found $($packageCounts.Count)")
}
foreach ($package in (Import-Csv -LiteralPath $packageLedgerPath -Delimiter "`t")) {
    if (-not $packageCounts.ContainsKey($package.package_root)) {
        $failures.Add("Package ledger root has no selected path: $($package.package_root)")
    } elseif ([int]$package.selected_path_count -ne $packageCounts[$package.package_root]) {
        $failures.Add("Package count mismatch: $($package.package_root)")
    }
}
if ($output.Count -ne 1689) {
    $failures.Add("Expected 1,689 audited output rows; found $($output.Count)")
}
if ($failures.Count -gt 0) {
    $failures | ForEach-Object { Write-Error $_ }
    throw "T276 S1 identity audit found $($failures.Count) discrepancy/discrepancies."
}

$output | Export-Csv -LiteralPath $fileLedgerPath -Delimiter "`t" -NoTypeInformation -Encoding utf8
$kindLines = $kindCounts.Keys | Sort-Object | ForEach-Object { '- `' + $_ + '`: ' + $kindCounts[$_] }
$packageLines = $packageCounts.Keys | Sort-Object | ForEach-Object { '- `' + $_ + '`: ' + $packageCounts[$_] }
$lines = @(
    '# M0 T276 S1 file identity and package taxonomy',
    '',
    '## Inputs',
    '',
    '- `m0-t274-s1-non-guest-mvdm-union-ledger.tsv` (canonical selected source);',
    '- `mvdm-file-recovery-ledger.tsv` (one row per selected path);',
    '- `mvdm-package-dependency-ledger.tsv` (declared package counts); and',
    '- local `src/mvdm-host` mirror plus selected external source paths.',
    '',
    '## Procedure',
    '',
    'For every selected host path, hash the canonical selected source and local',
    'mirror, compare both with the canonical union hash, then rebuild the file',
    'ledger in deterministic target-path order. The procedure does not compile,',
    'link, enable or alter a source body.',
    '',
    '## Result',
    '',
    "- Verified paths: $($output.Count)",
    "- Verified package roots: $($packageCounts.Count)",
    '- Mirror identity: all rows are `byte-exact-verified`.',
    '- Composition and build-profile fields remain `unreviewed`.',
    '',
    '### File kinds',
    '',
    $kindLines,
    '',
    '### Package path counts',
    '',
    $packageLines,
    '',
    '## Limitation',
    '',
    'This is source identity and taxonomy evidence only. It makes no claim that',
    'any file is directly composable, adapter-bound, linked, or runnable.',
    ''
)
Set-Content -LiteralPath $evidencePath -Value $lines -Encoding utf8
Write-Host "Verified $($output.Count) paths across $($packageCounts.Count) package roots."
