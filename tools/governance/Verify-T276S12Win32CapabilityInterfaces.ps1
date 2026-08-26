[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$path = Join-Path $root 'docs/etc/operations/m0-t276-s12-win32-capability-interface-ledger.tsv'
if (-not (Test-Path -LiteralPath $path)) { throw "Missing T276 Win32/NTDLL capability ledger: $path" }

$rows = @(Import-Csv -LiteralPath $path -Delimiter "`t")
if ($rows.Count -ne 11) { throw "Expected 11 Win32/NTDLL capability rows, found $($rows.Count)." }
if (($rows | Group-Object interface_id | Where-Object Count -ne 1).Count -ne 0) { throw 'Duplicate Win32/NTDLL capability interface identity.' }
if (($rows | Where-Object state -ne 'reviewed-provisional-not-enabled').Count -ne 0) { throw 'Win32/NTDLL audit must not enable a source or adapter.' }
foreach ($source in @('config.c','fprt.c','nt_error.c','nt_keycd.c','nt_nls.c','nt_ntfun.c','nt_pif.c','nt_sec.c','nt_unix.c')) {
    if (@($rows | Where-Object source_path -eq "softpc.new/host/src/$source").Count -eq 0) { throw "Missing audited source: $source" }
}
if (@($rows | Where-Object source_path -eq 'softpc.new/host/src/config.c' | Where-Object provisional_disposition -notmatch 'cross-owner').Count -ne 0) { throw 'config.c must remain a cross-owner composition review.' }
if (@($rows | Where-Object source_path -eq 'softpc.new/host/src/nt_sec.c' | Where-Object provisional_disposition -notmatch 'cross-owner').Count -ne 0) { throw 'nt_sec.c must remain a cross-owner composition review.' }
if (@($rows | Where-Object original_symbol_or_call_group -match 'host_direct_access_error' | Where-Object single_recovery_owner -notmatch 'adapter-win32; session').Count -ne 0) { throw 'Direct-access error contract must remain owned by adapter-win32 and session.' }
foreach ($row in $rows) {
    foreach ($column in 'source_span','original_symbol_or_call_group','original_contract','historical_dependency','single_recovery_owner','provisional_disposition','predecessor_gate','evidence') {
        if ([string]::IsNullOrWhiteSpace($row.$column)) { throw "Empty $column for $($row.interface_id)." }
    }
}
Write-Output "T276 S12 Win32/NTDLL capability interface verification passed: $($rows.Count) rows."
