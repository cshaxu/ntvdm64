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
foreach ($path in @($fileLedgerPath, $buildLedgerPath)) {
    if (-not (Test-Path -LiteralPath $path)) { throw "Missing input: $path" }
}

$files = @(Import-Csv -LiteralPath $fileLedgerPath -Delimiter "`t" |
    Where-Object { $_.target_path -like 'dos/*' } |
    Sort-Object target_path)
$builds = @(Import-Csv -LiteralPath $buildLedgerPath -Delimiter "`t")
if ($files.Count -ne 40) { throw "Expected 40 dos rows, found $($files.Count)." }

$sourceContracts = @{
    'dos/command/cmd.c'      = @('command-service-entry', 'adapter-mvdm-host-in; adapter-mvdm-host-out/softpc; adapter-mvdm-host-out/win32; session', 'original COMMAND service entry and result contract; preserve dispatch shape before individual services are admitted')
    'dos/command/cmdconf.c'  = @('command-bootstrap-configuration', 'adapter-mvdm-host-out/softpc; adapter-mvdm-host-out/win32; session; mvdm-support', 'original CONFIG.SYS/AUTOEXEC bootstrap and configuration transfer; retain the source-owned ordering')
    'dos/command/cmddata.c'  = @('command-shared-state', 'session; adapter-mvdm-host-out/softpc', 'original COMMAND global state and VDM-facing data definitions; recover only with explicit session ownership')
    'dos/command/cmddisp.c'  = @('command-service-dispatch', 'adapter-mvdm-host-in; adapter-mvdm-host-out/softpc', 'original COMMAND BOP service dispatch; selector meanings stay source-owned')
    'dos/command/cmdenv.c'   = @('command-environment', 'adapter-mvdm-host-out/win32; session', 'original DOS-to-host environment transformation and current-directory data; requires session-owned modern host environment binding')
    'dos/command/cmdexec.c'  = @('command-child-lifecycle', 'session; adapter-mvdm-host-out/win32; adapter-mvdm-host-out/softpc', 'original COMMAND child creation, wait and re-entry control flow; requires the session lifecycle contract, not a replacement command runner')
    'dos/command/cmdexit.c'  = @('command-child-return', 'session; adapter-mvdm-host-out/softpc', 'original COMMAND exit/re-entry result path; depends on source-owned child lifecycle and guest return contracts')
    'dos/command/cmdkeyb.c'  = @('command-keyboard-layout', 'adapter-mvdm-host-out/win32; session', 'original keyboard-layout discovery/configuration provider; public host input binding must retain original failure behavior')
    'dos/command/cmdmisc.c'  = @('command-bootstrap-console', 'session; adapter-mvdm-host-out/win32; adapter-mvdm-host-out/softpc; mvdm-support', 'original startup, console and command-information orchestration; it is a cross-provider composition point')
    'dos/command/cmdpif.c'   = @('command-pif-policy', 'adapter-mvdm-host-out/win32; session; mvdm-support', 'original PIF/environment policy reader; modern public binding must preserve PIF absence/failure semantics')
    'dos/command/cmdredir.c' = @('command-redirection-pipes', 'adapter-mvdm-host-out/redir; adapter-mvdm-host-out/win32; session; adapter-mvdm-host-out/softpc', 'original standard-stream redirection and pipe control flow; Redirector ownership is a prerequisite')
    'dos/dem/dem.c'          = @('dem-service-entry', 'adapter-mvdm-host-in; adapter-mvdm-host-out/softpc; adapter-mvdm-host-out/win32; session', 'original DEM entry/configuration contract; retain source-owned service semantics')
    'dos/dem/demdasd.c'      = @('dem-raw-dasd', 'adapter-mvdm-host-out/softpc; adapter-bochs; adapter-mvdm-host-out/win32; session', 'original absolute disk/floppy control path; machine-media ownership must be proved before enablement')
    'dos/dem/demdata.c'      = @('dem-shared-state', 'session; adapter-mvdm-host-out/softpc', 'original DEM global state and guest pointer data; recover only through session and same-shaped SoftPC facade')
    'dos/dem/demdir.c'       = @('dem-directory-namespace', 'adapter-mvdm-host-out/win32; adapter-mvdm-host-out/softpc; session', 'original directory/current-drive provider; public host path binding must preserve DOS-facing error order')
    'dos/dem/demdisp.c'      = @('dem-service-dispatch', 'adapter-mvdm-host-in; adapter-mvdm-host-out/softpc', 'original DEM BOP dispatch and not-yet-implemented failure routing; selectors remain in source')
    'dos/dem/demerror.c'     = @('dem-hard-error-state', 'adapter-mvdm-host-out/softpc; adapter-mvdm-host-out/win32; session', 'original hard-error state, guest register result and user-visible error ordering')
    'dos/dem/demfcb.c'       = @('dem-fcb-filesystem', 'adapter-mvdm-host-out/win32; adapter-mvdm-host-out/softpc; session', 'original FCB filesystem provider; host handles and guest memory require their dedicated same-shaped adapters')
    'dos/dem/demfile.c'      = @('dem-handle-filesystem', 'adapter-mvdm-host-out/win32; adapter-mvdm-host-out/softpc; adapter-mvdm-host-out/redir; adapter-mvdm-host-out/vdd; session', 'original DOS handle filesystem, pipe and VDD-aware provider; each external bridge remains separately owned')
    'dos/dem/demgset.c'      = @('dem-drive-system-state', 'adapter-mvdm-host-out/win32; adapter-mvdm-host-out/softpc; session', 'original drive, date/time and system-state query/set provider')
    'dos/dem/demhndl.c'      = @('dem-handle-lifecycle', 'adapter-mvdm-host-out/win32; adapter-mvdm-host-out/softpc; session', 'original DOS-handle lifecycle provider; opaque host identity mapping remains session-owned')
    'dos/dem/demioctl.c'     = @('dem-device-ioctl', 'adapter-mvdm-host-out/softpc; adapter-bochs; adapter-mvdm-host-out/win32; session', 'original IOCTL/provider path; raw device and BIOS-style branches require machine-media ownership proof')
    'dos/dem/demlabel.c'     = @('dem-volume-label', 'adapter-mvdm-host-out/win32; adapter-mvdm-host-out/softpc; session', 'original volume-label provider and DOS error mapping')
    'dos/dem/demlock.c'      = @('dem-file-locking', 'adapter-mvdm-host-out/win32; adapter-mvdm-host-out/softpc; session', 'original DOS file-lock provider; public host locking binding must retain source failure behavior')
    'dos/dem/demmisc.c'      = @('dem-miscellaneous-state', 'adapter-mvdm-host-out/softpc; adapter-mvdm-host-out/win32; session', 'original miscellaneous DEM state and guest/host conversion helpers')
    'dos/dem/demmsg.c'       = @('dem-message-catalog', 'adapter-mvdm-host-out/win32; session', 'original DEM message/resource and diagnostic helper path')
    'dos/dem/demsrch.c'      = @('dem-search-namespace', 'adapter-mvdm-host-out/win32; adapter-mvdm-host-out/softpc; session', 'original find-first/find-next namespace provider; guest DTA and host search state require declared mappings')
}

function Get-NonSourceContract {
    param([string]$Path)
    if ($Path -eq 'dos/dirs') {
        return @('original-build-directory-metadata', 'mvdm-support', 'original build-tree metadata; exact mirror only, never a runtime input', 'metadata-only')
    }
    if ($Path -eq 'dos/test/test.bat') {
        return @('historical-package-test', 'none until an admitted test harness', 'historical test evidence; no product test or runtime inference', 'test-evidence-only')
    }
    if ($Path -like '*/sources' -or $Path -like '*/makefile') {
        return @('original-build-description', 'mvdm-support; build governance', 'original build description; source selection evidence only, not a modern build edge', 'build-description-only')
    }
    if ($Path -like 'dos/command/*') {
        return @('command-public-declaration', 'mvdm-host; adapter-mvdm-host-in; adapter-mvdm-host-out/softpc; adapter-mvdm-host-out/win32; session', 'original COMMAND declaration/data surface; preserve exact source shape before provider enablement', 'declaration-only')
    }
    return @('dem-public-declaration', 'mvdm-host; adapter-mvdm-host-in; adapter-mvdm-host-out/softpc; adapter-mvdm-host-out/win32; session', 'original DEM declaration/data surface; preserve exact source shape before provider enablement', 'declaration-only')
}

function Get-LexicalFamilies {
    param([string]$SourcePath, [string]$FileKind)
    if ($FileKind -notin @('source', 'declaration')) { return 'none (non-code metadata)' }
    $text = Get-Content -LiteralPath $SourcePath -Raw
    $families = [System.Collections.Generic.List[string]]::new()
    if ($text -match '(?i)(softpc\.h|\bSim32|\bsas_|\bget[A-Z]|\bset[A-Z]|host_simulate)') { $families.Add('adapter-mvdm-host-out/softpc (SoftPC/SAS/guest-state lexical surface)') }
    if ($text -match '(?i)(windows\.h|winbase\.h|nt\.h|ntrtl\.h|nturtl\.h|ntdddisk\.h|process\.h|\bCreateFile|\bGetLastError|\bCreateProcess)') { $families.Add('adapter-mvdm-host-out/win32 (historical Win32/NTDLL lexical surface)') }
    if ($text -match '(?i)(vrnmpipe\.h|vdmredir|\bPipe(?:In|Out|File|Handle)|cmdredir)') { $families.Add('adapter-mvdm-host-out/redir (redirector/pipe lexical surface)') }
    if ($text -match '(?i)(nt_vdd\.h|\bVDD[A-Za-z0-9_]*\s*\()') { $families.Add('adapter-mvdm-host-out/vdd (VDD lexical surface)') }
    if ($text -match '(?i)(oemuni\.h|\bOem[A-Za-z0-9_]*\s*\()') { $families.Add('mvdm-support (OEM/NLS lexical surface)') }
    if ($text -match '(?i)(cmdsvc\.h|dossvc\.h|\bBOP\b)') { $families.Add('adapter-mvdm-host-in (BOP ABI lexical surface)') }
    if ($text -match '(?i)(mvdm\.h|\bVDM(?:INFO|ENV|TIB|STATE)|\bReEnter)') { $families.Add('session (VDM session/lifecycle lexical surface)') }
    if ($families.Count -eq 0) { return 'none detected in retained source text' }
    return [string]::Join('; ', $families)
}

$result = foreach ($file in $files) {
    $buildRows = @($builds | Where-Object { $_.file_id -eq $file.file_id })
    if ($file.file_kind -eq 'source' -and $buildRows.Count -ne 1) { throw "Expected one build-ledger row for source $($file.target_path), found $($buildRows.Count)." }
    if ($file.file_kind -ne 'source' -and $buildRows.Count -gt 1) { throw "Expected at most one build-ledger row for non-source $($file.target_path), found $($buildRows.Count)." }
    if (-not (Test-Path -LiteralPath $file.selected_source_path)) { throw "Missing selected source: $($file.selected_source_path)" }
    $buildMembership = if ($buildRows.Count -eq 1) { $buildRows[0].original_build_membership } else { 'not-a-translation-unit' }
    $buildEvidence = if ($buildRows.Count -eq 1) { $buildRows[0].original_build_description_ids } else { 'not-applicable' }

    if ($file.file_kind -eq 'source') {
        if (-not $sourceContracts.ContainsKey($file.target_path)) { throw "Missing source contract: $($file.target_path)" }
        $contract = $sourceContracts[$file.target_path]
        $role = $contract[0]
        $owner = 'mvdm-host'
        $prerequisite = $contract[1]
        $summary = $contract[2]
        $disposition = 'package-led adapter/interface review'
    } else {
        $contract = Get-NonSourceContract -Path $file.target_path
        $role = $contract[0]
        $owner = if ($role -eq 'original-build-directory-metadata') { 'mvdm-support' } elseif ($role -eq 'historical-package-test') { 'mvdm-tools' } else { 'mvdm-host' }
        $prerequisite = $contract[1]
        $summary = $contract[2]
        $disposition = $contract[3]
    }

    [pscustomobject][ordered]@{
        file_id = $file.file_id
        source_path = $file.target_path
        file_kind = $file.file_kind
        original_literal_build_membership = $buildMembership
        original_role_group = $role
        provisional_component_owner = $owner
        required_boundary_owner = $prerequisite
        lexical_external_families = Get-LexicalFamilies -SourcePath $file.selected_source_path -FileKind $file.file_kind
        original_contract_summary = $summary
        provisional_disposition = $disposition
        state = 'provisional-not-enabled'
        evidence = "$($file.target_path); $buildEvidence; m0-t276-s2-translation-unit-build-ledger.tsv"
    }
}

if ($result.Count -ne 40) { throw "Expected 40 generated dos disposition rows, found $($result.Count)." }
$result | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" | Set-Content -LiteralPath (Join-Path $operations 'm0-t276-s13-dos-file-disposition-ledger.tsv') -Encoding utf8
Write-Output "T276 S13 dos disposition generated: $($result.Count) rows."
