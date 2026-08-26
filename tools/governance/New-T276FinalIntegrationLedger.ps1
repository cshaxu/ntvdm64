[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path $RepositoryRoot).Path
$ops = Join-Path $root 'docs/etc/operations'
$source = @(Import-Csv (Join-Path $ops 'mvdm-file-recovery-ledger.tsv') -Delimiter "`t" | Sort-Object target_path)
if ($source.Count -ne 1689) { throw "Expected 1689 selected MVDM paths, found $($source.Count)." }
$out = foreach ($row in $source) {
    [pscustomobject][ordered]@{
        file_id = $row.file_id
        source_path = $row.target_path
        package_root = ($row.target_path -split '/')[0]
        file_kind = $row.file_kind
        final_disposition = 'unresolved'
        final_owner_or_link_boundary = 'unresolved'
        final_change_class = 'unresolved'
        named_adapter = 'unresolved'
        mapping_implication = 'unresolved'
        final_evidence = 'T276 source/build/package audit required'
        final_audit_state = 'unreviewed'
    }
}
$out | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" | Set-Content (Join-Path $ops 'm0-t276-final-integration-disposition-ledger.tsv') -Encoding utf8
Write-Output "T276 final expected-integration tracker generated: $($out.Count) unresolved rows."
