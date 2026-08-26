[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$path = Join-Path (Resolve-Path $RepositoryRoot).Path 'docs/etc/operations/m0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t" | Where-Object {$_.source_path -like 'v86/*'})
if ($rows.Count -ne 35) { throw "Expected 35 V86 rows, found $($rows.Count)." }
if (@($rows | Where-Object {$_.final_disposition -eq 'unresolved'}).Count) { throw 'No V86 row may remain unresolved after final package review.' }
if (@($rows | Where-Object {$_.source_path -like 'v86/monitor/i386/*.c' -or $_.source_path -eq 'v86/monitor/i386/fastpm.asm'} | Where-Object {$_.final_disposition -ne 'not-host-runtime'}).Count) { throw 'Kernel VDM monitor bodies cannot enter the final host runtime.' }
if (@($rows | Where-Object {$_.source_path -like 'v86/util/*' -and $_.final_disposition -ne 'tool-only'}).Count) { throw 'V86 utility inputs must be tool-only.' }
Write-Output "T276 S25 V86 final-disposition verification passed: $($rows.Count) rows."
