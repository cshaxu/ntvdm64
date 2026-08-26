[CmdletBinding()]
param([Parameter(Mandatory = $true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$path = Join-Path (Resolve-Path -LiteralPath $RepositoryRoot).Path 'docs/etc/operations/m0-t276-s18-ieuvddex-file-disposition-ledger.tsv'
if (-not (Test-Path -LiteralPath $path)) { throw "Missing output: $path" }
$rows = @(Import-Csv -LiteralPath $path -Delimiter "`t")
if ($rows.Count -ne 23) { throw "Expected 23 IEUVDDEx paths, found $($rows.Count)." }
if (@($rows | Group-Object source_path | Where-Object Count -ne 1).Count -ne 0) { throw 'Every IEUVDDEx path must have one disposition.' }
if (@($rows | Where-Object { $_.source_path -eq 'ieuvddex/i386/vdmtib.c' -and $_.provisional_disposition -eq 'debugger-product profile exclusion; copied diagnostic-snapshot API recovery prerequisite' }).Count -ne 1) { throw 'vdmtib.c must be retained as copied-snapshot debugger evidence, not a live VDM pointer route.' }
if (@($rows | Where-Object { $_.source_path -eq 'ieuvddex/ieuvddex.h' -and $_.required_boundary_owner -like 'adapter-debugger*' }).Count -ne 1) { throw 'IEUVDDEx declarations must be assigned to adapter-debugger.' }
if (@($rows | Where-Object { $_.source_path -match '^ieuvddex/(alpha|mips|ppc)/' -and $_.provisional_disposition -eq 'non-x86 profile exclusion; source evidence only' }).Count -ne 6) { throw 'All six non-x86 IEUVDDEx paths must remain excluded.' }
if (@($rows | Where-Object state -ne 'provisional-not-enabled').Count -ne 0) { throw 'S18 may not enable an IEUVDDEx body.' }
Write-Output 'T276 S18 IEUVDDEx disposition verification passed: 23 paths, debugger-product ownership and six non-x86 exclusions.'
