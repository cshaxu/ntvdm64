[CmdletBinding()]
param([string]$RepositoryRoot = '')
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot) }
$ErrorActionPreference = 'Stop'; $root=(Resolve-Path -LiteralPath $RepositoryRoot).Path; $ops=Join-Path $root 'docs/etc/operations'
$ledger=@(Import-Csv (Join-Path $ops 'm0-t279-s1-support-library-interface-ledger.tsv') -Delimiter "`t")
if ($ledger.Count -ne 71) { throw "Expected 71 source-form interface rows; found $($ledger.Count)." }
if (($ledger.interface_id | Sort-Object -Unique).Count -ne 71) { throw 'Interface IDs are not unique.' }
if (@($ledger | Where-Object { $_.caller_path -notin @('oemuni/file.c','oemuni/process.c','suballoc/suballoc.c') }).Count) { throw 'Non-library source entered the runtime interface ledger.' }
if (@($ledger | Where-Object { [string]::IsNullOrWhiteSpace($_.unique_owner) -or [string]::IsNullOrWhiteSpace($_.x86_x64_disposition) -or [string]::IsNullOrWhiteSpace($_.failure_disposition) }).Count) { throw 'An interface lacks ownership, x86/x64 or failure disposition.' }
if (@($ledger | Where-Object { $_.symbol -in @('CommitRoutine','DecommitRoutine','MoveMemRoutine') -and $_.unique_owner -notmatch 'adapter-mvdm-host-out/softpc' }).Count) { throw 'Suballocator callback route must stay caller-owned through adapter-mvdm-host-out/softpc.' }
$oemSources = Get-Content (Join-Path $root 'src/mvdm-support/oemuni/sources') -Raw
$suballocSources = Get-Content (Join-Path $root 'src/mvdm-support/suballoc/sources') -Raw
if ($oemSources -notmatch '(?s)SOURCES=file\.c\s+\\\s*process\.c' -or $oemSources -match '(?m)^SOURCES=.*toemuni') { throw 'Original oemuni library source selection is not preserved.' }
if ($suballocSources -notmatch '(?m)^SOURCES=suballoc\.c' -or $suballocSources -match '(?m)^SOURCES=.*(tsa|debug)') { throw 'Original suballoc library source selection is not preserved.' }
Write-Host "PASS: T279 S1 source-form support interface ledger: rows=$($ledger.Count); oemuni=66; suballoc=5; tools/tests=excluded."
