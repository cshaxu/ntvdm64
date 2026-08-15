[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$OutPath,

    [Parameter(Mandatory = $true)]
    [string]$MapPath,

    [Parameter(Mandatory = $true)]
    [string]$ObjectListPath,

    [Parameter(Mandatory = $true)]
    [string]$VectorOutputPath,

    [string]$GeneratorPath = ''
)

$ErrorActionPreference = 'Stop'

if ([string]::IsNullOrWhiteSpace($GeneratorPath)) {
    $GeneratorPath = Join-Path (Split-Path -Parent $PSScriptRoot) 'build\New-T95S6LinkArgumentVector.ps1'
}
if (-not (Test-Path -LiteralPath $GeneratorPath -PathType Leaf)) {
    throw "Accepted link-vector generator is unavailable: $GeneratorPath"
}
if ([string]::IsNullOrWhiteSpace($VectorOutputPath) -or -not [System.IO.Path]::IsPathRooted($VectorOutputPath)) {
    throw 'VectorOutputPath must be a nonempty absolute path.'
}
if ([string]::IsNullOrWhiteSpace($ObjectListPath) -or -not (Test-Path -LiteralPath $ObjectListPath -PathType Leaf)) {
    throw "ObjectListPath must name an existing list: $ObjectListPath"
}

# This entrypoint is intentionally invoked only through:
# powershell.exe -ExecutionPolicy Bypass -File Invoke-T95S6LinkVectorEntrypoint.ps1 ...
. $GeneratorPath
if ($null -eq (Get-Command New-T95S6LinkArgumentVector -CommandType Function -ErrorAction SilentlyContinue) -or
    $null -eq (Get-Command Test-T95S6LinkArgumentVector -CommandType Function -ErrorAction SilentlyContinue)) {
    throw 'Accepted link-vector generator did not export its required functions.'
}

$decodedObjectPaths = Get-Content -LiteralPath $ObjectListPath -Raw | ConvertFrom-Json
$objectPaths = @($decodedObjectPaths | ForEach-Object { [string]$_ })
if ($objectPaths.Count -eq 0 -or @($objectPaths | Where-Object { [string]::IsNullOrWhiteSpace($_) }).Count -ne 0) {
    throw 'ObjectListPath must contain a nonempty JSON string array.'
}
$vector = @(New-T95S6LinkArgumentVector -OutPath $OutPath -MapPath $MapPath -ObjectPath $objectPaths)
[void](Test-T95S6LinkArgumentVector -ArgumentVector $vector -ExpectedOutPath $OutPath -ExpectedMapPath $MapPath -ExpectedObjectPath $objectPaths)
$outputDirectory = Split-Path -Parent $VectorOutputPath
if (-not (Test-Path -LiteralPath $outputDirectory -PathType Container)) {
    throw "Vector output directory does not exist: $outputDirectory"
}
$vector | ConvertTo-Json | Set-Content -LiteralPath $VectorOutputPath -Encoding utf8
Write-Output "T95 S6 link vector emitted and validated: $VectorOutputPath"
