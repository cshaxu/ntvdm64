param(
    [string]$OperationsDirectory = (Join-Path $PSScriptRoot '..\..\docs\etc\operations')
)

$ErrorActionPreference = 'Stop'
$edges = @(Import-Csv (Join-Path $OperationsDirectory 'm0-t277-effective-package-boundary-edge-ledger.tsv') -Delimiter "`t")
$cycles = @(Import-Csv (Join-Path $OperationsDirectory 'm0-t277-effective-package-cycle-ledger.tsv') -Delimiter "`t")
$profiles = @(Import-Csv (Join-Path $OperationsDirectory 'm0-t277-effective-build-profile-ledger.tsv') -Delimiter "`t")

if (($edges | Measure-Object reviewed_call_count -Sum).Sum -ne 325) { throw 'S8 edges do not sum to 325 reviewed calls.' }
if ($cycles.Count -ne 1 -or $cycles[0].source_packages -ne 'dos <-> softpc.new') { throw 'S8 requires exactly the explicit DOS/SoftPC source cycle.' }
if ($profiles.Count -ne 23) { throw "S8 requires 23 package profile rows; found $($profiles.Count)." }
if (@($profiles | Where-Object { $_.x86_admission_state -notmatch '^not-enabled;' -or $_.x64_admission_state -notmatch '^not-enabled;' }).Count) { throw 'A profile overclaims enabled state.' }
if (@($profiles | Where-Object { $_.build_claim -ne 'no compile, link or provider enablement claimed by T277' }).Count) { throw 'A profile overclaims build evidence.' }
if ((@($profiles | Where-Object package_root -eq 'dpmi')[0].x64_admission_state) -notmatch 'guest-only') { throw 'DPMI must remain guest-only.' }
if ((@($profiles | Where-Object package_root -eq 'softpc.new')[0].x64_admission_state) -notmatch 'Bochs route') { throw 'SoftPC profile must record Bochs executor replacement.' }
Write-Host "PASS: $($edges.Count) edges/325 calls, one source cycle and 23 not-enabled x86/x64 profiles."
