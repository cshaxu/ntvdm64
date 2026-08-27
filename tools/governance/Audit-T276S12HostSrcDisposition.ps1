[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$operations = Join-Path $root 'docs/etc/operations'
$filePath = Join-Path $operations 'mvdm-file-recovery-ledger.tsv'
$buildPath = Join-Path $operations 'm0-t276-s2-translation-unit-build-ledger.tsv'
foreach ($path in @($filePath, $buildPath)) { if (-not (Test-Path -LiteralPath $path)) { throw "Missing input: $path" } }
$files = @(Import-Csv -LiteralPath $filePath -Delimiter "`t" | Where-Object { $_.target_path -like 'softpc.new/host/src/*' -and $_.file_kind -eq 'source' } | Sort-Object target_path)
$builds = @(Import-Csv -LiteralPath $buildPath -Delimiter "`t")
if ($files.Count -ne 56) { throw "Expected 56 host/src source rows, found $($files.Count)." }

$groups = [ordered]@{
    'opennt-bop-dispatch' = @('nt_bop.c')
    'adapter-mvdm-host-out/monitor' = @('nt_eoi.c','nt_event.c','nt_inthk.c','nt_reset.c')
    'session-and-monitor-thread' = @('nt_thred.c')
    'cross-owner-startup-composition' = @('nt_msscs.c')
    'adapter-mvdm-host-out/softpc-machine-facade' = @('copy_fnc.c','cpucstbs.c','nt_cpu.c','nt_emm.c','nt_mem.c','nt_sas.c','nt_umb.c','sim32.c','stubs.c','x86_emm.c')
    'cross-owner-launch-configuration' = @('config.c')
    'adapter-mvdm-host-out/win32-host-capability' = @('fprt.c','nt_error.c','nt_keycd.c','nt_nls.c','nt_ntfun.c','nt_pif.c','nt_unix.c')
    'cross-owner-video-machine-initialization' = @('nt_sec.c')
    'adapter-mvdm-host-out/vdd-or-debugger' = @('nt_vdd.c','nt_yoda.c')
    'machine-device-bridge-review' = @('nt_cga.c','nt_com.c','nt_det.c','nt_ega.c','nt_fdisk.c','nt_fulsc.c','nt_graph.c','nt_hosts.c','nt_input.c','nt_lpt.c','nt_mouse.c','nt_munge.c','nt_rez.c','nt_rflop.c','nt_sound.c','nt_timer.c','nt_vga.c','nt_wcom.c')
    'literal-build-unobserved-auxiliary' = @('nt_aorc.c','nt_cprgs.c','nt_ertbl.c','nt_hfx.c','nt_hunt.c','nt_mess.c','nt_smenu.c','nt_term.c','nt_vflop.c','stf_conf.c')
}

$classification = @{}
foreach ($entry in $groups.GetEnumerator()) {
    foreach ($name in $entry.Value) {
        if ($classification.ContainsKey($name)) { throw "Duplicate host/src classification for $name" }
        $classification[$name] = $entry.Key
    }
}

function Get-Contract {
    param([string]$Group)
    switch ($Group) {
        'opennt-bop-dispatch' { return @('adapter-mvdm-host-in; adapter-mvdm-host-out/softpc; adapter-mvdm-host-out/win32; specialist adapters', 'original BOP selector/host dispatch; preserve original routing shape, evaluate each reached historical boundary', 'adapter-bound review') }
        'adapter-mvdm-host-out/monitor' { return @('adapter-mvdm-host-out/monitor; adapter-mvdm-host-out/softpc', 'original event/interrupt/reset/monitor glue; retain interface shape and classify VDM monitor versus machine calls', 'adapter-bound review') }
        'session-and-monitor-thread' { return @('session; adapter-mvdm-host-out/monitor', 'original worker/thread coordination; recover only session-local observable contract, never NT4 global worker shell', 'adapter-bound review') }
        'cross-owner-startup-composition' { return @('app; session; adapter-mvdm-host-out/softpc; adapter-mvdm-host-out/win32; adapter-mvdm-host-out/vdd; mvdm-host owner packages', 'original NTVDM startup/control composition reaches COMMAND, XMS, debugger/VDD, machine and host APIs; preserve it as a cross-owner composition nexus and do not misclassify it as a standalone monitor provider', 'composition-prerequisite review') }
        'adapter-mvdm-host-out/softpc-machine-facade' { return @('adapter-mvdm-host-out/softpc; adapter-bochs', 'original CCPU/SAS/memory facade; same-shaped calls may bind to Bochs mechanics only through declared adapter boundary', 'adapter-bound review') }
        'cross-owner-launch-configuration' { return @('app; session; adapter-mvdm-host-out/win32; adapter-mvdm-host-out/softpc; adapter-mvdm-host-out/monitor; mvdm-host owner packages', 'original launch/configuration composition crosses PIF, console, host environment, monitor and machine setup; retain exact mirror and do not classify it as a standalone Win32 provider', 'composition-prerequisite review') }
        'adapter-mvdm-host-out/win32-host-capability' { return @('adapter-mvdm-host-out/win32; session; app; adapter-mvdm-host-out/debugger as individually proved', 'original host capability/NTDLL/Win32 source; preserve algorithm and error ordering while auditing each public-modern binding and its session/product owner', 'adapter-bound review') }
        'cross-owner-video-machine-initialization' { return @('adapter-mvdm-host-out/monitor; adapter-mvdm-host-out/softpc; adapter-bochs; adapter-mvdm-host-out/win32', 'original video-memory and ROM-mapping initialization crosses monitor, machine and virtual-memory boundaries; retain exact mirror and do not classify it as a standalone Win32 provider', 'composition-prerequisite review') }
        'adapter-mvdm-host-out/vdd-or-debugger' { return @('adapter-mvdm-host-out/vdd; adapter-mvdm-host-out/debugger', 'original VDD or debugger product bridge; no provider policy or Bochs object enters source', 'adapter-bound review') }
        'machine-device-bridge-review' { return @('adapter-mvdm-host-out/softpc; adapter-bochs; adapter-mvdm-host-out/win32 as individually proved', 'mixed original device bridge; determine Bochs-native replacement versus same-shaped host-device binding before enablement', 'device-bound review') }
        'literal-build-unobserved-auxiliary' { return @('none until original build/profile evidence expands', 'not in the literal host/src SOURCES list; retain exact mirror and do not infer runtime reachability', 'profile/build-unobserved') }
        default { throw "Unknown host/src group: $Group" }
    }
}

$rows = foreach ($file in $files) {
    $name = Split-Path -Leaf $file.target_path
    if (-not $classification.ContainsKey($name)) { throw "No host/src classification for $name" }
    $membership = @($builds | Where-Object file_id -eq $file.file_id)
    if ($membership.Count -ne 1) { throw "Expected one translation-unit build row for $($file.target_path), found $($membership.Count)." }
    $contract = Get-Contract -Group $classification[$name]
    [pscustomobject][ordered]@{
        file_id = $file.file_id
        source_path = $file.target_path
        original_literal_build_membership = $membership[0].original_build_membership
        original_role_group = $classification[$name]
        required_boundary_owner = $contract[0]
        original_contract_summary = $contract[1]
        provisional_disposition = $contract[2]
        state = 'provisional-not-enabled'
        evidence = "$($file.target_path); softpc.new/host/src/sources; m0-t276-s2-translation-unit-build-ledger.tsv"
    }
}

$rows | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" | Set-Content -LiteralPath (Join-Path $operations 'm0-t276-s12-host-src-disposition-ledger.tsv') -Encoding utf8
Write-Output "T276 S12 host/src disposition generated: $($rows.Count) source rows."
