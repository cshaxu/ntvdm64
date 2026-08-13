$ErrorActionPreference = 'Stop'

$repositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
. (Join-Path $repositoryRoot 'tools\New-T95S6LinkArgumentVector.ps1')
$testRoot = Join-Path ([System.IO.Path]::GetTempPath()) ("t95-s6-link-vector-" + [guid]::NewGuid().ToString('N'))

function Assert-Throws([scriptblock]$Action, [string]$ExpectedText) {
    try { & $Action } catch {
        if ($_.Exception.Message -notmatch [regex]::Escape($ExpectedText)) {
            throw "Expected failure containing '$ExpectedText', got '$($_.Exception.Message)'."
        }
        return
    }
    throw "Expected failure containing '$ExpectedText'."
}

try {
    New-Item -ItemType Directory -Path $testRoot | Out-Null
    $objects = @((Join-Path $testRoot 'one.obj'), (Join-Path $testRoot 'two.obj'))
    foreach ($object in $objects) { New-Item -ItemType File -Path $object | Out-Null }
    $out = Join-Path $testRoot 'retention-fixture.exe'
    $map = Join-Path $testRoot 'retention-fixture.map'
    $arguments = @(New-T95S6LinkArgumentVector -OutPath $out -MapPath $map -ObjectPath $objects)
    if ($arguments[1] -cne ('/OUT:' + [System.IO.Path]::GetFullPath($out))) { throw 'Output path was not one exact link token.' }
    if ($arguments[5] -cne ('/MAP:' + [System.IO.Path]::GetFullPath($map))) { throw 'Map path was not one exact link token.' }
    if ($arguments -notcontains '/OPT:REF' -or $arguments -notcontains '/INCREMENTAL:NO') { throw 'Retention options are missing.' }
    if (-not (Test-T95S6LinkArgumentVector -ArgumentVector $arguments -ExpectedOutPath $out -ExpectedMapPath $map -ExpectedObjectPath $objects)) { throw 'Positive vector validation failed.' }

    $splitOut = @($arguments)
    $splitOut[1] = '/OUT:'
    Assert-Throws { Test-T95S6LinkArgumentVector -ArgumentVector $splitOut -ExpectedOutPath $out -ExpectedMapPath $map -ExpectedObjectPath $objects } 'Required link token'
    $emptyMap = @($arguments)
    $emptyMap[5] = '/MAP:'
    Assert-Throws { Test-T95S6LinkArgumentVector -ArgumentVector $emptyMap -ExpectedOutPath $out -ExpectedMapPath $map -ExpectedObjectPath $objects } 'Required link token'
    $duplicate = @($arguments + '/OPT:REF')
    Assert-Throws { Test-T95S6LinkArgumentVector -ArgumentVector $duplicate -ExpectedOutPath $out -ExpectedMapPath $map -ExpectedObjectPath $objects } 'Required link token is not present exactly once: /OPT:REF'
    $productOption = @($arguments + '/DLL')
    Assert-Throws { Test-T95S6LinkArgumentVector -ArgumentVector $productOption -ExpectedOutPath $out -ExpectedMapPath $map -ExpectedObjectPath $objects } 'Forbidden, malformed, duplicate, or unexpected option'
    $missingObject = @($arguments)
    $missingObject[7] = (Join-Path $testRoot 'missing.obj')
    Assert-Throws { Test-T95S6LinkArgumentVector -ArgumentVector $missingObject -ExpectedOutPath $out -ExpectedMapPath $map -ExpectedObjectPath $objects } 'Object path does not exist'
    $duplicateObject = @($arguments)
    $duplicateObject[8] = $duplicateObject[7]
    Assert-Throws { Test-T95S6LinkArgumentVector -ArgumentVector $duplicateObject -ExpectedOutPath $out -ExpectedMapPath $map -ExpectedObjectPath $objects } 'duplicate object token'

    Write-Host 'T95 S6 link argument-vector validation passed.'
} finally {
    if (Test-Path -LiteralPath $testRoot) { Remove-Item -LiteralPath $testRoot -Recurse -Force }
}
