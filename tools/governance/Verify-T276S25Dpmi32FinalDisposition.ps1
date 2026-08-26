[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$path = Join-Path (Resolve-Path $RepositoryRoot).Path 'docs/etc/operations/m0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t" | Where-Object {$_.source_path -like 'dpmi32/*'})
if ($rows.Count -ne 25) { throw "Expected 25 dpmi32 rows, found $($rows.Count)." }
if (@($rows | Where-Object {$_.final_disposition -eq 'unresolved'}).Count) { throw 'No dpmi32 row may remain unresolved after the final package audit.' }
if (@($rows | Where-Object {$_.final_audit_state -ne 'final-disposition-audited; not implemented'}).Count) { throw 'Every dpmi32 row needs the final audited implementation state.' }
$x86Bodies = @(
    'dpmi32/buffer.c', 'dpmi32/data.c', 'dpmi32/debug.c', 'dpmi32/dpmi32.c',
    'dpmi32/dpmiint.c', 'dpmi32/dpmiselr.c', 'dpmi32/i386/dpmi386.c',
    'dpmi32/i386/dpmimem.c', 'dpmi32/i386/dpmimisc.c', 'dpmi32/int21map.c',
    'dpmi32/modesw.c', 'dpmi32/register.c', 'dpmi32/savestat.c',
    'dpmi32/stack.c', 'dpmi32/vxd.c', 'dpmi32/xmem.c'
)
if (@($rows | Where-Object {$_.source_path -in $x86Bodies -and $_.final_disposition -ne 'adapter-backed'}).Count) { throw 'Selected original x86 DPMI32 provider bodies must be adapter-backed.' }
if (@($rows | Where-Object {$_.source_path -in @('dpmi32/dpmimemr.c', 'dpmi32/dpmimscr.c') -and $_.final_disposition -ne 'not-host-runtime'}).Count) { throw 'RISC-only DPMI32 bodies must remain out of the final host graph.' }
Write-Output "T276 S25 DPMI32 final-disposition verification passed: $($rows.Count) rows."
