[CmdletBinding()]
param([Parameter(Mandatory)][string]$WorkerBuild)
$ErrorActionPreference = 'Stop'
$image = Join-Path (Resolve-Path -LiteralPath $WorkerBuild).Path 'ccpu-monitor-boundary-test.exe'
if (!(Test-Path -LiteralPath $image -PathType Leaf)) { throw "Missing fixture: $image" }
$cases = @(0..7) + @(10..106) + @(109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,135,136,137,138,139,140,141,142,143,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212)
foreach ($case in $cases) {
    & $image $case
    if ($LASTEXITCODE -ne 0) { throw "CCPU monitor case $case failed: $LASTEXITCODE" }
}
Write-Host "CCPU monitor boundary: $($cases.Count) cases passed"
