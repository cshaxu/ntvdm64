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

$retired = @(
    'config_done_service', 'host_volume_snapshot', 'mutation_overlay',
    'mutation_profile', 'profile_search_snapshot', 'readonly_namespace',
    'search_plan', 'search_request', 'search_result', 'search_session',
    'search_transaction', 'startup_configuration_policy',
    'startup_configuration_provider', 'startup_configuration_source'
)
$roots = @('src', 'tests', 'tools')
foreach ($name in $retired) {
    $hits = @(rg -l "bx_ntvdm_$name" $roots --glob '*.[ch]' --glob '*.cc' --glob '*.ps1' --glob '*.json')
    Require ($hits.Count -eq 0) "Retired DEM/COMMAND v1 input remains: $name"
}

$legacyRouteHits = @(rg -l -i 'bop-v1.*(dem|command|cmdinfo)|(dem|command|cmdinfo).*bop-v1' $roots --glob '*.[ch]' --glob '*.cc' --glob '*.ps1' --glob '*.json' --glob '!tools/governance/Test-T236S7V1Retirement.ps1')
Require ($legacyRouteHits.Count -eq 0) 'A DEM/COMMAND v1 route remains below src/, tests/, or tools/.'

$manifestPath = Join-Path $RepositoryRoot 'tools/build/t225-s7-full-module-manifest.json'
$manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
$bxVdm = @($manifest.modules | Where-Object { $_.name -eq 'bx-vdm' })
Require ($bxVdm.Count -eq 1) 'Expected one formal bx-vdm module.'
Require ((@($bxVdm[0].sources) -join [Environment]::NewLine) -notmatch 'bop-v1') 'Formal bx-vdm module retains a bop-v1 source.'

Write-Host 'T236 S7 DEM/COMMAND v1 retirement gate passed.'
