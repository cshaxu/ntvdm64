[CmdletBinding()]
param([string]$RepositoryRoot = '')

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot) }
$ErrorActionPreference = 'Stop'
$ops = Join-Path (Resolve-Path -LiteralPath $RepositoryRoot).Path 'docs/etc/operations'
$work = @(Import-Csv -LiteralPath (Join-Path $ops 'm0-t277-s2-source-form-worklist.tsv') -Delimiter "`t" | Where-Object { $_.caller_package -eq 'softpc.new' })
if ($work.Count -ne 197) { throw "Expected 197 SoftPC work items; found $($work.Count)." }

$groups = [ordered]@{
    'CRT-resource-helper' = @('malloc','getenv','host_memset')
    'COMMAND-package-entry' = @('CMDInit')
    'session-stop-and-event' = @('TerminateVDM','cmdPushExitInConsoleBuffer','nt_block_event_thread','nt_resume_event_thread','nt_std_handle_notification')
    'BOP-dispatch-control' = @('MS_bop_2','MS_bop_5','MS_bop_8','MS_bop_B','MS_bop_E','MS_bop_F','illegal_bop')
    'machine-CPU-SAS-PIC-facade' = @('ccpu386exitthread','ccpu386newthread','enable_stream_io','getPE','host_cpu_init','host_cpu_interrupt','host_simulate','ica_clear_int','ica_hw_interrupt','ica_inb','ica_outb','reset','sas_hw_at','sas_overwrite_memory','setIOPL','setNT','setTF','sim32_effective_addr')
    'serial-host-endpoint' = @('com_int_data','com_lsr_change','FastCommGetLineControl','FastCommSetBaudRate','FastCommSetLineControl','host_com_char_read','host_com_check_adapter','host_com_da_int_change','host_com_lock','host_com_unlock','setup_RTSDTR','SyncBaseLineSettings','tx_holding_register_empty','tx_shift_register_empty')
    'host-device-and-resource-endpoint' = @('HostFdiskReset','HostTerminatePDB','host_direct_access_error','host_find_file','host_get_spc_home','host_GetSysTime','host_lpt_flush_initialize','InitDetect','printer_is_being_closed')
    'mouse-display-console-endpoint' = @('host_mouse_conditional_off_enabled','host_mouse_install1','host_mouse_install2','MouseAttachMenuItem','MouseDetachMenuItem','MouseInFocus','MouseOutOfFocus','MouseReattachMenuItem','MouseSystemMenuOFF','MouseSystemMenuON','MovePointerToWindowCentre','nt_mark_screen_refresh')
    'monitor-debugger-VDD-boundary' = @('DBGDispatch','DBGInit','DisableEmulatorIretHooks','DispatchPageFault','EnableEmulatorIretHooks','host_iret_bop_table_addr','WOWIdle')
}

$symbolFamily = @{}
foreach ($family in $groups.Keys) { foreach ($symbol in $groups[$family]) { if ($symbolFamily.ContainsKey($symbol)) { throw "Duplicate group assignment: $symbol" }; $symbolFamily[$symbol] = $family } }
$actual = @($work.symbol | Sort-Object -Unique)
$missing = @($actual | Where-Object { -not $symbolFamily.ContainsKey($_) })
$extra = @($symbolFamily.Keys | Where-Object { $_ -notin $actual })
if ($missing.Count -or $extra.Count) { throw "SoftPC group map mismatch. Missing=[$($missing -join ',')]; Extra=[$($extra -join ',')]" }

function SourceForm($family) {
    switch ($family) {
        'CRT-resource-helper' { return 'original CRT/helper forms; softpc.new/host/src/nt_unix.c:307 is the host_memset body and standard.h carries CRT allocation/environment declarations' }
        'COMMAND-package-entry' { return 'dos/command/cmd.c:23 defines original BOOL CMDInit(int,char**); softpc.new/host/src/nt_msscs.c calls it during host startup' }
        'session-stop-and-event' { return 'softpc.new/host/src/nt_event.c and nt_msscs.c define original event/notification forms; v86/scaffold TerminateVDM is an excluded standalone product body' }
        'BOP-dispatch-control' { return 'softpc.new/host/src/nt_bop.c defines MS_bop_2/5/8/B/E/F and illegal BOP paths; base/bios tables are historical selector tables, not a second dispatcher' }
        'machine-CPU-SAS-PIC-facade' { return 'softpc.new/host/src/nt_aorc.c, nt_cpu.c and sim32.c expose historical CPU/SAS facade forms; base/ccpu386, cvidc, system and video bodies are Bochs-replaced executors' }
        'serial-host-endpoint' { return 'softpc.new/host/src/nt_com.c and nt_ntfun.c provide original host serial forms; base/comms holds the historical serial executor' }
        'host-device-and-resource-endpoint' { return 'softpc.new/host/src/nt_error.c, nt_fdisk.c, nt_lpt.c, nt_msscs.c, nt_rez.c, nt_timer.c and nt_det.c provide original host endpoint forms' }
        'mouse-display-console-endpoint' { return 'softpc.new/host/src/nt_mouse.c, nt_graph.c, nt_event.c and nt_fulsc.c provide original host mouse/display/console forms' }
        'monitor-debugger-VDD-boundary' { return 'softpc.new/host/src/nt_eoi.c, nt_msscs.c and nt_bop.c call original monitor/debug/VDD forms; dbg and v86 bodies remain separate original packages' }
        default { throw "Unknown family $family" }
    }
}
function Disposition($family) {
    switch ($family) {
        'CRT-resource-helper' { return @('native CRT binding or original host helper','original function form and caller-side NULL/error path','none','retain original caller failure; no product-global CRT wrapper','no adapter body') }
        'COMMAND-package-entry' { return @('mvdm-host COMMAND package','original BOOL CMDInit(int,char**) call/return','none','retain original BOOL failure result; whole COMMAND package required','no startup product shell') }
        'session-stop-and-event' { return @('session -> adapter-mvdm-host-out/win32; adapter-mvdm-host-out/vdd successor where original hook is reached','original void event/notification/termination calls and source ordering','session-owned event/resource identity only','controlled stop never exits the surrounding app; VDD/CSRSS product branches remain explicit successors','no private event thread or process-global standard handles') }
        'BOP-dispatch-control' { return @('adapter-mvdm-host-in -> mvdm-host; adapter-mvdm-host-out/vdd successor for MS_bop_5','original BOP entry shape and original dispatcher ordering','fixed-width CPU frame and checked guest-memory copy only','retain original unavailable/decline/controlled-stop outcome; no selector logic in Bochs','no second BOP dispatcher or trace-selected body enablement') }
        'machine-CPU-SAS-PIC-facade' { return @('adapter-mvdm-host-out/softpc -> adapter-bochs','same-named historical CPU/SAS/PIC request form','session guest_memory lease for ranges; no raw pointer or parallel CPU state','typed mechanical failure is returned at the original caller boundary; Bochs remains sole executor','no CCPU/CVIDC/base device executor link') }
        'serial-host-endpoint' { return @('mvdm-host serial provider -> adapter-mvdm-host-out/win32 and adapter-mvdm-host-out/softpc -> adapter-bochs','original serial function parameters and BOOL/void result forms','session host_resource mapping owns endpoint identity; guest values stay fixed-width','preserve original result/timeout/error path; unavailable endpoint is explicit','no host HANDLE or COM executor crosses into guest/core') }
        'host-device-and-resource-endpoint' { return @('mvdm-host host provider -> adapter-mvdm-host-out/win32; session owns resource records','original endpoint call shapes and sequencing','session host_resource mapping for host endpoint identity; no guest pointer','preserve original error/cleanup result; unsupported physical endpoint is explicit','no direct device I/O or Bochs policy in OpenNT caller') }
        'mouse-display-console-endpoint' { return @('mvdm-host UI provider -> adapter-mvdm-host-out/win32; session owns UI/session state','original mouse/display/console call shapes','session host_resource mapping for window/console resources only','retain original status ordering; unsupported full-screen/old UI branch names a later monitor/WOW successor','no GUI or OpenNT semantics in Bochs') }
        'monitor-debugger-VDD-boundary' { return @('adapter-mvdm-host-out/monitor, adapter-mvdm-host-out/debugger or adapter-mvdm-host-out/vdd as selected by original branch','same-shaped original monitor/debug/VDD call form','copied diagnostic/frame records or scoped guest_memory lease only','private NT4 monitor/VDD/debugger transport remains explicit unavailable/successor behavior','no kernel VDM, CSRSS or debugger body link') }
        default { throw "Unknown family $family" }
    }
}

$rows = foreach ($item in $work | Sort-Object review_order,caller_path,caller_line) {
    $family = $symbolFamily[$item.symbol]
    $d = Disposition $family
    [pscustomobject]@{
        work_item_id=$item.work_item_id; caller_path=$item.caller_path; caller_line=$item.caller_line; symbol=$item.symbol; family=$family
        original_provider_form=(SourceForm $family); owner=$d[0]; retained_interface_shape=$d[1]; mapping_rule=$d[2]; failure_disposition=$d[3]; implementation_constraint=$d[4]
        audit_state='source-form-family-reviewed; implementation-not-enabled'; evidence=$item.evidence
    }
}
$rows | Export-Csv -LiteralPath (Join-Path $ops 'm0-t277-s5-softpc-host-control-disposition-ledger.tsv') -Delimiter "`t" -NoTypeInformation -Encoding utf8
Write-Host "Wrote $($rows.Count) T277 S5 SoftPC host-control dispositions in $($groups.Count) families."
