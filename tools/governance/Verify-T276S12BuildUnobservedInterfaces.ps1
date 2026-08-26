[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot
)
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$path = Join-Path $root 'docs/etc/operations/m0-t276-s12-build-unobserved-interface-ledger.tsv'
if (-not (Test-Path -LiteralPath $path)) { throw "Missing T276 build-unobserved ledger: $path" }
$rows = @(Import-Csv -LiteralPath $path -Delimiter "`t")
if ($rows.Count -ne 10) { throw "Expected 10 build-unobserved rows, found $($rows.Count)." }
if (($rows | Group-Object interface_id | Where-Object Count -ne 1).Count -ne 0) { throw 'Duplicate build-unobserved identity.' }
if (($rows | Where-Object state -ne 'reviewed-provisional-not-enabled').Count -ne 0) { throw 'Build-unobserved audit must not enable source or adapter.' }
foreach ($source in @('nt_aorc.c','nt_cprgs.c','nt_ertbl.c','nt_hfx.c','nt_hunt.c','nt_mess.c','nt_smenu.c','nt_term.c','nt_vflop.c','stf_conf.c')) { if (@($rows | Where-Object source_path -eq "softpc.new/host/src/$source").Count -ne 1) { throw "Missing or duplicate audited source: $source" } }
foreach ($source in @('nt_aorc.c','nt_cprgs.c','nt_vflop.c')) { if (@($rows | Where-Object source_path -eq "softpc.new/host/src/$source" | Where-Object provisional_disposition -notmatch 'Bochs-replacement').Count -ne 0) { throw "$source must retain a Bochs-replacement exclusion." } }
foreach ($row in $rows) { foreach ($column in 'source_span','original_symbol_or_call_group','original_contract','historical_dependency','single_recovery_owner','provisional_disposition','predecessor_gate','evidence') { if ([string]::IsNullOrWhiteSpace($row.$column)) { throw "Empty $column for $($row.interface_id)." } } }
Write-Output "T276 S12 build-unobserved interface verification passed: $($rows.Count) rows."
