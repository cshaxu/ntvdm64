[CmdletBinding()]
param(
    [string]$RepositoryRoot = ''
)

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$ErrorActionPreference = 'Stop'
$ledger = Join-Path $RepositoryRoot 'docs/etc/operations/m0-t282-s8-command-owner-dependency-ledger-001.tsv'
$rows = Import-Csv -LiteralPath $ledger -Delimiter "`t"
$required = @(
    'CMD-54-00','CMD-54-01','CMD-54-02','CMD-54-03','CMD-54-04','CMD-54-05',
    'CMD-54-06','CMD-54-07','CMD-54-08','CMD-54-09','CMD-54-0A','CMD-54-0B',
    'CMD-54-0C','CMD-54-0D','CMD-54-0E','CMD-54-0F','CMD-54-10',
    'CMD-INIT-001','CMD-INIT-002','CMD-HELPER-ENV-001','CMD-HELPER-ENV-002',
    'CMD-HELPER-DIR-001','CMD-HELPER-CMDINFO-001','CMD-HELPER-SAVE-001',
    'CMD-HELPER-CONSOLE-001','CMD-HELPER-CONFIG-001','CMD-HELPER-PIF-001',
    'CMD-HELPER-STREAM-001','CMD-HELPER-BINARY-001','CMD-HELPER-CHILD-001',
    'CMD-HELPER-WOW-001','CMD-HELPER-DISPATCH-001')
foreach ($id in $required) {
    $row = @($rows | Where-Object { $_.id -eq $id })
    if ($row.Count -ne 1 -or [string]::IsNullOrWhiteSpace($row[0].current_disposition) -or
        [string]::IsNullOrWhiteSpace($row[0].next_owner_slice)) {
        throw "Missing complete COMMAND owner-partition row: $id"
    }
}
if (@($rows | Where-Object { [string]::IsNullOrWhiteSpace($_.recovery_rung) }).Count -ne 0) {
    throw 'Every COMMAND partition row needs a recovery rung.'
}
Write-Host "PASS: T282 S8 COMMAND owner dependency partition ($($rows.Count) rows)"
