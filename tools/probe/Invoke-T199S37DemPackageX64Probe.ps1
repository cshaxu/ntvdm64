param(
    [string]$RepositoryRoot = '',
    [string]$BuildRoot = '',
    [ValidateSet('x64')]
    [string]$HostArchitecture = 'x64'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

& (Join-Path $PSScriptRoot 'Invoke-T198S16BootNamespaceVdmX64Probe.ps1') `
    -RepositoryRoot $RepositoryRoot `
    -BuildRoot $BuildRoot `
    -Fixture dem-package `
    -HostArchitecture $HostArchitecture
exit $LASTEXITCODE
