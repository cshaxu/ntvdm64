[CmdletBinding()]
param([Parameter(Mandatory)][string]$WorkerBuild)
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $WorkerBuild).Path
foreach ($case in @(
    @{ image = 'debug-register-binding-test.exe'; arguments = @() },
    @{ image = 'ccpu-halt-reset-test.exe'; arguments = @() }
)) {
    $image = Join-Path $root $case.image
    if (!(Test-Path -LiteralPath $image -PathType Leaf)) { throw "Missing fixture: $image" }
    & $image @($case.arguments)
    if ($LASTEXITCODE -ne 0) { throw "Debug context fixture failed: $image $($case.arguments) ($LASTEXITCODE)" }
}
Write-Host 'CCPU debug context fixtures passed'
