[CmdletBinding()]
param([string]$RepositoryRoot = '')

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}

$ErrorActionPreference = 'Stop'
$ops = Join-Path (Resolve-Path -LiteralPath $RepositoryRoot).Path 'docs/etc/operations'
$work = @(
    Import-Csv -LiteralPath (Join-Path $ops 'm0-t277-s2-source-form-worklist.tsv') -Delimiter "`t" |
        Where-Object { $_.caller_package -in @('suballoc', 'xms.486', 'dpmi32') }
)
$ledger = @(Import-Csv -LiteralPath (Join-Path $ops 'm0-t277-s3-machine-family-disposition-ledger.tsv') -Delimiter "`t")
$corrections = @(Import-Csv -LiteralPath (Join-Path $ops 'm0-t277-s3-file-disposition-corrections.tsv') -Delimiter "`t")

if ($work.Count -ne 38) { throw "Expected 38 S2 first-tranche work items; found $($work.Count)." }
if ($ledger.Count -ne $work.Count) { throw "S3 ledger count $($ledger.Count) does not cover S2 tranche $($work.Count)." }
if (($ledger.work_item_id | Sort-Object -Unique).Count -ne $work.Count) { throw 'S3 work-item identities are not unique.' }
if ((Compare-Object ($work.work_item_id | Sort-Object) ($ledger.work_item_id | Sort-Object))) { throw 'S3 ledger work-item coverage differs from the S2 tranche.' }
if (($ledger | Where-Object { $_.audit_state -ne 'source-form-family-reviewed; implementation-not-enabled' }).Count) { throw 'S3 must not enable an implementation state.' }
if (($ledger | Where-Object { [string]::IsNullOrWhiteSpace($_.original_provider_form) -or [string]::IsNullOrWhiteSpace($_.owner) -or [string]::IsNullOrWhiteSpace($_.failure_disposition) }).Count) { throw 'S3 requires provider form, owner, and failure disposition on every row.' }
if (($ledger.family | Sort-Object -Unique).Count -ne 7) { throw 'Expected seven source-form families.' }
if ($corrections.Count -ne 3) { throw "Expected three T276 XMS treatment corrections; found $($corrections.Count)." }
if (($corrections.corrected_change_class | Where-Object { $_ -ne 'overlay-required' }).Count) { throw 'Every S3 XMS correction must require an overlay.' }
foreach ($path in @('xms.486/xmsa20.c', 'xms.486/xmsblock.c', 'xms.486/xmsumb.c')) {
    if (@($corrections | Where-Object { $_.source_path -eq $path }).Count -ne 1) { throw "Expected exactly one correction for $path." }
}
if (($ledger | Where-Object { $_.source_compatibility -like 'T277 correction:*' }).Count -lt 5) { throw 'Expected per-call source compatibility correction evidence for the XMS raw-pointer assumptions.' }

Write-Host "T277 S3 machine-family disposition passed: work-items=$($ledger.Count); families=7; xms-corrections=$($corrections.Count)."
