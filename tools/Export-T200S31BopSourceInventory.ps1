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
        '57' { return 'not handled: current unavailable facade is selection-incompatible with ingress' }
        '59' { return 'bound typed stop (no-support terminal safeguard)' }
        '5A' { return 'bound resume (idle placeholder; lifecycle seam pending)' }
        '5B' { return 'bound typed stop (debug-break terminal safeguard)' }
        '5E' { return 'bound config-complete or source-derived no-op path' }
        'FE' { return 'bound typed stop (engine terminal placeholder)' }
        default { return 'bound machine/top-level deferred stop or no admitted provider' }
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
        auditState = 'source identity and owner assigned; ABI/failure/API review pending'
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
    schema = 'ntdos64.t200.s31.opennt-bop-source-inventory.v1'
    generatedUtc = [DateTime]::UtcNow.ToString('o')
    sourceCount = $seeded.Count
    selectorCounts = [ordered]@{
        '50' = 73; '51' = 1; '52' = 12; '53' = 25; '54' = 17; '56' = 16
        '57' = 50; 'topLevel' = 9
    }
    entries = $seeded
} | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath $output -Encoding utf8
Write-Host "Exported $($ordered.Count) OpenNT BOP source entries to $output"
