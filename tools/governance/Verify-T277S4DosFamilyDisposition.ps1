[CmdletBinding()]
param([string]$RepositoryRoot = '')

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}

$ErrorActionPreference = 'Stop'
$ops = Join-Path (Resolve-Path -LiteralPath $RepositoryRoot).Path 'docs/etc/operations'
$work = @(Import-Csv -LiteralPath (Join-Path $ops 'm0-t277-s2-source-form-worklist.tsv') -Delimiter "`t" | Where-Object { $_.caller_package -eq 'dos' })
$ledger = @(Import-Csv -LiteralPath (Join-Path $ops 'm0-t277-s4-dos-family-disposition-ledger.tsv') -Delimiter "`t")

if ($work.Count -ne 73) { throw "Expected 73 S2 DOS work items; found $($work.Count)." }
if ($ledger.Count -ne $work.Count) { throw "S4 ledger count $($ledger.Count) does not cover S2 DOS work $($work.Count)." }
if ((Compare-Object ($work.work_item_id | Sort-Object) ($ledger.work_item_id | Sort-Object))) { throw 'S4 ledger work-item coverage differs from the S2 DOS set.' }
if (($ledger.work_item_id | Sort-Object -Unique).Count -ne $work.Count) { throw 'S4 work-item identities are not unique.' }
if (($ledger | Where-Object { $_.audit_state -ne 'source-form-family-reviewed; implementation-not-enabled' }).Count) { throw 'S4 must not enable an implementation state.' }
if (($ledger | Where-Object { [string]::IsNullOrWhiteSpace($_.original_provider_form) -or [string]::IsNullOrWhiteSpace($_.owner) -or [string]::IsNullOrWhiteSpace($_.failure_disposition) }).Count) { throw 'S4 requires provider form, owner, and failure disposition on every row.' }
if (($ledger.family | Sort-Object -Unique).Count -ne 8) { throw 'Expected eight DOS source-form families.' }
if (@($ledger | Where-Object { $_.symbol -eq 'TerminateVDM' -and $_.owner -notlike 'session*' }).Count) { throw 'TerminateVDM must be session-owned controlled stop, not a process exit.' }
if (@($ledger | Where-Object { $_.symbol -in @('cmdPipeFileDataEOF','cmdPipeFileEOF') -and $_.mapping_rule -notlike 'session host_resource*' }).Count) { throw 'Pipe calls must name the unique session host_resource mapping instance.' }
if (@($ledger | Where-Object { $_.symbol -eq 'host_simulate' -and $_.owner -ne 'adapter-mvdm-host-out/softpc -> adapter-bochs' }).Count) { throw 'host_simulate must retain the S3 machine owner.' }

Write-Host "T277 S4 DOS family disposition passed: work-items=$($ledger.Count); families=8."
