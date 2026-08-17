$ErrorActionPreference = 'Stop'
$repositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$header = Get-Content -LiteralPath (Join-Path $repositoryRoot 'src/bx-mantle/bx_ntvdm_port_action_v1.h') -Raw
$source = Get-Content -LiteralPath (Join-Path $repositoryRoot 'src/bx-mantle/bx_ntvdm_port_action_v1.cc') -Raw
foreach ($required in @('BX_NTVDM_PORT_ACTION_V1_READ8','BX_NTVDM_PORT_ACTION_V1_WRITE8','bx_devices.inp','bx_devices.outp','bx_ntvdm_port_action_v1_lifecycle_active')) {
  if ($header.IndexOf($required,[StringComparison]::OrdinalIgnoreCase) -lt 0 -and $source.IndexOf($required,[StringComparison]::OrdinalIgnoreCase) -lt 0) { throw "Missing BX-MANTLE-083 invariant: $required" }
}
$code=[regex]::Replace($source,'(?s)/\*.*?\*/|//.*?(\r?\n|$)','')
foreach ($forbidden in @('BOP','OpenNT','DOS','WOW','DEM','selector','callback','plugin','firmware','void *','BX_CPU')) {
  if ($code.IndexOf($forbidden,[StringComparison]::OrdinalIgnoreCase) -ge 0) { throw "Forbidden BX-MANTLE-083 dependency: $forbidden" }
}
Write-Output 't225-s23-byte-port-action-boundary: source invariants verified'