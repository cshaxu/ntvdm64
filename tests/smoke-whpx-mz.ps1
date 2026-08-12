param(
    [Parameter(Mandatory = $true)][string]$Dispatcher,
    [Parameter(Mandatory = $true)][string]$Runner
)

$program = Join-Path $env:TEMP ("whpx-mz-" + [guid]::NewGuid() + ".exe")
try {
    # 32-byte MZ header, a five-byte AH=4Ch module, entry 0000:0000.
    [byte[]]$header = 0x4D,0x5A,0x25,0x00,0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFE,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x00
    [byte[]]$code = 0xB8,0x09,0x4C,0xCD,0x21
    [System.IO.File]::WriteAllBytes($program, $header + $code)
    & $Dispatcher --engine $Runner $program | Out-Null
    if ($LASTEXITCODE -ne 9) { throw "WHPX MZ engine exit was $LASTEXITCODE, expected 9" }
}
finally {
    Remove-Item -LiteralPath $program -Force -ErrorAction SilentlyContinue
}
