[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path $RepositoryRoot).Path
$ops = Join-Path $root 'docs/etc/operations'
$path = Join-Path $ops 'm0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t")
$debugTools = @($rows | Where-Object {$_.source_path -match '^(bde|dbg|vdmdbg|vdmexts)/'})
if ($debugTools.Count -ne 47) { throw "Expected 47 debugger-tool rows, found $($debugTools.Count)." }

foreach ($row in $debugTools) {
    $row.final_disposition = 'tool-only'
    $row.final_owner_or_link_boundary = 'independent original NTSD/WinDbg debugger extension or VDM debugging-support product; never linked into the main host runtime'
    $row.final_change_class = 'none; byte-exact source mirror retained for separately admitted debugger-tool recovery'
    $row.named_adapter = 'none; a future adapter-mvdm-host-out/debugger requires its own whole-tool admission'
    $row.mapping_implication = 'no debugger-process address, HANDLE or host pointer crosses a main-runtime boundary; any future tool uses copied diagnostic snapshots or an explicitly admitted broker'
    $row.final_evidence = 'T276 S25 debugger-tools source review: original TARGETTYPE=DYNLINK extension/DLL or static debug support product, with NTSD/WinDbg external-debugger dependencies'
    $row.final_audit_state = 'final-disposition-audited; not implemented'
}

$rows | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" | Set-Content $path -Encoding utf8
Write-Output "T276 S25 debugger-tool final dispositions applied: tool-only=$($debugTools.Count)."
