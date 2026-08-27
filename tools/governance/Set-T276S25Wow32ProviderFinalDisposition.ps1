[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path $RepositoryRoot).Path
$ops = Join-Path $root 'docs/etc/operations'
$path = Join-Path $ops 'm0-t276-final-integration-disposition-ledger.tsv'
$subdomainPath = Join-Path $ops 'm0-t276-s22-wow32-active-subdomain-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t")
$active = @(Import-Csv $subdomainPath -Delimiter "`t")
if ($active.Count -ne 77) { throw "Expected 77 active WOW32 provider bodies, found $($active.Count)." }
$overlayDomains = @('thunk-frame-and-structure','message-and-window-procedure','gdi-and-window-object','kernel-task-and-dos-session','user-interface')
$bindingDomains = @('shell-ole-and-printing','multimedia','network')

foreach ($provider in $active) {
    $row = @($rows | Where-Object {$_.source_path -eq $provider.source_path})
    if ($row.Count -ne 1) { throw "Expected exactly one final-ledger row for $($provider.source_path)." }
    $row = $row[0]
    $row.final_audit_state = 'final-disposition-audited; not implemented'
    if ($provider.original_subdomain -in $overlayDomains) {
        $row.final_disposition = 'overlay-required'
        $row.final_owner_or_link_boundary = 'mvdm-host original WOW32 provider with adapter-wow overlay -> adapter-mvdm-host-out/win32, adapter-softpc and adapter-vdm-monitor as reached; session owns mapping instances'
        $row.final_change_class = 'overlay-required; retain original provider flow while replacing private USER/GDI/NT4 monitor or direct 32-bit pointer/HANDLE transport'
        $row.named_adapter = 'adapter-wow; adapter-mvdm-host-out/win32; adapter-softpc; adapter-vdm-monitor; session guest-memory, host-resource and completion-callback mapping managers'
        $row.mapping_implication = 'all frame/guest pointer access is a bounded lease or copy; host resource identity is an opaque mapped ID; no DWORD cast may carry a native pointer or HANDLE'
        $row.final_evidence = "T276 S22 WOW32 active-subdomain baseline: $($provider.original_subdomain) requires private-product boundary overlay"
    }
    elseif ($provider.original_subdomain -in $bindingDomains) {
        $row.final_disposition = 'adapter-backed'
        $row.final_owner_or_link_boundary = 'mvdm-host original WOW32 provider -> adapter-wow and adapter-mvdm-host-out/win32 public/same-shaped capability bindings; session owns identity mapping'
        $row.final_change_class = 'binding-only; preserve original provider dispatch, structures and failure order'
        $row.named_adapter = 'adapter-wow; adapter-mvdm-host-out/win32; session host-resource mapping manager'
        $row.mapping_implication = 'native resources are opaque host-resource IDs; guest frame access remains a bounded session lease/copy'
        $row.final_evidence = "T276 S22 WOW32 active-subdomain baseline: $($provider.original_subdomain) provider can bind through named adapters"
    }
    else { throw "Unclassified WOW32 subdomain: $($provider.original_subdomain)." }
}
$rows | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" | Set-Content $path -Encoding utf8
$selected = @($rows | Where-Object {$_.source_path -in $active.source_path})
$counts = $selected | Group-Object final_disposition | Sort-Object Name | ForEach-Object { "$($_.Name)=$($_.Count)" }
Write-Output "T276 S25 WOW32 provider final dispositions applied: $($counts -join ', ')."
