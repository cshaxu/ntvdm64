[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$NativeExecutable,
    [Parameter(Mandatory = $true)][string]$InputRoot,
    [Parameter(Mandatory = $true)][string]$RunRoot,
    [ValidateRange(1, 120)][int]$WatchdogSeconds = 30,
    [ValidateRange(1, 100000000)][uint64]$InstructionTickBudget = 1000000,
    [switch]$ObserveBudgetTerminalPosition
)
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
function Hash([string]$Path) { return (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant() }
$exe = (Resolve-Path -LiteralPath $NativeExecutable).Path
$input = (Resolve-Path -LiteralPath $InputRoot).Path
$run = [IO.Path]::GetFullPath($RunRoot)
if (Test-Path -LiteralPath $run) { throw "Refusing to overwrite T228 run root: $run" }
foreach ($file in @('profile.json','TARGET.EXE','fixture-config.nt','fixture-autoexec.nt','guest-exec-lifecycle-ledger-inputs-manifest.json')) {
    if (!(Test-Path -LiteralPath (Join-Path $input $file) -PathType Leaf)) { throw "Required T228 input missing: $file" }
}
New-Item -ItemType Directory -Path $run | Out-Null
$records = @()
foreach ($mode in @('direct','readonly')) {
    $stdout = Join-Path $run ($mode + '.stdout.log')
    $stderr = Join-Path $run ($mode + '.stderr.log')
    $argumentVector = @('--byob-profile',(Join-Path $input 'profile.json'),'--byob-root',$input,'--mutation-mode',$mode,'--instruction-tick-budget',[string]$InstructionTickBudget,'--observe-guest-exec-lifecycle-ledger')
    if ($ObserveBudgetTerminalPosition) { $argumentVector += '--observe-budget-terminal-position' }
    $argumentVector += (Join-Path $input 'TARGET.EXE')
    $oldConfig = [Environment]::GetEnvironmentVariable('NTDOS64_STARTUP_CONFIG_SOURCE','Process')
    $oldAutoexec = [Environment]::GetEnvironmentVariable('NTDOS64_STARTUP_AUTOEXEC_SOURCE','Process')
    [Environment]::SetEnvironmentVariable('NTDOS64_STARTUP_CONFIG_SOURCE',(Join-Path $input 'fixture-config.nt'),'Process')
    [Environment]::SetEnvironmentVariable('NTDOS64_STARTUP_AUTOEXEC_SOURCE',(Join-Path $input 'fixture-autoexec.nt'),'Process')
    try {
        $started = [DateTime]::UtcNow
        $startInfo = [Diagnostics.ProcessStartInfo]::new()
        $startInfo.FileName = $exe; $startInfo.UseShellExecute = $false
        $startInfo.RedirectStandardOutput = $true; $startInfo.RedirectStandardError = $true
        $startInfo.CreateNoWindow = $true
        $startInfo.Arguments = (($argumentVector | ForEach-Object { '"' + $_.Replace('"','\"') + '"' }) -join ' ')
        $process = [Diagnostics.Process]::new(); $process.StartInfo = $startInfo
        if (!$process.Start()) { throw 'T228 native process did not start' }
        $stdoutTask = $process.StandardOutput.ReadToEndAsync(); $stderrTask = $process.StandardError.ReadToEndAsync()
        $exited = $process.WaitForExit($WatchdogSeconds * 1000)
        $killed = $false; $exitCode = $null
        if (!$exited) { $process.Kill($true); $process.WaitForExit(); $killed = $true } else { $process.WaitForExit(); $exitCode = [int]$process.ExitCode }
        [IO.File]::WriteAllText($stdout,$stdoutTask.GetAwaiter().GetResult(),[Text.UTF8Encoding]::new($false))
        [IO.File]::WriteAllText($stderr,$stderrTask.GetAwaiter().GetResult(),[Text.UTF8Encoding]::new($false))
        $records += [ordered]@{mode=$mode;startedUtc=$started.ToString('o');elapsedMilliseconds=([DateTime]::UtcNow-$started).TotalMilliseconds;exitedWithinWatchdog=$exited;killedByWatchdog=$killed;exitCode=$exitCode;stdout=([IO.Path]::GetFileName($stdout));stderr=([IO.Path]::GetFileName($stderr));stdoutSha256=(Hash $stdout);stderrSha256=(Hash $stderr)}
        $process.Dispose()
    }
    finally {
        [Environment]::SetEnvironmentVariable('NTDOS64_STARTUP_CONFIG_SOURCE',$oldConfig,'Process')
        [Environment]::SetEnvironmentVariable('NTDOS64_STARTUP_AUTOEXEC_SOURCE',$oldAutoexec,'Process')
    }
}
[ordered]@{schema='ntdos64.t228.s1.guest-exec-integration-observation.v1';nativeExecutable=[ordered]@{path=$exe;sha256=(Hash $exe)};inputRoot=$input;inputManifestSha256=(Hash (Join-Path $input 'guest-exec-lifecycle-ledger-inputs-manifest.json'));watchdogSeconds=$WatchdogSeconds;instructionTickBudget=$InstructionTickBudget;observation=[ordered]@{guestExecLifecycleLedger=$true;budgetTerminalPosition=[bool]$ObserveBudgetTerminalPosition};runs=$records} | ConvertTo-Json -Depth 7 | ForEach-Object { [IO.File]::WriteAllText((Join-Path $run 'observation.json'),$_+[Environment]::NewLine,[Text.UTF8Encoding]::new($false)) }
Write-Host "Recorded T228 S1 Direct/Readonly guest-exec integration observation: $run"