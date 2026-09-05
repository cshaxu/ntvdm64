[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$LogPath = 'build/M0-T309/S2/cohorts/x64/external-link-audit-machine.log',
    [string]$OutputPath = 'docs/etc/evidence/m0-t309-s2-p3-provider-residual-owner-ledger.tsv'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$log = Join-Path $root $LogPath
$output = Join-Path $root $OutputPath
if (!(Test-Path -LiteralPath $log -PathType Leaf)) { throw "Missing audit log: $log" }

function Classify([string]$symbol) {
    if ($symbol -match '^(diskette_io|nt_floppy_|nt_fdisk_|HostFloppyReset|HostFdiskReset|int13h_)') {
        return @('softpc-firmware-boundary', 'mvdm-host/softpc.new/host/src/nt_rflop.c; mvdm-host/softpc.new/roms/bios4.asm', 'not-host-runtime; selected SoftPC owns FDC/DMA/IRQ and firmware mechanics')
    }
    if ($symbol -match '^(host_direct_access_error|RcErrorDialogBox|RcMessageBox)$') {
        return @('original-source-candidate', 'mvdm-host/softpc.new/host/src/nt_error.c', 'adapter-backed through adapter-win32 and session controlled stop')
    }
    if ($symbol -match '^(HostTerminatePDB|VDD(Create|Terminate)UserHook)$') {
        return @('original-source-candidate', 'mvdm-host/softpc.new/host/src/nt_msscs.c', 'adapter-backed cross-owner VDD/session composition')
    }
    if ($symbol -match '^(GetPIFConfigFiles|GetPIFData|pfdata|DosSessionId)$') {
        return @('original-source-candidate', 'mvdm-host/softpc.new/host/src/nt_pif.c', 'adapter-backed through adapter-win32/session; no independent launch policy')
    }
    if ($symbol -match '^(nt_(init|block|resume)_event_thread|cmdPushExitInConsoleBuffer|nt_std_handle_notification|CntrlHandlerState|SCS_hStd(In|Out|Err))$') {
        return @('original-source-candidate', 'mvdm-host/softpc.new/host/src/nt_event.c; mvdm-host/softpc.new/host/src/nt_hosts.c', 'adapter-backed session/console lifecycle; no CSRSS reconstruction')
    }
    if ($symbol -eq 'TerminateVDM') {
        return @('existing-adapter', 'session; adapter-mvdm-host-out/monitor', 'same-shaped controlled session stop; do not enter the excluded v86 scaffold executor')
    }
    if ($symbol -match '^(ModuleLoad|ModuleFree|ModuleSegmentMove|DbgPrompt)$') {
        return @('debugger-product-boundary', 'mvdm-host/dbg/dbg.c', 'tool-only debugger product; retain original DEBUG conditional failure/absence rather than link a debugger shell')
    }
    if ($symbol -eq 'VDMForWOW') {
        return @('wow-second-executor-boundary', 'mvdm-host/v86/scaffold/i386/softpc.c', 'not-host-runtime; separate WOW/second executor remains excluded')
    }
    if ($symbol -eq 'pFDAccess') {
        return @('bochs-firmware-boundary', 'mvdm-host/softpc.new/host/src/nt_fdisk.c', 'not-host-runtime direct FDC access state; Bochs owns the device')
    }
    if ($symbol -eq 'bPifFastPaste') {
        return @('original-source-candidate', 'mvdm-host/softpc.new/host/src/nt_pif.c', 'adapter-backed PIF configuration state')
    }
    if ($symbol -eq 'host_lpt_flush_initialize') {
        return @('original-source-candidate', 'mvdm-host/softpc.new/host/src/nt_lpt.c', 'adapter-backed LPT endpoint lifecycle; Bochs remains device owner')
    }
    if ($symbol -match '^(fSeparateWow|VdmExitCode)$') {
        return @('original-source-candidate', 'mvdm-host/softpc.new/host/src/nt_reset.c', 'adapter-backed session/app lifecycle state; no historical process-global exit behavior')
    }
    if ($symbol -match '^(get|set)(AH|AL|BH|BL|CH|CL|DH|DL|DS|ES|BP|CF|ZF)$') {
        return @('existing-adapter', 'adapter-mvdm-host-out/softpc', 'same-shaped register facade')
    }
    if ($symbol -match '(VdmRedir|Redir|PREDIR)') {
        return @('existing-adapter', 'adapter-mvdm-host-out/redir', 'Redirector owner family')
    }
    if ($symbol -match '^(Rtl|Nt|__imp_|Create|Get|Set|Delete|Move|Remove|Search|Find|LoadString|PostMessage|OutputDebugString|CharToOem|OemToChar)') {
        return @('adapter-win32-or-public-leaf', 'adapter-mvdm-host-out/win32', 'preserve historical facade shape; bind public Win32/NTDLL only after source contract review')
    }
    if ($symbol -match '^(_|str|wcs|mem|realloc|isgraph|tolower|toupper)') {
        return @('public-crt-leaf', 'MSVC CRT', 'link public runtime; do not import historical CRT')
    }
    return @('owner-audit-required', '', 'physical caller retained; no source or adapter selection inferred from spelling')
}

$rows = foreach ($line in Get-Content -LiteralPath $log) {
    if ($line -notmatch '^(?<caller>original-(dem|command)-provider-cohort\.lib\([^\)]+\))\s*:\s*error LNK2019: unresolved external symbol (?<symbol>.+?) referenced') { continue }
    $decision = Classify $Matches.symbol.Trim()
    [pscustomobject]@{
        caller_object = $Matches.caller.Trim()
        symbol = $Matches.symbol.Trim()
        disposition = $decision[0]
        original_or_adapter_owner = $decision[1]
        current_conclusion = $decision[2]
        audit_input = 'M0 T309 S2 x64 forced-link log; physical caller/symbol pair'
    }
}

$lines = @("caller_object`tsymbol`tdisposition`toriginal_or_adapter_owner`tcurrent_conclusion`taudit_input")
foreach ($row in $rows) {
    $lines += @($row.caller_object, $row.symbol, $row.disposition, $row.original_or_adapter_owner, $row.current_conclusion, $row.audit_input) -join "`t"
}
Set-Content -LiteralPath $output -Value $lines -Encoding utf8
Write-Host "Wrote $($rows.Count) provider residual rows: $OutputPath"
