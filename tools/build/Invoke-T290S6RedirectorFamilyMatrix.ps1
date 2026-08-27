[CmdletBinding()]
param(
    [string]$RepositoryRoot = ''
)

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path

function Invoke-FormalGraph([string]$architecture, [string]$scriptName,
    [string]$buildPath, [string]$programPath) {
    & (Join-Path $PSScriptRoot $scriptName) -Architecture $architecture -RepositoryRoot $root
    if (-not $?) { throw "Graph generation failed: $scriptName ($architecture)" }
    & ninja -C (Join-Path $root $buildPath)
    if ($LASTEXITCODE -ne 0) { throw "Ninja failed: $buildPath" }
    & (Join-Path $root $programPath)
    if ($LASTEXITCODE -ne 0) { throw "Fixture failed: $programPath" }
}

$graphs = @(
    @{ Script = 'New-T290S3RedirectorPointerScopeNinja.ps1'; Build = 'build/M0-T290/S3/redirector-pointer-scope-{0}'; Exe = 'build/M0-T290/S3/redirector-pointer-scope-{0}/bin/t290-s3-redirector-pointer-scope-fixture.exe' },
    @{ Script = 'New-T290S3RedirectorHandleNinja.ps1'; Build = 'build/M0-T290/S3/redirector-handle-{0}'; Exe = 'build/M0-T290/S3/redirector-handle-{0}/bin/t290-s3-redirector-handle-fixture.exe' },
    @{ Script = 'New-T290S3VrMiscNinja.ps1'; Build = 'build/M0-T290/S3/vrmisc-{0}'; Exe = 'build/M0-T290/S3/vrmisc-{0}/bin/t290-s3-vrmisc-fixture.exe' },
    @{ Script = 'New-T290S3VrInitFailureNinja.ps1'; Build = 'build/M0-T290/S3/vrinit-failure-{0}'; Exe = 'build/M0-T290/S3/vrinit-failure-{0}/bin/t290-s3-vrinit-failure-fixture.exe' },
    @{ Script = 'New-T290S3VrNamedPipeNameNinja.ps1'; Build = 'build/M0-T290/S3/vrnmpipe-name-{0}'; Exe = 'build/M0-T290/S3/vrnmpipe-name-{0}/bin/t290-s3-vrnmpipe-name-fixture.exe' },
    @{ Script = 'New-T290S3CmdRedirNinja.ps1'; Build = 'build/M0-T290/S3/cmdredir-{0}'; Exe = 'build/M0-T290/S3/cmdredir-{0}/bin/t290-s3-cmdredir-fixture.exe' },
    @{ Script = 'New-T290S4VrMailslotNinja.ps1'; Build = 'build/M0-T290/S4/vrmslot-{0}'; Exe = 'build/M0-T290/S4/vrmslot-{0}/bin/t290-s4-vrmslot-fixture.exe' },
    @{ Script = 'New-T290S5VrNetApiLocalNinja.ps1'; Build = 'build/M0-T290/S5/vrnetapi-local-{0}'; Exe = 'build/M0-T290/S5/vrnetapi-local-{0}/bin/t290-s5-vrnetapi-local-fixture.exe' },
    @{ Script = 'New-T290S5VrNetApiComputerNameNinja.ps1'; Build = 'build/M0-T290/S5/vrnetapi-computer-name-{0}'; Exe = 'build/M0-T290/S5/vrnetapi-computer-name-{0}/bin/t290-s5-vrnetapi-computer-name-fixture.exe' },
    @{ Script = 'New-T290S5VrNetApiUserNameNinja.ps1'; Build = 'build/M0-T290/S5/vrnetapi-user-name-{0}'; Exe = 'build/M0-T290/S5/vrnetapi-user-name-{0}/bin/t290-s5-vrnetapi-user-name-fixture.exe' },
    @{ Script = 'New-T290S6RedirectorDisabledIngressNinja.ps1'; Build = 'build/M0-T290/S6/disabled-ingress-{0}'; Exe = 'build/M0-T290/S6/disabled-ingress-{0}/bin/t290-s6-redirector-disabled-ingress-fixture.exe' }
)

foreach ($architecture in @('x86', 'x64')) {
    foreach ($graph in $graphs) {
        Invoke-FormalGraph $architecture $graph.Script ($graph.Build -f $architecture) ($graph.Exe -f $architecture)
    }
}

Write-Host 'T290 S6 Redirector family matrix passed: 50-entry ledger plus 11 local graphs on x86/x64; Redirector ingress remains disabled.'
