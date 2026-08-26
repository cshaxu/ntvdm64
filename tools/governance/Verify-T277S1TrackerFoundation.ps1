[CmdletBinding()]
param([string]$RepositoryRoot = '')

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$ops = Join-Path $root 'docs/etc/operations'
function Read-Tsv([string]$Name) { @(Import-Csv -LiteralPath (Join-Path $ops $Name) -Delimiter "`t") }
function Require([bool]$Condition, [string]$Message) { if (-not $Condition) { throw $Message } }

$files = Read-Tsv 'm0-t277-file-recovery-ledger.tsv'
$interfaces = Read-Tsv 'm0-t277-interface-recovery-ledger.tsv'
$edges = Read-Tsv 'm0-t277-package-dependency-ledger.tsv'
$profiles = Read-Tsv 'm0-t277-build-profile-ledger.tsv'
$divergences = Read-Tsv 'm0-t277-divergence-exclusion-ledger.tsv'
Require ($files.Count -eq 1689) "Expected 1,689 file rows; found $($files.Count)."
Require ((@($files.file_id | Select-Object -Unique).Count) -eq 1689) 'File IDs are not unique.'
Require ((@($files | Where-Object { $_.tracker_state -notmatch 'symbol-closure-not-yet-proven' }).Count) -eq 0) 'S1 must not claim a symbol closure.'
Require ((@($interfaces.interface_id | Select-Object -Unique).Count) -eq $interfaces.Count) 'Interface IDs are not unique.'
Require ((@($interfaces | Where-Object { $_.row_kind -eq 'lexical-declaration-candidate' -and $_.reachability_state -ne 'lexical-candidate-not-source-reached' }).Count) -eq 0) 'A declaration-only row claims reachability.'
Require ((@($interfaces | Where-Object { $_.row_kind -eq 'call-declaration-correlation' }).Count) -gt 0) 'No caller/declaration correlations were imported.'
Require ((@($edges | Where-Object { $_.edge_kind -eq 'package-node' }).Count) -eq 23) 'Expected 23 package inventory nodes.'
Require ((@($profiles | Select-Object -ExpandProperty package_root -Unique).Count) -eq 23) 'Expected 23 build-profile rows.'
Require ($divergences.Count -eq 1689) "Expected 1,689 divergence/exclusion rows; found $($divergences.Count)."
Require ((@($divergences.file_id | Select-Object -Unique).Count) -eq 1689) 'Divergence/exclusion file IDs are not unique.'
Write-Host "T277 S1 tracker foundation passed: files=$($files.Count); interfaces=$($interfaces.Count); edges=$($edges.Count); profiles=$($profiles.Count); dispositions=$($divergences.Count)."
