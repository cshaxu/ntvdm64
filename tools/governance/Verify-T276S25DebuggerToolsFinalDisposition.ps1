[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$path = Join-Path (Resolve-Path $RepositoryRoot).Path 'docs/etc/operations/m0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t" | Where-Object {$_.source_path -match '^(bde|dbg|vdmdbg|vdmexts)/'})
if ($rows.Count -ne 47) { throw "Expected 47 debugger-tool rows, found $($rows.Count)." }
if (@($rows | Where-Object {$_.final_disposition -ne 'tool-only'}).Count) { throw 'Every external debugger-tool row must be tool-only.' }
if (@($rows | Where-Object {$_.final_audit_state -ne 'final-disposition-audited; not implemented'}).Count) { throw 'Every debugger-tool row needs the final audited implementation state.' }
Write-Output "T276 S25 debugger-tool final-disposition verification passed: $($rows.Count) rows."
