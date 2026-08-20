[CmdletBinding()]
param([string]$RepositoryRoot = '')

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
}

function Read-Required([string]$RelativePath) {
    $path = Join-Path $RepositoryRoot $RelativePath
    if (!(Test-Path -LiteralPath $path -PathType Leaf)) {
        throw "Missing required file: $RelativePath"
    }
    return Get-Content -LiteralPath $path -Raw
}
function Require([bool]$Condition, [string]$Message) {
    if (!$Condition) { throw $Message }
}

$manifest = Read-Required 'tools/build/t225-s7-full-module-manifest.json' | ConvertFrom-Json
$bxVdm = @($manifest.modules | Where-Object { $_.name -eq 'bx-vdm' })
Require ($bxVdm.Count -eq 1) 'Expected one bx-vdm formal module.'
foreach ($source in @(
    'src/bx-vdm/bop/opennt/command/cmddisp.c',
    'src/bx-vdm/bop/command_v2_generic_ud_bridge.c',
    'src/bx-vdm/bop/command_v2_runtime_session.c',
    'src/bx-vdm/bop/shim/command_native_session_shim.c')) {
    Require (@($bxVdm[0].sources) -contains $source) "Formal bx-vdm module omits COMMAND v2 source: $source"
}
Require ((@($bxVdm[0].sources) -join "`n") -notmatch 'bop-v1') 'Formal bx-vdm module retains a bop-v1 source.'

$composition = Read-Required 'src/bx-vdm/bop/dem_v2_composition_bridge.c'
$engine = Read-Required 'src/bx-mantle/bx_ntvdm_engine_run_v1.c'
$dispatcher = Read-Required 'src/bx-vdm/bop/opennt/command/cmddisp.c'
$shim = Read-Required 'src/bx-vdm/bop/shim/command_misc_shim.c'
Require ($composition -match 'bx_ntvdm_command_v2_generic_ud_recognizes' -and
    $composition -match 'bx_ntvdm_command_v2_generic_ud_dispatch') 'Composition does not own the COMMAND v2 route.'
Require ($engine -match 'bx_ntvdm_command_v2_runtime_session_bind_from_startup\(\)' -and
    $engine -match 'bx_ntvdm_command_v2_runtime_session_reset\(\)') 'Engine does not bind/reset COMMAND v2 runtime state.'
Require ($dispatcher -match 'PFNSVC\s+apfnSVCCmd\s*\[\]' -and
    $dispatcher -match 'cmdExitVDM' -and $dispatcher -match 'cmdGetStartInfo') 'Original CMD dispatcher table is absent.'
Require ($shim -match 'CmdDispatch\(call->service\)' -and
    $shim -notmatch 'else if \(call->service == BX_NTVDM_COMMAND_MISC_GET_NEXT\)') 'Shim retains a hand-written COMMAND service route.'
Write-Host 'T231 S10 COMMAND v2 route precedence and original dispatcher table verified.'
