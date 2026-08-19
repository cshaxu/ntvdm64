[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
function Read-Required([string]$relative) {
    $path = Join-Path $root $relative
    if (!(Test-Path -LiteralPath $path -PathType Leaf)) {
        throw "Missing required input: $relative"
    }
    return Get-Content -LiteralPath $path -Raw
}
function Require([bool]$condition, [string]$message) {
    if (!$condition) { throw $message }
}

$formal = Read-Required 'tools/build/t225-s7-full-module-manifest.json'
$entry = Read-Required 'src/bx-vdm/bop/dem_v2_composition_bridge.c'
$bridge = Read-Required 'src/bx-vdm/bop/dem_v2_generic_ud_bridge.c'
$engine = Read-Required 'src/bx-mantle/bx_ntvdm_engine_run_v1.c'
$legacy = Read-Required 'src/bx-vdm/bop-v1/bx_ntvdm_vdm_generic_ud_bridge_v1.c'

Require ($formal -notmatch 'bop-v1') 'Formal module manifest still admits bop-v1.'
Require ($entry -notmatch '#include\s+"[^"]*bop-v1/') 'v2 generic composition entry includes a bop-v1 input.'
Require (($entry -match 'bx_ntvdm_dem_v2_generic_ud_recognizes') -and ($entry -match 'bx_ntvdm_dem_v2_generic_ud_dispatch')) 'v2 composition entry does not own DEM recognition and dispatch.'
Require (($bridge -match 'C4') -and ($bridge -match '0x50u')) 'v2 bridge no longer recognizes the bounded DEM encoding.'
Require (($engine -match 'bx_ntvdm_dem_v2_runtime_session_bind\(\)') -and ($engine -match 'bx_ntvdm_dem_v2_runtime_session_reset\(\)')) 'Engine does not bind/reset the v2 Direct DEM session.'
Require ($legacy -match 'bx_ntvdm_mantle_generic_ud_bridge_v1') 'Historical v1 bridge identity changed; its exclusion must be re-audited.'

[ordered]@{
    schema = 'ntdos64.t230.s15.v2-dem-route-precedence.v1'
    formalManifestAdmitsBopV1 = $false
    v2EntryMentionsBopV1 = $false
    engineBindsV2Session = $true
    historicalV1BridgeRetainedOutsideFormalGraph = $true
    result = 'pass'
} | ConvertTo-Json -Depth 4
