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
        throw "Required T221 source/audit input is missing: $relative"
    }
    return Get-Content -LiteralPath $path -Raw
}
function Require-Text([string]$text, [string]$needle, [string]$description) {
    if (-not $text.Contains($needle)) {
        throw "Current source no longer proves $description ($needle)"
    }
}

$reconciliationPath = 'docs/etc/research/t219-s2-t200-bound-route-reconciliation-001.json'
$reconciliationText = Get-Content -LiteralPath (Join-Path $root $reconciliationPath) -Raw
$decodedReconciliation = ConvertFrom-Json -InputObject $reconciliationText
# Windows PowerShell 5.1 emits a top-level JSON array as one pipeline object,
# while newer PowerShell versions enumerate it.  Flatten explicitly so the
# audit is reproducible on the repository's ordinary Windows host.
$reconciliation = @($decodedReconciliation)
if ($reconciliation.Count -eq 1 -and $decodedReconciliation -is [array]) {
    $reconciliation = @($decodedReconciliation | ForEach-Object { $_ })
}
$rows = @($reconciliation | Where-Object { $_.selector -eq '50' })
if ($rows.Count -ne 73) { throw "T219 DEM reconciliation drift: expected 73 rows, got $($rows.Count)" }
$duplicates = @($rows | Group-Object service | Where-Object { $_.Count -ne 1 })
if ($duplicates.Count -ne 0) { throw 'T219 DEM reconciliation has duplicate service rows' }
$missing = @(0..72 | Where-Object { ('{0:X2}' -f $_) -notin $rows.service })
if ($missing.Count -ne 0) { throw "T219 DEM reconciliation has missing services: $($missing -join ',')" }

$session = Read-Required 'src/bx-vdm/bop-v1/bx_ntvdm_dem_package_session_v1.c'
$facade = Read-Required 'src/bx-vdm/bop-v1/bx_ntvdm_dem_package_facade_v1.c'
$whole = Read-Required 'src/bx-vdm/bop-v1/bx_ntvdm_dem_whole_provider_v1.c'
$plane = Read-Required 'src/bx-vdm/bop-v1/bx_ntvdm_dem_plane_v1.c'
$genericBridge = Read-Required 'src/bx-vdm/bop-v1/bx_ntvdm_vdm_generic_ud_bridge_v1.c'
$composition = Read-Required 'src/bx-vdm/bop-v1/bx_ntvdm_boot_namespace_composition_v1.c'

Require-Text $genericBridge 'boot_namespace_composition' 'generic UD bridge to boot namespace composition'
Require-Text $composition 'bx_ntvdm_dem_package_session_v1_dispatch' 'composition dispatch to DEM package session'
Require-Text $facade 'bx_ntvdm_dem_provider_v1_dispatch' 'original-no-op facade route'
Require-Text $whole 'bx_ntvdm_dem_whole_provider_v1_owns_service' 'whole-provider membership guard'
Require-Text $plane 'bx_ntvdm_dem_plane_v1_component' 'DEM owner-plane classifier'

$routeProofs = [ordered]@{
    'DEM GSET/CWD/drive-or-volume source-derived providers, conditional on snapshots/session context' = @(
        'bx_ntvdm_dem_gset_plane_v1_dispatch',
        'bx_ntvdm_dem_media_id_service_v1_prepare',
        'bx_ntvdm_dem_full_dpb_service_v1_prepare')
    'dem-misc-plane -> source-derived resume/no-op or named debug/system-symbol helper' = @(
        'bx_ntvdm_dem_misc_plane_v1_dispatch')
    'dem-package-facade -> dem-provider original-noop' = @(
        'bx_ntvdm_dem_package_facade_v1_dispatch')
    'dem-session-lifecycle-provider -> finite boot-namespace termination only' = @(
        'bx_ntvdm_dem_session_lifecycle_provider_v1_dispatch')
    'dem-whole-provider FCB direct/I-O partition when DTA prerequisites hold' = @(
        'fcb_direct_dispatch', 'fcb_io_dispatch')
    'dem-whole-provider FCB path partition (when installed)' = @(
        'fcb_path_dispatch')
    'dem-whole-provider handle partition (when installed and token/precondition matches)' = @(
        'handle_dispatch')
    'dem-whole-provider merged host/declared search partition (when installed)' = @(
        'search_dispatch')
    'dem-whole-provider namespace partition (when installed)' = @(
        'namespace_dispatch')
    'ordered DEM special-plane chain -> boot-namespace plane or cli-unavailable' = @(
        'bx_ntvdm_dem_readonly_namespace_failure_provider_v1_dispatch',
        'bx_ntvdm_boot_namespace_plane_v1_dispatch',
        'bx_ntvdm_dem_cli_unavailable_provider_v1_dispatch')
}

$outRows = foreach ($row in ($rows | Sort-Object { [Convert]::ToInt32($_.service, 16) })) {
    $suffix = $null
    foreach ($key in $routeProofs.Keys) {
        if ($row.currentBoundRoute.EndsWith($key)) { $suffix = $key; break }
    }
    if ($null -eq $suffix) { throw "No current-source proof category for DEM 50:$($row.service)" }
    $proofs = @($routeProofs[$suffix])
    foreach ($proof in $proofs) { Require-Text $session $proof "DEM 50:$($row.service) route category" }
    [ordered]@{
        identity = ('50:{0}' -f $row.service)
        originalHandler = $row.handler
        originalDispatcher = $row.originalSource
        ownerPackage = $row.ownerPackage
        currentBoundRoute = $row.currentBoundRoute
        currentSourceProof = @{
            staticChain = 'generic-ud bridge -> boot namespace composition -> dem package session'
            sessionSymbols = $proofs
            status = 'verified against current source text; not runtime execution evidence'
        }
        t219Relation = $row.t200Relation
        laterShim = $row.laterShim
        temporaryFallback = $row.staleOrTerminalFallback
        sharedProfileDisposition = $row.sharedProfileDisposition
        t221Disposition = 'ABI/failure and source-recovery rung mapping pending in T221 S1 P2; no provider completeness inferred'
    }
}

$output = [IO.Path]::GetFullPath($OutputPath)
$parent = Split-Path -Parent $output
if (-not (Test-Path -LiteralPath $parent)) { New-Item -ItemType Directory -Path $parent | Out-Null }
[ordered]@{
    schema = 'ntdos64.t221.s1.dem-bound-route-audit.v1'
    generatedUtc = [DateTime]::UtcNow.ToString('o')
    sourceInputs = @(
        $reconciliationPath,
        'src/bx-vdm/bop-v1/bx_ntvdm_vdm_generic_ud_bridge_v1.c',
        'src/bx-vdm/bop-v1/bx_ntvdm_boot_namespace_composition_v1.c',
        'src/bx-vdm/bop-v1/bx_ntvdm_dem_package_session_v1.c',
        'src/bx-vdm/bop-v1/bx_ntvdm_dem_package_facade_v1.c',
        'src/bx-vdm/bop-v1/bx_ntvdm_dem_whole_provider_v1.c',
        'src/bx-vdm/bop-v1/bx_ntvdm_dem_plane_v1.c')
    serviceCount = $outRows.Count
    staticOnly = $true
    entries = $outRows
} | ConvertTo-Json -Depth 7 | Set-Content -LiteralPath $output -Encoding utf8
Write-Host "Exported $($outRows.Count) current DEM route rows to $output"
