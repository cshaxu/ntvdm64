param([string]$OutputRoot = 'build/t473-s10/vdd-ffff')
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path (Join-Path $PSScriptRoot '../../..')).Path
if (-not [IO.Path]::IsPathRooted($OutputRoot)) { $OutputRoot = Join-Path $repo $OutputRoot }
$output = (Resolve-Path -LiteralPath $OutputRoot).Path
$exe = Join-Path $output 'fixture.exe'
foreach ($mode in @('install','uninstall')) {
    $stdout = Join-Path $output "$mode.stdout.txt"
    $stderr = Join-Path $output "$mode.stderr.txt"
    $p = Start-Process -FilePath $exe -ArgumentList "--ffff-$mode" -WindowStyle Hidden -PassThru -RedirectStandardOutput $stdout -RedirectStandardError $stderr
    $finished = $p.WaitForExit(2000)
    if (-not $finished) { $p.Kill(); $p.WaitForExit() }
    $contents = Get-Content -LiteralPath $stdout -Raw
    $entered = $contents -match "ENTER_$($mode.ToUpper())_FFFF"
    $returned = $contents -match 'RETURNED'
    Write-Output "$mode entered=$entered returned=$returned completed=$finished"
    if (-not $entered -or $returned -or $finished) { exit 1 }
}
