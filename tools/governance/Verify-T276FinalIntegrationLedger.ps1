[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$path = Join-Path (Resolve-Path $RepositoryRoot).Path 'docs/etc/operations/m0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t")
$allowed = 'direct','binding-only','adapter-backed','overlay-required','not-host-runtime','guest-only','firmware-only','tool-only','unresolved'
if ($rows.Count -ne 1689) { throw "Expected 1689 final-disposition rows, found $($rows.Count)." }
if (@($rows | Group-Object source_path | Where-Object Count -ne 1).Count) { throw 'Every selected MVDM path needs one final-disposition row.' }
if (@($rows | Where-Object {$allowed -notcontains $_.final_disposition}).Count) { throw 'A final-disposition row has an invalid classification.' }
if (@($rows | Where-Object {[string]::IsNullOrWhiteSpace($_.final_owner_or_link_boundary) -or [string]::IsNullOrWhiteSpace($_.final_change_class) -or [string]::IsNullOrWhiteSpace($_.named_adapter) -or [string]::IsNullOrWhiteSpace($_.mapping_implication) -or [string]::IsNullOrWhiteSpace($_.final_evidence)}).Count) { throw 'Every final-disposition row needs owner, change, adapter, mapping and evidence fields.' }
Write-Output "T276 final expected-integration tracker verification passed: $($rows.Count) rows."
