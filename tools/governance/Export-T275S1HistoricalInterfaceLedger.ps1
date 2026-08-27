param(
    [Parameter(Mandatory = $true)]
    [string]$OpenNtMvdmHostRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Assert-OriginalPath([string]$RelativePath) {
    $path = Join-Path $OpenNtMvdmHostRoot $RelativePath.Replace([char]'/', [char]92)
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) {
        throw "Missing audited original caller/declaration: $RelativePath"
    }
}

$rows = @(
    [pscustomobject][ordered]@{
        interface_id='ADAPTER-IF-001'; family='bop-transport'; original_caller='softpc.new/host/src/nt_bop.c'; original_declaration='softpc.new/host/inc/host_bop.h'; historical_dependency='CCPU exception ingress and register/IP advancement'; observable_contract='copy fault frame, dispatch outside CPU, return typed resume or controlled stop'; old_candidate_scope='adapter-mvdm-host-in/{bop_ingress,frame_transaction,generic_ud_bridge}.*'; source_first_disposition='split-recover in S2'; failure_or_boundary='no selector/service recognition; no guest-pointer retention; no Bochs type'; successor='T275 S2'
    }
    [pscustomobject][ordered]@{
        interface_id='ADAPTER-IF-002'; family='softpc-ccpu-sas'; original_caller='dos/dem/{demfile,demhndl,demsrch,demdasd}.c'; original_declaration='inc/softpc.h; softpc.new/base/inc/sas.h'; historical_dependency='process-global CCPU registers and SAS guest-address aliases'; observable_contract='same-shaped register read/write and bounded guest span access during one source call'; old_candidate_scope='adapter-softpc/{ccpu_frame_context,guest_pointer_manager,guest_range,guest_read_action,guest_write_abi,dem/ccpu_sas_facade,dem/direct_context}.*'; source_first_disposition='split-recover in S3'; failure_or_boundary='session mapping lease required; no raw pointer crosses component boundary'; successor='T275 S3'
    }
    [pscustomobject][ordered]@{
        interface_id='ADAPTER-IF-003'; family='softpc-ccpu-sas'; original_caller='softpc.new/base/ccpu386/{c_getset,c_main,c_intr,c_stack,c_xcptn}.c'; original_declaration='softpc.new/base/inc/{cpu3,cpu4,sas}.h'; historical_dependency='SoftPC CCPU execution engine'; observable_contract='CPU execution and native interrupt semantics'; old_candidate_scope='adapter-softpc/{machine_engine,machine_composition,machine_stage,finite_run,machine_profile_abi}.*'; source_first_disposition='retire-as-non-adapter-machine-shell'; failure_or_boundary='Bochs execution belongs to adapter-bochs; no SoftPC CPU body is recovered'; successor='T273 closed foundation / later machine profile admission'
    }
    [pscustomobject][ordered]@{
        interface_id='ADAPTER-IF-004'; family='softpc-ccpu-sas'; original_caller='xms.486/{xmsa20,xmsumb,xmsmemr}.c'; original_declaration='xms.486/xms.h; inc/softpc.h'; historical_dependency='SAS A20, UMB and guest memory access'; observable_contract='source-shaped A20 state and bounded guest-memory operations'; old_candidate_scope='adapter-softpc/{a20_capability,opennt_xms_softpc_facade,ordinary_ram_reservation,softpc_memory_size_shim}.*'; source_first_disposition='defer to XMS owner after S3 base mapping facade'; failure_or_boundary='do not add XMS policy to generic adapter; unavailable machine seam must fail deterministically'; successor='queued XMS owner package'
    }
    [pscustomobject][ordered]@{
        interface_id='ADAPTER-IF-005'; family='softpc-ccpu-sas'; original_caller='softpc.new/host/src/{nt_lpt,nt_mouse,nt_timer,nt_umb}.c'; original_declaration='softpc.new/host/inc/{host_def,nt_uis}.h'; historical_dependency='product device/port/state shell'; observable_contract='device-specific host callback semantics'; old_candidate_scope='adapter-softpc/{physical_irq,port_action,spckbd_handoff_shim,softpc_mouse_vector_shim,softpc_printer_openclose_shim,softpc_tape_io_shim}.*'; source_first_disposition='defer by original device owner'; failure_or_boundary='no generic device policy and no OpenNT/DOS meaning in adapter-bochs'; successor='machine/VDD/debugger owner packages'
    }
    [pscustomobject][ordered]@{
        interface_id='ADAPTER-IF-006'; family='softpc-ccpu-sas'; original_caller='none: project diagnostics only'; original_declaration='none'; historical_dependency='none'; observable_contract='instruction/segment/physical-write trace observation'; old_candidate_scope='adapter-softpc/{instruction_history,instruction_window_abi,segment_access_observation,software_interrupt_observation,physical_write_observation,ivt_watch,first_fault_observation,interrupt_return_observation}.*'; source_first_disposition='retire from production recovery'; failure_or_boundary='diagnostic evidence is not a historical adapter ABI'; successor='docs/etc evidence or a separately admitted diagnostic facility'
    }
    [pscustomobject][ordered]@{
        interface_id='ADAPTER-IF-007'; family='win32-ntdll'; original_caller='dos/dem/{demdir,demfile,demgset,demsrch}.c'; original_declaration='inc/{oemuni,vdmredir}.h'; historical_dependency='NTVDM OEM path/file/search and NtVdmControl helpers'; observable_contract='original names, ANSI/OEM conversion order, Win32 last-error and explicit unsupported result'; old_candidate_scope='adapter-mvdm-host-out/win32/facade/{opennt_dem_oem,opennt_dem_file_oem,opennt_dem_gset,opennt_dem_search,demfile_create_observation}.*'; source_first_disposition='recover only reached source-shaped facades in S4'; failure_or_boundary='public Win32 replacement only; `NtVdmControl` remains explicit unavailable unless monitor facade admits it'; successor='T275 S4'
    }
    [pscustomobject][ordered]@{
        interface_id='ADAPTER-IF-008'; family='win32-ntdll'; original_caller='dos/command/{cmdenv,cmdexec,cmdconf,cmdkeyb}.c'; original_declaration='inc/{cmdsvc,vdmtib}.h'; historical_dependency='NT RTL/environment/OEM/keyboard and BaseSrv command exchange'; observable_contract='original conversion, environment, current-directory, status and command-record ordering'; old_candidate_scope='adapter-mvdm-host-out/win32/facade/{opennt_rtl_compat,opennt_command_oem,opennt_vdm_api}.*'; source_first_disposition='recover only local public-API/session-shaped calls in S4'; failure_or_boundary='BaseSrv/CSR broker is not silently emulated; external/multi-VDM paths remain broker disposition'; successor='T275 S4 and S6'
    }
    [pscustomobject][ordered]@{
        interface_id='ADAPTER-IF-009'; family='win32-ntdll'; original_caller='softpc.new/host/src/nt_error.c'; original_declaration='softpc.new/host/inc/error.h'; historical_dependency='NTVDM resource/error dialog product surface'; observable_contract='reason/reply ordering and VDM-session termination boundary'; old_candidate_scope='adapter-mvdm-host-out/win32/facade/opennt_error_dialog_facade.*'; source_first_disposition='recover public-dialog subset in S4'; failure_or_boundary='no process termination and no CSRSS window placement'; successor='T275 S4'
    }
    [pscustomobject][ordered]@{
        interface_id='ADAPTER-IF-010'; family='vdm-monitor'; original_caller='v86/monitor/i386/{monitor,int,sas}.c; ieuvddex/i386/vdmtib.c'; original_declaration='inc/{vdmtib,v86def}.h; sim32/sim32.h'; historical_dependency='NtVdmControl, VDM_TIB, V86 event/handler family'; observable_contract='same-sized fixed-width records, handler disposition and deterministic unavailable response'; old_candidate_scope='none'; source_first_disposition='define complete disposition family in S5 before implementation'; failure_or_boundary='no kernel/private NtVdmControl recreation; no local resource or Bochs object in ABI'; successor='T275 S5'
    }
    [pscustomobject][ordered]@{
        interface_id='ADAPTER-IF-011'; family='broker'; original_caller='dos/command/{cmdexec,cmdexit,cmdmisc}.c'; original_declaration='inc/vdmtib.h; softpc.new/host/src/nt_thred.c'; historical_dependency='CCPU worker and CSR/BaseSrv multi-VDM coordination'; observable_contract='registration, copied command/completion notification, identity and disconnect cleanup'; old_candidate_scope='none'; source_first_disposition='new minimal public-IPC implementation only after exact interface ledger'; failure_or_boundary='one imported MVDM host context per process; no CCSR/BaseSrv/CCPU recreation and no native handle/pointer transport'; successor='T275 S6'
    }
)

foreach ($row in $rows) {
    foreach ($path in ($row.original_caller -split ';')) {
        if ($path -notmatch '[{}]' -and $path -notmatch '^none') { Assert-OriginalPath $path.Trim() }
    }
    foreach ($path in ($row.original_declaration -split ';')) {
        if ($path -notmatch '[{}]' -and $path -notmatch '^none') { Assert-OriginalPath $path.Trim() }
    }
}

$directory = Split-Path -Parent $OutputPath
if (-not (Test-Path -LiteralPath $directory -PathType Container)) { throw "Output directory does not exist: $directory" }
$rows | ConvertTo-Csv -Delimiter "`t" -NoTypeInformation | Set-Content -LiteralPath $OutputPath -Encoding utf8
Write-Output "Wrote $($rows.Count) historical interface dispositions."
