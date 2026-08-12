$ErrorActionPreference = 'Stop'

$repositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$entrypoint = Join-Path $repositoryRoot 'tools\Invoke-T95S6LinkVectorEntrypoint.ps1'
$powershellExe = (Get-Command powershell.exe -ErrorAction Stop).Source
$testRoot = Join-Path ([System.IO.Path]::GetTempPath()) ("t95-s6-link-entrypoint-" + [guid]::NewGuid().ToString('N'))

try {
    New-Item -ItemType Directory -Path $testRoot | Out-Null
    $objects = @((Join-Path $testRoot 'one.obj'), (Join-Path $testRoot 'two.obj'))
    foreach ($object in $objects) { New-Item -ItemType File -Path $object | Out-Null }
    $out = Join-Path $testRoot 'fixture.exe'
    $map = Join-Path $testRoot 'fixture.map'
    $vectorPath = Join-Path $testRoot 'vector.json'
    $objectListPath = Join-Path $testRoot 'objects.json'
    $objects | ConvertTo-Json | Set-Content -LiteralPath $objectListPath -Encoding utf8
    $positive = & $powershellExe -NoProfile -ExecutionPolicy Bypass -File $entrypoint -OutPath $out -MapPath $map -ObjectListPath $objectListPath -VectorOutputPath $vectorPath 2>&1
    if ($LASTEXITCODE -ne 0) { throw "Explicit-policy entrypoint failed: $($positive -join "`n")" }
    if (-not (Test-Path -LiteralPath $vectorPath -PathType Leaf)) { throw 'Entrypoint did not emit a vector.' }
    $decodedVector = Get-Content -LiteralPath $vectorPath -Raw | ConvertFrom-Json
    $vector = @($decodedVector)
    if ($vector.Count -ne 9 -or $vector[1] -cne ('/OUT:' + [System.IO.Path]::GetFullPath($out)) -or
        $vector[5] -cne ('/MAP:' + [System.IO.Path]::GetFullPath($map)) -or
        $vector -notcontains '/OPT:REF' -or $vector -notcontains '/INCREMENTAL:NO') {
        throw 'Entrypoint emitted an invalid vector.'
    }

    $negativeVector = Join-Path $testRoot 'negative.json'
    $missingGenerator = Join-Path $testRoot 'missing-generator.ps1'
    $priorErrorAction = $ErrorActionPreference
    $ErrorActionPreference = 'Continue'
    $negative = & $powershellExe -NoProfile -ExecutionPolicy Bypass -File $entrypoint -OutPath $out -MapPath $map -ObjectListPath $objectListPath -VectorOutputPath $negativeVector -GeneratorPath $missingGenerator 2>&1
    $negativeExit = $LASTEXITCODE
    $ErrorActionPreference = $priorErrorAction
    if ($negativeExit -eq 0) { throw 'Entrypoint accepted a missing generator.' }
    if (($negative -join "`n") -notmatch [regex]::Escape('Accepted link-vector generator is unavailable')) {
        throw "Missing-generator failure was not explicit: $($negative -join "`n")"
    }
    if (Test-Path -LiteralPath $negativeVector) { throw 'Missing-generator failure emitted a vector.' }

    Write-Host 'T95 S6 explicit-policy link-vector entrypoint verification passed.'
} finally {
    if (Test-Path -LiteralPath $testRoot) { Remove-Item -LiteralPath $testRoot -Recurse -Force }
}
