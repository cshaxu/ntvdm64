[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path $RepositoryRoot).Path
$ops = Join-Path $root 'docs/etc/operations'
$finalPath = Join-Path $ops 'm0-t276-final-integration-disposition-ledger.tsv'
$sourcePath = Join-Path $ops 'm0-t276-s12-softpc-file-disposition-ledger.tsv'
$rows = @(Import-Csv $finalPath -Delimiter "`t")
$source = @(Import-Csv $sourcePath -Delimiter "`t")
$eligible = @($source | Where-Object {$_.provisional_composition_disposition -in @('bochs-replacement-profile-excluded','adapter-bochs-firmware-review','tool-profile-excluded','profile-excluded','build-control-only')})
if ($eligible.Count -ne 558) { throw "Expected 558 conclusively classified SoftPC rows, found $($eligible.Count)." }

foreach ($entry in $eligible) {
    $row = @($rows | Where-Object {$_.source_path -eq $entry.target_path})
    if ($row.Count -ne 1) { throw "Expected one final ledger row for $($entry.target_path)." }
    $row = $row[0]
    $row.final_audit_state = 'final-disposition-audited; not implemented'
    switch ($entry.provisional_composition_disposition) {
        'bochs-replacement-profile-excluded' {
            $row.final_disposition = 'not-host-runtime'
            $row.final_owner_or_link_boundary = 'retained mvdm-host SoftPC machine evidence; Bochs owns the final CPU/device execution'
            $row.final_change_class = 'none; byte-exact source mirror retained outside the final host link graph'
            $row.named_adapter = 'adapter-softpc -> adapter-bochs is the same-shaped machine boundary, not an import of this executor'
            $row.mapping_implication = 'not applicable to this excluded executor; any admitted caller uses adapter-softpc and the session guest-memory mapping-manager lease'
            $row.final_evidence = 'T276 S12 SoftPC baseline: original CCPU/CPU/device execution is a parallel-machine exclusion replaced by Bochs'
        }
        'adapter-bochs-firmware-review' {
            $row.final_disposition = 'firmware-only'
            $row.final_owner_or_link_boundary = 'mvdm-softpc-firmware immutable machine input selected only by an adapter-bochs manifest'
            $row.final_change_class = 'none; byte-exact firmware/source input retained and never linked as a host provider'
            $row.named_adapter = 'adapter-bochs'
            $row.mapping_implication = 'firmware bytes are immutable machine input; they expose neither native pointers nor host handles'
            $row.final_evidence = 'T276 S12 SoftPC baseline: BIOS/ROM/data paths are firmware-input questions only, never a second machine executor'
        }
        'tool-profile-excluded' {
            $row.final_disposition = 'tool-only'
            $row.final_owner_or_link_boundary = 'mvdm-tools historical build/debug utility evidence; never a host link input'
            $row.final_change_class = 'none; byte-exact source mirror retained'
            $row.named_adapter = 'none'
            $row.mapping_implication = 'not applicable to the host runtime'
            $row.final_evidence = 'T276 S12 SoftPC baseline: original build-tool input is a separately built historical tool'
        }
        default {
            $row.final_disposition = 'not-host-runtime'
            $row.final_owner_or_link_boundary = 'retained original SoftPC build/product control evidence; no final host link input'
            $row.final_change_class = 'none; byte-exact source mirror retained'
            $row.named_adapter = 'none'
            $row.mapping_implication = 'not applicable to the host runtime'
            $row.final_evidence = 'T276 S12 SoftPC baseline: profile/build-control path is not part of the final x86/x64 host runtime'
        }
    }
}

$rows | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" | Set-Content $finalPath -Encoding utf8
$counts = $eligible | Group-Object provisional_composition_disposition | Sort-Object Name | ForEach-Object { "$($_.Name)=$($_.Count)" }
Write-Output "T276 S25 conclusive SoftPC final dispositions applied: $($counts -join ', ')."
