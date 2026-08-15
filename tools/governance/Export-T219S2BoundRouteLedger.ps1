param(
    [Parameter(Mandatory = $true)][string]$CanonicalInventoryPath,
    [Parameter(Mandatory = $true)][string]$OutputPath
)

$ErrorActionPreference = 'Stop'
$inventory = (Get-Content -Raw -LiteralPath $CanonicalInventoryPath | ConvertFrom-Json).entries

function In-Set([int]$Value, [int[]]$Set) { return $Set -contains $Value }
function HexId([int]$Value) { return ('{0:X2}' -f $Value) }

$demNoop = 31,36,38,40,43,64,67
$demMisc = 29,35,52,53,54,55,56,57,58,59,62
$demWholeHandle = 0,2,8,22,30,39
$demWholeNamespace = 1,3,4,5,6,18,19,23,24,34,39,44,66,68,71,72
$demWholeFcbPath = 7,32,44,45,49
$demWholeFcbDirect = 46,47
$demWholeFcbIo = 47
$demWholeSearch = 9,10,11,12
$demSharedProfile = 13,15,18,19,20,21,25,26,27,28,37,65,70
$commandSharedProfile = 2,4,15

$rows = foreach ($entry in $inventory) {
    $selector = [Convert]::ToInt32([string]$entry.selector, 16)
    $service = if ($null -eq $entry.service) { $null } else { [int]$entry.service }
    $route = ''
    $relation = 'confirmed'
    $shim = $null
    $fallback = $null
    $profile = 'not-applicable'
    $owner = $entry.ownerPackage

    if ($selector -eq 0x50) {
        $prefix = 'generic-ud-bridge -> boot-namespace-composition -> dem-package-session'
        if (In-Set $service $demNoop) {
            $route = "$prefix -> dem-package-facade -> dem-provider original-noop"
            $fallback = 'none; source-shaped original no-op is terminal for this route'
        } elseif (In-Set $service $demMisc) {
            $route = "$prefix -> dem-misc-plane -> source-derived resume/no-op or named debug/system-symbol helper"
            $relation = 'changed'
            $shim = 'post-T200 DEM misc-plane helper routing'
            $fallback = 'if the helper declines, the ordered DEM chain reaches cli-unavailable'
        } elseif (In-Set $service $demWholeHandle) {
            $route = "$prefix -> dem-whole-provider handle partition (when installed and token/precondition matches)"
            $relation = 'changed'
            $shim = 'T202 direct-host DEM whole-provider / file-session seam'
            $fallback = 'cli-unavailable after an owning whole-provider rejection; legacy boot namespace is no longer a primary handle path'
            $profile = 'migrate-to-shared-profile-abi'
        } elseif (In-Set $service $demWholeNamespace) {
            $route = "$prefix -> dem-whole-provider namespace partition (when installed)"
            $relation = 'changed'
            $shim = 'T202 direct-host DEM namespace partition, with declared startup-image overlay'
            $fallback = 'cli-unavailable after partition rejection; boot namespace remains only a declared-image/fixture fallback'
            $profile = 'migrate-to-shared-profile-abi'
        } elseif (In-Set $service $demWholeFcbPath) {
            $route = "$prefix -> dem-whole-provider FCB path partition (when installed)"
            $relation = 'changed'
            $shim = 'T202 direct-host DEM FCB path seam'
            $fallback = 'cli-unavailable after partition rejection'
            $profile = 'migrate-to-shared-profile-abi'
        } elseif (In-Set $service $demWholeFcbDirect -or In-Set $service $demWholeFcbIo) {
            $route = "$prefix -> dem-whole-provider FCB direct/I-O partition when DTA prerequisites hold"
            $relation = 'changed'
            $shim = 'T202 direct-host DEM FCB state seam'
            $fallback = 'cli-unavailable after prerequisite or provider rejection'
            $profile = 'migrate-to-shared-profile-abi'
        } elseif (In-Set $service $demWholeSearch) {
            $route = "$prefix -> dem-whole-provider merged host/declared search partition (when installed)"
            $relation = 'changed'
            $shim = 'T202 direct-host merged search seam; declared-image overlay is post-T200'
            $fallback = 'cli-unavailable after search-provider rejection'
            $profile = 'migrate-to-shared-profile-abi'
        } elseif (In-Set $service $demSharedProfile) {
            $route = "$prefix -> DEM GSET/CWD/drive-or-volume source-derived providers, conditional on snapshots/session context"
            $relation = 'changed'
            $shim = 'T210 shared session host-context consumer and later direct-host drive/CWD migrations'
            $fallback = 'older snapshot/synthetic or cli-unavailable branches remain reachable if shared context is absent'
            $profile = 'already-consumes-shared-profile-abi; remove remaining snapshot/synthetic fallbacks'
        } elseif ($service -eq 60) {
            $route = "$prefix -> dem-session-lifecycle-provider -> finite boot-namespace termination only"
            $relation = 'confirmed-with-gap'
            $fallback = 'stale for direct host resources: no copied PDB ownership, so direct file/search resources are not released'
            $profile = 'migrate-to-shared-profile-abi-after-PDB-owner-design'
        } else {
            $route = "$prefix -> ordered DEM special-plane chain -> boot-namespace plane or cli-unavailable"
            $fallback = 'T200-era generic cli-unavailable remains the final package fallback; exact source failure is not yet recovered'
            $profile = 'review-at-owner-package-admission'
        }
    } elseif ($selector -eq 0x54) {
        $prefix = 'generic-ud-bridge -> boot-namespace-composition -> command-package-session'
        switch ($service) {
            0 { $route = "$prefix -> console-keyboard provider / package-owned deferred outcome"; $relation = 'changed'; $shim = 'post-T200 COMMAND package-session consolidation' }
            1 { $route = "$prefix -> bootstrap get-next provider; exhausted declared plan stops"; $relation = 'changed'; $shim = 'post-T200 declared launch-plan seam' }
            2 { $route = "$prefix -> bootstrap COMSPEC source-derived seam"; $relation = 'changed'; $shim = 'T217 OpenNT-shaped initial-environment/COMSPEC seam'; $profile = 'migrate-to-shared-profile-abi' }
            3 { $route = "$prefix -> command execution lifecycle provider"; $relation = 'changed'; $shim = 'post-T200 COMMAND execution lifecycle seam' }
            4 { $route = "$prefix -> session-host-context projection -> current-directory service"; $relation = 'changed'; $shim = 'T210 shared host-context provider'; $profile = 'already-consumes-shared-profile-abi' }
            5 { $route = "$prefix -> launch-plane registration provider"; $relation = 'changed'; $shim = 'post-T200 COMMAND launch-plane seam' }
            6 { $route = "$prefix -> package-owned explicit-unavailable/deferred console branch"; $relation = 'changed'; $shim = 'post-T200 COMMAND package-session consolidation' }
            7 { $route = "$prefix -> command execution lifecycle provider"; $relation = 'changed'; $shim = 'post-T200 COMMAND execution lifecycle seam' }
            8 { $route = "$prefix -> package-owned explicit-unavailable/deferred console branch"; $relation = 'changed'; $shim = 'post-T200 COMMAND package-session consolidation' }
            9 { $route = "$prefix -> command console-keyboard provider"; $relation = 'changed'; $shim = 'post-T200 console capability seam' }
            10 { $route = "$prefix -> package-owned explicit-unavailable/deferred console branch"; $relation = 'changed'; $shim = 'post-T200 COMMAND package-session consolidation' }
            11 { $route = "$prefix -> lifecycle provider (currently no full parent-return composition)"; $relation = 'changed'; $shim = 'post-T200 lifecycle provider seam'; $fallback = 'not a completed COMMAND return; selected result is package-local terminal/defer' }
            12 { $route = "$prefix -> bootstrap boot-file service"; $relation = 'changed'; $shim = 'post-T200 boot-input seam' }
            13 { $route = "$prefix -> bootstrap boot-file service"; $relation = 'changed'; $shim = 'post-T200 boot-input seam' }
            14 { $route = "$prefix -> command console-keyboard provider"; $relation = 'changed'; $shim = 'post-T200 console capability seam' }
            15 { $route = "$prefix -> bootstrap initial-environment source-derived seam"; $relation = 'changed'; $shim = 'T217 OpenNT cmdenv-shaped environment seam'; $profile = 'migrate-to-shared-profile-abi' }
            16 { $route = "$prefix -> facade deferred typed stop"; $fallback = 'correct deferred terminal, not a COMMAND provider' }
        }
    } elseif ($selector -eq 0x52) {
        $prefix = 'generic-ud-bridge -> native-bop-composition -> xms-package-session'
        if (In-Set $service @(0,1,2,3,4,5,10,11)) {
            $route = "$prefix -> mantle A20/extended-memory mechanical capability"
            $relation = 'changed'
            $shim = 'post-T200 XMS session expanded to move/page-size/free-query in addition to A20/allocation'
        } else { $route = "$prefix -> typed controlled stop (UMB/INT15 prerequisites unadmitted)"; $fallback = 'explicit non-successful XMS package outcome' }
    } elseif ($selector -eq 0x53) {
        $route = 'generic-ud-bridge -> native-bop-composition -> dpmi-package-session -> typed controlled stop'
        $fallback = 'intentional complete-family deferral; no protected-mode/LDT/VDD composition'
    } elseif ($selector -eq 0x57) {
        $route = 'generic-ud-bridge -> boot-namespace-composition -> redir-package-facade -> CF=1 AX=1 resume +4'
        $fallback = 'reachable source-derived unavailable family; retain until a redirector profile is admitted'
    } elseif ($selector -eq 0x56) {
        $route = 'generic-ud-bridge -> boot-namespace-composition -> debugger-package-facade -> typed controlled stop'
        $fallback = 'intentional non-debug profile deferral'
    } else {
        $route = 'generic-ud-bridge -> boot-namespace-composition -> top-level/machine facade where selected; otherwise unbound to native fallback'
        $fallback = 'selector-specific terminal/idle/config/machine disposition; not a general provider'
    }
    [pscustomobject]@{
        identity = $entry.identity
        handler = $entry.handler
        selector = ('{0:X2}' -f $selector)
        service = if ($null -eq $service) { $null } else { ('{0:X2}' -f $service) }
        originalSource = $entry.source
        ownerPackage = $owner
        currentBoundRoute = $route
        t200Relation = $relation
        laterShim = $shim
        staleOrTerminalFallback = $fallback
        sharedProfileDisposition = $profile
    }
}

if (@($rows).Count -ne 203) { throw "Expected 203 entries; got $(@($rows).Count)." }
$duplicates = @($rows | Group-Object { "$($_.selector):$($_.service):$($_.handler)" } | Where-Object Count -ne 1)
if ($duplicates.Count -ne 0) { throw 'Endpoint identity is not unique.' }
$directory = Split-Path -Parent $OutputPath
if ($directory) { New-Item -ItemType Directory -Force -Path $directory | Out-Null }
$rows | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath $OutputPath -Encoding utf8
Write-Host "Exported $(@($rows).Count) reconciled bound-route entries to $OutputPath"
