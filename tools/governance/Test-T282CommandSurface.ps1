[CmdletBinding()]
param([string]$RepositoryRoot = '')
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot) }
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$source = @(Import-Csv -LiteralPath (Join-Path $root 'docs/etc/operations/m0-t282-s1-command-source-disposition-ledger.tsv') -Delimiter "`t")
$service = @(Import-Csv -LiteralPath (Join-Path $root 'docs/etc/operations/m0-t282-s1-command-service-disposition-ledger.tsv') -Delimiter "`t")
if ($source.Count -ne 11 -or @($source | Group-Object source_path | Where-Object Count -gt 1).Count) { throw 'COMMAND source ledger is not a unique 11-unit set.' }
if ($service.Count -ne 17 -or @($service | Group-Object service | Where-Object Count -gt 1).Count) { throw 'COMMAND service ledger is not a unique 17-entry set.' }
foreach ($item in $source) { if ([string]::IsNullOrWhiteSpace($item.first_required_boundary) -or [string]::IsNullOrWhiteSpace($item.next_owner_group)) { throw 'Missing COMMAND source boundary owner.' } }
foreach ($item in $service) { if ([string]::IsNullOrWhiteSpace($item.first_required_boundary) -or [string]::IsNullOrWhiteSpace($item.package_group)) { throw 'Missing COMMAND service boundary owner.' } }
Write-Output 'T282 S1 COMMAND surface verification passed: 11 source units and 17 service entries.'
