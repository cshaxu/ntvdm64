param([Parameter(Mandatory = $true)][string]$Nvtdm)

$root = Join-Path $env:TEMP ("nvtdm-runtime-" + [guid]::NewGuid())
$manifest = $root + ".txt"
try {
    New-Item -ItemType Directory -Path $root | Out-Null
    foreach ($name in "NTIO.SYS", "NTDOS.SYS", "COMMAND.COM", "HIMEM.SYS", "DOSX.EXE") {
        [System.IO.File]::WriteAllBytes((Join-Path $root $name), [byte[]](0x41, 0x42, 0x43))
    }
    & $Nvtdm --runtime $root --record-manifest $manifest
    if ($LASTEXITCODE -ne 0) { throw "manifest command failed: exit=$LASTEXITCODE" }
    $text = [System.IO.File]::ReadAllText($manifest)
    if ($text -notmatch "nvtdm-runtime-manifest-v1" -or $text -notmatch "NTIO.SYS\|3\|" -or
        $text -notmatch "DOSX.EXE\|3\|") { throw "manifest content is incomplete" }
    & $Nvtdm --runtime $root --verify-manifest $manifest
    if ($LASTEXITCODE -ne 0) { throw "manifest verification failed: exit=$LASTEXITCODE" }
    [System.IO.File]::WriteAllBytes((Join-Path $root "NTIO.SYS"), [byte[]](0x41, 0x42, 0x44))
    & $Nvtdm --runtime $root --verify-manifest $manifest
    if ($LASTEXITCODE -ne 3) { throw "changed runtime was accepted: exit=$LASTEXITCODE" }
}
finally {
    Remove-Item -LiteralPath $root -Recurse -Force -ErrorAction SilentlyContinue
    Remove-Item -LiteralPath $manifest -Force -ErrorAction SilentlyContinue
}
