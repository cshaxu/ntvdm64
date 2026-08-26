[CmdletBinding()]
param([string]$RepositoryRoot = '')

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
foreach ($relative in @(
    'docs/etc/operations/m0-t280-s10-dem-ntioapi-abi-closure-plan-001.md',
    'docs/etc/operations/m0-t280-s10-dem-ntioapi-reached-abi-ledger.tsv',
    'docs/etc/operations/m0-t280-s10-dem-ntioapi-abi-evidence-001.md',
    'docs/etc/operations/m0-t280-s10-dem-ntioapi-implementation-plan-001.md',
    'src/opennt-platform-abi/source/opennt/public/sdk/inc/ntioapi.h'
)) {
    if (-not (Test-Path -LiteralPath (Join-Path $root $relative) -PathType Leaf)) { throw "Missing T280 S10 artifact: $relative" }
}
$rows = Import-Csv -LiteralPath (Join-Path $root 'docs/etc/operations/m0-t280-s10-dem-ntioapi-reached-abi-ledger.tsv') -Delimiter "`t"
if ($rows.Count -ne 11) { throw "Expected 11 reached NTIO rows; found $($rows.Count)." }
foreach ($row in $rows) {
    foreach ($field in @('original_declaration_or_call','original_location_and_consumers','modern_availability','final_recovery_disposition','failure_and_lifetime_rule','mapping_manager_rule')) {
        if ([string]::IsNullOrWhiteSpace($row.$field)) { throw "$($row.id) misses $field." }
    }
}
if (@($rows | Where-Object { $_.mapping_manager_rule -notmatch 'no mapping|separate guest' }).Count -ne 0) {
    throw 'An NTIO host record/handle was incorrectly classified as a mapping-manager identity.'
}
$all = ($rows | ConvertTo-Csv -NoTypeInformation) -join "`n"
foreach ($token in @('NtQueryDirectoryFile','NtQueryVolumeInformationFile','NtOpenSymbolicLinkObject','FILE_BOTH_DIR_INFORMATION','FILE_FS_DEVICE_INFORMATION')) {
    if ($all -notmatch [regex]::Escape($token)) { throw "Reached NTIO ledger misses $token." }
}
 $implementation = Get-Content -Raw -LiteralPath (Join-Path $root 'docs/etc/operations/m0-t280-s10-dem-ntioapi-implementation-plan-001.md')
foreach ($token in @('reached-subset facade', 'adapter-win32', 'host-local', 'session.guest_memory', 'explicit source-shaped')) {
    if ($implementation -notmatch [regex]::Escape($token)) { throw "T280 S10 implementation plan misses $token." }
}
Write-Host 'PASS: T280 S10 ledger covers all reached NTIO declaration/call families with mapping isolation.'
