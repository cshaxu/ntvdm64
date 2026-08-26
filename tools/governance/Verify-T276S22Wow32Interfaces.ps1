[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$path = Join-Path (Resolve-Path $RepositoryRoot).Path 'docs/etc/operations/m0-t276-s22-wow32-interface-family-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t")
if ($rows.Count -ne 9) { throw "Expected 9 WOW32 interface-family rows, found $($rows.Count)." }
if (@($rows | Group-Object family_id | Where-Object Count -ne 1).Count) { throw 'Every WOW32 interface family needs one row.' }
$required = 'WOW32-IF-01','WOW32-IF-02','WOW32-IF-03','WOW32-IF-04','WOW32-IF-05','WOW32-IF-06','WOW32-IF-07','WOW32-IF-08','WOW32-IF-09'
foreach ($id in $required) { if (@($rows | Where-Object family_id -eq $id).Count -ne 1) { throw "Missing required WOW32 interface family $id." } }
if (@($rows | Where-Object { [string]::IsNullOrWhiteSpace($_.single_boundary_owner) -or [string]::IsNullOrWhiteSpace($_.mapping_requirement) -or [string]::IsNullOrWhiteSpace($_.provisional_recovery_disposition) }).Count) { throw 'Every WOW32 interface family needs owner, mapping and recovery disposition.' }
if (@($rows | Where-Object { $_.mapping_requirement -notmatch 'mapping-manager' }).Count) { throw 'Every WOW32 interface family must record the session mapping-manager decision.' }
Write-Output 'T276 S22 WOW32 interface-family verification passed: 9 source-proven families with explicit mapping and owner dispositions.'
