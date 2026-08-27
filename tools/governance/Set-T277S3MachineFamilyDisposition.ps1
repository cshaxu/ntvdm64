[CmdletBinding()]
param([string]$RepositoryRoot = '')
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot) }
$ErrorActionPreference = 'Stop'; $ops = Join-Path (Resolve-Path -LiteralPath $RepositoryRoot).Path 'docs/etc/operations'
$work = @(Import-Csv -LiteralPath (Join-Path $ops 'm0-t277-s2-source-form-worklist.tsv') -Delimiter "`t" | Where-Object {$_.caller_package -in @('suballoc','xms.486','dpmi32')})
if ($work.Count -ne 38) { throw "Expected 38 first-tranche rows; found $($work.Count)." }
function Family($row) {
    switch ($row.symbol) {
        'malloc' { return @('CRT-allocation','native CRT binding','original allocator call remains unchanged','allocation failure/result is deferred to caller-return-path proof','no adapter body') }
        'xmsEnableA20Wrapping' { return @('XMS-A20-and-state-byte','adapter-mvdm-host-out/softpc -> adapter-bochs -> session guest_memory','same-shaped A20 toggle plus checked epoch-scoped Himem state-byte lease','lease-unavailable behavior requires caller/return-path proof','no XMS allocator in adapter') }
        'xmsDisableA20Wrapping' { return @('XMS-A20-and-state-byte','adapter-mvdm-host-out/softpc -> adapter-bochs -> session guest_memory','same-shaped A20 toggle plus checked epoch-scoped Himem state-byte lease','lease-unavailable behavior requires caller/return-path proof','no XMS allocator in adapter') }
        'sas_overwrite_memory' { return @('XMS-range-overwrite-notification','adapter-mvdm-host-out/softpc -> adapter-bochs -> session guest_memory','checked guest range mutation notification; no raw host PVOID','range-error behavior requires caller/return-path proof','no pointer identity crosses adapter') }
        'host_simulate' { return @('DPMI-bounded-machine-run','adapter-mvdm-host-out/softpc -> adapter-bochs','same-shaped bounded run/stop/resume request','stop/failure result requires original caller-return-path proof','Bochs remains sole executor') }
        'EnableEmulatorIretHooks' { return @('DPMI-IRET-hook-control','adapter-mvdm-host-out/monitor -> adapter-mvdm-host-out/softpc','same-shaped per-bound-session IRET observation enable','unavailable behavior requires original caller-return-path proof','no kernel VDM hook') }
        'DisableEmulatorIretHooks' { return @('DPMI-IRET-hook-control','adapter-mvdm-host-out/monitor -> adapter-mvdm-host-out/softpc','same-shaped per-bound-session IRET observation disable','unavailable behavior requires original caller-return-path proof','no kernel VDM hook') }
        'switch_to_real_mode' { return @('DPMI-real-mode-transition','adapter-mvdm-host-out/softpc -> adapter-bochs','typed mode-transition request preserving original entry/result contract','failure behavior requires original caller-return-path proof','no BIOS or DPMI policy in Bochs') }
        default { return @('DPMI-atomic-CPU-frame','adapter-mvdm-host-out/softpc -> adapter-bochs','atomic typed CPU state read/commit preserving get/set ordering','failure behavior requires original caller-return-path proof','requires later source-form ABI detail before implementation') }
    }
}
function ProviderForm($row) {
    switch ($row.symbol) {
        'malloc' { return 'softpc.new/base/inc/standard.h declares CRT malloc; no MVDM provider body is selected' }
        'xmsEnableA20Wrapping' { return 'xms.486/xmsa20.c defines original XMS helper; it calls sas_enable_20_bit_wrapping and updates pHimemA20State' }
        'xmsDisableA20Wrapping' { return 'xms.486/xmsa20.c defines original XMS helper; it calls sas_disable_20_bit_wrapping and updates pHimemA20State' }
        'sas_overwrite_memory' { return 'softpc.new/host/src/nt_aorc.c:858 is the historical SAS function-table facade; CCPU/SAS executor remains Bochs-replaced' }
        'host_simulate' { return 'softpc.new/host/inc/host_cpu.h exposes a host_simulate function pointer; no CCPU loop may be linked' }
        'EnableEmulatorIretHooks' { return 'softpc.new/host/src/nt_eoi.c:478 defines historical hook control' }
        'DisableEmulatorIretHooks' { return 'softpc.new/host/src/nt_eoi.c:488 defines historical hook control' }
        'switch_to_real_mode' { return 'softpc.new/base/bios/bios.c publishes BOP FD handler; BIOS executor is firmware-only/Bochs-replaced' }
        default { return 'softpc.new/host/src/nt_aorc.c plus CCPU/CVIDC alternative CPU-private providers; parallel executor is excluded' }
    }
}
function Compatibility($row) {
    if ($row.caller_path -eq 'xms.486/xmsblock.c') { return 'T277 correction: overlay-required; original DWORD/PBYTE address casts cannot carry x64 guest-memory leases.' }
    if ($row.caller_path -eq 'xms.486/xmsumb.c') { return 'T277 correction: overlay-required; original PVOID-to-DWORD UMB conversion cannot represent x64 native addresses.' }
    if ($row.symbol -in @('xmsEnableA20Wrapping','xmsDisableA20Wrapping')) { return 'T277 correction: overlay-required; pHimemA20State retains a GetVDMAddr pointer across calls, forbidden for an epoch-scoped lease.' }
    return 'binding/adapter shape remains provisional pending caller-return-path review.'
}
$rows = foreach($item in $work | Sort-Object review_order,caller_path,caller_line) { $f=Family $item; [pscustomobject]@{ work_item_id=$item.work_item_id; caller_path=$item.caller_path; caller_line=$item.caller_line; symbol=$item.symbol; family=$f[0]; original_provider_form=(ProviderForm $item); owner=$f[1]; retained_interface_shape=$f[2]; failure_disposition=$f[3]; source_compatibility=(Compatibility $item); implementation_constraint=$f[4]; audit_state='source-form-family-reviewed; implementation-not-enabled'; evidence=$item.evidence } }
$rows | Export-Csv -LiteralPath (Join-Path $ops 'm0-t277-s3-machine-family-disposition-ledger.tsv') -Delimiter "`t" -NoTypeInformation -Encoding utf8
Write-Host "Wrote $($rows.Count) T277 S3 machine-family dispositions."
