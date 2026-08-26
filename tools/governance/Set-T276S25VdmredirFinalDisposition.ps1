[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path $RepositoryRoot).Path
$path = Join-Path $root 'docs/etc/operations/m0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t")
$redir = @($rows | Where-Object {$_.source_path -like 'vdmredir/*'})
if ($redir.Count -ne 25) { throw "Expected 25 Redirector rows, found $($redir.Count)." }
foreach ($row in $redir) {
    $row.final_audit_state = 'final-disposition-audited; not implemented'
    if ($row.file_kind -eq 'source') {
        $row.final_disposition = 'adapter-backed'
        $row.final_owner_or_link_boundary = 'opennt-mvdm-host original vdmredir provider -> adapter-redir protocol boundary -> adapter-win32/session; BOP consumers remain opennt-bop'
        $row.final_change_class = 'binding-only; retain original Redirector protocol ordering, dispatch and failure semantics'
        $row.named_adapter = 'adapter-redir; adapter-win32; session'
        $row.mapping_implication = 'DOS far buffers use checked session guest-memory leases/copies; host handles and async completion IDs remain session-owned'
        $row.final_evidence = 'T276 S21: vdmredir is one original DLL owner package of 15 provider bodies; no standalone file-redirector shim may replace it'
    } elseif ($row.file_kind -eq 'declaration') {
        $row.final_disposition = 'binding-only'
        $row.final_owner_or_link_boundary = 'opennt-platform-abi Redirector protocol declaration supplied to the original provider and adapter-redir binding'
        $row.final_change_class = 'binding-only; retain original layout/protocol declarations'
        $row.named_adapter = 'adapter-redir; adapter-win32; session'
        $row.mapping_implication = 'declarations transfer no native identity; DOS pointers and host handles follow the session mapping-manager/host-resource rule'
        $row.final_evidence = 'T276 S21: Redirector declaration and same-shaped adapter prerequisite'
    } else {
        $row.final_disposition = 'not-host-runtime'
        $row.final_owner_or_link_boundary = 'retained original Redirector DLL build/export/resource evidence; no standalone final host link input'
        $row.final_change_class = 'none; byte-exact source mirror retained'
        $row.named_adapter = 'none'
        $row.mapping_implication = 'not applicable to the final host runtime'
        $row.final_evidence = 'T276 S21: original Redirector DLL build/export input only'
    }
}
$rows | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" | Set-Content $path -Encoding utf8
$counts = $redir | Group-Object final_disposition | Sort-Object Name | ForEach-Object { "$($_.Name)=$($_.Count)" }
Write-Output "T276 S25 Redirector final dispositions applied: $($counts -join ', ')."
