[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$path = Join-Path $root 'docs/etc/operations/m0-t276-s12-softpc-facade-interface-ledger.tsv'
if (-not (Test-Path -LiteralPath $path)) { throw "Missing T276 SoftPC/SAS facade ledger: $path" }

$rows = @(Import-Csv -LiteralPath $path -Delimiter "`t")
if ($rows.Count -ne 12) { throw "Expected 12 SoftPC/SAS facade rows, found $($rows.Count)." }
if (($rows | Group-Object interface_id | Where-Object Count -ne 1).Count -ne 0) { throw 'Duplicate SoftPC/SAS interface identity.' }
if (($rows | Where-Object state -ne 'reviewed-provisional-not-enabled').Count -ne 0) { throw 'SoftPC/SAS audit must not enable a source or adapter.' }
foreach ($source in @('copy_fnc.c','cpucstbs.c','nt_cpu.c','nt_emm.c','nt_mem.c','nt_sas.c','nt_umb.c','sim32.c','stubs.c','x86_emm.c')) {
    if (@($rows | Where-Object source_path -eq "softpc.new/host/src/$source").Count -eq 0) { throw "Missing audited source: $source" }
}
if (@($rows | Where-Object provisional_disposition -match 'Bochs-replacement profile exclusion').Count -eq 0) { throw 'Missing explicit second-CPU exclusion.' }
foreach ($row in $rows) {
    foreach ($column in 'source_span','original_symbol_or_call_group','original_contract','historical_dependency','single_recovery_owner','provisional_disposition','predecessor_gate','evidence') {
        if ([string]::IsNullOrWhiteSpace($row.$column)) { throw "Empty $column for $($row.interface_id)." }
    }
}
Write-Output "T276 S12 SoftPC/SAS facade interface verification passed: $($rows.Count) rows."
