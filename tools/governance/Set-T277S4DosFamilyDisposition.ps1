[CmdletBinding()]
param([string]$RepositoryRoot = '')

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}

$ErrorActionPreference = 'Stop'
$ops = Join-Path (Resolve-Path -LiteralPath $RepositoryRoot).Path 'docs/etc/operations'
$work = @(Import-Csv -LiteralPath (Join-Path $ops 'm0-t277-s2-source-form-worklist.tsv') -Delimiter "`t" | Where-Object { $_.caller_package -eq 'dos' })
if ($work.Count -ne 73) { throw "Expected 73 DOS work items; found $($work.Count)." }

function Form($symbol) {
    switch ($symbol) {
        'CMDInit' { return 'dos/command/cmd.c:23 defines the original BOOL CMDInit(int,char**) package initializer' }
        'TerminateVDM' { return 'v86/scaffold/i386/softpc.c:397 supplies the historical standalone-product termination body; it is excluded from the rebootstrap host runtime' }
        'malloc' { return 'softpc.new/base/inc/standard.h exposes the original CRT allocation form' }
        'getenv' { return 'the original host uses the CRT getenv form; softpc.new/obj.vdm/ntvdm.c contains a product-specific wrapper that is not a host-runtime provider' }
        'nt_block_event_thread' { return 'softpc.new/host/src/nt_event.c:1364 defines the historical event-thread block call and invokes private VDD/event machinery' }
        'nt_resume_event_thread' { return 'softpc.new/host/src/nt_event.c:1501 defines the historical event-thread resume call and stream-I/O restoration path' }
        'nt_std_handle_notification' { return 'softpc.new/host/src/nt_msscs.c:1067 records stdoutRedirected; X86GFX extends it with historical full-screen behavior' }
        'cmdPushExitInConsoleBuffer' { return 'softpc.new/host/src/nt_event.c:1879 defines the original console-exit notification path' }
        'cmdPipeFileDataEOF' { return 'dos/command/cmdredir.c:602 defines the original pipe-data/EOF query over PPIPE_INPUT and HANDLE' }
        'cmdPipeFileEOF' { return 'dos/command/cmdredir.c:641 defines the original pipe EOF query over PPIPE_INPUT and HANDLE' }
        'host_direct_access_error' { return 'softpc.new/host/src/nt_error.c:217 defines original direct-device error dispatch by reason code' }
        'host_lpt_flush_initialize' { return 'softpc.new/host/src/nt_lpt.c:688 defines original LPT flush-state initialization' }
        'host_simulate' { return 'softpc.new/host/inc/host_cpu.h publishes original host_simulate; S3 records its Bochs-only executor disposition' }
        'HostFdiskReset' { return 'softpc.new/host/src/nt_fdisk.c:262/589 defines the original fixed-disk reset entry' }
        'HostFloppyReset' { return 'softpc.new/host/src/nt_rflop.c:241/857 defines the original floppy reset entry' }
        'HostTerminatePDB' { return 'softpc.new/host/src/nt_msscs.c:1411 calls original FloppyTerminatePDB and FdiskTerminatePDB cleanup' }
        default { throw "Unexpected DOS S4 symbol '$symbol'." }
    }
}

function Family($symbol) {
    switch ($symbol) {
        'CMDInit' { return @('COMMAND-package-initialization','mvdm-host COMMAND package','original BOOL CMDInit(int,char**) call/return form','none','retain caller-visible BOOL failure; package body is not enabled','whole COMMAND package must close before enablement') }
        'TerminateVDM' { return @('session-controlled-termination','session -> adapter-bochs','original void termination call; typed controlled-stop record only','no raw process termination or host pointer','request session stop; never terminate the surrounding app process','historical v86 product shell is excluded; session successor must preserve caller ordering') }
        'malloc' { return @('CRT-allocation-and-environment','native CRT binding','original CRT allocation call and caller-side NULL handling','none','preserve original caller-side allocation failure path','no adapter body') }
        'getenv' { return @('CRT-allocation-and-environment','native CRT binding','original CRT getenv call and caller-side NULL handling','none','preserve original caller-side absent-variable path','no product-global environment wrapper') }
        'nt_block_event_thread' { return @('COMMAND-session-event-lifecycle','session -> adapter-mvdm-host-out/win32; adapter-vdd successor for original VDD hook','original void block call and BlockFlags ordering','session-owned event identity; no host HANDLE enters guest state','preserve source ordering; private VDD callback is a named later adapter-vdd branch','no CSRSS/NT4 event-thread recreation') }
        'nt_resume_event_thread' { return @('COMMAND-session-event-lifecycle','session -> adapter-mvdm-host-out/win32; adapter-vdd successor for original VDD hook','original void resume call and stream-I/O restoration ordering','session-owned event identity; no host HANDLE enters guest state','preserve source ordering; private VDD callback is a named later adapter-vdd branch','no CSRSS/NT4 event-thread recreation') }
        'nt_std_handle_notification' { return @('COMMAND-session-event-lifecycle','session -> adapter-mvdm-host-out/win32','original BOOL redirection notification, including stdoutRedirected state','session host_resource mapping instance owns any host standard-stream identity','record redirection state; X86GFX/full-screen branch is a named display successor','no process-global SetStdHandle mutation') }
        'cmdPushExitInConsoleBuffer' { return @('COMMAND-session-event-lifecycle','session -> adapter-mvdm-host-out/win32','original void console-exit notification call','session-local completion notification; no console HANDLE enters guest state','non-WOW local notification may be recovered; WOW/console injection is an adapter-wow successor','no private console-group or CSRSS recreation') }
        'cmdPipeFileDataEOF' { return @('COMMAND-local-pipe-observation','mvdm-host COMMAND provider -> session -> adapter-mvdm-host-out/win32','original BOOL(HANDLE,BOOL*) data/EOF query and timeout ordering','session host_resource mapping instance resolves the opaque host stream endpoint','retain TRUE/new-data-or-EOF and fEOF behavior; brokered pipe protocol belongs to adapter-redir','no guest-visible host HANDLE or new pipe protocol') }
        'cmdPipeFileEOF' { return @('COMMAND-local-pipe-observation','mvdm-host COMMAND provider -> session -> adapter-mvdm-host-out/win32','original BOOL(HANDLE) EOF query and timeout ordering','session host_resource mapping instance resolves the opaque host stream endpoint','retain EOF result; brokered pipe protocol belongs to adapter-redir','no guest-visible host HANDLE or new pipe protocol') }
        'host_direct_access_error' { return @('top-level-direct-device-error','mvdm-host nt_error -> adapter-mvdm-host-out/win32 -> session','original void reason-coded direct-access error call','no guest pointer or raw process-control state crosses the boundary','preserve reason and per-thread duplicate suppression; Ignore resumes caller path, Terminate requests controlled stop','WOW hard-error and old console placement remain named later branches') }
        'host_lpt_flush_initialize' { return @('host-device-state-cleanup','mvdm-host LPT provider -> adapter-mvdm-host-out/win32','original void LPT flush-state initialization','session-owned host-resource state only','preserve original no-result initialization ordering; unavailable host port is explicit provider failure','no Bochs device or DOS semantic in adapter-mvdm-host-out/win32') }
        'host_simulate' { return @('bounded-machine-run','adapter-softpc -> adapter-bochs','original host_simulate call shape','S3 typed machine request; no pointer mapping','use S3 caller-return-path failure disposition','Bochs remains sole executor') }
        'HostFdiskReset' { return @('host-device-state-cleanup','mvdm-host fixed-disk provider -> adapter-mvdm-host-out/win32','original void fixed-disk reset call','session host_resource state only','preserve original reset ordering; unavailable host device remains explicit failure','no direct physical I/O or Bochs disk semantic in caller') }
        'HostFloppyReset' { return @('host-device-state-cleanup','mvdm-host floppy provider -> adapter-mvdm-host-out/win32','original void floppy reset call','session host_resource state only','preserve original reset ordering; unavailable host device remains explicit failure','no direct physical I/O or Bochs floppy semantic in caller') }
        'HostTerminatePDB' { return @('host-device-state-cleanup','mvdm-host disk/floppy providers -> session','original void USHORT PDB cleanup call','PDB is a guest value; any host resource lookup uses session host_resource mapping only','preserve original cleanup ordering; missing owner record is a no-resource cleanup result','no raw host handle encoded in PDB') }
        default { throw "Unexpected DOS S4 symbol '$symbol'." }
    }
}

$rows = foreach ($item in $work | Sort-Object review_order, caller_path, caller_line) {
    $f = Family $item.symbol
    [pscustomobject]@{
        work_item_id = $item.work_item_id
        caller_path = $item.caller_path
        caller_line = $item.caller_line
        symbol = $item.symbol
        family = $f[0]
        original_provider_form = Form $item.symbol
        owner = $f[1]
        retained_interface_shape = $f[2]
        mapping_rule = $f[3]
        failure_disposition = $f[4]
        implementation_constraint = $f[5]
        audit_state = 'source-form-family-reviewed; implementation-not-enabled'
        evidence = $item.evidence
    }
}
$rows | Export-Csv -LiteralPath (Join-Path $ops 'm0-t277-s4-dos-family-disposition-ledger.tsv') -Delimiter "`t" -NoTypeInformation -Encoding utf8
Write-Host "Wrote $($rows.Count) T277 S4 DOS family dispositions."
