[CmdletBinding()]
param([Parameter(Mandatory)][string]$WorkerBuild)
$ErrorActionPreference = 'Stop'
$image = Join-Path (Resolve-Path -LiteralPath $WorkerBuild).Path 'ccpu-monitor-boundary-test.exe'
if (!(Test-Path -LiteralPath $image -PathType Leaf)) { throw "Missing fixture: $image" }
# This diagnostic preserves a known non-pass; it is not a product success gate.
foreach ($case in @(93, 94, 95, 96, 97, 98, 99, 100, 101, 104, 105, 106)) {
    $output = @(& $image $case 2>&1 | ForEach-Object { $_.ToString() })
    $actual = $LASTEXITCODE
    $expected = if ($case -in @(94, 97, 100, 105)) { 0 } else { 1 }
    $expectedLine = switch ($case) {
        { $_ -in 93, 96, 99, 105 } { 'DPMI_REMAPPED_CODE handled=0 eip=00001000' }
        { $_ -in 94, 97, 100, 104 } { 'DPMI_REMAPPED_CODE handled=1 eip=00001002' }
        { $_ -in 95, 98, 101 } { 'DPMI_REMAPPED_CODE handled=0 eip=00000fff' }
        106 { 'DPMI_REMAPPED_CODE handled=1 eip=00001001' }
    }
    if ($actual -ne $expected -or !($output -ccontains $expectedLine)) {
        throw "Unexpected DPMI code-page result: case=$case exit=$actual output=$output"
    }
    Write-Host "case=$case observed-exit=$actual $output"
}
Write-Host 'PASS diagnostic: identity controls pass; remapped and split-page false-negative and false-positive cases remain known non-passes'
