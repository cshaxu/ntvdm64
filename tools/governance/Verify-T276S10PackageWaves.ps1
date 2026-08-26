[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$operations = Join-Path $root 'docs/etc/operations'
$packagePath = Join-Path $operations 'mvdm-package-dependency-ledger.tsv'
$wavePath = Join-Path $operations 'm0-t276-s10-package-profile-wave-ledger.tsv'
foreach ($path in @($packagePath, $wavePath)) {
    if (-not (Test-Path -LiteralPath $path)) { throw "Missing required ledger: $path" }
}

$packages = Import-Csv -LiteralPath $packagePath -Delimiter "`t"
$waves = Import-Csv -LiteralPath $wavePath -Delimiter "`t"
if ($packages.Count -ne 23) { throw "Expected 23 selected package roots, found $($packages.Count)." }
if ($waves.Count -ne $packages.Count) { throw "Wave rows $($waves.Count) do not match package rows $($packages.Count)." }
if (($waves | Group-Object package_root | Where-Object Count -ne 1).Count -ne 0) { throw 'Wave ledger has duplicate package roots.' }
foreach ($package in $packages) {
    $row = @($waves | Where-Object package_root -eq $package.package_root)
    if ($row.Count -ne 1) { throw "Missing wave row for $($package.package_root)." }
    foreach ($column in 'original_role','original_build_profile_disposition','predecessor_packages_or_interfaces','candidate_wave','profile_state','blocked_successor','evidence') {
        if ([string]::IsNullOrWhiteSpace($row[0].$column)) { throw "Empty $column for $($package.package_root)." }
    }
    if ($row[0].profile_state -ne 'unreviewed') { throw "S10 must not enable profile $($package.package_root)." }
}

$allowed = @('W0-source-carriers','W1-support','W2-machine-boundary','W3-monitor-memory','W4-host-service','W5-product-extensions')
if (($waves.candidate_wave | Where-Object { $_ -notin $allowed }).Count -ne 0) { throw 'Wave ledger contains an unapproved wave.' }
Write-Output "T276 S10 package-wave verification passed: $($waves.Count) package rows."
