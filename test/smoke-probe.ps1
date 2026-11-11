param([Parameter(Mandatory = $true)][string]$Nvtdm)

$path = Join-Path $env:TEMP ("nvtdm-probe-" + [guid]::NewGuid().ToString() + ".com")
try {
    # mov dx,010Ch; mov ah,09h; int 21h; mov ax,4C07h; int 21h; "probe$"
    [byte[]]$program = 0xBA,0x0C,0x01,0xB4,0x09,0xCD,0x21,0xB8,0x07,0x4C,0xCD,0x21,0x70,0x72,0x6F,0x62,0x65,0x24
    [System.IO.File]::WriteAllBytes($path, $program)
    $output = & $Nvtdm --probe-real-mode $path 2>&1
    if ($LASTEXITCODE -ne 7 -or ($output -join "`n") -notmatch "probe") {
        throw "real-mode probe failed: exit=$LASTEXITCODE output=$($output -join ' | ')"
    }
}
finally {
    Remove-Item -LiteralPath $path -Force -ErrorAction SilentlyContinue
}
