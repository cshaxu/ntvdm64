[CmdletBinding()]
param([string]$RepositoryRoot = '')

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$governance = Join-Path $root 'tools/governance'
$ops = Join-Path $root 'docs/etc/operations'

$verifiers = @(
    'Verify-T277S1TrackerFoundation.ps1',
    'Verify-T277S2SourceFormWorklist.ps1',
    'Verify-T277S3MachineFamilyDisposition.ps1',
    'Verify-T277S4DosFamilyDisposition.ps1',
    'Verify-T277S5ProviderCandidates.ps1',
    'Verify-T277S5SoftpcHostControlDisposition.ps1',
    'Verify-T277S6RedirWowDisposition.ps1',
    'Verify-T277S7EffectiveCorrelationLedger.ps1'
)
foreach ($verifier in $verifiers) {
    & (Join-Path $governance $verifier) -RepositoryRoot $root
    if ($null -ne $LASTEXITCODE -and $LASTEXITCODE -ne 0) { throw "$verifier failed with exit code $LASTEXITCODE." }
}
& (Join-Path $governance 'Verify-T277S8PackageProfileLedgers.ps1') -OperationsDirectory $ops

$files = @(Import-Csv (Join-Path $ops 'm0-t277-file-recovery-ledger.tsv') -Delimiter "`t")
$divergences = @(Import-Csv (Join-Path $ops 'm0-t277-divergence-exclusion-ledger.tsv') -Delimiter "`t")
$effective = @(Import-Csv (Join-Path $ops 'm0-t277-effective-source-correlation-ledger.tsv') -Delimiter "`t")
$edges = @(Import-Csv (Join-Path $ops 'm0-t277-effective-package-boundary-edge-ledger.tsv') -Delimiter "`t")
$profiles = @(Import-Csv (Join-Path $ops 'm0-t277-effective-build-profile-ledger.tsv') -Delimiter "`t")

if ($files.Count -ne 1689 -or ($files.file_id | Sort-Object -Unique).Count -ne 1689) { throw 'File recovery identity closure failed.' }
if ($divergences.Count -ne 1689 -or ($divergences.file_id | Sort-Object -Unique).Count -ne 1689) { throw 'Divergence/exclusion identity closure failed.' }
if (Compare-Object ($files.file_id | Sort-Object) ($divergences.file_id | Sort-Object)) { throw 'File and divergence ledgers do not cover the same identities.' }
if (@($effective | Where-Object effective_state -eq 'reviewed-future-host').Count -ne 325) { throw 'Reviewed source correlation closure failed.' }
if (@($effective | Where-Object effective_state -eq 'non-runtime-or-profile-excluded').Count -ne 3372) { throw 'Excluded source correlation closure failed.' }
if (@($effective | Where-Object { $_.effective_state -eq 'reviewed-future-host' -and ([string]::IsNullOrWhiteSpace($_.owner) -or [string]::IsNullOrWhiteSpace($_.family) -or [string]::IsNullOrWhiteSpace($_.failure_disposition) -or [string]::IsNullOrWhiteSpace($_.source_form_evidence)) }).Count) { throw 'A reviewed future-host correlation lacks source-form ownership or failure evidence.' }
if (($edges | Measure-Object reviewed_call_count -Sum).Sum -ne 325) { throw 'Package-edge aggregation does not join reviewed source calls.' }
if ($profiles.Count -ne 23) { throw 'Package profile closure failed.' }
Write-Host 'PASS: T277 closure audit: files=1689; divergence=1689; correlations=3697; reviewed=325; excluded=3372; lexical=4803; profiles=23; explicit-source-cycles=1.'
