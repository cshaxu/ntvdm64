[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$OutputDirectory = 'docs/etc/operations'
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$out = Join-Path $root $OutputDirectory

$files = @(
    'mapembed.c', 'walias.c', 'walloc16.c', 'wcall16.c', 'wcall32.c',
    'wheap.c', 'wow32.c', 'wowtbl.c', 'wparam.c', 'wres16.c', 'wres32.c',
    'wstruc.c'
)

$families = [ordered]@{
    'GETVDMPTR' = [ordered]@{
        id = 'WOW-TF-01'; contract = 'bounded VDM structure/string access';
        lifetime = 'synchronous checked guest-memory lease; must end before callback, worker, or component return';
        existing = 'no; Redirector-only prepared scope is intentionally not reusable';
        owner = 'adapter-mvdm-host-out/softpc -> session.guest_memory';
        prerequisite = 'source-shaped WOW pointer scope with dynamic bounds and explicit read/write lease release';
        disposition = 'adapter-backed after complete scope design; no provider body enabled'
    }
    'FREEVDMPTR' = [ordered]@{
        id = 'WOW-TF-01'; contract = 'end transient VDM pointer use';
        lifetime = 'ends the matching synchronous checked lease';
        existing = 'no; upstream x86 no-op is unsafe outside the historical process alias';
        owner = 'adapter-mvdm-host-out/softpc -> session.guest_memory';
        prerequisite = 'paired WOW pointer scope; preserve source release ordering';
        disposition = 'adapter-backed after complete scope design; no provider body enabled'
    }
    'FLUSHVDMPTR' = [ordered]@{
        id = 'WOW-TF-01'; contract = 'publish modified VDM bytes before releasing the mapping';
        lifetime = 'must commit the matching synchronous checked lease before release';
        existing = 'no; no generic code-flush or unbounded alias is admitted';
        owner = 'adapter-mvdm-host-out/softpc -> session.guest_memory -> adapter-bochs';
        prerequisite = 'paired WOW pointer scope with explicit write/commit designation';
        disposition = 'adapter-backed after complete scope design; no provider body enabled'
    }
    'GETFRAMEPTR' = [ordered]@{
        id = 'WOW-TF-02'; contract = 'resolve a protected VDMFRAME/CBVDMFRAME at the current WOW task stack';
        lifetime = 'synchronous frame lease only; VDMFRAME address remains a numeric VPVOID outside the lease';
        existing = 'partial; monitor has source-shaped VDM_TIB storage but no WOW task/frame projection';
        owner = 'adapter-mvdm-host-out/monitor -> adapter-mvdm-host-out/softpc -> session.guest_memory';
        prerequisite = 'complete task/frame projection and bounded protected stack-frame lease; no host TEB/CCPU stack alias';
        disposition = 'unresolved source-shaped interface; no provider body enabled'
    }
    'CallBack16' = [ordered]@{
        id = 'WOW-TF-03'; contract = 'construct CBVDMFRAME, enter the Win16 callback, then consume AX:DX and restore the task stack';
        lifetime = 'parameter frame and guest frame leases end before an asynchronous operation; continuation is session-owned';
        existing = 'no; current monitor intentionally has no WOW callback scheduler';
        owner = 'adapter-mvdm-host-out/wow -> adapter-mvdm-host-out/monitor -> adapter-mvdm-host-out/softpc -> session';
        prerequisite = 'source-shaped single-session callback transaction with task-frame projection and typed pause/resume; no CSRSS/BaseSrv or second executor';
        disposition = 'unresolved whole-subdomain prerequisite; no provider body enabled'
    }
    'Sim32GetVDMPointer' = [ordered]@{
        id = 'WOW-TF-04'; contract = 'historical monitor pointer acquisition for numeric VDM address/span';
        lifetime = 'only a bounded synchronous guest-memory lease; address zero plus size zero cannot become a whole-memory host alias';
        existing = 'partial; protected-span and Redirector forms are purpose-limited and cannot satisfy this call shape';
        owner = 'adapter-mvdm-host-out/softpc -> session.guest_memory';
        prerequisite = 'per-call bounded span binding or original source-defined unavailable result; no global IntelMemoryBase';
        disposition = 'unresolved source-shaped interface; no provider body enabled'
    }
    'GetVDMAddr' = [ordered]@{
        id = 'WOW-TF-04'; contract = 'historical real-mode monitor pointer acquisition';
        lifetime = 'only one bounded synchronous real-mode lease; returned pointer cannot cross a component boundary';
        existing = 'no for WOW; current macro is Redirector-only and scope-bound';
        owner = 'adapter-mvdm-host-out/softpc -> session.guest_memory';
        prerequisite = 'per-call fixed BIOS-data span binding or source-defined unavailable result';
        disposition = 'unresolved source-shaped interface; no provider body enabled'
    }
    'NtCurrentTeb' = [ordered]@{
        id = 'WOW-TF-05'; contract = 'read historical monitor-thread identity or install CurrentMonitorTeb for fast callback code';
        lifetime = 'current bound session worker only; no pointer is durable or crosses an ABI';
        existing = 'partial; monitor binds adapter-private TEB.Vdm only, not a historical monitor TEB/product shell';
        owner = 'adapter-mvdm-host-out/monitor -> session';
        prerequisite = 'per-call monitor-thread facade or source-defined unavailable branch; no private NT TEB field recovery';
        disposition = 'unresolved source-shaped interface; no provider body enabled'
    }
    'NtCurrentPeb' = [ordered]@{
        id = 'WOW-TF-06'; contract = 'inspect NT4 private FastPebLock before AeDebug process launch';
        lifetime = 'private product-shell state; no substitute pointer is permitted';
        existing = 'no; public Win32 has no equivalent FastPebLock contract';
        owner = 'adapter-mvdm-host-out/monitor -> adapter-mvdm-host-out/win32 -> session';
        prerequisite = 'preserve the source fallback/decline branch after a focused hard-error/AeDebug policy audit';
        disposition = 'source-defined unavailable/fallback required; no provider body enabled'
    }
    'CURRENTPTD' = [ordered]@{
        id = 'WOW-TF-02'; contract = 'obtain the historical current WOW task descriptor containing numeric vpStack/vpCBStack state';
        lifetime = 'bound session worker only; task descriptor fields are copied numeric data, never native pointers';
        existing = 'no; monitor VDM_TIB does not contain the WOW task descriptor';
        owner = 'adapter-mvdm-host-out/wow -> adapter-mvdm-host-out/monitor -> session';
        prerequisite = 'source-shaped one-session WOW task descriptor projection with no process-global current task';
        disposition = 'unresolved source-shaped interface; no provider body enabled'
    }
    'SETVDMSTACK' = [ordered]@{
        id = 'WOW-TF-03'; contract = 'replace the historical VDM stack before a synchronous callback re-entry';
        lifetime = 'one typed callback transaction; numeric VPVOID only';
        existing = 'no; no monitor callback transaction or stack projection is implemented';
        owner = 'adapter-mvdm-host-out/monitor -> adapter-mvdm-host-out/softpc -> adapter-bochs -> session';
        prerequisite = 'same callback transaction that validates and commits one complete guest frame';
        disposition = 'unresolved whole-subdomain prerequisite; no provider body enabled'
    }
    'SETFASTVDMSTACK' = [ordered]@{
        id = 'WOW-TF-03'; contract = 'replace the fast-monitor VDM stack before a callback';
        lifetime = 'one typed callback transaction; numeric VPVOID only';
        existing = 'no; FASTBOPPING/fast-monitor product shell is not recovered';
        owner = 'adapter-mvdm-host-out/monitor -> session';
        prerequisite = 'explicit future fast-monitor owner package or source-defined unavailable selection';
        disposition = 'source-defined unavailable/fallback required; no provider body enabled'
    }
    'VDMSTACK' = [ordered]@{
        id = 'WOW-TF-03'; contract = 'read the callback-return VDM stack numeric address';
        lifetime = 'one typed callback transaction; numeric VPVOID only';
        existing = 'no; no callback transaction is implemented';
        owner = 'adapter-mvdm-host-out/monitor -> session';
        prerequisite = 'same callback transaction that owns resume and result extraction';
        disposition = 'unresolved whole-subdomain prerequisite; no provider body enabled'
    }
    'host_simulate' = [ordered]@{
        id = 'WOW-TF-03'; contract = 'historically re-enter the CCPU executor for a callback';
        lifetime = 'bounded synchronous callback transaction';
        existing = 'no; CCPU executor is replaced by typed Bochs mechanics';
        owner = 'adapter-mvdm-host-out/softpc -> adapter-bochs -> session';
        prerequisite = 'source-shaped callback resume transaction; must not create a second executor';
        disposition = 'adapter-backed after complete callback transaction design; no provider body enabled'
    }
    'FastWOWCallbackCall' = [ordered]@{
        id = 'WOW-TF-03'; contract = 'invoke the NT4 fast monitor callback stub';
        lifetime = 'private fast-monitor worker/product shell';
        existing = 'no; private fast monitor is intentionally absent';
        owner = 'adapter-mvdm-host-out/monitor -> session';
        prerequisite = 'explicit future fast-monitor package or original non-fast fallback selection';
        disposition = 'source-defined unavailable/fallback required; no provider body enabled'
    }
    'CurrentMonitorTeb' = [ordered]@{
        id = 'WOW-TF-05'; contract = 'publish the current NT4 monitor TEB to fast callback assembly';
        lifetime = 'private fast-monitor worker/product shell';
        existing = 'no; current monitor TLS is not a historical monitor TEB';
        owner = 'adapter-mvdm-host-out/monitor -> session';
        prerequisite = 'explicit future fast-monitor package or original non-fast fallback selection';
        disposition = 'source-defined unavailable/fallback required; no provider body enabled'
    }
    'getIntelRegistersPointer' = [ordered]@{
        id = 'WOW-TF-07'; contract = 'obtain the historical CCPU register-context pointer at WOW initialization';
        lifetime = 'historical global executor pointer';
        existing = 'no; adapter-bochs exposes typed register operations, not a register-context address';
        owner = 'adapter-mvdm-host-out/softpc -> adapter-bochs -> session';
        prerequisite = 'per-call typed register snapshot binding or a source-defined unavailable initialization branch';
        disposition = 'unresolved source-shaped interface; no provider body enabled'
    }
    'getMSW' = [ordered]@{
        id = 'WOW-TF-07'; contract = 'read the original machine-status word to choose WOW real/protected behavior';
        lifetime = 'scalar immediate machine query';
        existing = 'partial; source-shaped scalar register facade exists for reached XMS/DPMI bodies but this WOW binding is unverified';
        owner = 'adapter-mvdm-host-out/softpc -> adapter-bochs -> session';
        prerequisite = 'audit the exact scalar call shape against current register facade before body admission';
        disposition = 'binding audit required; no provider body enabled'
    }
}

$rows = [System.Collections.Generic.List[object]]::new()
foreach ($file in $files) {
    $relative = 'wow32/' + $file
    $path = Join-Path $root ('src/mvdm-host/' + $relative)
    if (-not (Test-Path -LiteralPath $path)) { throw "Missing selected source: $relative" }
    $lineNumber = 0
    foreach ($line in Get-Content -LiteralPath $path) {
        $lineNumber++
        foreach ($token in $families.Keys) {
            if ($line -notmatch [regex]::Escape($token)) { continue }
            if ($line.TrimStart().StartsWith('//')) { continue }
            $family = $families[$token]
            $rows.Add([PSCustomObject][ordered]@{
                audit_id = ('{0}-{1:d3}' -f $family.id, $lineNumber)
                source_path = $relative
                source_line = $lineNumber
                interface_family = $family.id
                source_token = $token
                original_contract = $family.contract
                pointer_or_identity_lifetime = $family.lifetime
                current_same_shaped_binding = $family.existing
                required_owner_path = $family.owner
                exact_unavailable_prerequisite = $family.prerequisite
                final_s2_disposition = $family.disposition
                source_excerpt = $line.Trim()
            })
        }
    }
}

if ($rows.Count -eq 0) { throw 'No T291/S2 interface occurrences were found.' }
$requiredTokens = @('GETVDMPTR', 'FREEVDMPTR', 'FLUSHVDMPTR', 'GETFRAMEPTR',
    'CallBack16', 'Sim32GetVDMPointer', 'GetVDMAddr', 'NtCurrentTeb',
    'NtCurrentPeb', 'CURRENTPTD', 'SETVDMSTACK', 'SETFASTVDMSTACK', 'VDMSTACK',
    'host_simulate', 'FastWOWCallbackCall', 'CurrentMonitorTeb',
    'getIntelRegistersPointer', 'getMSW')
foreach ($token in $requiredTokens) {
    if (@($rows | Where-Object { $_.source_token -eq $token }).Count -eq 0) {
        throw "Expected reached WOW thunk/frame token was not found: $token"
    }
}
$rows | Sort-Object source_path, source_line, source_token |
    Export-Csv (Join-Path $out 'm0-t291-s2-wow-thunk-frame-interface-occurrence-ledger.tsv') -Delimiter "`t" -NoTypeInformation -Encoding utf8

$familyRows = foreach ($token in $families.Keys) {
    $family = $families[$token]
    $occurrences = @($rows | Where-Object { $_.source_token -eq $token })
    [PSCustomObject][ordered]@{
        interface_family = $family.id
        source_token = $token
        occurrence_count = $occurrences.Count
        source_callers = [string]::Join('; ', @($occurrences.source_path | Sort-Object -Unique))
        original_contract = $family.contract
        pointer_or_identity_lifetime = $family.lifetime
        current_same_shaped_binding = $family.existing
        required_owner_path = $family.owner
        exact_unavailable_prerequisite = $family.prerequisite
        final_s2_disposition = $family.disposition
    }
}
$familyRows | Export-Csv (Join-Path $out 'm0-t291-s2-wow-thunk-frame-interface-family-ledger.tsv') -Delimiter "`t" -NoTypeInformation -Encoding utf8

$coverageRows = foreach ($file in $files) {
    $relative = 'wow32/' + $file
    $occurrences = @($rows | Where-Object { $_.source_path -eq $relative })
    [PSCustomObject][ordered]@{
        source_path = $relative
        reached_interface_occurrence_count = $occurrences.Count
        reached_interface_families = if ($occurrences.Count -eq 0) {
            'none in S2 scope'
        } else {
            [string]::Join('; ', @($occurrences.interface_family | Sort-Object -Unique))
        }
        coverage_disposition = if ($occurrences.Count -eq 0) {
            'reviewed; no S2 thunk/frame boundary call'
        } else {
            'reviewed; every reached S2 call is in occurrence ledger'
        }
    }
}
$coverageRows | Export-Csv (Join-Path $out 'm0-t291-s2-wow-thunk-frame-source-coverage-ledger.tsv') -Delimiter "`t" -NoTypeInformation -Encoding utf8

Write-Host "T291 S2 WOW thunk/frame audit exported: $($rows.Count) reached interface occurrences, $($familyRows.Count) source-token dispositions and $($coverageRows.Count) source-coverage rows."
