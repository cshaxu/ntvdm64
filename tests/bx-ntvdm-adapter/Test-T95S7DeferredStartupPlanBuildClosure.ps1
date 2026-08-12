$ErrorActionPreference = 'Stop'

$repository = Split-Path -Parent (Split-Path $PSScriptRoot)
$generator = Get-Content (Join-Path $repository 'tools\New-T95S7RuntimeTraceBuildRoot.ps1') -Raw
foreach ($required in @(
        '[switch]$DeferredStartupPlan',
        '$executionPlanMacro = if ($DeferredStartupPlan) { 0 } else { 1 }',
        '$deferredPlanMacro = if ($DeferredStartupPlan) { 1 } else { 0 }',
        'BX_NTVDM_ENABLE_DEFERRED_STARTUP_PLAN={0}',
        'BX_NTVDM_ENABLE_EXECUTION_PLAN={0}',
        'bx_ntvdm_adapter_runtime.c',
        'bx_ntvdm_startup_plan_abi.c',
        'byob_profile.c')) {
    if (-not $generator.Contains($required)) {
        throw "Deferred startup build generator lacks required closure term: $required"
    }
}
if ($generator -match 'optromimage|machine_startup_handoff') {
    throw 'Build-root generator incorrectly owns composition input or profile semantics.'
}
Write-Output 'Deferred startup-plan build closure verification passed.'
