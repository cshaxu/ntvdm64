[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path $RepositoryRoot).Path
$ops = Join-Path $root 'docs/etc/operations'
$path = Join-Path $ops 'm0-t276-final-integration-disposition-ledger.tsv'
$active = @(Import-Csv (Join-Path $ops 'm0-t276-s22-wow32-active-subdomain-ledger.tsv') -Delimiter "`t")
$rows = @(Import-Csv $path -Delimiter "`t" | Where-Object {$_.source_path -in $active.source_path})
if ($rows.Count -ne 77) { throw "Expected 77 active WOW32 provider rows, found $($rows.Count)." }
if (@($rows | Where-Object {$_.final_disposition -eq 'unresolved'}).Count) { throw 'No active WOW32 provider may remain unresolved.' }
$overlayDomains = @('thunk-frame-and-structure','message-and-window-procedure','gdi-and-window-object','kernel-task-and-dos-session','user-interface')
$bindingDomains = @('shell-ole-and-printing','multimedia','network')
foreach ($entry in $active) {
    $disposition = ($rows | Where-Object {$_.source_path -eq $entry.source_path}).final_disposition
    if ($entry.original_subdomain -in $overlayDomains -and $disposition -ne 'overlay-required') { throw "$($entry.source_path) must be overlay-required." }
    if ($entry.original_subdomain -in $bindingDomains -and $disposition -ne 'adapter-backed') { throw "$($entry.source_path) must be adapter-backed." }
}
Write-Output "T276 S25 WOW32 provider final-disposition verification passed: $($rows.Count) rows."
