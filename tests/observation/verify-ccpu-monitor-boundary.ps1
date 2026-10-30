[CmdletBinding()]
param([Parameter(Mandatory)][string]$WorkerBuild)
$ErrorActionPreference = 'Stop'
$image = Join-Path (Resolve-Path -LiteralPath $WorkerBuild).Path 'ccpu-monitor-boundary-test.exe'
if (!(Test-Path -LiteralPath $image -PathType Leaf)) { throw "Missing fixture: $image" }
$cases = @(0..7) + @(10..61)
foreach ($case in $cases) {
    & $image $case
    if ($LASTEXITCODE -ne 0) { throw "CCPU monitor case $case failed: $LASTEXITCODE" }
}
Write-Host "CCPU monitor boundary: $($cases.Count) cases passed"
