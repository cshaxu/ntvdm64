param([Parameter(Mandatory = $true)][string]$Nvtdm)

$root = Join-Path $env:TEMP ("nvtdm-machine-missing-root-" + [guid]::NewGuid())
$program = Join-Path $env:TEMP ("nvtdm-machine-missing-" + [guid]::NewGuid() + ".com")
try {
    New-Item -ItemType Directory -Path $root | Out-Null
    # Open missing C:\MISSING.COM; JC chooses exit 1, otherwise exit 0.
    [byte[]]$code = 0xBA,0x14,0x01,0xB8,0x00,0x3D,0xCD,0x21,0x72,0x05,0xB8,0x00,0x4C,0xCD,0x21,0xB8,0x01,0x4C,0xCD,0x21
    [byte[]]$path = [System.Text.Encoding]::ASCII.GetBytes("C:\MISSING.COM`0")
    [System.IO.File]::WriteAllBytes($program, $code + $path)
    & $Nvtdm --probe-real-mode-root $root $program | Out-Null
    if ($LASTEXITCODE -ne 1) { throw "guest did not observe DOS carry error: exit=$LASTEXITCODE" }
}
finally {
    Remove-Item -LiteralPath $root -Recurse -Force -ErrorAction SilentlyContinue
    Remove-Item -LiteralPath $program -Force -ErrorAction SilentlyContinue
}
