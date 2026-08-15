[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$OutputPath
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$root = [IO.Path]::GetFullPath($RepositoryRoot)
function Read-Required([string]$relative) {
    $path = Join-Path $root $relative
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) {
        throw "Required OpenNT audit source missing: $relative"
    }
    return Get-Content -LiteralPath $path
}
function Extract-Array([string]$relative, [string]$startPattern,
    [string]$selector, [int]$radix) {
    $lines = Read-Required $relative
    $started = $false; $index = 0
    $rows = [System.Collections.Generic.List[object]]::new()
    foreach ($line in $lines) {
        if (-not $started) {
            if ($line -match $startPattern) { $started = $true }
            continue
        }
        if ($line -match '^\s*};') { break }
        # Historical dispatch arrays are not mechanically uniform: the last
        # entry commonly has no trailing comma, and a few entries have no
        # trailing source comment.  The array position is the service ABI, so
        # neither form may be omitted from an audit inventory.
        if ($line -match '^\s*(?<name>[A-Za-z_][A-Za-z0-9_]*)\s*,?\s*(?://\s*(?<comment>.*))?\s*$') {
            $comment = $Matches.comment
            if ([string]::IsNullOrWhiteSpace($comment)) {
                $comment = "unannotated dispatch entry at index $index"
            }
            $rows.Add([ordered]@{
                selector = $selector; service = $index; handler = $Matches.name
                sourceComment = $comment; source = $relative
            })
            $index++
        }
    }
    if ($rows.Count -eq 0) { throw "No dispatch entries parsed from $relative" }
    return $rows
}

function Get-OwnerPackage([string]$selector, $service) {
    switch ($selector) {
        '50' { return 'DEM direct-host namespace/file/FCB/search/state package' }
        '51' { return 'WOW host-composition package' }
        '52' { return 'XMS/A20/allocator/UMB/INT15 package' }
        '53' { return 'DPMI protected-mode/LDT/exception/memory package' }
        '54' { return 'COMMAND bootstrap/launch/console/environment/lifecycle package' }
        '56' { return 'Debugger package' }
        '57' { return 'Redirector package' }
        default { return 'Top-level or machine/BIOS owner package' }
    }
}

function Get-CurrentIngressState([string]$selector, $service) {
    switch ($selector) {
        { $_ -in @('50', '52', '53', '54', '57') } { return 'mapped-deferred (generic registry)' }
        '51' { return 'mapped-deferred (WOW)' }
        '56' { return 'mapped-deferred (stack-resident debugger mode)' }
        { $_ -in @('59', '5B', '5F') } { return 'explicit-unavailable (top-level)' }
        default { return 'mapped-deferred (top-level/machine)' }
    }
}

function Get-CurrentCompositionState([string]$selector, $service) {
    $number = if ($null -eq $service) { -1 } else { [int]$service }
    switch ($selector) {
        '50' {
            if ($number -in @(31, 36, 38, 40, 43, 64, 67)) {
                return 'bound original-no-op provider (not a direct-host capability)'
            }
            if ($number -eq 66) {
                return 'bound FASTREAD compatibility branch; source-derived contract review pending'
            }
            # DEM session dispatch fans out through GSET, FCB, raw-media,
            # boot namespace and fallback planes after its facade.  A facade
            # membership list alone cannot truthfully say which leaf owns a
            # given runtime precondition; leave that exact leaf for the
            # per-service ABI/failure audit rather than fabricate failure.
            return 'bound DEM package route; individual provider/failure branch review pending'
        }
        '51' { return 'bound typed stop through top-level facade; WOW composition absent' }
        '52' {
            if ($number -in @(0, 2, 3, 5, 11)) {
                return 'bound partial mantle-backed XMS implementation; full source ABI/failure review pending'
            }
            return 'bound typed stop; XMS component deferred'
        }
        '53' { return 'bound typed stop; complete DPMI composition deferred' }
        '54' {
            if ($number -in @(0, 1, 2, 4, 5, 7, 9, 11, 12, 13, 14, 15, 16)) {
                return 'bound source-derived boot/profile helper; package closure not established'
            }
            return 'bound deferred or explicit-unavailable COMMAND route'
        }
        '56' { return 'bound typed stop; debugger profile deferred' }
        '57' { return 'bound source-derived unavailable facade (CF=1, AX=1, RIP+4); no network profile admitted' }
        '59' { return 'bound typed stop (no-support terminal safeguard)' }
        '5A' { return 'bound resume (idle placeholder; lifecycle seam pending)' }
        '5B' { return 'bound typed stop (debug-break terminal safeguard)' }
        '5E' { return 'bound config-complete or source-derived no-op path' }
        'FE' { return 'bound typed stop (engine terminal placeholder)' }
        default { return 'bound machine/top-level deferred stop or no admitted provider' }
    }
}

function Get-TargetDisposition([string]$selector, $service) {
    $number = if ($null -eq $service) { -1 } else { [int]$service }
    switch ($selector) {
        '50' {
            if ($number -in @(31, 36, 38, 40, 43, 64, 67)) { return 'original-no-op' }
            if ($number -eq 66) { return 'source-derived compatibility' }
            if ($number -in @(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,30,32,34,39,44,45,46,47,48,49,65,68,70)) {
                return 'original+CLI-seam/direct-host candidate'
            }
            if ($number -in @(33,41,42)) { return 'deferred/unavailable device profile' }
            if ($number -in @(50,51,63)) { return 'original+CLI-seam/direct-handle candidate' }
            return 'deferred/source-derived owner-package recovery'
        }
        '51' { return 'deferred/unavailable WOW composition' }
        '52' {
            if ($number -in @(0,2,3,5,11)) { return 'original+mechanical-seam candidate' }
            if ($number -eq 9) { return 'deferred/unavailable machine interrupt profile' }
            return 'deferred owner-package recovery'
        }
        '53' { return 'deferred/unavailable protected-mode composition' }
        '54' {
            if ($number -in @(4,6,7,9,12,13,14)) { return 'original+CLI-seam candidate' }
            if ($number -in @(8,10)) { return 'deferred explicit host-launch profile' }
            if ($number -in @(0,1,2,3,5,11,15,16)) { return 'source-derived CLI session/engine seam' }
            return 'deferred/unavailable COMMAND package' }
        '56' { return 'deferred debugger profile' }
        '57' { return 'source-derived unavailable network/IPC profile' }
        '59' { return 'explicit unavailable or diagnostic profile' }
        '5A' { return 'original+CLI lifecycle seam candidate' }
        '5B' { return 'explicit unavailable or diagnostic profile' }
        '5C' { return 'deferred machine/device profile' }
        '5D' { return 'deferred machine/device profile' }
        '5E' { return 'original+mechanical-seam candidate' }
        '5F' { return 'deferred/unavailable machine handoff' }
        'FD' { return 'deferred machine/device profile' }
        'FE' { return 'source-derived engine terminal' }
        default { return 'unclassified' }
    }
}

function Get-ProfileRelation([string]$selector, $service) {
    $target = Get-TargetDisposition $selector $service
    if ($target -like '*direct-host*' -or $target -like '*direct-handle*') { return 'default direct host profile' }
    if ($target -like '*network*') { return 'default unavailable; opt-in network/IPC profile' }
    if ($target -like '*host-launch*') { return 'default unavailable; opt-in host-launch/session profile' }
    if ($target -like '*machine*' -or $target -like '*mechanical*') { return 'admitted machine/device profile required' }
    if ($target -like '*debugger*') { return 'default unavailable; opt-in debugger profile' }
    if ($target -like '*WOW*') { return 'default unavailable; complete WOW composition required' }
    return 'default CLI profile or owning package admission required'
}

function Get-WorkaroundAction([string]$selector, $service) {
    $number = if ($null -eq $service) { -1 } else { [int]$service }
    switch ($selector) {
        '50' {
            if ($number -in @(31, 36, 38, 40, 43, 64, 67)) {
                return 'retain original no-op, with package-level ABI confirmation'
            }
            if ($number -eq 66) {
                return 'migrate FASTREAD compatibility branch into the DEM package or replace with its source-derived failure'
            }
            if ($number -in @(33, 41, 42)) {
                return 'retain explicit unavailable result until an admitted raw-device profile replaces it'
            }
            return 'migrate current adapter route into the complete DEM owner package; replace synthetic leaves with the shared capability ABI'
        }
        '51' { return 'replace terminal-stop workaround only with complete WOW host composition' }
        '52' {
            if ($number -in @(0, 2, 3, 5, 11)) {
                return 'retain partial mechanical candidate as non-success evidence; migrate only after full XMS ABI/failure review'
            }
            return 'retain deferred stop until the XMS owner package admits a complete provider'
        }
        '53' { return 'retain deferred stop until a complete protected-mode owner composition replaces it' }
        '54' { return 'migrate source-derived helper into the complete COMMAND package; delete endpoint-local substitutes after replacement' }
        '56' { return 'retain non-debug terminal safeguard until an opt-in debugger package replaces it' }
        '57' { return 'retain reachable family unavailable facade; delete or migrate the unbound legacy unavailable provider during Redirector recovery' }
        '59' { return 'retain explicit unavailable/diagnostic result; no silent success substitute' }
        '5A' { return 'migrate simple resume placeholder to the admitted engine lifecycle/cancellation seam' }
        '5B' { return 'retain explicit diagnostic/unavailable result outside an opt-in debugger profile' }
        '5C' { return 'retain deferred stop until an admitted machine/device profile replaces it' }
        '5D' { return 'retain deferred stop until an admitted machine/device profile replaces it' }
        '5E' { return 'migrate bounded config/no-op branch into the machine/config owner after ABI review' }
        '5F' { return 'retain explicit unavailable result; do not repurpose as adapter extension point' }
        'FD' { return 'retain deferred stop until an admitted machine/device profile replaces it' }
        'FE' { return 'replace terminal placeholder only with the declared engine terminal/result ABI' }
        default { return 'unclassified' }
    }
}

$all = [System.Collections.Generic.List[object]]::new()
foreach ($row in (Extract-Array 'src/opennt/base/mvdm/dos/dem/demdisp.c' 'apfnSVC\s*\[\]\s*=' '50' 10)) { $all.Add($row) }
foreach ($row in (Extract-Array 'src/opennt/base/mvdm/dos/command/cmddisp.c' 'apfnSVCCmd\s*\[\]\s*=' '54' 10)) { $all.Add($row) }
foreach ($row in (Extract-Array 'src/opennt/base/mvdm/xms.486/xmsdisp.c' 'apfnXMSSvc\s*\[\]\s*=' '52' 10)) { $all.Add($row) }
foreach ($row in (Extract-Array 'src/opennt/base/mvdm/dpmi32/dpmi32.c' 'DpmiDispatchTable.*=\s*\{' '53' 10)) { $all.Add($row) }

$redir = Read-Required 'src/opennt/base/mvdm/inc/rdrsvc.h'
foreach ($line in $redir) {
    if ($line -match '^#define\s+(?<name>SVC_[A-Z0-9_]+)\s+0x(?<value>[0-9a-fA-F]+)\s*(//\s*(?<comment>.*))?$') {
        $all.Add([ordered]@{
            selector = '57'; service = [Convert]::ToInt32($Matches.value, 16)
            handler = $Matches.name; sourceComment = $Matches.comment
            source = 'src/opennt/base/mvdm/inc/rdrsvc.h'
        })
    }
}
$debug = Read-Required 'src/opennt/base/mvdm/inc/dbgsvc.h'
foreach ($line in $debug) {
    if ($line -match '^#define\s+(?<name>DBG_[A-Z0-9_]+)\s+(?<value>\d+)\s*$') {
        $all.Add([ordered]@{
            selector = '56'; service = [int]$Matches.value; handler = $Matches.name
            sourceComment = 'debugger stack service'; source = 'src/opennt/base/mvdm/inc/dbgsvc.h'
        })
    }
}

$top = Read-Required 'src/opennt/base/mvdm/inc/bop.h'
foreach ($line in $top) {
    if ($line -match '^#define\s+(?<name>BOP_[A-Z0-9_]+)\s+0x(?<value>[0-9a-fA-F]+)') {
        $selector = [Convert]::ToInt32($Matches.value, 16)
        if ($selector -notin @(0x50, 0x52, 0x53, 0x54, 0x56, 0x57)) {
            $all.Add([ordered]@{
                selector = ('{0:X2}' -f $selector); service = $null; handler = $Matches.name
                sourceComment = 'top-level selector'; source = 'src/opennt/base/mvdm/inc/bop.h'
            })
        }
    }
}

$ordered = @($all | Sort-Object selector, @{ Expression = { if ($null -eq $_.service) { -1 } else { $_.service } } }, handler)
$seeded = foreach ($entry in $ordered) {
    [ordered]@{
        selector = $entry.selector
        service = $entry.service
        handler = $entry.handler
        sourceComment = $entry.sourceComment
        source = $entry.source
        ownerPackage = Get-OwnerPackage $entry.selector $entry.service
        currentIngressState = Get-CurrentIngressState $entry.selector $entry.service
        currentCompositionState = Get-CurrentCompositionState $entry.selector $entry.service
        targetDisposition = Get-TargetDisposition $entry.selector $entry.service
        profileRelation = Get-ProfileRelation $entry.selector $entry.service
        workaroundAction = Get-WorkaroundAction $entry.selector $entry.service
        auditState = 'source identity and owner assigned; ABI/failure/API review pending'
    }
}
$duplicates = @($seeded | Group-Object {
        "$($_.selector):$(if ($null -eq $_.service) { '-' } else { $_.service })"
    } | Where-Object { $_.Count -ne 1 })
if ($duplicates.Count -ne 0) {
    throw "OpenNT BOP inventory has duplicate selector/service identities"
}
foreach ($entry in $seeded) {
    if ([string]::IsNullOrWhiteSpace($entry.ownerPackage) -or
        [string]::IsNullOrWhiteSpace($entry.targetDisposition) -or
        [string]::IsNullOrWhiteSpace($entry.profileRelation) -or
        [string]::IsNullOrWhiteSpace($entry.workaroundAction) -or
        $entry.targetDisposition -eq 'unclassified' -or
        $entry.workaroundAction -eq 'unclassified') {
        throw "OpenNT BOP inventory has an unclassified admission record"
    }
}
$expectedCounts = [ordered]@{
    '50' = 73; '52' = 12; '53' = 25; '54' = 17; '56' = 16; '57' = 50
}
foreach ($selector in $expectedCounts.Keys) {
    $actual = @($ordered | Where-Object { $_.selector -eq $selector }).Count
    if ($actual -ne $expectedCounts[$selector]) {
        throw "OpenNT BOP inventory drift for selector ${selector}: expected $($expectedCounts[$selector]), got $actual"
    }
}
$output = [IO.Path]::GetFullPath($OutputPath)
$parent = Split-Path -Parent $output
if (-not (Test-Path -LiteralPath $parent)) { New-Item -ItemType Directory -Path $parent | Out-Null }
[ordered]@{
    schema = 'ntdos64.t200.s31.opennt-bop-source-inventory.v2'
    generatedUtc = [DateTime]::UtcNow.ToString('o')
    sourceCount = $seeded.Count
    selectorCounts = [ordered]@{
        '50' = 73; '51' = 1; '52' = 12; '53' = 25; '54' = 17; '56' = 16
        '57' = 50; 'topLevel' = 9
    }
    entries = $seeded
} | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath $output -Encoding utf8
Write-Host "Exported $($ordered.Count) OpenNT BOP source entries to $output"
