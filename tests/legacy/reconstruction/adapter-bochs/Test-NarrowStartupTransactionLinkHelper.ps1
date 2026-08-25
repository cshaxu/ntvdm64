$ErrorActionPreference = 'Stop'
$repository = Split-Path -Parent (Split-Path $PSScriptRoot)
$tool = Get-Content (Join-Path $repository 'tools\probe\Invoke-T95S6NarrowStartupTransactionLink.ps1') -Raw
foreach ($required in @(
        'narrow-startup-transaction-build-root.json',
        'nmake.exe /f runner-native-startup-transaction.mak runner-native-startup-transaction.exe',
        'setlocal EnableExtensions EnableDelayedExpansion',
        'set "RUNNER_EXITCODE=!ERRORLEVEL!"',
        'EnableUnmatchedUdDiagnostic',
        'unmatchedUdDiagnosticEnabled',
        'diagnostic opt-in does not match its build-root record',
        'EnableBopCatalogListener',
        'bopCatalogListenerEnabled',
        'BOP listener opt-in does not match its build-root record',
        'EnableCpuResultBridge',
        'cpuResultBridgeEnabled',
        'CPU-result bridge opt-in does not match its build-root record',
        'Refusing to overwrite an existing narrow startup link record.',
        'process/recorded exit mismatch')) {
    if (-not $tool.Contains($required)) { throw "Missing narrow link helper control: $required" }
}
foreach ($forbidden in @('bochs.exe', 'configure', 'devenv', 'msbuild', 'nmake.exe all')) {
    if ($tool.IndexOf($forbidden, [StringComparison]::OrdinalIgnoreCase) -ge 0) {
        throw "Forbidden narrow link helper token: $forbidden"
    }
}
Write-Output 'Narrow startup-transaction link helper verification passed.'
