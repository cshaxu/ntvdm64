[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path $RepositoryRoot).Path
$ops = Join-Path $root 'docs/etc/operations'
$path = Join-Path $ops 'm0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t")
$w0w1Roots = @('dirs', 'makefil0', 'inc', 'oemuni', 'suballoc', 'vdmutils')
$w0w1 = @($rows | Where-Object {$w0w1Roots -contains $_.package_root})
if ($w0w1.Count -ne 140) { throw "Expected 140 W0/W1 rows, found $($w0w1.Count)." }

foreach ($row in $w0w1) {
    $row.final_audit_state = 'final-disposition-audited; not implemented'
    switch -Wildcard ($row.source_path) {
        'vdmutils/*' {
            $row.final_disposition = 'tool-only'
            $row.final_owner_or_link_boundary = 'mvdm-tools exact historical standalone-product mirror; never a host link input'
            $row.final_change_class = 'none; byte-exact source mirror retained'
            $row.named_adapter = 'none'
            $row.mapping_implication = 'not applicable to the host runtime'
            $row.final_evidence = 'T276 S11: vdmutils forcedos/graftabl/pifedit/win are standalone historical products, not reusable host providers'
            continue
        }
        'oemuni/file.c' {
            $row.final_disposition = 'adapter-backed'
            $row.final_owner_or_link_boundary = 'mvdm-support oemuni library -> adapter-win32 same-shaped NT RTL and public Win32 bindings'
            $row.final_change_class = 'binding-only; retain original algorithm, entrypoints, error conversion and temporary-string ownership'
            $row.named_adapter = 'adapter-win32'
            $row.mapping_implication = 'no guest or host identity mapping in this library body; any native process handle stays behind adapter-win32'
            $row.final_evidence = 'T276 S11 MVDM-W1-BIND-001; oemuni/sources lists file.c and original consumers link oemuni.lib'
            continue
        }
        'oemuni/process.c' {
            $row.final_disposition = 'adapter-backed'
            $row.final_owner_or_link_boundary = 'mvdm-support oemuni library -> adapter-win32 same-shaped process, environment and NT RTL bindings'
            $row.final_change_class = 'binding-only; retain original conversion, CreateProcess ordering and SetLastError semantics'
            $row.named_adapter = 'adapter-win32'
            $row.mapping_implication = 'native process and standard-stream handles remain adapter-win32/session-owned; no historical ULONG cast is permitted'
            $row.final_evidence = 'T276 S11 MVDM-W1-BIND-002; oemuni/sources lists process.c and original consumers link oemuni.lib'
            continue
        }
        'suballoc/suballoc.c' {
            $row.final_disposition = 'binding-only'
            $row.final_owner_or_link_boundary = 'mvdm-support original suballoc library; original machine-facing callbacks bind at caller through adapter-softpc'
            $row.final_change_class = 'binding-only; retain original allocator algorithm and ULONG callback contract'
            $row.named_adapter = 'adapter-softpc at callers only'
            $row.mapping_implication = 'the library does not map ULONG values to pointers; caller callbacks resolve guest ranges only through the session guest-memory mapping-manager lease'
            $row.final_evidence = 'T276 S11 MVDM-W1-BIND-003; suballoc/sources lists suballoc.c as the only library source'
            continue
        }
        'oemuni/toemuni.c' {
            $row.final_disposition = 'tool-only'
            $row.final_owner_or_link_boundary = 'mvdm-tools historical UMTEST source; never a host link input'
            $row.final_change_class = 'none; byte-exact source mirror retained'
            $row.named_adapter = 'none'
            $row.mapping_implication = 'not applicable to the host runtime'
            $row.final_evidence = 'T276 S11: oemuni/sources identifies toemuni.c as UMTEST, not an oemuni library member'
            continue
        }
        'suballoc/debug.c' {
            $row.final_disposition = 'tool-only'
            $row.final_owner_or_link_boundary = 'mvdm-tools historical allocator diagnostic/test source; never a host link input'
            $row.final_change_class = 'none; byte-exact source mirror retained'
            $row.named_adapter = 'none'
            $row.mapping_implication = 'not applicable to the host runtime'
            $row.final_evidence = 'T276 S11: debug.c is not a library member and tsa.c is the original UMTEST program'
            continue
        }
        'suballoc/tsa.c' {
            $row.final_disposition = 'tool-only'
            $row.final_owner_or_link_boundary = 'mvdm-tools historical allocator diagnostic/test source; never a host link input'
            $row.final_change_class = 'none; byte-exact source mirror retained'
            $row.named_adapter = 'none'
            $row.mapping_implication = 'not applicable to the host runtime'
            $row.final_evidence = 'T276 S11: debug.c is not a library member and tsa.c is the original UMTEST program'
            continue
        }
        'inc/*' {
            $row.final_disposition = 'binding-only'
            $row.final_owner_or_link_boundary = 'mvdm-platform-abi declaration/build carrier supplied to the selected imported MVDM consumer'
            $row.final_change_class = 'binding-only; retain declarations and assembly include forms byte-exact unless a later registered ABI binding requires a minimal include/import change'
            $row.named_adapter = 'consumer-specific adapter only; no generic adapter'
            $row.mapping_implication = 'declarations do not transfer native identity; any reached pointer or HANDLE boundary follows the named consumer adapter and session mapping-manager rule'
            $row.final_evidence = 'T276 S11: inc is a declaration/resource carrier with no standalone runtime object; T276 S3 declaration evidence'
            continue
        }
        'oemuni/*' {
            $row.final_disposition = 'binding-only'
            $row.final_owner_or_link_boundary = 'mvdm-support source/build/declaration carrier for the selected original library bodies'
            $row.final_change_class = 'binding-only; retain byte-exact carrier and bind only the selected original library translation units'
            $row.named_adapter = 'adapter-win32 for oemuni bodies; adapter-softpc only at suballoc machine-facing callers'
            $row.mapping_implication = 'carrier has no independent native identity transfer; reached boundaries use the owning adapter/session mapping-manager rule'
            $row.final_evidence = 'T276 S11 W0/W1 file disposition and runtime-support binding ledgers'
            continue
        }
        'suballoc/*' {
            $row.final_disposition = 'binding-only'
            $row.final_owner_or_link_boundary = 'mvdm-support source/build/declaration carrier for the selected original library bodies'
            $row.final_change_class = 'binding-only; retain byte-exact carrier and bind only the selected original library translation units'
            $row.named_adapter = 'adapter-win32 for oemuni bodies; adapter-softpc only at suballoc machine-facing callers'
            $row.mapping_implication = 'carrier has no independent native identity transfer; reached boundaries use the owning adapter/session mapping-manager rule'
            $row.final_evidence = 'T276 S11 W0/W1 file disposition and runtime-support binding ledgers'
            continue
        }
        default {
            $row.final_disposition = 'not-host-runtime'
            $row.final_owner_or_link_boundary = 'retained original build-control evidence; no final host link input'
            $row.final_change_class = 'none; byte-exact source mirror retained'
            $row.named_adapter = 'none'
            $row.mapping_implication = 'not applicable to the host runtime'
            $row.final_evidence = 'T276 S11: original top-level build-control carrier, not a standalone runtime object'
        }
    }
}

$rows | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" | Set-Content $path -Encoding utf8
$counts = $w0w1 | Group-Object final_disposition | Sort-Object Name | ForEach-Object { "$($_.Name)=$($_.Count)" }
Write-Output "T276 S25 W0/W1 final dispositions applied: $($counts -join ', ')."
