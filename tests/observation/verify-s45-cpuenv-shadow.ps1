[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [string]$BuildRoot,
    [string]$RuntimeRoot = 'O:\winnt',
    [string]$FixturePath = '',
    [string]$ExpectedTraceCsv = 'LOAD_OK,NT_OPEN_OK,NT_ENUM_OK,NT_ENUM_END,NT_CLOSE',
    [switch]$NoShadow,
    [string]$GuestArguments = 'MEM.EXE'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$build = (Resolve-Path -LiteralPath $BuildRoot).Path
$runtime = (Resolve-Path -LiteralPath $RuntimeRoot).Path
if (!$FixturePath) {
    $FixturePath = Join-Path $PSScriptRoot '..\mvdm-host\fixtures\S45-CPUENV-NTVDM.REG'
}
$fixture = if (!$NoShadow) { (Resolve-Path -LiteralPath $FixturePath).Path } else { $null }
$shadow = Join-Path $runtime 'NTVDM.REG'
$logs = Join-Path $runtime 'logs'
$output = Join-Path $logs 's45-cpuenv-mem.stdout.log'
$errorOutput = Join-Path $logs 's45-cpuenv-mem.stderr.log'
$trace = Join-Path $logs 's45-cpuenv-registry.log'
$processes = @(Get-Process ntvdm,basesrv,run16 -ErrorAction SilentlyContinue)

if ($processes.Count -ne 0) {
    throw 'S45 shadow-registry test requires no existing run16/basesrv/ntvdm session.'
}
New-Item -ItemType Directory -Force -Path $logs | Out-Null
foreach ($name in @('run16.exe','basesrv.exe','ntvdm.exe','dtmgr.exe')) {
    Copy-Item -LiteralPath (Join-Path $build $name) -Destination (Join-Path $runtime $name) -Force
}
if ($fixture) { Copy-Item -LiteralPath $fixture -Destination $shadow -Force }
Remove-Item -LiteralPath $output,$errorOutput,$trace -Force -ErrorAction SilentlyContinue

try {
    $env:NTVDM_SHADOW_REGISTRY_TRACE = $trace
    $process = Start-Process -FilePath (Join-Path $runtime 'run16.exe') -ArgumentList $GuestArguments `
        -WorkingDirectory $runtime -RedirectStandardOutput $output -RedirectStandardError $errorOutput `
        -WindowStyle Hidden -PassThru
    if (!$process.WaitForExit(30000)) {
        Stop-Process -Id $process.Id -Force
        throw "run16 $GuestArguments timed out."
    }
    $text = if (Test-Path -LiteralPath $output) { Get-Content -LiteralPath $output -Raw } else { '' }
    if ($text -notmatch 'bytes total conventional memory' -or
        $text -match 'Bad command or filename') {
        throw 'MEM did not produce its established successful guest output.'
    }
    $markers = @(Get-Content -LiteralPath $trace -ErrorAction Stop)
    $expectedTrace = @($ExpectedTraceCsv.Split(',') | ForEach-Object { $_.Trim() } | Where-Object { $_ })
    $missing = @($expectedTrace | Where-Object { $markers -notcontains $_ })
    if ($missing.Count -ne 0) {
        throw "Shadow-registry original-reader trace lacks: $($missing -join ', ')"
    }
    Write-Output 'S45_SHADOW_REGISTRY_REAL_WORKER_OK'
}
finally {
    Get-Process ntvdm,basesrv,run16 -ErrorAction SilentlyContinue | Stop-Process -Force
    Remove-Item -LiteralPath $shadow -Force -ErrorAction SilentlyContinue
}
