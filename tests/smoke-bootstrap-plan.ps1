param([Parameter(Mandatory = $true)][string]$Nvtdm)

$root = Join-Path $env:TEMP ("nvtdm-bootstrap-" + [guid]::NewGuid())
$manifest = $root + ".txt"
try {
    New-Item -ItemType Directory -Path $root | Out-Null
    $ntio = [byte[]](0x90, 0xC4, 0xC4, 0x50, 0x11, 0xF4)
    [System.IO.File]::WriteAllBytes((Join-Path $root "NTIO.SYS"), $ntio)
    foreach ($name in "NTDOS.SYS", "COMMAND.COM", "HIMEM.SYS", "DOSX.EXE") {
        [System.IO.File]::WriteAllBytes((Join-Path $root $name), [byte[]](0x41, 0x42, 0x43))
    }
    & $Nvtdm --runtime $root --record-manifest $manifest
    if ($LASTEXITCODE -ne 0) { throw "manifest command failed: exit=$LASTEXITCODE" }
    $before = [System.IO.File]::ReadAllBytes((Join-Path $root "NTIO.SYS"))
    $output = & $Nvtdm --runtime $root --plan-bootstrap $manifest 2>&1
    if ($LASTEXITCODE -ne 0 -or ($output -join "`n") -notmatch "rewrites=1") {
        throw "bootstrap plan failed: exit=$LASTEXITCODE output=$($output -join ' | ')"
    }
    $after = [System.IO.File]::ReadAllBytes((Join-Path $root "NTIO.SYS"))
    if ($before.Length -ne $after.Length -or (Compare-Object $before $after)) {
        throw "bootstrap planner modified the BYOB NTIO input"
    }
    [System.IO.File]::WriteAllBytes((Join-Path $root "NTIO.SYS"), [byte[]](0x90, 0xC4, 0xC4, 0x50, 0x12, 0xF4))
    & $Nvtdm --runtime $root --plan-bootstrap $manifest 2>&1 | Out-Null
    if ($LASTEXITCODE -ne 3) { throw "manifest mismatch was accepted: exit=$LASTEXITCODE" }
}
finally {
    Remove-Item -LiteralPath $root -Recurse -Force -ErrorAction SilentlyContinue
    Remove-Item -LiteralPath $manifest -Force -ErrorAction SilentlyContinue
}
