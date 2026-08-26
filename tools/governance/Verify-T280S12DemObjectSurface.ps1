[CmdletBinding()]
param([string]$RepositoryRoot = '')
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot) }
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$ledgerPath = Join-Path $root 'docs/etc/operations/m0-t280-s12-dem-object-surface-ledger.tsv'
$evidencePath = Join-Path $root 'docs/etc/operations/m0-t280-s12-dem-object-surface-evidence-001.md'
if (!(Test-Path $ledgerPath) -or !(Test-Path $evidencePath)) { throw 'Missing T280 S12 evidence.' }
$rows = Import-Csv $ledgerPath -Delimiter "`t"
if ($rows.Count -ne 3) { throw 'T280 S12 must cover exactly three reached DEM consumers.' }
foreach ($name in @('demsrch.c','demerror.c','demgset.c')) { if (@($rows | Where-Object source -eq $name).Count -ne 1) { throw "Missing unique S12 row: $name" } }
if (@($rows | Where-Object { [string]::IsNullOrWhiteSpace($_.owner) -or [string]::IsNullOrWhiteSpace($_.mapping_rule) }).Count) { throw 'S12 row lacks owner or mapping rule.' }
Write-Host 'PASS: T280 S12 classifies every reached DEM object observation with one owner and mapping disposition.'
