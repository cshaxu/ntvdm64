[CmdletBinding()]
param([string]$RepositoryRoot = '')
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot) }
$ErrorActionPreference = 'Stop'; $ops = Join-Path (Resolve-Path -LiteralPath $RepositoryRoot).Path 'docs/etc/operations'
function Read-Tsv([string]$Name) { @(Import-Csv -LiteralPath (Join-Path $ops $Name) -Delimiter "`t") }
function Require([bool]$Condition, [string]$Message) { if (-not $Condition) { throw $Message } }
$work = Read-Tsv 'm0-t277-s2-source-form-worklist.tsv'; $summary = Read-Tsv 'm0-t277-s2-source-form-summary.tsv'
Require ($work.Count -eq 325) "Expected 325 future-host work items; found $($work.Count)."
Require ((@($work.work_item_id | Select-Object -Unique).Count) -eq 325) 'Work item IDs are not unique.'
Require ((@($work | Where-Object { $_.caller_final_disposition -notin @('adapter-backed','binding-only','overlay-required') }).Count) -eq 0) 'An excluded caller entered the worklist.'
Require ((@($work | Group-Object caller_final_disposition | Where-Object { $_.Name -eq 'adapter-backed' }).Count) -eq 1) 'Expected adapter-backed entries.'
Require ((@($work | Where-Object { $_.source_form_state -ne 'pending-original-caller-and-provider-form-review' }).Count) -eq 0) 'S2 worklist claims source-form completion prematurely.'
$all = $summary | Where-Object { $_.summary_id -eq 'MVDM-T277-S2-ALL' }; Require (($all.count -as [int]) -eq 3697) 'Full correlation population is not preserved.'
Write-Host "T277 S2 source-form worklist passed: work-items=$($work.Count); all-correlations=$($all.count)."
