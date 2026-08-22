[CmdletBinding()]
param([string]$RepositoryRoot = '')

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
}

function Require([bool]$Condition, [string]$Message) {
    if (!$Condition) { throw $Message }
}

$manifestPath = Join-Path $RepositoryRoot 'tools\build\t225-s7-full-module-manifest.json'
$manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
$mantle = @($manifest.modules | Where-Object { $_.name -eq 'bx-mantle' })
$core = @($manifest.modules | Where-Object { $_.name -eq 'bx-core' })
$vdm = @($manifest.modules | Where-Object { $_.name -eq 'bx-vdm' })
Require ($mantle.Count -eq 1 -and $core.Count -eq 1 -and $vdm.Count -eq 1) 'Formal manifest module identities are incomplete.'

$mantleSources = @($mantle[0].sources)
$coreSources = @($core[0].sources)
$vdmSources = @($vdm[0].sources)
foreach ($forbidden in @(
    'src/bx-mantle/bx_ntvdm_engine_contract_v1.c',
    'src/bx-mantle/bx_ntvdm_engine_run_v1.c',
    'src/bx-mantle/bx_ntvdm_extended_memory_v1.cc')) {
    Require ($mantleSources -notcontains $forbidden) "Formal mantle closure retains retired composition input: $forbidden"
}
foreach ($required in @(
    'src/bx-vdm/bx_ntvdm_engine_contract_v1.c',
    'src/bx-vdm/bx_ntvdm_engine_run_v1.c')) {
    Require ($vdmSources -contains $required) "Formal bx-vdm closure omits composition input: $required"
}

$forbiddenPattern = '(?i)\bbop\b|dem_v2|command_v2|xms_|\bumb\b|\bcomspec\b|\bntdos\b'
foreach ($relative in @($mantleSources + $coreSources)) {
    $path = Join-Path $RepositoryRoot $relative
    Require (Test-Path -LiteralPath $path -PathType Leaf) "Formal machine source is missing: $relative"
    $match = Select-String -LiteralPath $path -Pattern $forbiddenPattern
    Require ($null -eq $match) "Machine source contains VDM/DOS service vocabulary: $relative"
}

[ordered]@{
    schema = 'ntdos64.t237.s4.machine-semantic-boundary.v1'
    formalMantleCompositionInputs = $false
    formalMantleAllocatorPolicy = $false
    formalVdmOwnsEngineComposition = $true
    coreAndMantleContainBopOrServiceVocabulary = $false
    result = 'pass'
} | ConvertTo-Json -Depth 4
