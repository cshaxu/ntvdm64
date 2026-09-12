[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$OutputDirectory = 'build/M0-T405/S1/source-coverage-001'
)

$ErrorActionPreference = 'Stop'
if (-not $RepositoryRoot) { $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot) }
$repo = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$output = [IO.Path]::GetFullPath((Join-Path $repo $OutputDirectory))
$buildPrefix = [IO.Path]::GetFullPath((Join-Path $repo 'build')) + [IO.Path]::DirectorySeparatorChar
if (-not $output.StartsWith($buildPrefix, [StringComparison]::OrdinalIgnoreCase)) {
    throw 'Audit output must remain below the repository build directory.'
}
if (Test-Path -LiteralPath $output) { throw 'Use a new audit output directory; do not overwrite evidence.' }
$ledgerRoot = Join-Path $repo 'artifacts/documentation-archive/20260910/etc/operations/ledgers'
$topologyPath = Join-Path $ledgerRoot 'm0-t274-s5-mvdm-host-topology-manifest.tsv'
$supplementPath = Join-Path $ledgerRoot 'm0-t301-s1-opennt-src-2-mvdm-extra-import-manifest.tsv'
$topology = @(Import-Csv -LiteralPath $topologyPath -Delimiter "`t")
$supplement = @(Import-Csv -LiteralPath $supplementPath -Delimiter "`t")
$selected = @{}
$extra = @{}
foreach ($row in $topology) {
    $key = 'src/mvdm-host/' + $row.target_path
    if ($selected.ContainsKey($key)) { throw "Duplicate topology: $key" }
    $selected[$key] = $row
}
foreach ($row in $supplement) {
    if ($extra.ContainsKey($row.target_path)) { throw "Duplicate supplement: $($row.target_path)" }
    $extra[$row.target_path] = $row
}
$tracked = @(& git -C $repo ls-files src)
if ($LASTEXITCODE -ne 0) { throw 'git ls-files failed' }
$trackedSet = @{}
foreach ($name in $tracked) { $trackedSet[$name] = $true }
$rows = foreach ($name in $tracked | Where-Object { $_.StartsWith('src/mvdm-host/') }) {
    $source = ''; $expected = ''; $kind = 'unpaired'; $disposition = ''
    if ($selected.ContainsKey($name)) {
        $record = $selected[$name]
        $source = $record.selected_source_path
        $expected = $record.selected_source_sha256
        $kind = 'selected-topology'
        $disposition = $record.s5_disposition
    } elseif ($extra.ContainsKey($name)) {
        $record = $extra[$name]
        $source = Join-Path $record.source_root $record.source_relative_path
        $expected = $record.source_sha256
        $kind = 'supplement-evidence-only'
        $disposition = $record.disposition
    }
    $currentHash = (Get-FileHash -LiteralPath (Join-Path $repo $name)).Hash.ToLowerInvariant()
    $sourceHash = ''; $identity = 'unpaired'; $sourceVerified = $false
    if ($source) {
        if (Test-Path -LiteralPath $source -PathType Leaf) {
            $sourceHash = (Get-FileHash -LiteralPath $source).Hash.ToLowerInvariant()
            $sourceVerified = $sourceHash -eq $expected
            $identity = if ($currentHash -eq $sourceHash) { 'byte-equal' } else { 'byte-different' }
        } else { $identity = 'source-missing' }
    }
    [pscustomobject]@{ path=$name; provenance=$kind; source=$source; expected_source_sha256=$expected;
        actual_source_sha256=$sourceHash; source_verified=$sourceVerified; current_sha256=$currentHash;
        identity=$identity; historical_disposition=$disposition }
}
$absent = foreach ($name in $selected.Keys | Sort-Object) {
    if ($trackedSet.ContainsKey($name)) { continue }
    $record = $selected[$name]
    $relative = $record.target_path
    $relocated = @(('src/mvdm-tools/' + $relative), ('src/mvdm-softpc-firmware/' + $relative)) |
        Where-Object { $trackedSet.ContainsKey($_) }
    if (@($relocated).Count -gt 1) { throw "Ambiguous relocation: $name" }
    $target = @($relocated) | Select-Object -First 1
    $identity = 'not-at-known-relocation'
    if ($target) {
        $hash = (Get-FileHash -LiteralPath (Join-Path $repo $target)).Hash
        $identity = if ($hash -eq $record.selected_source_sha256) { 'relocated-byte-equal-to-manifest' } else { 'relocated-byte-different' }
    }
    [pscustomobject]@{ old_path=$name; relocated_path=$target; identity=$identity }
}
$head = & git -C $repo rev-parse HEAD
if ($LASTEXITCODE -ne 0) { throw 'git rev-parse failed' }
$summary = [ordered]@{ head=$head; topology_sha256=(Get-FileHash $topologyPath).Hash;
    supplement_sha256=(Get-FileHash $supplementPath).Hash; tracked_host_count=@($rows).Count;
    paired_identities=@($rows | Group-Object provenance,identity,source_verified | Select-Object Name,Count);
    absent_paths=@($absent).Count; relocation_identities=@($absent | Group-Object identity | Select-Object Name,Count);
    limitation='Byte identity and membership only; no semantic, line-diff or actual-link acceptance.' }
$null = New-Item -ItemType Directory -Path $output
$rows | Export-Csv -LiteralPath (Join-Path $output 'host-coverage.csv') -NoTypeInformation -Encoding UTF8
$absent | Export-Csv -LiteralPath (Join-Path $output 'old-paths.csv') -NoTypeInformation -Encoding UTF8
$summary | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $output 'summary.json') -Encoding UTF8
$summary | ConvertTo-Json -Depth 5
