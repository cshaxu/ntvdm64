[CmdletBinding()]
param([Parameter(Mandatory = $true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$path = Join-Path (Resolve-Path -LiteralPath $RepositoryRoot).Path 'docs/etc/operations/m0-t276-s17-v86-file-disposition-ledger.tsv'
if (-not (Test-Path -LiteralPath $path)) { throw "Missing output: $path" }
$rows = @(Import-Csv -LiteralPath $path -Delimiter "`t")
if ($rows.Count -ne 35) { throw "Expected 35 V86 paths, found $($rows.Count)." }
if (@($rows | Group-Object source_path | Where-Object Count -ne 1).Count -ne 0) { throw 'Every V86 path must have one disposition.' }
if (@($rows | Where-Object { $_.source_path -eq 'v86/monitor/i386/monitor.c' -and $_.provisional_disposition -eq 'historical-monitor-body profile exclusion; same-shaped monitor API recovery prerequisite' }).Count -ne 1) { throw 'monitor.c must remain an excluded kernel-VDM monitor body.' }
if (@($rows | Where-Object { $_.source_path -eq 'v86/monitor/i386/monitorp.h' -and $_.required_boundary_owner -like '*mapping manager*' }).Count -ne 1) { throw 'monitor declaration must require the unique mapping manager.' }
if (@($rows | Where-Object { $_.source_path -eq 'v86/scaffold/i386/softpc.c' -and $_.provisional_component_owner -eq 'app' }).Count -ne 1) { throw 'The standalone SoftPC scaffold must remain app-owned product-shell evidence.' }
if (@($rows | Where-Object { $_.source_path -eq 'v86/util/quit.asm' -and $_.provisional_component_owner -eq 'opennt-mvdm-tools' }).Count -ne 1) { throw 'quit.asm must remain a tool-only input.' }
if (@($rows | Where-Object state -ne 'provisional-not-enabled').Count -ne 0) { throw 'S17 may not enable a V86 body.' }
Write-Output 'T276 S17 V86 disposition verification passed: 35 paths, monitor/scaffold/tool split, and one mapping-manager monitor contract.'
