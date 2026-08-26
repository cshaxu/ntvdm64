[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
& (Join-Path $root 'tools/governance/Verify-T276S13DosDisposition.ps1') -RepositoryRoot $root
& (Join-Path $root 'tools/governance/Verify-T276S13DosInterfaces.ps1') -RepositoryRoot $root
$path = Join-Path $root 'docs/etc/operations/m0-t276-s13-dos-file-disposition-ledger.tsv'
$rows = @(Import-Csv -LiteralPath $path -Delimiter "`t")
if (@($rows | Where-Object { $_.source_path -like 'dos/dem/*' -and $_.file_kind -eq 'source' }).Count -ne 16) { throw 'Expected 16 DEM source units.' }
if (@($rows | Where-Object { $_.source_path -like 'dos/command/*' -and $_.file_kind -eq 'source' }).Count -ne 11) { throw 'Expected 11 COMMAND source units.' }
if (@($rows | Where-Object { $_.provisional_disposition -eq 'package-led adapter/interface review' }).Count -ne 27) { throw 'Expected all 27 source units to remain package-led interface reviews.' }
Write-Output 'T276 S13 DOS package completion verification passed: 40 paths, 27 source units, 8 boundary families.'
