[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$path = Join-Path (Resolve-Path $RepositoryRoot).Path 'docs/etc/operations/m0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t" | Where-Object {$_.source_path -like 'fax/*'})
if ($rows.Count -ne 21) { throw "Expected 21 fax rows, found $($rows.Count)." }
if (@($rows | Where-Object {$_.final_disposition -eq 'unresolved'}).Count) { throw 'No fax row may remain unresolved after the final package audit.' }
if (@($rows | Where-Object {$_.final_audit_state -ne 'final-disposition-audited; not implemented'}).Count) { throw 'Every fax row needs the final audited implementation state.' }
$uiBodies = @('fax/wowfaxui/wfhelper.c', 'fax/wowfaxui/wfsheets.c', 'fax/wowfaxui/wfupgrad.c', 'fax/wowfaxui/wowfaxui.c')
if (@($rows | Where-Object {$_.source_path -in $uiBodies -and $_.final_disposition -ne 'adapter-backed'}).Count) { throw 'Selected WOWFAXUI bodies must be adapter-backed.' }
if (@($rows | Where-Object {$_.source_path -eq 'fax/wowfax/wowfax.c' -and $_.final_disposition -ne 'not-host-runtime'}).Count) { throw 'The private historical GDI driver must remain out of the final host graph.' }
Write-Output "T276 S25 FAX final-disposition verification passed: $($rows.Count) rows."
