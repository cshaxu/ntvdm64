param([Parameter(Mandatory = $true)][string]$Nvtdm)

$root = Join-Path $env:TEMP ("nvtdm-root-" + [guid]::NewGuid())
$outside = Join-Path $env:TEMP ("nvtdm-outside-" + [guid]::NewGuid())
try {
    New-Item -ItemType Directory -Path $root | Out-Null
    New-Item -ItemType Directory -Path $outside | Out-Null
    New-Item -ItemType Directory -Path (Join-Path $root "GAMES") | Out-Null
    [System.IO.File]::WriteAllText((Join-Path $root "GAMES\DEMO.COM"), "owned fixture")
    [System.IO.File]::WriteAllText((Join-Path $outside "OUT.COM"), "outside fixture")
    $output = & $Nvtdm --probe-dos-path $root "C:\GAMES\DEMO.COM" 2>&1
    if ($LASTEXITCODE -ne 0 -or ($output -join "`n") -notmatch "GAMES\\DEMO.COM") {
        throw "rooted path was rejected: $($output -join ' | ')"
    }
    & $Nvtdm --probe-rooted-read $root "C:\GAMES\DEMO.COM" | Out-Null
    if ($LASTEXITCODE -ne 0) { throw "rooted read was rejected" }
    & $Nvtdm --probe-file-service $root "C:\GAMES\DEMO.COM" | Out-Null
    if ($LASTEXITCODE -ne 0) { throw "DOS file service was rejected" }
    & $Nvtdm --probe-dos-path $root "C:\..\escape.txt" 2>$null
    if ($LASTEXITCODE -ne 3) { throw "path traversal was accepted" }
    & $Nvtdm --probe-dos-path $root "NUL" 2>$null
    if ($LASTEXITCODE -ne 3) { throw "DOS device name was accepted" }
    & $Nvtdm --probe-dos-path $root "COM1.TXT" 2>$null
    if ($LASTEXITCODE -ne 3) { throw "DOS device extension name was accepted" }
    New-Item -ItemType Junction -Path (Join-Path $root "JUMP") -Target $outside | Out-Null
    & $Nvtdm --probe-rooted-read $root "C:\JUMP\OUT.COM" 2>$null
    if ($LASTEXITCODE -ne 3) { throw "junction escape was accepted" }
}
finally {
    Remove-Item -LiteralPath $root -Recurse -Force -ErrorAction SilentlyContinue
    Remove-Item -LiteralPath $outside -Recurse -Force -ErrorAction SilentlyContinue
}
