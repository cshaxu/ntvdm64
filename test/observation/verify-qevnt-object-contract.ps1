[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$BuildRoot,
    [string]$WorkerBuild = 'build/.ninja/msvc-x86-release/worker'
)
& (Join-Path $PSScriptRoot 'verify-softpc-object-contract.ps1') @PSBoundParameters -Contract qevnt
exit $LASTEXITCODE
