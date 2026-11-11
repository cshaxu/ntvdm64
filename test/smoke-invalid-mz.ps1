param([Parameter(Mandatory = $true)][string]$Nvtdm)

$program = Join-Path $env:TEMP ("nvtdm-invalid-mz-" + [guid]::NewGuid() + ".exe")
try {
    # Header claims a 512-byte image but contains only its 32-byte header.
    [byte[]]$header = 0x4D,0x5A,0x00,0x00,0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFE,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x00
    [System.IO.File]::WriteAllBytes($program, $header)
    & $Nvtdm --probe-real-mode $program 2>$null
    if ($LASTEXITCODE -ne 1) { throw "truncated MZ image was not rejected: exit=$LASTEXITCODE" }
}
finally {
    Remove-Item -LiteralPath $program -Force -ErrorAction SilentlyContinue
}
