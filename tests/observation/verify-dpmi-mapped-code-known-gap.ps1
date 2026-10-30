[CmdletBinding()]
param([Parameter(Mandatory)][string]$WorkerBuild)
$ErrorActionPreference = 'Stop'
$image = Join-Path (Resolve-Path -LiteralPath $WorkerBuild).Path 'ccpu-monitor-boundary-test.exe'
if (!(Test-Path -LiteralPath $image -PathType Leaf)) { throw "Missing fixture: $image" }
# This diagnostic preserves a known non-pass; it is not a product success gate.
foreach ($case in @(93, 94, 95)) {
    $output = @(& $image $case 2>&1 | ForEach-Object { $_.ToString() })
    $actual = $LASTEXITCODE
    $expected = if ($case -eq 94) { 0 } else { 1 }
    if ($actual -ne $expected -or !($output -match '^DPMI_REMAPPED_CODE handled=')) {
        throw "Unexpected DPMI code-page result: case=$case exit=$actual output=$output"
    }
    Write-Host "case=$case observed-exit=$actual $output"
}
Write-Host 'PASS diagnostic: identity control passes; remapped and split-page cases remain known non-passes'
