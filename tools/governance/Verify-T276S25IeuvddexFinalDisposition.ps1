[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$path = Join-Path (Resolve-Path $RepositoryRoot).Path 'docs/etc/operations/m0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t" | Where-Object {$_.source_path -like 'ieuvddex/*'})
if ($rows.Count -ne 23) { throw "Expected 23 IEUVDDEx rows, found $($rows.Count)." }
if (@($rows | Where-Object {$_.final_disposition -ne 'not-host-runtime'}).Count) { throw 'Every IEUVDDEx row must remain outside the app host runtime.' }
if (@($rows | Where-Object {$_.named_adapter -notmatch 'adapter-mvdm-host-out/debugger'}).Count) { throw 'Every IEUVDDEx row must name the future debugger boundary.' }
Write-Output 'T276 S25 IEUVDDEx final-disposition verification passed: 23 not-host-runtime rows.'
