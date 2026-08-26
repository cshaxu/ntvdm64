[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$path = Join-Path $root 'docs/etc/operations/m0-t276-s12-monitor-session-interface-ledger.tsv'
if (-not (Test-Path -LiteralPath $path)) { throw "Missing T276 monitor/session ledger: $path" }

$rows = @(Import-Csv -LiteralPath $path -Delimiter "`t")
if ($rows.Count -ne 15) { throw "Expected 15 monitor/session rows, found $($rows.Count)." }
if (($rows | Group-Object interface_id | Where-Object Count -ne 1).Count -ne 0) { throw 'Duplicate monitor/session interface identity.' }
if (($rows | Where-Object state -ne 'reviewed-provisional-not-enabled').Count -ne 0) { throw 'Monitor/session audit must not enable a source or adapter.' }
foreach ($source in @('nt_eoi.c','nt_event.c','nt_inthk.c','nt_reset.c','nt_thred.c','nt_msscs.c')) {
    if (@($rows | Where-Object source_path -eq "softpc.new/host/src/$source").Count -eq 0) { throw "Missing audited source: $source" }
}
foreach ($row in $rows) {
    foreach ($column in 'source_span','original_symbol_or_call_group','original_contract','historical_dependency','single_recovery_owner','provisional_disposition','predecessor_gate','evidence') {
        if ([string]::IsNullOrWhiteSpace($row.$column)) { throw "Empty $column for $($row.interface_id)." }
    }
}
Write-Output "T276 S12 monitor/session interface verification passed: $($rows.Count) rows."
