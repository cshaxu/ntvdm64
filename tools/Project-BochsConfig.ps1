[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$ManifestPath,

    [Parameter(Mandatory = $true)]
    [string]$AnalysisRoot,

    [string]$InputConfigPath = ''
)

$ErrorActionPreference = 'Stop'

function Get-RequiredProperty([object]$Object, [string]$Name) {
    $property = $Object.PSObject.Properties[$Name]
    if ($null -eq $property) { throw "Manifest is missing required property '$Name'." }
    return [string]$property.Value
}

function Assert-OnlyProperties([object]$Object, [string[]]$Allowed, [string]$Context) {
    foreach ($property in $Object.PSObject.Properties) {
        if ($Allowed -notcontains $property.Name) {
            throw "$Context has unexpected property '$($property.Name)'."
        }
    }
}

if (-not (Test-Path -LiteralPath $ManifestPath -PathType Leaf)) { throw "Missing manifest: $ManifestPath" }
if (Test-Path -LiteralPath $AnalysisRoot) { throw "Analysis root must be fresh: $AnalysisRoot" }

$manifest = Get-Content -LiteralPath $ManifestPath -Raw | ConvertFrom-Json
Assert-OnlyProperties $manifest @('schemaVersion', 'sourceConfigSha256', 'replacements', 'additions', 'derivedAssertions') 'Manifest'
if ((Get-RequiredProperty $manifest 'schemaVersion') -ne '1') { throw 'Unsupported manifest schemaVersion.' }
$expectedHash = (Get-RequiredProperty $manifest 'sourceConfigSha256').ToUpperInvariant()
if ($expectedHash -notmatch '^[0-9A-F]{64}$') { throw 'sourceConfigSha256 must be an uppercase SHA-256 hash.' }
foreach ($collectionName in @('replacements', 'additions', 'derivedAssertions')) {
    if ($null -eq $manifest.PSObject.Properties[$collectionName]) { throw "Manifest is missing required collection '$collectionName'." }
}

if ([string]::IsNullOrWhiteSpace($InputConfigPath)) {
    $InputConfigPath = Join-Path (Split-Path -Parent $PSScriptRoot) 'src\bochs\config.h'
}
if (-not (Test-Path -LiteralPath $InputConfigPath -PathType Leaf)) { throw "Missing input config: $InputConfigPath" }
$actualHash = (Get-FileHash -LiteralPath $InputConfigPath -Algorithm SHA256).Hash.ToUpperInvariant()
if ($actualHash -ne $expectedHash) { throw "Pinned source hash mismatch: expected $expectedHash, got $actualHash." }

$sourceLines = [System.IO.File]::ReadAllLines((Resolve-Path -LiteralPath $InputConfigPath))
$defines = @{}
for ($index = 0; $index -lt $sourceLines.Count; $index++) {
    if ($sourceLines[$index] -match '^\s*#\s*define\s+([A-Za-z_][A-Za-z0-9_]*)\b(?:\s+(.*?))?\s*$') {
        $name = $Matches[1]
        if (-not $defines.ContainsKey($name)) { $defines[$name] = [System.Collections.ArrayList]::new() }
        $value = if ($null -eq $Matches[2]) { '' } else { $Matches[2].Trim() }
        [void]$defines[$name].Add([pscustomobject]@{ Index = $index; Value = $value })
    }
}

$seenNames = @{}
$changes = @()
foreach ($replacement in @($manifest.replacements)) {
    Assert-OnlyProperties $replacement @('name', 'oldValue', 'newValue', 'classification') 'Replacement'
    $name = Get-RequiredProperty $replacement 'name'
    if ($seenNames.ContainsKey($name)) { throw "Duplicate manifest macro '$name'." }
    $seenNames[$name] = $true
    $matchCount = if ($defines.ContainsKey($name)) { $defines[$name].Count } else { 0 }
    if ($matchCount -ne 1) { throw "Replacement '$name' requires exactly one source definition; found $matchCount." }
    $oldValue = Get-RequiredProperty $replacement 'oldValue'
    $definition = $defines[$name][0]
    if ($definition.Value -cne $oldValue) { throw "Replacement '$name' source value mismatch: expected '$oldValue', got '$($definition.Value)'." }
    $newValue = Get-RequiredProperty $replacement 'newValue'
    if ([string]::IsNullOrWhiteSpace($newValue)) { throw "Replacement '$name' has an empty newValue." }
    [void](Get-RequiredProperty $replacement 'classification')
    $sourceLines[$definition.Index] = "#define $name $newValue"
    $changes += [pscustomobject]@{ action = 'replace'; name = $name; oldValue = $oldValue; newValue = $newValue; classification = $replacement.classification }
}

$additions = @()
foreach ($addition in @($manifest.additions)) {
    Assert-OnlyProperties $addition @('name', 'expectedSource', 'newValue', 'classification') 'Addition'
    $name = Get-RequiredProperty $addition 'name'
    if ($seenNames.ContainsKey($name)) { throw "Duplicate manifest macro '$name'." }
    $seenNames[$name] = $true
    if ((Get-RequiredProperty $addition 'expectedSource') -cne '<absent>') { throw "Addition '$name' must explicitly expect <absent>." }
    if ($defines.ContainsKey($name)) { throw "Addition '$name' expected no source definition." }
    $newValue = Get-RequiredProperty $addition 'newValue'
    if ([string]::IsNullOrWhiteSpace($newValue)) { throw "Addition '$name' has an empty newValue." }
    [void](Get-RequiredProperty $addition 'classification')
    $additions += [pscustomobject]@{ action = 'add'; name = $name; oldValue = '<absent>'; newValue = $newValue; classification = $addition.classification }
}

$derived = @()
foreach ($assertion in @($manifest.derivedAssertions)) {
    Assert-OnlyProperties $assertion @('name', 'sourceCondition', 'targetValue', 'classification') 'Derived assertion'
    $name = Get-RequiredProperty $assertion 'name'
    if ($seenNames.ContainsKey($name)) { throw "Macro '$name' cannot be both replaced and derived." }
    $seenNames[$name] = $true
    $condition = Get-RequiredProperty $assertion 'sourceCondition'
    if (-not (($sourceLines -join "`n") -match [regex]::Escape($condition))) {
        throw "Derived assertion '$name' condition was not found in the pinned source."
    }
    $derived += [pscustomobject]@{ name = $name; sourceCondition = $condition; targetValue = Get-RequiredProperty $assertion 'targetValue'; classification = Get-RequiredProperty $assertion 'classification' }
}

$guardIndex = [Array]::FindIndex([string[]]$sourceLines, [Predicate[string]]{ param($line) $line -match '^#define _BX_CONFIG_H_ 1\s*$' })
if ($guardIndex -lt 0) { throw 'Pinned source has no recognized _BX_CONFIG_H_ guard insertion point.' }
if ($additions.Count -gt 0) {
    $prefix = @($sourceLines[0..$guardIndex])
    $suffix = @($sourceLines[($guardIndex + 1)..($sourceLines.Count - 1)])
    $additionLines = foreach ($addition in $additions) { "#define $($addition.name) $($addition.newValue)" }
    $sourceLines = @($prefix + $additionLines + $suffix)
    $changes += $additions
}

$outputText = ($sourceLines -join "`r`n") + "`r`n"
New-Item -ItemType Directory -Path $AnalysisRoot -Force | Out-Null
$outputPath = Join-Path $AnalysisRoot 'config.h'
[System.IO.File]::WriteAllText($outputPath, $outputText, [System.Text.UTF8Encoding]::new($false))
$outputHash = (Get-FileHash -LiteralPath $outputPath -Algorithm SHA256).Hash.ToUpperInvariant()
$report = [ordered]@{
    schemaVersion = 1
    inputConfig = (Resolve-Path -LiteralPath $InputConfigPath).Path
    inputSha256 = $actualHash
    outputConfig = (Resolve-Path -LiteralPath $outputPath).Path
    outputSha256 = $outputHash
    replacements = @($changes | Sort-Object name)
    derivedAssertions = @($derived | Sort-Object name)
}
$reportPath = Join-Path $AnalysisRoot 'projection-report.json'
$report | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath $reportPath -Encoding utf8
Write-Host "Projected pinned Bochs config to $outputPath"
Write-Host "Input SHA-256:  $actualHash"
Write-Host "Output SHA-256: $outputHash"
