[CmdletBinding()]
param([Parameter(Mandatory)][string]$WorkerBuild)
$ErrorActionPreference = 'Stop'
$image = Join-Path (Resolve-Path -LiteralPath $WorkerBuild).Path 'ccpu-monitor-boundary-test.exe'
if (!(Test-Path -LiteralPath $image -PathType Leaf)) { throw "Missing fixture: $image" }
$cases = @(0..7) + @(10..106) + @(109..120) + @(148,149,150,151,152,153,154,155,156,160,161,162,163,164,165,166,167)
foreach ($case in $cases) {
    & $image $case
    if ($LASTEXITCODE -ne 0) { throw "CCPU monitor case $case failed: $LASTEXITCODE" }
}
Write-Host "CCPU monitor boundary: $($cases.Count) cases passed"
