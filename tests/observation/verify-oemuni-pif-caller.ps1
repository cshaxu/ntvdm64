param([Parameter(Mandatory = $true)][string]$Fixture, [string]$WorkRoot)

$ErrorActionPreference = 'Stop'
$fixturePath = (Resolve-Path -LiteralPath $Fixture).Path
$repo = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '../..')).Path
if (!$WorkRoot) { $WorkRoot = Join-Path $repo 'build' }
$work = Join-Path (Resolve-Path -LiteralPath $WorkRoot).Path ('pif-' + [guid]::NewGuid().ToString('N').Substring(0, 8))
# Multiple short components remain long even on volumes with 8.3 names.
$longDirectories = @()
$longWork = $work
1..8 | ForEach-Object {
    $longWork = Join-Path $longWork 'longpath'
    $longDirectories += $longWork
}
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
        for ($i = $longDirectories.Count - 1; $i -ge 0; --$i) {
            Remove-Item -LiteralPath $longDirectories[$i]
        }
        $passed = $true
    } finally { Pop-Location }
} finally {
    if ($passed) { Remove-Item -LiteralPath $work }
    else { Write-Warning "PIF fixture evidence retained at $work" }
}
