[CmdletBinding()]
param([string]$RepositoryRoot = '')

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path $PSScriptRoot)
}
$source = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'tools\probe\Invoke-T194S10V6BoundedObservation.ps1') -Raw
foreach ($required in @(
    "retryPolicy = 'none'",
    'taskkill.exe /T /F /PID $process.Id',
    'if (-not $process.WaitForExit(5000)) { $cleanupWaitTimedOut = $true }',
    'treeKillOutput = $treeKillOutput',
    'cleanupWaitTimedOut = $cleanupWaitTimedOut',
    'function Try-Hash([string]$Path)',
    'catch { return $null }',
    'stdoutSha256 = Try-Hash $stdoutPath',
    'stderrSha256 = Try-Hash $stderrPath',
    '[IO.File]::WriteAllText($outcomePath, ($record | ConvertTo-Json -Depth 5)'
)) {
    if (-not $source.Contains($required)) { throw "Missing T194 controller boundary term: $required" }
}
foreach ($forbidden in @('WaitForExit()', 'Start-Sleep', 'for (;;)', 'while ($true)', 'Invoke-Expression')) {
    if ($source.Contains($forbidden)) { throw "Forbidden unbounded/retry controller term: $forbidden" }
}
$finallyStart = $source.IndexOf('finally {')
$write = $source.IndexOf('[IO.File]::WriteAllText($outcomePath', $finallyStart)
if ($finallyStart -lt 0 -or $write -lt $finallyStart) {
    throw 'Terminal observation record is not in finally.'
}
Write-Output 'T194 S34 observation-controller boundary verification passed.'
