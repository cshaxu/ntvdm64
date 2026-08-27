[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path $RepositoryRoot).Path
$path = Join-Path $root 'docs/etc/operations/m0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t")
$v86 = @($rows | Where-Object {$_.source_path -like 'v86/*'})
if ($v86.Count -ne 35) { throw "Expected 35 V86 rows, found $($v86.Count)." }
foreach ($row in $v86) {
    $row.final_audit_state = 'final-disposition-audited; not implemented'
    if ($row.source_path -eq 'v86/monitor/i386/monitorp.h') {
        $row.final_disposition = 'binding-only'
        $row.final_owner_or_link_boundary = 'adapter-mvdm-host-out/monitor same-shaped monitor declarations -> session guest-memory mapping manager -> adapter-mvdm-host-out/softpc -> adapter-bochs'
        $row.final_change_class = 'binding-only; retain original monitor declaration/layout shape while excluding kernel monitor bodies'
        $row.named_adapter = 'adapter-mvdm-host-out/monitor; adapter-mvdm-host-out/softpc; adapter-bochs'
        $row.mapping_implication = 'CPU frames are copied and guest ranges use the single session guest-memory mapping-manager lease'
        $row.final_evidence = 'T276 S17: monitor declaration/API recovery prerequisite; kernel VDM monitor itself is excluded'
        continue
    }
    if ($row.source_path -like 'v86/scaffold/i386/*.h') {
        $row.final_disposition = 'binding-only'
        $row.final_owner_or_link_boundary = 'app/adapter declaration evidence for individually admitted same-shaped boundaries; excluded standalone ntvdm.exe scaffold is not composed'
        $row.final_change_class = 'binding-only; retain original declaration/layout form outside the product-shell graph'
        $row.named_adapter = 'adapter-mvdm-host-in; adapter-mvdm-host-out/softpc; adapter-bochs; adapter-mvdm-host-out/win32; session'
        $row.mapping_implication = 'any later recovered boundary uses fixed-width frames and the session mapping-manager rule; declarations transfer no native identity'
        $row.final_evidence = 'T276 S17: scaffold declarations are evidence only; modern app owns composition and cannot import the old product shell'
        continue
    }
    if ($row.source_path -like 'v86/util/*') {
        $row.final_disposition = 'tool-only'
        $row.final_owner_or_link_boundary = 'mvdm-tools independent 16-bit BOP_UNSIMULATE utility; never a host link input'
        $row.final_change_class = 'none; byte-exact source mirror retained'
        $row.named_adapter = 'none'
        $row.mapping_implication = 'not applicable to the host runtime'
        $row.final_evidence = 'T276 S17: util/quit.asm is an independently-built 16-bit utility'
        continue
    }
    $row.final_disposition = 'not-host-runtime'
    $row.final_owner_or_link_boundary = 'retained historical kernel monitor or ntvdm.exe product-shell evidence; modern app/adapter-mvdm-host-out/monitor own any separately admitted boundary'
    $row.final_change_class = 'none; byte-exact source mirror retained outside the final host graph'
    $row.named_adapter = 'adapter-mvdm-host-out/monitor; adapter-mvdm-host-out/softpc; adapter-bochs; app'
    $row.mapping_implication = 'no NtVdmControl or raw VDM pointer survives; future admitted paths use copied frames and session guest-memory mapping-manager leases'
    $row.final_evidence = 'T276 S17: kernel VDM monitor and standalone ntvdm.exe scaffold are explicit product-shell exclusions'
}
$rows | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" | Set-Content $path -Encoding utf8
$counts = $v86 | Group-Object final_disposition | Sort-Object Name | ForEach-Object { "$($_.Name)=$($_.Count)" }
Write-Output "T276 S25 V86 final dispositions applied: $($counts -join ', ')."
