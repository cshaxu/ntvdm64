[CmdletBinding()]
param([Parameter(Mandatory = $true)][string]$RepositoryRoot)
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
& (Join-Path $root 'tools/governance/Verify-T276S14XmsDisposition.ps1') -RepositoryRoot $root
& (Join-Path $root 'tools/governance/Verify-T276S14XmsInterfaces.ps1') -RepositoryRoot $root
$path = Join-Path $root 'docs/etc/operations/m0-t276-s14-xms-file-disposition-ledger.tsv'
$rows = @(Import-Csv -LiteralPath $path -Delimiter "`t")
if (@($rows | Where-Object file_kind -eq 'source').Count -ne 8) { throw 'Expected eight selected XMS source paths.' }
if (@($rows | Where-Object { $_.original_build_membership -like 'observed-in-*sources' }).Count -ne 7) { throw 'Expected six top-level plus one i386 original source selections.' }
if (@($rows | Where-Object { $_.provisional_disposition -like '*exclusion' }).Count -ne 2) { throw 'Expected the two historical architecture-specific exclusions.' }
Write-Output 'T276 S14 XMS package completion verification passed: 15 paths, 8 source paths, 8 boundary families.'
