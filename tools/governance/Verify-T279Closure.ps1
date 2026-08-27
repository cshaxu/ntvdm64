[CmdletBinding()]
param([string]$RepositoryRoot = '')

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path

& (Join-Path $root 'tools/governance/Verify-T278S2PhysicalMirrorMove.ps1') -RepositoryRoot $root
& (Join-Path $root 'tools/governance/Verify-T279S1SupportLibraryInterfaceLedger.ps1') -RepositoryRoot $root
& (Join-Path $root 'tools/governance/Verify-T279S2AdapterWin32Binding.ps1') -RepositoryRoot $root
& (Join-Path $root 'tools/governance/Verify-T279S3SupportLibraryBuild.ps1') -RepositoryRoot $root -RequireArtifacts

$ledger = @(Import-Csv (Join-Path $root 'docs/etc/operations/m0-t278-destination-source-identity-ledger.tsv') -Delimiter "`t" |
    Where-Object destination_component -eq 'mvdm-support')
if ($ledger.Count -ne 112) { throw "Expected 112 exact support paths; found $($ledger.Count)." }

$generator = Get-Content -LiteralPath (Join-Path $root 'tools/build/New-T279SupportLibraryNinja.ps1') -Raw
foreach ($forbidden in @('mvdm-host', 'adapter-mvdm-host-in', 'adapter-mvdm-host-out/softpc', 'adapter-bochs', 'session/', 'app/', 'bochs-core')) {
    if ($generator -match [regex]::Escape($forbidden)) {
        throw "Support-library build generator imports forbidden runtime input: $forbidden"
    }
}

$closure = Join-Path $root 'docs/history/m0-t279-closure-20260826.md'
if (-not (Test-Path -LiteralPath $closure -PathType Leaf)) { throw 'Missing T279 closure record.' }
Write-Host 'PASS: T279 closure: 112 exact support paths; original x86/x64 static libraries; four x64 overlays; no provider route.'
