[CmdletBinding()]
param([string]$RepositoryRoot = '')

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$plan = Join-Path $root 'docs/etc/operations/m0-t280-s6-dem-compile-surface-plan-001.md'
$ledger = Join-Path $root 'docs/etc/operations/m0-t280-s6-dem-compile-surface-ledger.tsv'
$evidence = Join-Path $root 'docs/etc/operations/m0-t280-s6-dem-compile-surface-evidence-001.md'
foreach ($path in @($plan, $ledger, $evidence)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing T280 S6 artifact: $path" }
}
$expected = @(
    'dem.c','demfcb.c','demdata.c','demdir.c','demdisp.c','demerror.c','demfile.c','demgset.c',
    'demhndl.c','demioctl.c','demlock.c','demmisc.c','demmsg.c','demsrch.c','demdasd.c','demlabel.c'
)
$rows = Import-Csv -LiteralPath $ledger -Delimiter "`t"
if ($rows.Count -ne $expected.Count) { throw "Expected $($expected.Count) DEM rows; found $($rows.Count)." }
foreach ($source in $expected) {
    $row = @($rows | Where-Object source -eq $source)
    if ($row.Count -ne 1) { throw "Expected one ledger row for $source." }
    foreach ($field in @('x64_syntax_survey','x86_syntax_survey','first_blocking_family','future_owner','mapping_manager_rule','next_disposition')) {
        if ([string]::IsNullOrWhiteSpace($row[0].$field)) { throw "$source has no $field disposition." }
    }
}
$text = Get-Content -LiteralPath $plan -Raw
foreach ($token in @('guest_memory','mapping-manager','no original DEM body edit','sixteen')) {
    if ($text -notmatch [regex]::Escape($token)) { throw "T280 S6 plan misses required boundary token: $token" }
}
Write-Host 'PASS: T280 S6 ledger covers the whole original DEM selection with explicit owner and mapping disposition.'
