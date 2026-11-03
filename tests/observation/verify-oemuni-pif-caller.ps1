param([Parameter(Mandatory = $true)][string]$Fixture)

$ErrorActionPreference = 'Stop'
$fixturePath = (Resolve-Path -LiteralPath $Fixture).Path
$repo = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '../..')).Path
$work = Join-Path $repo ('build/pif-' + [guid]::NewGuid().ToString('N').Substring(0, 8))
$longWork = Join-Path $work ('l' * 48)
New-Item -ItemType Directory -Path $work | Out-Null
$passed = $false
try {
    Push-Location -LiteralPath $work
    try {
        & $fixturePath
        if ($LASTEXITCODE -ne 0) { throw "PIF caller fixture failed: $LASTEXITCODE" }
        New-Item -ItemType Directory -Path $longWork | Out-Null
        Push-Location -LiteralPath $longWork
        try {
            & $fixturePath --expect-long-startdir
            if ($LASTEXITCODE -ne 0) { throw "PIF long-directory control failed: $LASTEXITCODE" }
        } finally { Pop-Location }
        Remove-Item -LiteralPath $longWork
        $passed = $true
    } finally { Pop-Location }
} finally {
    if ($passed) { Remove-Item -LiteralPath $work }
    else { Write-Warning "PIF fixture evidence retained at $work" }
}
