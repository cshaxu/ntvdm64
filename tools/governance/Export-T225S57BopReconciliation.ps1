[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$OutputPath
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$root = [IO.Path]::GetFullPath($RepositoryRoot)
$temporary = Join-Path ([IO.Path]::GetTempPath()) ('runner-t225-s57-' + [guid]::NewGuid().ToString('N') + '.json')

function Number([object]$value) {
    if ($null -eq $value) { return -1 }
    return [int]$value
}

function Is-Dem-Noop([int]$service) {
    return $service -in @(0x1f, 0x24, 0x26, 0x28, 0x2b, 0x40, 0x43)
}

function Get-BoundRoute([string]$selector, [int]$service) {
    switch ($selector) {
        '50' {
            if ((Is-Dem-Noop $service)) {
                return 'generic-UD bridge -> boot composition -> DEM package facade -> original no-op provider -> typed resume'
            }
            if ($service -eq 0x42) {
                return 'generic-UD bridge -> boot composition -> DEM package session -> FASTREAD compatibility branch; source/failure reconciliation remains package work'
            }
            return 'generic-UD bridge -> boot composition -> DEM package session -> facade/drive-view/whole-provider partition or explicit CLI-unavailable fallback'
        }
        '51' { return 'generic-UD bridge -> boot composition -> top-level facade -> typed stop; WOW host composition absent' }
        '52' {
            if ($service -in @(0, 1, 2, 3, 4, 5, 10, 11)) {
                return 'generic-UD bridge -> native composition -> XMS package session -> machine-backed A20/extended-memory seam'
            }
            return 'generic-UD bridge -> native composition -> XMS package session -> typed stop for unadmitted UMB/INT15 mechanics'
        }
        '53' { return 'generic-UD bridge -> native composition -> DPMI package session -> typed stop; protected-mode composition deferred' }
        '54' {
            if ($service -eq 16) {
                return 'generic-UD bridge -> boot composition -> COMMAND package session -> source-proven deferred terminal (no historical DosSessionId broker)'
            }
            return 'generic-UD bridge -> boot composition -> COMMAND package session -> bootstrap/launch/console/environment/lifecycle provider or its explicit limited outcome'
        }
        '56' { return 'generic-UD bridge -> boot composition -> debugger facade -> typed stop; debugger profile deferred' }
        '57' { return 'generic-UD bridge -> boot composition -> redirector facade -> source-derived unavailable result (CF=1, AX=1, RIP+4)' }
        '59' { return 'generic-UD bridge -> boot composition -> top-level facade -> explicit unavailable terminal safeguard' }
        '5A' { return 'generic-UD bridge -> boot composition -> top-level facade -> bounded idle resume placeholder' }
        '5B' { return 'generic-UD bridge -> boot composition -> top-level facade -> explicit diagnostic stop' }
        '5E' { return 'generic-UD bridge -> boot composition -> top-level facade -> config-complete/no-op branch where admitted' }
        'FE' { return 'generic-UD bridge -> boot composition -> top-level facade -> engine terminal placeholder' }
        '12' { return 'generic-UD bridge -> boot composition -> machine facade -> bounded memory profile helper' }
        '15' { return 'generic-UD bridge -> boot composition -> machine facade -> bounded INT 15 memory helper' }
        '5F' { return 'generic-UD bridge -> boot composition -> machine facade -> bounded machine-handoff helper or explicit unavailable result' }
        '5C' { return 'generic-UD bridge -> boot composition -> machine facade -> typed deferred stop' }
        '5D' { return 'generic-UD bridge -> boot composition -> machine facade -> typed deferred stop' }
        'FD' { return 'generic-UD bridge -> boot composition -> machine facade -> typed deferred stop' }
        default { throw "No current bound-route classification for selector $selector" }
    }
}

function Get-RecoveryRung([string]$selector, [int]$service) {
    switch ($selector) {
        '50' { if ((Is-Dem-Noop $service)) { return 'rung-1 original no-op retained' }; return 'rung-2 original owner contract through checked adapter-softpc seam; detailed ABI/failure closure pending' }
        '54' { return 'rung-2 original owner contract through checked adapter-softpc session seam; detailed ABI/failure closure pending' }
        '57' { return 'rung-2 source-derived original unavailable contract' }
        '52' { return 'rung-2 original XMS ownership through admitted mechanical seam, or source-proven deferred stop' }
        '53' { return 'rung-2 source-proven deferred protected-mode composition' }
        '56' { return 'rung-2 source-proven deferred debugger composition' }
        '51' { return 'rung-2 source-proven deferred WOW composition' }
        default { return 'owner-specific rung-2 machine/top-level seam or source-proven deferred outcome' }
    }
}

function Get-ProfileDisposition([string]$selector, [int]$service) {
    switch ($selector) {
        '50' {
            if ((Is-Dem-Noop $service)) { return 'not host-mutation controlled: original no-op in all modes' }
            if ($service -in @(0x21, 0x29, 0x2a)) { return 'deferred: opt-in device/raw-media capability; Direct/Readonly must not fabricate success; Overlay not selected' }
            return 'Direct: current host-capability candidate; Readonly: current refusal/read path requires package regression; Overlay: ABI retained, implementation deferred' }
        '54' { return 'Direct: session capability candidate; Readonly: source-shaped refusal/contained path required; Overlay: ABI retained, implementation deferred' }
        '57' { return 'Direct/Readonly: explicit unavailable pending opt-in network/IPC package; Overlay: not selected' }
        '52' { return 'machine capability, not host-mutation profile; unsupported members remain deferred in all modes' }
        '53' { return 'protected-mode capability, not host-mutation profile; deferred in all modes' }
        '56' { return 'debug capability, not host-mutation profile; deferred in all modes' }
        '51' { return 'WOW capability, not host-mutation profile; deferred in all modes' }
        default { return 'machine/engine capability, not host-mutation profile; only individually admitted outcomes apply' }
    }
}

function Get-WorkaroundAction([string]$selector, [int]$service) {
    switch ($selector) {
        '50' {
            if ((Is-Dem-Noop $service)) { return 'retain only after complete DEM package ABI confirmation' }
            if ($service -eq 0x42) { return 'migrate FASTREAD branch into DEM owner package or replace with its source-derived failure' }
            return 'migrate/reconcile package-local route under complete DEM package; replace synthetic fallback only after source/failure regression' }
        '54' { return 'migrate/reconcile package-local route under complete COMMAND package; delete endpoint-local substitute only after family regression' }
        '57' { return 'retain one reachable unavailable facade; migrate/delete redundant unbound fallback only with Redirector package recovery' }
        '52' { return 'retain partial mechanical seams as non-success evidence; complete XMS family before any success claim' }
        '53' { return 'retain explicit deferred stop until complete DPMI composition is admitted' }
        '56' { return 'retain explicit deferred stop until opt-in debugger composition is admitted' }
        '51' { return 'retain explicit deferred stop until complete WOW composition is admitted' }
        '5A' { return 'migrate idle placeholder only with engine lifecycle/cancellation package' }
        '5E' { return 'migrate config/no-op branch only with machine/config owner review' }
        default { return 'retain explicit machine/top-level deferred outcome until its whole owner package is admitted' }
    }
}

try {
    & (Join-Path $root 'tools/governance/Export-T200S31BopSourceInventory.ps1') -RepositoryRoot $root -OutputPath $temporary
    $source = Get-Content -LiteralPath $temporary -Raw | ConvertFrom-Json
    $rows = foreach ($entry in $source.entries) {
        $service = Number $entry.service
        $identity = if ($service -lt 0) { $entry.selector } else { '{0}:{1:X2}' -f $entry.selector, $service }
        [pscustomobject][ordered]@{
            identity = $identity
            selector = $entry.selector
            service = if ($service -lt 0) { $null } else { $service }
            handler = $entry.handler
            originalSource = $entry.source
            ownerPackage = $entry.ownerPackage
            currentBoundRoute = Get-BoundRoute $entry.selector $service
            sourceRecoveryRung = Get-RecoveryRung $entry.selector $service
            modeDisposition = Get-ProfileDisposition $entry.selector $service
            workaroundAction = Get-WorkaroundAction $entry.selector $service
            implementationStatus = 'mapped current route; not a claim of complete original provider recovery'
        }
    }
    if (@($rows).Count -ne 203) { throw "Expected 203 endpoint rows, got $(@($rows).Count)" }
    $duplicates = @($rows | Group-Object identity | Where-Object { $_.Count -ne 1 })
    if ($duplicates.Count -ne 0) { throw 'Duplicate selector/service identity in reconciliation ledger' }
    foreach ($row in $rows) {
        foreach ($field in @('ownerPackage','currentBoundRoute','sourceRecoveryRung','modeDisposition','workaroundAction','implementationStatus')) {
            if ([string]::IsNullOrWhiteSpace([string]$row.PSObject.Properties[$field].Value)) { throw "Blank $field for $($row.identity)" }
        }
    }
    $output = [IO.Path]::GetFullPath($OutputPath)
    $parent = Split-Path -Parent $output
    if (!(Test-Path -LiteralPath $parent)) { New-Item -ItemType Directory -Path $parent | Out-Null }
    [ordered]@{
        schema = 'runner.t225.s57.bop-owner-package-profile-ledger.v1'
        sourceInventorySchema = $source.schema
        sourceEntryCount = $source.sourceCount
        entries = $rows
    } | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath $output -Encoding utf8
    Write-Host "Exported $(@($rows).Count) T225 S57 BOP reconciliation rows to $output"
} finally {
    if (Test-Path -LiteralPath $temporary) { Remove-Item -LiteralPath $temporary -Force }
}
