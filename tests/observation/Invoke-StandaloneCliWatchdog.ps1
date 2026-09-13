[CmdletBinding()]
param(
    [Parameter(Mandatory=$true)][string]$Executable,
    [Parameter(Mandatory=$true)][string]$Arguments,
    [Parameter(Mandatory=$true)][string]$RecordPath,
    [ValidateRange(1,60)][int]$WatchdogSeconds = 20
)
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$exe = (Resolve-Path -LiteralPath $Executable).Path
$record = [IO.Path]::GetFullPath($RecordPath)
if (Test-Path -LiteralPath $record) { throw "Refusing to overwrite record: $record" }
New-Item -ItemType Directory -Force -Path (Split-Path -Parent $record) | Out-Null
$stdout = $record + '.stdout.log'; $stderr = $record + '.stderr.log'
$started = [DateTime]::UtcNow
$p = Start-Process -FilePath $exe -ArgumentList $Arguments -PassThru -NoNewWindow -RedirectStandardOutput $stdout -RedirectStandardError $stderr
$testProcessId = $p.Id
$exited = $p.WaitForExit($WatchdogSeconds * 1000)
$killed = $false; $exitCode = $null
if ($exited) { $exitCode = $p.ExitCode } else { & taskkill.exe /T /F /PID $testProcessId | Out-Null; $killed = $true }
$p.Dispose()
[ordered]@{exe=$exe;sha256=(Get-FileHash $exe -Algorithm SHA256).Hash.ToLowerInvariant();arguments=$Arguments;pid=$testProcessId;startedUtc=$started.ToString('o');elapsedMilliseconds=([DateTime]::UtcNow-$started).TotalMilliseconds;exitedWithinWatchdog=$exited;killedByWatchdog=$killed;exitCode=$exitCode;stdout=$stdout;stderr=$stderr} | ConvertTo-Json | ForEach-Object { [IO.File]::WriteAllText($record,$_+[Environment]::NewLine,[Text.UTF8Encoding]::new($false)) }
if (!$exited) { exit 124 }; exit $exitCode
