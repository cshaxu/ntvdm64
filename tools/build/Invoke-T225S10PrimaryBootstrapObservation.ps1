[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$NativeExecutable,
    [Parameter(Mandatory = $true)][string]$InputRoot,
    [Parameter(Mandatory = $true)][string]$RunRoot,
    [ValidateRange(1, 120)][int]$WatchdogSeconds = 20
)
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
function Hash([string]$Path) { return (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant() }
$exe = (Resolve-Path -LiteralPath $NativeExecutable).Path
$input = (Resolve-Path -LiteralPath $InputRoot).Path
$run = [IO.Path]::GetFullPath($RunRoot)
if (Test-Path -LiteralPath $run) { throw "Refusing to overwrite S10 run root: $run" }
foreach ($file in @('profile.json','TARGET.EXE','fixture-config.nt','fixture-autoexec.nt','primary-bootstrap-manifest.json')) {
    if (!(Test-Path -LiteralPath (Join-Path $input $file) -PathType Leaf)) { throw "Required S10 input missing: $file" }
}
New-Item -ItemType Directory -Path $run | Out-Null
$records = @()
foreach ($mode in @('direct','readonly')) {
    $stdout = Join-Path $run ($mode + '.stdout.log')
    $stderr = Join-Path $run ($mode + '.stderr.log')
    $arguments = '--byob-profile "' + (Join-Path $input 'profile.json') + '" --byob-root "' + $input + '" --mutation-mode ' + $mode + ' "' + (Join-Path $input 'TARGET.EXE') + '"'
    $oldConfig = [Environment]::GetEnvironmentVariable('NTVDM64_STARTUP_CONFIG_SOURCE','Process')
    $oldAutoexec = [Environment]::GetEnvironmentVariable('NTVDM64_STARTUP_AUTOEXEC_SOURCE','Process')
    [Environment]::SetEnvironmentVariable('NTVDM64_STARTUP_CONFIG_SOURCE',(Join-Path $input 'fixture-config.nt'),'Process')
    [Environment]::SetEnvironmentVariable('NTVDM64_STARTUP_AUTOEXEC_SOURCE',(Join-Path $input 'fixture-autoexec.nt'),'Process')
    try {
        $started = [DateTime]::UtcNow
        $process = Start-Process -FilePath $exe -ArgumentList $arguments -RedirectStandardOutput $stdout -RedirectStandardError $stderr -NoNewWindow -PassThru
        $exited = $process.WaitForExit($WatchdogSeconds * 1000)
        $killed = $false
        $exitCode = $null
        if (!$exited) { Stop-Process -Id $process.Id -Force; $killed = $true } else { $exitCode = $process.ExitCode }
        $records += [ordered]@{mode=$mode;startedUtc=$started.ToString('o');elapsedMilliseconds=([DateTime]::UtcNow-$started).TotalMilliseconds;exitedWithinWatchdog=$exited;killedByWatchdog=$killed;exitCode=$exitCode;stdout=([IO.Path]::GetFileName($stdout));stderr=([IO.Path]::GetFileName($stderr))}
        $process.Dispose()
    }
    finally {
        [Environment]::SetEnvironmentVariable('NTVDM64_STARTUP_CONFIG_SOURCE',$oldConfig,'Process')
        [Environment]::SetEnvironmentVariable('NTVDM64_STARTUP_AUTOEXEC_SOURCE',$oldAutoexec,'Process')
    }
}

[ordered]@{schema='runner.t225.s10.primary-bootstrap-observation.v1';nativeExecutable=[ordered]@{path=$exe;sha256=(Hash $exe)};inputRoot=$input;inputManifestSha256=(Hash (Join-Path $input 'primary-bootstrap-manifest.json'));watchdogSeconds=$WatchdogSeconds;runs=$records} | ConvertTo-Json -Depth 7 | ForEach-Object { [IO.File]::WriteAllText((Join-Path $run 'observation.json'),$_+[Environment]::NewLine,[Text.UTF8Encoding]::new($false)) }
Write-Host "Recorded T225 S10 Direct/Readonly primary bootstrap observation: $run"