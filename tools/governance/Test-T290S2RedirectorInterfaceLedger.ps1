[CmdletBinding()]
param(
    [string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

$ledger = Join-Path $RepositoryRoot 'docs\etc\operations\m0-t290-s2-redirector-external-interface-recovery-ledger.tsv'
$sourceRoot = Join-Path $RepositoryRoot 'src\mvdm-host\vdmredir'
$rows = Import-Csv -LiteralPath $ledger -Delimiter "`t"
if ($rows.Count -ne 16) { throw "Redirector interface ledger must have 16 boundary rows; found $($rows.Count)" }

foreach ($required in @('sync named pipes', 'local mailslots', 'async pipe events', 'physical completion interrupt', 'VDD user hook', 'NetBIOS NCB', 'DLC/LLC private driver', 'COMMAND broker consumer', 'DEM pipe consumer')) {
    if (@($rows | Where-Object { $_.interface_family -eq $required }).Count -ne 1) {
        throw "Missing or duplicate Redirector boundary row: $required"
    }
}

$dlc = $rows | Where-Object { $_.interface_family -eq 'DLC/LLC private driver' }
if ($dlc.status -notmatch 'no public modern semantic equivalent') { throw 'DLC/LLC disposition must not invent a modern provider.' }
$irq = $rows | Where-Object { $_.interface_family -eq 'physical completion interrupt' }
if ($irq.selected_boundary -notmatch 'adapter-bochs') { throw 'Physical completion must remain selector-blind adapter-bochs mechanics.' }
$pointers = $rows | Where-Object { $_.interface_family -eq 'real/protected guest aliases' }
if ($pointers.selected_boundary -notmatch 'session guest_memory') { throw 'Guest aliases must use the session guest-memory lease boundary.' }

$sourceFiles = Get-ChildItem -LiteralPath $sourceRoot -Filter '*.c' -File
if (@($sourceFiles | Select-String -Pattern 'adapter-bochs|bochs-core|src\.old' ).Count -ne 0) {
    throw 'Original Redirector mirror directly names a prohibited product boundary.'
}

Write-Output 'T290 S2 Redirector interface ledger passed: 16 source-first boundary dispositions.'
