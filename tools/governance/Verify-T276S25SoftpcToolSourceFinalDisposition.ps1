[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$path = Join-Path (Resolve-Path $RepositoryRoot).Path 'docs/etc/operations/m0-t276-final-integration-disposition-ledger.tsv'
$tools = @(Import-Csv $path -Delimiter "`t" | Where-Object {
    $_.file_kind -eq 'source' -and ($_.source_path -like 'softpc.new/base/dasm386/*' -or $_.source_path -like 'softpc.new/base/debug/*' -or $_.source_path -eq 'softpc.new/debugger/vdmdebug.c')
})
$entries = @(Import-Csv $path -Delimiter "`t" | Where-Object {$_.source_path -in @('softpc.new/obj.vdm/c_regs.c', 'softpc.new/obj.vdm/ntvdm.c')})
if ($tools.Count -ne 12 -or @($tools | Where-Object {$_.final_disposition -ne 'tool-only'}).Count) { throw 'SoftPC debugger/disassembler sources must all be tool-only.' }
if ($entries.Count -ne 2 -or @($entries | Where-Object {$_.final_disposition -ne 'not-host-runtime'}).Count) { throw 'Historical standalone product-entry sources must remain out of the final host graph.' }
Write-Output "T276 S25 SoftPC tool/product source final-disposition verification passed: $($tools.Count + $entries.Count) rows."
