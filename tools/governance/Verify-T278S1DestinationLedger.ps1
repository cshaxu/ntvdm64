[CmdletBinding()]
param([string]$RepositoryRoot = '')

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$ops = Join-Path $root 'docs/etc/operations'
$ledger = @(Import-Csv (Join-Path $ops 'm0-t278-destination-source-identity-ledger.tsv') -Delimiter "`t")
if ($ledger.Count -ne 1689) { throw "Expected 1689 re-root rows; found $($ledger.Count)." }
if (($ledger.source_path | Sort-Object -Unique).Count -ne 1689) { throw 'Re-root source paths are not unique.' }
if (@($ledger | Where-Object { $_.current_component -ne 'mvdm-host' -or $_.movement_state -ne 'frozen-pre-move' }).Count) { throw 'S1 must retain every path in the canonical host union before movement.' }
$allowed = @('mvdm-host','mvdm-support','mvdm-tools','mvdm-softpc-firmware')
if (@($ledger | Where-Object { $_.destination_component -notin $allowed }).Count) { throw 'An invalid destination component was recorded.' }
if (@($ledger | Where-Object { $_.package_root -in @('dirs','makefil0','inc','oemuni','suballoc') -and $_.destination_component -ne 'mvdm-support' }).Count) { throw 'A support root has an incorrect destination.' }
if (@($ledger | Where-Object { $_.package_root -eq 'vdmutils' -and $_.destination_component -ne 'mvdm-tools' }).Count) { throw 'vdmutils has an incorrect destination.' }
if (@($ledger | Where-Object { $_.destination_component -eq 'mvdm-softpc-firmware' -and $_.source_path -notmatch '^softpc\.new/(base/bios|bios|roms|data)(/|$)' }).Count) { throw 'A non-firmware path was sent to the firmware component.' }
$counts = $ledger | Group-Object destination_component | Sort-Object Name | ForEach-Object { "$($_.Name)=$($_.Count)" }
Write-Host "PASS: T278 S1 frozen destination ledger: $($counts -join '; ')."
