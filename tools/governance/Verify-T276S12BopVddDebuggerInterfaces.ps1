[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$path = Join-Path $root 'docs/etc/operations/m0-t276-s12-bop-vdd-debugger-interface-ledger.tsv'
if (-not (Test-Path -LiteralPath $path)) { throw "Missing T276 BOP/VDD/debugger interface ledger: $path" }
$rows = @(Import-Csv -LiteralPath $path -Delimiter "`t")
if ($rows.Count -ne 12) { throw "Expected 12 BOP/VDD/debugger rows, found $($rows.Count)." }
if (($rows | Group-Object interface_id | Where-Object Count -ne 1).Count -ne 0) { throw 'Duplicate BOP/VDD/debugger interface identity.' }
if (($rows | Where-Object state -ne 'reviewed-provisional-not-enabled').Count -ne 0) { throw 'BOP/VDD/debugger audit must not enable a source or adapter.' }
foreach ($source in @('nt_bop.c','nt_vdd.c','nt_yoda.c')) {
    if (@($rows | Where-Object source_path -eq "softpc.new/host/src/$source").Count -eq 0) { throw "Missing audited source: $source" }
}
if (@($rows | Where-Object source_path -eq 'softpc.new/host/src/nt_bop.c' | Where-Object single_recovery_owner -notmatch 'adapter-bop').Count -eq 0) { throw 'BOP dispatcher review must name mechanical adapter-bop boundary.' }
if (@($rows | Where-Object source_path -eq 'softpc.new/host/src/nt_vdd.c' | Where-Object single_recovery_owner -match 'adapter-vdd').Count -eq 0) { throw 'VDD review must name adapter-vdd boundary.' }
if (@($rows | Where-Object source_path -eq 'softpc.new/host/src/nt_yoda.c' | Where-Object provisional_disposition -notmatch 'profile exclusion').Count -ne 0) { throw 'Yoda debugger must remain profile-gated.' }
foreach ($row in $rows) { foreach ($column in 'source_span','original_symbol_or_call_group','original_contract','historical_dependency','single_recovery_owner','provisional_disposition','predecessor_gate','evidence') { if ([string]::IsNullOrWhiteSpace($row.$column)) { throw "Empty $column for $($row.interface_id)." } } }
Write-Output "T276 S12 BOP/VDD/debugger interface verification passed: $($rows.Count) rows."
