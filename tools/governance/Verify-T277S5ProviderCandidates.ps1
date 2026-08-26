[CmdletBinding()]
param([string]$RepositoryRoot = '')

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}

$ErrorActionPreference = 'Stop'
$ops = Join-Path (Resolve-Path -LiteralPath $RepositoryRoot).Path 'docs/etc/operations'
$work = @(Import-Csv -LiteralPath (Join-Path $ops 'm0-t277-s2-source-form-worklist.tsv') -Delimiter "`t" | Where-Object { $_.caller_package -eq 'softpc.new' })
$rows = @(Import-Csv -LiteralPath (Join-Path $ops 'm0-t277-s5-softpc-provider-candidate-ledger.tsv') -Delimiter "`t")
$symbols = @($work.symbol | Sort-Object -Unique)

if ($work.Count -ne 197) { throw "Expected 197 SoftPC work items; found $($work.Count)." }
if ($symbols.Count -ne 76) { throw "Expected 76 SoftPC symbols; found $($symbols.Count)." }
if ((Compare-Object $symbols ($rows.symbol | Sort-Object -Unique))) { throw 'Provider-candidate ledger does not cover the exact SoftPC symbol set.' }
if (($rows | Where-Object { $_.evidence_state -ne 'lexical-candidate-only; definition-versus-call-or-declaration-not-inferred' }).Count) { throw 'Provider candidates must not be treated as definition proof.' }
if (@($rows | Where-Object { $_.candidate_class -eq 'no-lexical-provider-candidate' }).Count) { throw 'Every current SoftPC symbol must retain at least one source candidate.' }
if (($rows | Where-Object { [string]::IsNullOrWhiteSpace($_.candidate_path) -or [string]::IsNullOrWhiteSpace($_.candidate_line) }).Count) { throw 'Every provider candidate must cite a mirror source location.' }

Write-Host "T277 S5 provider-candidate evidence passed: work-items=$($work.Count); symbols=$($symbols.Count); candidates=$($rows.Count)."
