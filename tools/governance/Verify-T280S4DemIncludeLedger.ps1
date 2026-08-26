[CmdletBinding()]
param([string]$RepositoryRoot = '')

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$ledger = @(Import-Csv -LiteralPath (Join-Path $root 'docs/etc/operations/m0-t280-s4-dem-include-abi-ledger.tsv') -Delimiter "`t")
$expectedBodies = @('dem.c','demfcb.c','demdata.c','demdir.c','demdisp.c','demerror.c','demfile.c','demgset.c','demhndl.c','demioctl.c','demlock.c','demmisc.c','demmsg.c','demsrch.c','demdasd.c','demlabel.c')
if (@($ledger | Select-Object -ExpandProperty dem_source -Unique).Count -ne 16) { throw 'Expected exactly 16 DEM source bodies.' }
foreach ($body in $expectedBodies) {
    if (@($ledger | Where-Object dem_source -eq "dos/dem/$body").Count -eq 0) { throw "Missing DEM source in include ledger: $body" }
}
foreach ($required in @('local-mirror','support-mirror','platform-abi-mirror','public-sdk-or-crt','adapter-gap')) {
    if (@($ledger | Where-Object disposition -eq $required).Count -eq 0) { throw "Missing DEM include disposition: $required" }
}
$gapRows = @($ledger | Where-Object disposition -eq 'adapter-gap')
if (@($gapRows | Where-Object { $_.direct_include -eq 'winbasep.h' -and $_.final_owner -eq 'adapter-win32' }).Count -eq 0) { throw 'Missing adapter-win32 winbasep.h recovery gap.' }
if (@($gapRows | Where-Object { $_.direct_include -eq 'vdm.h' -and $_.final_owner -eq 'adapter-win32 -> session' }).Count -eq 0) { throw 'Missing adapter-win32/session vdm.h recovery gap.' }
if (@($ledger | Where-Object { $_.admission_state -ne 'S4-audited; no DEM body enabled' }).Count -ne 0) { throw 'S4 must not enable a DEM body.' }
Write-Host "PASS: T280 S4 DEM include ledger covers $($ledger.Count) direct includes across all 16 original bodies."
