[CmdletBinding()]
param([string]$RepositoryRoot = '')

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$ledgerPath = Join-Path $root 'docs/etc/operations/m0-t280-s2-ingress-machine-facade-ledger.tsv'
$ledger = @(Import-Csv -LiteralPath $ledgerPath -Delimiter "`t")
$expected = @('T280-S2-001','T280-S2-002','T280-S2-003','T280-S2-004','T280-S2-005','T280-S2-006','T280-S2-007','T280-S2-008','T280-S2-009','T280-S2-010','T280-S2-011','T280-S2-012','T280-S2-013')

if ($ledger.Count -ne $expected.Count) { throw "Expected $($expected.Count) S2 interface rows; found $($ledger.Count)." }
foreach ($id in $expected) {
    if (@($ledger | Where-Object interface_id -eq $id).Count -ne 1) { throw "Missing or duplicate interface row: $id." }
}
if (@($ledger | Where-Object { $_.exclusive_owner_chain -match 'bochs-core' }).Count -ne 0) {
    throw 'An S2 OpenNT caller cannot target bochs-core directly.'
}
foreach ($row in $ledger) {
    if ([string]::IsNullOrWhiteSpace($row.original_interface_shape) -or
        [string]::IsNullOrWhiteSpace($row.exclusive_owner_chain) -or
        [string]::IsNullOrWhiteSpace($row.mapping_rule) -or
        [string]::IsNullOrWhiteSpace($row.unavailable_or_successor)) {
        throw "Incomplete source-shaped boundary record: $($row.interface_id)."
    }
}
if (@($ledger | Where-Object { $_.implementation_status -notmatch 'not enabled' }).Count -ne 0) {
    throw 'S2 cannot enable an original host-control implementation.'
}
foreach ($id in @('T280-S2-005','T280-S2-006','T280-S2-007')) {
    if ((@($ledger | Where-Object interface_id -eq $id)[0].implementation_status) -notmatch '^not-host-runtime source body') {
        throw "$id must retain nt_aorc.c as not-host-runtime evidence, not an importable provider body."
    }
}
foreach ($id in @('T280-S2-009','T280-S2-010','T280-S2-011','T280-S2-012','T280-S2-013')) {
    if ((@($ledger | Where-Object interface_id -eq $id)[0].implementation_status) -notmatch '^overlay-required') {
        throw "$id must retain sim32.c as overlay-required evidence."
    }
}
Write-Host 'PASS: T280 S2 ingress and machine-façade declaration map is complete and Bochs-blind.'
