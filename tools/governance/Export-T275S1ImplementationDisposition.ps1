param(
    [Parameter(Mandatory = $true)]
    [string]$CandidateInventoryPath,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Get-Disposition([string]$Path) {
    if ($Path -like 'adapter-mvdm-host-in/*') {
        return [pscustomobject]@{ ledger_id='ADAPTER-IF-001'; disposition='split-recover'; successor='T275 S2'; rationale='selector-blind copied ingress/completion only' }
    }
    if ($Path -like 'adapter-mvdm-host-out/win32/facade/demfile_create_observation.*') {
        return [pscustomobject]@{ ledger_id='ADAPTER-IF-007'; disposition='retire'; successor='documentation-only'; rationale='project diagnostic is not an original Win32 facade' }
    }
    if ($Path -like 'adapter-mvdm-host-out/win32/*') {
        $id = if ($Path -match 'error_dialog') { 'ADAPTER-IF-009' } elseif ($Path -match 'command|rtl|vdm_api') { 'ADAPTER-IF-008' } else { 'ADAPTER-IF-007' }
        return [pscustomobject]@{ ledger_id=$id; disposition='recover-if-reached'; successor='T275 S4'; rationale='retain only named source-shaped public-Win32 facade' }
    }
    if ($Path -match '/(instruction_history|instruction_window_abi|observation_transaction_abi|segment_access_observation|software_interrupt_observation|physical_write_observation|ivt_watch|first_fault_observation|interrupt_return_observation)\.') {
        return [pscustomobject]@{ ledger_id='ADAPTER-IF-006'; disposition='retire'; successor='documentation-only'; rationale='project observation has no original adapter caller' }
    }
    if ($Path -match '/(machine_engine|machine_composition|machine_stage|machine_profile_abi|finite_run)\.') {
        return [pscustomobject]@{ ledger_id='ADAPTER-IF-003'; disposition='retire'; successor='adapter-bochs only'; rationale='old machine shell is not a same-shaped SoftPC facade' }
    }
    if ($Path -match '/(a20_capability|opennt_xms_softpc_facade|ordinary_ram_reservation|softpc_memory_size_shim|softpc_emm_unavailable_shim)\.') {
        return [pscustomobject]@{ ledger_id='ADAPTER-IF-004'; disposition='defer'; successor='XMS owner package'; rationale='XMS machine semantics remain source-owner-specific' }
    }
    if ($Path -match '/(physical_irq|port_action|spckbd_handoff_shim|softpc_mouse_vector_shim|softpc_printer_openclose_shim|softpc_tape_io_shim|interrupt06_provider)\.') {
        return [pscustomobject]@{ ledger_id='ADAPTER-IF-005'; disposition='defer'; successor='machine/VDD/debugger packages'; rationale='device-specific semantics do not belong in generic facade' }
    }
    if ($Path -match '/(controlled_stop_service|cpu_result|cpu_delta_abi|cpu_state_abi|exception_abi|pending_action|bulk_result_transaction|multi_write_abi|multi_write_transaction|mechanical_action)\.') {
        return [pscustomobject]@{ ledger_id='ADAPTER-IF-001'; disposition='migrate'; successor='T275 S2'; rationale='typed BOP result/completion mechanics belong with transport, not SoftPC facade' }
    }
    if ($Path -match '/(ccpu_frame_context|guest_pointer_manager|guest_range|guest_read_action|guest_write_abi|guest_gather_read_action|dem/ccpu_sas_facade|dem/direct_context|session_resource_abi)\.') {
        return [pscustomobject]@{ ledger_id='ADAPTER-IF-002'; disposition='split-recover'; successor='T275 S3'; rationale='bounded register/SAS mapping facade, using session-owned mapping instances' }
    }
    if ($Path -match '/(host_handle_manager|idle_shim|protected_range_action)\.') {
        return [pscustomobject]@{ ledger_id='ADAPTER-IF-002'; disposition='retire'; successor='session or owner package'; rationale='duplicate manager or narrow project helper, not original same-shaped facade' }
    }
    throw "No audited disposition rule for implementation candidate: $Path"
}

$inventory = Import-Csv -LiteralPath $CandidateInventoryPath -Delimiter "`t"
$implementations = @($inventory | Where-Object { $_.candidate_kind -eq 'implementation-candidate' } | Sort-Object reference_path)
if ($implementations.Count -ne 61) { throw "Expected 61 implementation candidates, found $($implementations.Count)." }

$rows = foreach ($candidate in $implementations) {
    $decision = Get-Disposition $candidate.reference_path
    [pscustomobject][ordered]@{
        reference_path = $candidate.reference_path
        sha256 = $candidate.sha256
        interface_id = $decision.ledger_id
        disposition = $decision.disposition
        successor = $decision.successor
        rationale = $decision.rationale
    }
}

$directory = Split-Path -Parent $OutputPath
if (-not (Test-Path -LiteralPath $directory -PathType Container)) { throw "Output directory does not exist: $directory" }
$rows | ConvertTo-Csv -Delimiter "`t" -NoTypeInformation | Set-Content -LiteralPath $OutputPath -Encoding utf8
Write-Output "Wrote $($rows.Count) implementation dispositions."
