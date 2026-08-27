[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$priorPath = Join-Path $root 'docs/etc/operations/m0-t280-s1-host-control-dem-workset-ledger.tsv'
$outPath = Join-Path $root 'docs/etc/operations/m0-t285-s1-wave2-current-workset.tsv'

if (-not (Test-Path -LiteralPath $priorPath -PathType Leaf)) {
    throw "Missing prior Wave 2 workset: $priorPath"
}

$replacements = [ordered]@{
    'opennt-mvdm-host' = 'mvdm-host'
    'adapter-bop' = 'adapter-mvdm-host-in'
    'adapter-softpc' = 'adapter-mvdm-host-out/softpc'
    'adapter-win32' = 'adapter-mvdm-host-out/win32'
    'adapter-vdm-monitor' = 'adapter-mvdm-host-out/monitor'
    'adapter-redir' = 'adapter-mvdm-host-out/redir'
    'adapter-wow' = 'adapter-mvdm-host-out/wow'
    'adapter-vdd' = 'adapter-mvdm-host-out/vdd'
    'adapter-debugger' = 'adapter-mvdm-host-out/debugger'
}

function Convert-CanonicalText([string]$Text) {
    $result = $Text
    foreach ($entry in $replacements.GetEnumerator()) {
        $result = $result.Replace($entry.Key, $entry.Value)
    }
    return $result
}

$prior = @(Import-Csv -LiteralPath $priorPath -Delimiter "`t")
if ($prior.Count -ne 81) {
    throw "Expected exactly 81 prior Wave 2 rows; found $($prior.Count)."
}

$rows = foreach ($row in $prior) {
    $sourcePath = $row.source_path.Replace('/', [IO.Path]::DirectorySeparatorChar)
    $absolutePath = Join-Path $root (Join-Path 'src/mvdm-host' $sourcePath)
    if (-not (Test-Path -LiteralPath $absolutePath -PathType Leaf)) {
        throw "Missing current selected source: $($row.source_path)"
    }

    [pscustomobject][ordered]@{
        work_item_id = $row.work_item_id.Replace('T280-', 'T285-')
        source_path = $row.source_path
        current_source_sha256 = (Get-FileHash -LiteralPath $absolutePath -Algorithm SHA256).Hash.ToLowerInvariant()
        source_owner = Convert-CanonicalText $row.source_owner
        final_disposition = $row.final_disposition
        implementation_group = $row.implementation_group
        final_owner_or_link_boundary = Convert-CanonicalText $row.final_owner_or_link_boundary
        named_adapter = Convert-CanonicalText $row.named_adapter
        mapping_implication = Convert-CanonicalText $row.mapping_implication
        inherited_evidence = $row.final_evidence
        admission_state = 'T285-S1-current-path-verified; implementation-not-enabled'
    }
}

$demCount = @($rows | Where-Object { $_.source_path -like 'dos/dem/*' }).Count
$hostCount = @($rows | Where-Object { $_.source_path -like 'softpc.new/host/src/*' }).Count
if ($demCount -ne 22 -or $hostCount -ne 59) {
    throw "Wave 2 package denominator changed: DEM=$demCount host=$hostCount."
}

$stale = @($rows | Where-Object {
    $_.final_owner_or_link_boundary -match 'adapter-(bop|softpc|win32|vdm-monitor|redir|wow|vdd|debugger)' -or
    $_.named_adapter -match 'adapter-(bop|softpc|win32|vdm-monitor|redir|wow|vdd|debugger)' -or
    $_.mapping_implication -match 'adapter-(bop|softpc|win32|vdm-monitor|redir|wow|vdd|debugger)'
})
if ($stale.Count -ne 0) {
    throw "Canonicalization left $($stale.Count) stale adapter root records."
}

$rows | Export-Csv -LiteralPath $outPath -Delimiter "`t" -NoTypeInformation -Encoding utf8
Write-Output "Wrote $outPath"
Write-Output "rows=$($rows.Count) dem=$demCount host=$hostCount"
$rows | Group-Object final_disposition | Sort-Object Name | ForEach-Object {
    Write-Output "disposition:$($_.Name)=$($_.Count)"
}
$rows | Group-Object implementation_group | Sort-Object Name | ForEach-Object {
    Write-Output "group:$($_.Name)=$($_.Count)"
}
