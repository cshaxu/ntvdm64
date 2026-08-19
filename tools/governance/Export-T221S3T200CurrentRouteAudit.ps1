[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$ReconciliationPath,
    [Parameter(Mandatory = $true)][string]$OutputPath
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$root = [IO.Path]::GetFullPath($RepositoryRoot)

function Read-Required([string]$relative) {
    $path = Join-Path $root $relative
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) {
        throw "Required current-route source missing: $relative"
    }
    Get-Content -LiteralPath $path -Raw
}
function Require-Text([string]$text, [string]$needle, [string]$description) {
    if (-not $text.Contains($needle)) {
        throw "Current source does not prove ${description}: $needle"
    }
}

$bridge = Read-Required 'src/bx-vdm/bx_ntvdm_vdm_generic_ud_bridge_v1.c'
$boot = Read-Required 'src/bx-vdm/bop-v1/bx_ntvdm_boot_namespace_composition_v1.c'
$native = Read-Required 'src/bx-vdm/bop-v1/bx_ntvdm_native_bop_composition_v1.c'
$dem = Read-Required 'src/bx-vdm/bop-v1/bx_ntvdm_dem_package_session_v1.c'
$command = Read-Required 'src/bx-vdm/bop-v1/bx_ntvdm_command_package_session_v1.c'
$xms = Read-Required 'src/bx-vdm/bop-v1/bx_ntvdm_xms_package_session_v1.c'
$dpmi = Read-Required 'src/bx-vdm/bop-v1/bx_ntvdm_dpmi_package_session_v1.c'

Require-Text $bridge 'bx_ntvdm_boot_namespace_composition_v1_handle' 'first boot-composition offer'
Require-Text $bridge 'bx_ntvdm_native_bop_composition_v1_handle' 'native fallback offer'
Require-Text $boot 'bx_ntvdm_dem_package_session_v1_dispatch' 'DEM package binding'
Require-Text $boot 'bx_ntvdm_command_package_session_v1_dispatch' 'COMMAND package binding'
Require-Text $boot 'bx_ntvdm_redir_package_facade_v1_dispatch' 'Redirector unavailable-family binding'
Require-Text $boot 'bx_ntvdm_debugger_package_facade_v1_dispatch' 'Debugger deferred-family binding'
Require-Text $boot 'bx_ntvdm_top_level_package_facade_v1_dispatch' 'top-level/machine binding'
Require-Text $native 'bx_ntvdm_xms_package_session_v1_dispatch' 'XMS package binding'
Require-Text $native 'bx_ntvdm_dpmi_package_session_v1_dispatch' 'DPMI package binding'
Require-Text $dem 'bx_ntvdm_dem_whole_provider_v1_' 'DEM whole-provider seam'
Require-Text $dem 'bx_ntvdm_dem_search_partition_v1_' 'DEM search route'
Require-Text $dem 'bx_ntvdm_dem_namespace_route_partition_v1_' 'DEM namespace route'
Require-Text $dem 'bx_ntvdm_dem_fcb_' 'DEM FCB route'
Require-Text $command 'bx_ntvdm_command_package_session_v1_dispatch' 'COMMAND session dispatcher'
Require-Text $xms 'plane.service==1u' 'XMS move route'
Require-Text $dpmi 'bx_ntvdm_cpu_result_v2_stop' 'DPMI explicit stop outcome'

$decoded = ConvertFrom-Json -InputObject (Get-Content -LiteralPath $ReconciliationPath -Raw)
$rows = @($decoded)
if ($rows.Count -eq 1 -and $decoded -is [array]) { $rows = @($decoded | ForEach-Object { $_ }) }
if ($rows.Count -ne 203) { throw "Expected 203 T200 reconciliation rows; got $($rows.Count)." }
$duplicates = @($rows | Group-Object { "$($_.selector):$($_.service):$($_.handler)" } | Where-Object Count -ne 1)
if ($duplicates.Count -ne 0) { throw 'T200 reconciliation identities are not unique.' }

$outRows = foreach ($row in $rows) {
    $selector = [string]$row.selector
    $proof = switch ($selector) {
        '50' { 'generic UD bridge -> boot composition -> DEM package session; selected whole-provider, special-plane, or explicit fallback leaf' }
        '54' { 'generic UD bridge -> boot composition -> COMMAND package session -> package provider/deferred leaf' }
        '52' { 'generic UD bridge -> native composition -> XMS package session -> mantle capability or explicit stop' }
        '53' { 'generic UD bridge -> native composition -> DPMI package session -> explicit stop' }
        '57' { 'generic UD bridge -> boot composition -> redirector facade -> reachable unavailable result' }
        '56' { 'generic UD bridge -> boot composition -> debugger facade -> typed stop' }
        default { 'generic UD bridge -> boot composition -> top-level/machine facade where selected' }
    }
    $later = [string]$row.laterShim
    $profile = [string]$row.sharedProfileDisposition
    $fallback = [string]$row.staleOrTerminalFallback
    if ($selector -eq '50' -and $row.currentBoundRoute -like '*whole-provider*') {
        $later = if ([string]::IsNullOrWhiteSpace($later)) {
            'T221 selector-blind whole-provider profile/view seam'
        } else { "$later; T221 selector-blind whole-provider profile/view seam" }
    }
    [ordered]@{
        identity = if ($null -eq $row.service) { $selector } else { "$selector`:$($row.service)" }
        handler = $row.handler
        originalSource = $row.originalSource
        ownerPackage = $row.ownerPackage
        currentBoundRoute = $row.currentBoundRoute
        currentSourceProof = $proof
        t200Relation = $row.t200Relation
        laterShim = if ([string]::IsNullOrWhiteSpace($later)) { $null } else { $later }
        staleOrTerminalFallback = if ([string]::IsNullOrWhiteSpace($fallback)) { $null } else { $fallback }
        sharedProfileDisposition = $profile
        auditVerdict = 'static route retained; no compatibility or runtime-success claim'
    }
}

$output = [IO.Path]::GetFullPath($OutputPath)
$directory = Split-Path -Parent $output
if (-not (Test-Path -LiteralPath $directory)) { New-Item -ItemType Directory -Force -Path $directory | Out-Null }
[ordered]@{
    schema = 'ntdos64.t221.s3.t200-current-bound-route-audit.v1'
    generatedUtc = [DateTime]::UtcNow.ToString('o')
    staticOnly = $true
    sourceCount = $outRows.Count
    sourceInputs = @(
        'src/bx-vdm/bx_ntvdm_vdm_generic_ud_bridge_v1.c',
        'src/bx-vdm/bop-v1/bx_ntvdm_boot_namespace_composition_v1.c',
        'src/bx-vdm/bop-v1/bx_ntvdm_native_bop_composition_v1.c',
        'src/bx-vdm/bop-v1/bx_ntvdm_dem_package_session_v1.c',
        'src/bx-vdm/bop-v1/bx_ntvdm_command_package_session_v1.c',
        'src/bx-vdm/bop-v1/bx_ntvdm_xms_package_session_v1.c',
        'src/bx-vdm/bop-v1/bx_ntvdm_dpmi_package_session_v1.c')
    entries = $outRows
} | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath $output -Encoding utf8
Write-Host "Exported $($outRows.Count) current T200 bound-route audit rows to $output"
