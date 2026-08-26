[CmdletBinding()]
param([Parameter(Mandatory = $true)][string]$RepositoryRoot)
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$path = Join-Path $root 'docs/etc/operations/m0-t276-s14-xms-file-disposition-ledger.tsv'
if (-not (Test-Path -LiteralPath $path)) { throw "Missing output: $path" }
$rows = @(Import-Csv -LiteralPath $path -Delimiter "`t")
if ($rows.Count -ne 15) { throw "Expected 15 XMS rows, found $($rows.Count)." }
if (@($rows | Group-Object source_path | Where-Object Count -ne 1).Count -ne 0) { throw 'Every XMS path must have one disposition.' }
if (@($rows | Where-Object { [string]::IsNullOrWhiteSpace($_.required_boundary_owner) }).Count -ne 0) { throw 'Every XMS path needs a boundary owner/disposition.' }
if (@($rows | Where-Object state -ne 'provisional-not-enabled').Count -ne 0) { throw 'S14 must not enable an XMS source body.' }
if (@($rows | Where-Object { $_.source_path -eq 'xms.486/i386/xmsmem86.c' -and $_.provisional_disposition -eq 'same-shaped mapping-manager replacement prerequisite' }).Count -ne 1) { throw 'The x86 direct-pointer routine must require same-shaped mapping-manager replacement.' }
if (@($rows | Where-Object { $_.source_path -eq 'xms.486/xmsumb.c' -and $_.provisional_disposition -eq 'machine-profile-gated review' }).Count -ne 1) { throw 'The UMB provider must remain machine-profile gated.' }
Write-Output 'T276 S14 XMS disposition verification passed: 15 paths with mapping-manager and UMB gates.'
