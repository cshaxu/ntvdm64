param(
    [string]$RepositoryRoot = (Split-Path -Parent (Split-Path -Parent $PSScriptRoot))
)

$ErrorActionPreference = 'Stop'
$root = Join-Path $RepositoryRoot 'src\bx-vdm'
# T236 retired every root-level handwritten DEM/COMMAND recognizer.  T237
# likewise retires the remaining XMS provider; product recognizers now live
# only in their owner-package composition directories.  Keep this as a
# negative gate so a new root-level legacy recognizer cannot quietly revive a
# parallel v1 route.
$frozen = @()
$core = @()
$found = @()
Get-ChildItem -LiteralPath $root -Filter '*.c' | ForEach-Object {
    $text = Get-Content -LiteralPath $_.FullName -Raw
    if ($text -match 'bytes\[2\]' -and $text -match 'bytes\[3\]') { $found += $_.Name }
}
foreach ($name in $found) {
    if ($name -notin $frozen -and $name -notin $core) { throw "Unregistered direct BOP recognizer: $name" }
}
if ($found.Count -ne 0) { throw "Retired root-level BOP recognizer remains: $($found -join ', ')" }
Write-Output "Legacy BOP endpoint register verified: direct=$($found.Count)"
