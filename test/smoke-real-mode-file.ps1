param([Parameter(Mandatory = $true)][string]$Nvtdm)

$root = Join-Path $env:TEMP ("nvtdm-machine-root-" + [guid]::NewGuid())
$program = Join-Path $env:TEMP ("nvtdm-machine-" + [guid]::NewGuid() + ".com")
try {
    New-Item -ItemType Directory -Path (Join-Path $root "GAMES") -Force | Out-Null
    [System.IO.File]::WriteAllBytes((Join-Path $root "GAMES\DEMO.COM"), [byte[]](1,2,3,4,5))
    # Open C:\GAMES\DEMO.COM, read five bytes, and return the byte count as exit code.
    [byte[]]$code = 0xBA,0x1F,0x01,0xB8,0x00,0x3D,0xCD,0x21,0x93,0xB9,0x05,0x00,0xBA,0x31,0x01,0xB4,0x3F,0xCD,0x21,0x8A,0xD8,0xB4,0x3E,0xCD,0x21,0xB4,0x4C,0x8A,0xC3,0xCD,0x21
    [byte[]]$path = [System.Text.Encoding]::ASCII.GetBytes("C:\GAMES\DEMO.COM`0")
    [System.IO.File]::WriteAllBytes($program, $code + $path + [byte[]](0,0,0,0,0))
    & $Nvtdm --probe-real-mode-root $root $program | Out-Null
    if ($LASTEXITCODE -ne 5) { throw "guest file read did not return byte count: exit=$LASTEXITCODE" }
}
finally {
    Remove-Item -LiteralPath $root -Recurse -Force -ErrorAction SilentlyContinue
    Remove-Item -LiteralPath $program -Force -ErrorAction SilentlyContinue
}
