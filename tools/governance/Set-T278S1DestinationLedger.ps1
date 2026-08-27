[CmdletBinding()]
param([string]$RepositoryRoot = '')

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$ops = Join-Path $root 'docs/etc/operations'
$union = @(Import-Csv (Join-Path $ops 'm0-t274-s1-non-guest-mvdm-union-ledger.tsv') -Delimiter "`t")
$selected = @(Import-Csv (Join-Path $ops 'm0-t277-file-recovery-ledger.tsv') -Delimiter "`t")
if ($selected.Count -ne 1689) { throw "Expected 1689 selected T277 file paths; found $($selected.Count)." }
$selectedPath = @{}
foreach ($row in $selected) { $selectedPath[$row.source_path] = $true }
$union = @($union | Where-Object { $selectedPath.ContainsKey($_.target_path) })
if ($union.Count -ne 1689) { throw "Expected the T274 union to supply all 1689 selected paths; found $($union.Count)." }

function Get-Destination([string]$Path, [string]$Package) {
    if ($Package -in @('dirs','makefil0','inc','oemuni','suballoc')) { return 'mvdm-support' }
    if ($Package -eq 'vdmutils') { return 'mvdm-tools' }
    if ($Path -match '^softpc\.new/(base/bios|bios|roms|data)(/|$)') { return 'mvdm-softpc-firmware' }
    return 'mvdm-host'
}

$rows = foreach ($entry in $union | Sort-Object target_path) {
    $destination = Get-Destination $entry.target_path $entry.package_root
    $current = Join-Path $root ('src/mvdm-host/' + $entry.target_path)
    if (-not (Test-Path -LiteralPath $current -PathType Leaf)) { throw "Canonical source is missing: $($entry.target_path)" }
    $currentHash = (Get-FileHash -LiteralPath $current -Algorithm SHA256).Hash.ToLowerInvariant()
    $expectedHash = if ([string]::IsNullOrWhiteSpace($entry.primary_sha256)) { $entry.secondary_sha256 } else { $entry.primary_sha256 }
    if ([string]::IsNullOrWhiteSpace($expectedHash)) { throw "Canonical union has no selected hash: $($entry.target_path)" }
    if ($currentHash -ne $expectedHash.ToLowerInvariant()) { throw "Canonical source hash mismatch: $($entry.target_path)" }
    [pscustomobject]@{
        re_root_id = ('MVDM-T278-S1-' + $entry.target_path.Replace('/','-').Replace('.','-').ToUpperInvariant())
        source_path = $entry.target_path
        package_root = $entry.package_root
        source_sha256 = $expectedHash.ToLowerInvariant()
        current_component = 'mvdm-host'
        destination_component = $destination
        destination_relative_path = $entry.target_path
        movement_state = 'frozen-pre-move'
        source_identity_evidence = 'm0-t274-s1-non-guest-mvdm-union-ledger.tsv selected primary/secondary SHA-256; current canonical mirror hash verified'
    }
}
$out = Join-Path $ops 'm0-t278-destination-source-identity-ledger.tsv'
$rows | Export-Csv -LiteralPath $out -Delimiter "`t" -NoTypeInformation -Encoding utf8
Write-Host "Wrote $($rows.Count) frozen MVDM destination rows."
