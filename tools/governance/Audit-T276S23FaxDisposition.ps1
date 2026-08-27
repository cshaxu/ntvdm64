[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path $RepositoryRoot).Path
$ops = Join-Path $root 'docs/etc/operations'
$rows = @(Import-Csv (Join-Path $ops 'mvdm-file-recovery-ledger.tsv') -Delimiter "`t" | Where-Object { $_.target_path -like 'fax/*' } | Sort-Object target_path)
if ($rows.Count -ne 21) { throw "Expected 21 selected fax paths, found $($rows.Count)." }
$out = foreach ($row in $rows) {
    $path = $row.target_path
    if ($path -eq 'fax/wowfax/wowfax.c') {
        $role = 'historical-gdi-driver-provider-body'; $owner = 'adapter-mvdm-host-out/wow; adapter-mvdm-host-out/win32; session; broker'; $contract = 'Original GDI driver body runs in historical graphics-engine/CSRSS product context.'; $disposition = 'private graphics-engine prerequisite; no body enabled'
    } elseif ($path -match '^fax/wowfaxui/.+\.c$') {
        $role = 'historical-fax-ui-dll-provider-body'; $owner = 'adapter-mvdm-host-out/wow; adapter-mvdm-host-out/win32; session; broker'; $contract = 'Original printer UI DLL body coordinates WOW fax structures, shared section and spooler/UI calls.'; $disposition = 'whole fax UI owner-package prerequisite; no body enabled'
    } elseif ($row.file_kind -eq 'declaration') {
        $role = 'historical-fax-declaration'; $owner = 'adapter-mvdm-host-out/wow; adapter-mvdm-host-out/win32; session; broker'; $contract = 'Original fax driver/UI ABI and shared layout evidence.'; $disposition = 'declaration recovery prerequisite'
    } elseif ($path -match '/(sources|makefile|\.def|\.rc)$') {
        $role = 'historical-fax-build-or-resource-input'; $owner = 'adapter-mvdm-host-out/wow; adapter-mvdm-host-out/win32; session; broker'; $contract = 'Original driver/DLL build, export or resource input.'; $disposition = 'build/resource evidence only'
    } else {
        $role = 'historical-fax-product-resource'; $owner = 'adapter-mvdm-host-out/wow; adapter-mvdm-host-out/win32; session; broker'; $contract = 'Original fax product installation/help/PPD resource.'; $disposition = 'product resource evidence only'
    }
    [pscustomobject][ordered]@{
        file_id = $row.file_id
        source_path = $path
        file_kind = $row.file_kind
        original_role_group = $role
        provisional_component_owner = 'mvdm-host'
        required_boundary_owner = $owner
        original_contract_summary = $contract
        provisional_disposition = $disposition
        state = 'provisional-not-enabled'
        evidence = "$path; fax/wowfax/sources or fax/wowfaxui/sources where applicable"
    }
}
$out | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" | Set-Content (Join-Path $ops 'm0-t276-s23-fax-file-disposition-ledger.tsv') -Encoding utf8
Write-Output "T276 S23 fax disposition generated: $($out.Count) rows."
