[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path $RepositoryRoot).Path
$ops = Join-Path $root 'docs/etc/operations'
$path = Join-Path $ops 'm0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t")
$dos = @($rows | Where-Object {$_.source_path -like 'dos/*'})
if ($dos.Count -ne 40) { throw "Expected 40 dos rows, found $($dos.Count)." }

foreach ($row in $dos) {
    $row.final_audit_state = 'final-disposition-audited; not implemented'
    if ($row.file_kind -eq 'source') {
        $row.final_disposition = 'adapter-backed'
        if ($row.source_path -like 'dos/dem/*') {
            $row.final_owner_or_link_boundary = 'mvdm-host original DEM provider -> adapter-mvdm-host-in -> adapter-softpc -> adapter-bochs; host capability through adapter-win32 and session'
        } else {
            $row.final_owner_or_link_boundary = 'mvdm-host original COMMAND provider -> adapter-mvdm-host-in -> adapter-softpc -> adapter-bochs; host/session capability through adapter-win32, session and adapter-redir'
        }
        $row.final_change_class = 'binding-only; preserve original provider algorithm, dispatcher ordering, structures and failure routes'
        $row.named_adapter = 'adapter-mvdm-host-in; adapter-softpc; adapter-bochs; adapter-win32; session; adapter-redir where reached'
        $row.mapping_implication = 'guest frames and pointer-shaped fields use the sole session guest-memory mapping-manager instance; host resources remain opaque mapped identities'
        $row.final_evidence = 'T276 S13 DOS package and interface baselines: selected original DEM/COMMAND provider body'
    }
    elseif ($row.file_kind -eq 'declaration') {
        $row.final_disposition = 'binding-only'
        $row.final_owner_or_link_boundary = 'mvdm-platform-abi original DEM/COMMAND declaration surface shared by the source provider packages and same-shaped adapters'
        $row.final_change_class = 'binding-only; preserve declaration, layout and calling-convention shape'
        $row.named_adapter = 'adapter-mvdm-host-in; adapter-softpc; adapter-win32; session; adapter-redir where reached'
        $row.mapping_implication = 'reached pointer fields resolve through the appropriate one of the three session mapping-manager instances; no raw host identity is represented in guest fields'
        $row.final_evidence = 'T276 S13 DOS package baseline: original declaration carrier'
    }
    else {
        $row.final_disposition = 'not-host-runtime'
        $row.final_owner_or_link_boundary = 'retained original DOS package build, metadata or test evidence; no final host link input'
        $row.final_change_class = 'none; byte-exact source mirror retained'
        $row.named_adapter = 'none'
        $row.mapping_implication = 'not applicable to the final host runtime'
        $row.final_evidence = 'T276 S13 DOS package baseline: original build/test metadata only'
    }
}

$rows | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" | Set-Content $path -Encoding utf8
$counts = $dos | Group-Object final_disposition | Sort-Object Name | ForEach-Object { "$($_.Name)=$($_.Count)" }
Write-Output "T276 S25 DOS final dispositions applied: $($counts -join ', ')."
