[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$operations = Join-Path $root 'docs/etc/operations'
$fileLedgerPath = Join-Path $operations 'mvdm-file-recovery-ledger.tsv'
$buildLedgerPath = Join-Path $operations 'm0-t276-s2-translation-unit-build-ledger.tsv'
foreach ($path in @($fileLedgerPath, $buildLedgerPath)) { if (-not (Test-Path -LiteralPath $path)) { throw "Missing input: $path" } }
$files = @(Import-Csv -LiteralPath $fileLedgerPath -Delimiter "`t" | Where-Object { $_.target_path -like 'xms.486/*' } | Sort-Object target_path)
$builds = @(Import-Csv -LiteralPath $buildLedgerPath -Delimiter "`t")
if ($files.Count -ne 15) { throw "Expected 15 XMS rows, found $($files.Count)." }

$contracts = @{
    'xms.486/xms.c' = @('xms-initialization-and-extended-memory-range', 'adapter-mvdm-host-out/softpc; adapter-bochs; session; mvdm-support', 'original XMS initialization and suballocator arrangement; extended memory must be a bounded Bochs-owned guest range, never a host pointer', 'package-led machine-seam review')
    'xms.486/xmsa20.c' = @('xms-a20-service', 'adapter-mvdm-host-out/softpc; adapter-bochs; session', 'original A20 request/query control flow and results; same-shaped A20 facade owns the only machine transition', 'package-led machine-seam review')
    'xms.486/xmsblock.c' = @('xms-extended-block-services', 'adapter-mvdm-host-out/softpc; adapter-bochs; session; mvdm-support', 'original allocation/free/reallocation/move/query provider; guest memory is copied through typed machine seams', 'package-led machine-seam review')
    'xms.486/xmsdisp.c' = @('xms-service-dispatch', 'adapter-mvdm-host-in; adapter-mvdm-host-out/softpc', 'original XMS service dispatch and unsupported-service failure route; selector/service meanings remain source-owned', 'package-led interface review')
    'xms.486/xmsmisc.c' = @('xms-system-information-and-int15-hook', 'adapter-mvdm-host-out/softpc; adapter-bochs; session', 'original system-page, extended-memory and INT 15 hook/provider logic; IVT and interrupt effects require machine seam proof', 'package-led machine-seam review')
    'xms.486/xmsumb.c' = @('xms-umb-services', 'adapter-mvdm-host-out/softpc; adapter-bochs; session', 'original UMB list, request and release control flow; a durable bounded guest-byte/UMB seam is required before enablement', 'machine-profile-gated review')
    'xms.486/i386/xmsmem86.c' = @('xms-i386-direct-address-memory-routine', 'session guest-memory mapping manager; adapter-mvdm-host-out/softpc; adapter-bochs', 'historical x86 routine passes ULONG values as direct host addresses to NT virtual memory and RtlMoveMemory; retain its three same-shaped exported contracts, but resolve their values through the session mapping-manager into checked Bochs guest ranges', 'same-shaped mapping-manager replacement prerequisite')
    'xms.486/xmsmemr.c' = @('xms-risc-memory-routine', 'adapter-mvdm-host-out/softpc; adapter-bochs', 'historical non-i386 memory-management routine; not selected by the x86 original profile and retains only source/contract evidence', 'profile/build-unobserved exclusion')
}

function Get-NonSourceContract {
    param([string]$Path)
    if ($Path -eq 'xms.486/xms.h') { return @('xms-public-declaration', 'mvdm-host; adapter-mvdm-host-in; adapter-mvdm-host-out/softpc; session; mvdm-support', 'original XMS data and external declaration surface; preserve exact shape before any provider binding', 'declaration-only') }
    if ($Path -like 'xms.486/*/sources' -or $Path -eq 'xms.486/sources' -or $Path -eq 'xms.486/makefile') { return @('original-build-description', 'mvdm-support; build governance', 'original architecture/build selection evidence only; no modern build edge', 'build-description-only') }
    throw "No non-source contract for $Path"
}

function Get-BuildMembership {
    param($File, $BuildRows)
    if ($File.target_path -eq 'xms.486/i386/xmsmem86.c') { return 'observed-in-original-i386-sources' }
    if ($File.file_kind -eq 'source' -and $BuildRows.Count -eq 1) { return $BuildRows[0].original_build_membership }
    if ($File.file_kind -eq 'source') { return 'not-observed-in-original-sources' }
    return 'not-a-translation-unit'
}

function Get-LexicalFamilies {
    param([string]$Path, [string]$Kind)
    if ($Kind -notin @('source', 'declaration')) { return 'none (non-code metadata)' }
    $text = Get-Content -LiteralPath $Path -Raw
    $families = [System.Collections.Generic.List[string]]::new()
    if ($text -match '(?i)(xmssvc\.h|xmsexp\.h|\bXMSDispatch\b)') { $families.Add('adapter-mvdm-host-in (XMS service ABI lexical surface)') }
    if ($text -match '(?i)(softpc\.h|\bsas_|\bSim32|\bGetVDMAddr|\bget[A-Z]|\bset[A-Z]|\bUpdateKbdInt15)') { $families.Add('adapter-mvdm-host-out/softpc (SoftPC/SAS/guest-state lexical surface)') }
    if ($text -match '(?i)(NtAllocateVirtualMemory|NtFreeVirtualMemory|RtlMoveMemory|VdmAllocateVirtualMemory)') { $families.Add('same-shaped mapping-manager replacement (historical virtual-memory lexical surface)') }
    if ($text -match '(?i)(mvdm\.h|\bVDM(?:ForWOW|Addr)|\bExtMemSA)') { $families.Add('session (VDM state/lifecycle lexical surface)') }
    if ($text -match '(?i)(suballoc\.h|\bSA(?:Initialize|Alloc|Free|Realloc))') { $families.Add('mvdm-support (suballoc lexical surface)') }
    if ($text -match '(?i)(ReserveUMB|UMB_OWNER|\bUMB\b)') { $families.Add('adapter-mvdm-host-out/softpc/adapter-bochs (UMB machine-range lexical surface)') }
    if ($families.Count -eq 0) { return 'none detected in retained source text' }
    return [string]::Join('; ', $families)
}

$result = foreach ($file in $files) {
    $buildRows = @($builds | Where-Object { $_.file_id -eq $file.file_id })
    if ($buildRows.Count -gt 1) { throw "Expected at most one build row for $($file.target_path), found $($buildRows.Count)." }
    if (-not (Test-Path -LiteralPath $file.selected_source_path)) { throw "Missing selected source: $($file.selected_source_path)" }
    $membership = Get-BuildMembership -File $file -BuildRows $buildRows
    $buildEvidence = if ($buildRows.Count -eq 1) { $buildRows[0].original_build_description_ids } elseif ($file.target_path -eq 'xms.486/i386/xmsmem86.c') { 'xms.486/i386/sources' } else { 'not-applicable' }
    if ($file.file_kind -eq 'source') {
        if (-not $contracts.ContainsKey($file.target_path)) { throw "Missing XMS source contract: $($file.target_path)" }
        $contract = $contracts[$file.target_path]
        $role = $contract[0]; $owner = 'mvdm-host'; $required = $contract[1]; $summary = $contract[2]; $disposition = $contract[3]
    } else {
        $contract = Get-NonSourceContract -Path $file.target_path
        $role = $contract[0]; $owner = if ($role -eq 'original-build-description') { 'mvdm-support' } else { 'mvdm-host' }; $required = $contract[1]; $summary = $contract[2]; $disposition = $contract[3]
    }
    [pscustomobject][ordered]@{
        file_id = $file.file_id
        source_path = $file.target_path
        file_kind = $file.file_kind
        original_build_membership = $membership
        original_role_group = $role
        provisional_component_owner = $owner
        required_boundary_owner = $required
        lexical_external_families = Get-LexicalFamilies -Path $file.selected_source_path -Kind $file.file_kind
        original_contract_summary = $summary
        provisional_disposition = $disposition
        state = 'provisional-not-enabled'
        evidence = "$($file.target_path); $buildEvidence; m0-t276-s2-translation-unit-build-ledger.tsv"
    }
}
if ($result.Count -ne 15) { throw "Expected 15 generated XMS rows, found $($result.Count)." }
$result | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" | Set-Content -LiteralPath (Join-Path $operations 'm0-t276-s14-xms-file-disposition-ledger.tsv') -Encoding utf8
Write-Output "T276 S14 XMS disposition generated: $($result.Count) rows."
