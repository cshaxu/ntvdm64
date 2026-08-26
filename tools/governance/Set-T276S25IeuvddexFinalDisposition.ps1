[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path $RepositoryRoot).Path
$path = Join-Path $root 'docs/etc/operations/m0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t")
$debug = @($rows | Where-Object {$_.source_path -like 'ieuvddex/*'})
if ($debug.Count -ne 23) { throw "Expected 23 IEUVDDEx rows, found $($debug.Count)." }
foreach ($row in $debug) {
    $row.final_disposition = 'not-host-runtime'
    $row.final_owner_or_link_boundary = 'retained historical NTSD/WinDbg debugger-extension evidence; future same-shaped extension boundary is adapter-debugger, not the app host runtime'
    $row.final_change_class = 'none; byte-exact source mirror retained outside the final host link graph'
    $row.named_adapter = 'adapter-debugger future extension boundary; session publishes copied diagnostic snapshots only'
    $row.mapping_implication = 'no direct host-process/guest pointer access; any future diagnostic data is copied from session-owned state and never creates a second guest-memory manager'
    $row.final_evidence = 'T276 S18: ieuvddex is the historical ntvdm.dll NTSD/WinDbg extension, not a VDD or host runtime provider'
    $row.final_audit_state = 'final-disposition-audited; not implemented'
}
$rows | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" | Set-Content $path -Encoding utf8
Write-Output 'T276 S25 IEUVDDEx final dispositions applied: 23 not-host-runtime rows.'
