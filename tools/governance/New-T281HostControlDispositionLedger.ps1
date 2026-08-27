[CmdletBinding()]
param([string]$RepositoryRoot = '')

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$inputPath = Join-Path $root 'docs/etc/operations/m0-t276-s12-host-src-disposition-ledger.tsv'
$outputPath = Join-Path $root 'docs/etc/operations/m0-t281-s5-host-control-disposition-ledger.tsv'
$rows = Import-Csv -LiteralPath $inputPath -Delimiter "`t"

$map = @{
    'machine-device-bridge-review' = @('bochs-replacement', 'Bochs device/profile owner')
    'adapter-mvdm-host-out/softpc-machine-facade' = @('adapter-prerequisite', 'adapter-mvdm-host-out/softpc and adapter-bochs mechanical facade')
    'adapter-mvdm-host-out/win32-host-capability' = @('adapter-prerequisite', 'adapter-mvdm-host-out/win32 same-shaped host capability group')
    'adapter-mvdm-host-out/monitor' = @('adapter-prerequisite', 'adapter-mvdm-host-out/monitor plus named session/machine prerequisite')
    'session-and-monitor-thread' = @('adapter-prerequisite', 'session worker and adapter-mvdm-host-out/monitor group')
    'opennt-bop-dispatch' = @('adapter-prerequisite', 'adapter-mvdm-host-in/adapter-mvdm-host-out/softpc plus complete owner provider groups')
    'adapter-mvdm-host-out/vdd-or-debugger' = @('specialist-owner', 'adapter-mvdm-host-out/vdd or adapter-mvdm-host-out/debugger package')
    'literal-build-unobserved-auxiliary' = @('specialist-owner', 'original build/profile owner')
    'cross-owner-launch-configuration' = @('specialist-owner', 'app/session multi-owner startup composition')
    'cross-owner-startup-composition' = @('specialist-owner', 'app/session plus DEM/COMMAND/XMS/VDD composition')
    'cross-owner-video-machine-initialization' = @('specialist-owner', 'app/monitor/Bochs machine initialization composition')
}

$out = foreach ($row in $rows) {
    if (-not $map.ContainsKey($row.original_role_group)) {
        throw "Unmapped host-control role: $($row.original_role_group)"
    }
    $decision = $map[$row.original_role_group]
    [pscustomobject]@{
        source_path = $row.source_path
        original_role_group = $row.original_role_group
        disposition = $decision[0]
        next_owner_group = $decision[1]
        required_boundary_owner = $row.required_boundary_owner
        prior_evidence = $row.evidence
    }
}
if ($out.Count -ne 56 -or ($out | Group-Object source_path | Where-Object Count -gt 1)) {
    throw 'T281 S5 source ownership is not a unique 56-path partition.'
}
$out | Export-Csv -LiteralPath $outputPath -Delimiter "`t" -NoTypeInformation -Encoding utf8
Write-Output "Wrote T281 S5 host-control disposition ledger: $($out.Count) unique source rows."
