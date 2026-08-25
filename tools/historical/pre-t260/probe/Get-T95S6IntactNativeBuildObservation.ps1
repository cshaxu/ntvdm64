[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$BuildRoot
)

$ErrorActionPreference = 'Stop'
$build = [IO.Path]::GetFullPath($BuildRoot)
$recordPath = Join-Path $build 'intact-native-build-observation.json'
if (-not (Test-Path -LiteralPath $recordPath -PathType Leaf)) { throw "Missing observation record: $recordPath" }
$record = Get-Content -LiteralPath $recordPath -Raw | ConvertFrom-Json
$process = Get-Process -Id $record.processId -ErrorAction SilentlyContinue
$exitCode = $null
if (Test-Path -LiteralPath $record.exitCodeFile -PathType Leaf) { $exitCode = (Get-Content -LiteralPath $record.exitCodeFile -Raw).Trim() }
$logBytes = if (Test-Path -LiteralPath $record.log -PathType Leaf) { (Get-Item -LiteralPath $record.log).Length } else { 0 }
[ordered]@{
    schema = $record.schema
    buildRoot = $build
    processId = $record.processId
    processRunning = ($null -ne $process)
    exitCode = $exitCode
    logBytes = $logBytes
    targetExists = (Test-Path -LiteralPath (Join-Path $build 'runner-native-container.exe') -PathType Leaf)
    observationComplete = ($null -ne $exitCode)
} | ConvertTo-Json
