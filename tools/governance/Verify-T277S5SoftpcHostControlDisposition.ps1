[CmdletBinding()]
param([string]$RepositoryRoot = '')
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot) }
$ErrorActionPreference='Stop'; $ops=Join-Path (Resolve-Path -LiteralPath $RepositoryRoot).Path 'docs/etc/operations'
$work=@(Import-Csv (Join-Path $ops 'm0-t277-s2-source-form-worklist.tsv') -Delimiter "`t" | Where-Object {$_.caller_package -eq 'softpc.new'})
$ledger=@(Import-Csv (Join-Path $ops 'm0-t277-s5-softpc-host-control-disposition-ledger.tsv') -Delimiter "`t")
if($work.Count -ne 197 -or $ledger.Count -ne 197){throw "Expected 197 SoftPC work and ledger rows; got work=$($work.Count), ledger=$($ledger.Count)."}
if(Compare-Object ($work.work_item_id|Sort-Object) ($ledger.work_item_id|Sort-Object)){throw 'S5 ledger differs from the S2 SoftPC work set.'}
if(($ledger.work_item_id|Sort-Object -Unique).Count -ne 197){throw 'S5 work identifiers are not unique.'}
if(($ledger|Where-Object {$_.audit_state -ne 'source-form-family-reviewed; implementation-not-enabled'}).Count){throw 'S5 must not enable an implementation state.'}
if(($ledger|Where-Object {[string]::IsNullOrWhiteSpace($_.original_provider_form)-or [string]::IsNullOrWhiteSpace($_.owner)-or [string]::IsNullOrWhiteSpace($_.failure_disposition)}).Count){throw 'S5 requires provider form, owner, and failure disposition on every row.'}
if(($ledger.family|Sort-Object -Unique).Count -ne 9){throw 'Expected nine SoftPC source-form families.'}
if(@($ledger|Where-Object {$_.family -eq 'machine-CPU-SAS-PIC-facade' -and $_.owner -ne 'adapter-mvdm-host-out/softpc -> adapter-bochs'}).Count){throw 'All CPU/SAS/PIC facade rows must use the sole mechanical owner.'}
if(@($ledger|Where-Object {$_.family -eq 'BOP-dispatch-control' -and $_.owner -notlike 'adapter-mvdm-host-in*'}).Count){throw 'All BOP dispatch rows must enter through adapter-mvdm-host-in.'}
if(@($ledger|Where-Object {$_.family -eq 'session-stop-and-event' -and $_.failure_disposition -notlike 'controlled stop*'}).Count){throw 'Session stop/event rows must reject process termination.'}
Write-Host "T277 S5 SoftPC host-control disposition passed: work-items=$($ledger.Count); families=9."
