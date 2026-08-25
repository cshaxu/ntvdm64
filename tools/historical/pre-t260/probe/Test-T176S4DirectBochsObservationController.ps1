[CmdletBinding()]
param([string]$RepositoryRoot = '')

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$scriptPath = Join-Path $RepositoryRoot 'tools\probe\Invoke-T176S4DirectBochsObservation.ps1'
$tokens = $null
$errors = $null
$ast = [System.Management.Automation.Language.Parser]::ParseFile($scriptPath, [ref]$tokens, [ref]$errors)
if ($null -eq $ast -or $errors.Count -ne 0) { throw 'Controller script does not parse.' }
$text = Get-Content -Raw -LiteralPath $scriptPath
foreach ($forbidden in @('cmd.exe', 'taskkill', 'Start-Process', 'ReadToEnd', 'Get-Process', '[Environment]::SetEnvironmentVariable')) {
    if ($text.IndexOf($forbidden, [StringComparison]::OrdinalIgnoreCase) -ge 0) {
        throw "Forbidden controller surface found: $forbidden"
    }
}
foreach ($automaticVariable in @('PID', 'PSHOME', 'PSVersionTable', 'Host', 'Error', 'Input', 'Matches', 'Args')) {
    if ($text -match ('(?im)^\s*\$' + [regex]::Escape($automaticVariable) + '\s*=')) {
        throw "Controller assigns to PowerShell automatic variable: $automaticVariable"
    }
}
foreach ($required in @(
    '$startInfo.FileName = $bochsPath',
    '$process.WaitForExit($WatchdogSeconds * 1000)',
    '$process.Kill()',
    'RUNNER_ADAPTER_PROFILE',
    'RUNNER_ADAPTER_ROOT',
    'RUNNER_ADAPTER_LAUNCH_KIND',
    'RUNNER_ADAPTER_LAUNCH_TAIL',
    'RUNNER_ADAPTER_BRIDGE',
    '$null = $startInfo.EnvironmentVariables',
    '$startInfo.EnvironmentVariables[$name] = [string]$environmentValues[$name]',
    'Outer timeout must include watchdog plus 15-second evidence-collection grace.'
)) {
    if ($text.IndexOf($required, [StringComparison]::Ordinal) -lt 0) {
        throw "Required direct-PID contract fragment is absent: $required"
    }
}
if ($text -notmatch '(?s)if \(-not \$process\.WaitForExit\(\$WatchdogSeconds \* 1000\)\) \{\s*\$timedOut = \$true\s*\$process\.Kill\(\)') {
    throw 'Exact PID termination is not confined to the timeout branch.'
}
Write-Host 'T176 S4 direct-PID observation controller static contract verified; no executable launched.'
