[CmdletBinding()]
param([string]$RepositoryRoot = '')

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent (Split-Path $MyInvocation.MyCommand.Path))
}
$source = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'tools\Invoke-T188S10CopiedStateObservation.ps1') -Raw
foreach ($required in @(
    "retryPolicy = 'none'",
    "if (Test-Path -LiteralPath `$evidence) { throw",
    'taskkill.exe /T /F /PID $process.Id',
    'if (-not $process.WaitForExit(5000)) { $cleanupWaitTimedOut = $true }',
    '[IO.File]::WriteAllText($outcomePath, ($record | ConvertTo-Json -Depth 5)',
    'treeKillOutput = $treeKillOutput',
    'cleanupWaitTimedOut = $cleanupWaitTimedOut'
)) {
    if (-not $source.Contains($required)) { throw "Missing controller boundary term: $required" }
}
foreach ($forbidden in @('WaitForExit()', 'Start-Sleep', 'for (;;)', 'while ($true)', 'Invoke-Expression')) {
    if ($source.Contains($forbidden)) { throw "Forbidden unbounded/retry controller term: $forbidden" }
}
$finallyStart = $source.IndexOf('} finally {')
$write = $source.IndexOf('[IO.File]::WriteAllText($outcomePath', $finallyStart)
if ($finallyStart -lt 0 -or $write -lt $finallyStart) { throw 'Terminal observation record is not in finally.' }
$bundle = Join-Path $RepositoryRoot 'artifacts\build\current\t188-copied-state-bundle-r1'
$manifest = Join-Path $bundle 'ntdos64-bochs-engine-bundle-v1.txt'
if (-not (Test-Path -LiteralPath $manifest -PathType Leaf)) { throw 'Missing T188 fixed observation bundle manifest.' }
$manifestText = Get-Content -LiteralPath $manifest -Raw
if (-not $manifestText.Contains('engine-sha256=766D3A7AAB980B05C683DD99530801F61578F9F886DD0A1122BF810D6EA8E938')) {
    throw 'T188 bundle does not contain the S8 frozen engine.'
}
foreach ($asset in @('ntdos64-bochs.exe', 'ROM\BIOS-bochs-latest', 'ROM\VGABIOS-lgpl-latest', 'ROM\ntdos64-post-handoff.rom')) {
    if (-not (Test-Path -LiteralPath (Join-Path $bundle $asset) -PathType Leaf)) { throw "Missing fixed bundle asset: $asset" }
}
Write-Output 'T188 S9 observation-controller boundary verification passed.'
