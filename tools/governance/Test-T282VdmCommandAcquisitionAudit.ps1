[CmdletBinding()]
param([string]$RepositoryRoot = '')
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot) }
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$path = Join-Path (Resolve-Path -LiteralPath $RepositoryRoot).Path 'docs/etc/operations/m0-t282-s5-vdm-command-acquisition-ledger.tsv'
$rows = @(Import-Csv -LiteralPath $path -Delimiter "`t")
if ($rows.Count -ne 7 -or @($rows | Group-Object slice | Where-Object Count -gt 1).Count) { throw 'VDM command-acquisition ledger lacks a unique seven-slice audit.' }
foreach ($row in $rows) { if ([string]::IsNullOrWhiteSpace($row.final_disposition) -or [string]::IsNullOrWhiteSpace($row.owner)) { throw 'VDM command-acquisition row lacks a final disposition or owner.' } }
Write-Output 'T282 S5 VDM command-acquisition audit verification passed.'
