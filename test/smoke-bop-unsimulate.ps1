param([Parameter(Mandatory = $true)][string]$Nvtdm)

$path = Join-Path $env:TEMP ("nvtdm-bop-" + [guid]::NewGuid().ToString() + ".com")
try {
    # OpenNT's QUIT.COM body: BOP, marker C4h, selector FEh (UNSIMULATE).
    [System.IO.File]::WriteAllBytes($path, [byte[]](0xC4, 0xC4, 0xFE))

    $output = & $Nvtdm --probe-bop-unsimulate $path 2>&1
    if ($LASTEXITCODE -ne 0 -or ($output -join "`n") -notmatch "BOP_UNSIMULATE") {
        throw "BOP_UNSIMULATE probe failed: exit=$LASTEXITCODE output=$($output -join ' | ')"
    }

    $output = & $Nvtdm --probe-real-mode $path 2>&1
    if ($LASTEXITCODE -eq 0 -or ($output -join "`n") -notmatch "unsupported opcode C4") {
        throw "generic real-mode probe unexpectedly accepted BOP: exit=$LASTEXITCODE output=$($output -join ' | ')"
    }
}
finally {
    Remove-Item -LiteralPath $path -Force -ErrorAction SilentlyContinue
}
