[CmdletBinding()]
param([string]$RepositoryRoot = '')

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$path = Join-Path $root 'docs/etc/operations/m0-t281-s5-host-control-disposition-ledger.tsv'
$rows = Import-Csv -LiteralPath $path -Delimiter "`t"
if ($rows.Count -ne 56) { throw "Expected 56 host-control rows, found $($rows.Count)." }
if (@($rows | Group-Object source_path | Where-Object Count -gt 1).Count -ne 0) { throw 'Duplicate host-control source disposition.' }
if (@($rows | Where-Object { $_.disposition -notin @('eligible-next', 'adapter-prerequisite', 'specialist-owner', 'bochs-replacement') }).Count -ne 0) { throw 'Unknown T281 S5 disposition.' }
if (@($rows | Where-Object { [string]::IsNullOrWhiteSpace($_.next_owner_group) -or [string]::IsNullOrWhiteSpace($_.required_boundary_owner) }).Count -ne 0) { throw 'Missing owner boundary.' }
Write-Output 'T281 S5 host-control disposition verification passed: 56 unique source rows.'
