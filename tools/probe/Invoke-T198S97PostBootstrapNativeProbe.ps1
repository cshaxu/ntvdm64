param(
    [string]$RepositoryRoot = '',
    [Parameter(Mandatory = $true)][string]$BuildRoot
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
$build = [IO.Path]::GetFullPath($BuildRoot)
$inner = Join-Path $repository 'tools\probe\Invoke-T198S94SourceBuiltNormalReturnNativeProbe.ps1'
if (Test-Path -LiteralPath $build) {
    throw "Refusing to overwrite existing build root: $build"
}
if (-not (Test-Path -LiteralPath $inner -PathType Leaf)) {
    throw "Required exact-input native probe missing: $inner"
}

# S97 deliberately reuses S94's exact immutable input preparation and its
# one-run harness.  The current composition manifest is rebuilt by that
# harness, so this wrapper changes neither guest input nor run cardinality.
& $inner -RepositoryRoot $repository -BuildRoot $build
if ($LASTEXITCODE -ne 0) {
    throw "S97 exact-input native probe failed: $LASTEXITCODE"
}

$innerRecord = Join-Path $build 't198-s94-source-built-normal-return-native-probe.json'
if (-not (Test-Path -LiteralPath $innerRecord -PathType Leaf)) {
    throw "S97 required inner provenance record missing: $innerRecord"
}
$record = Get-Content -LiteralPath $innerRecord -Raw | ConvertFrom-Json
if ($record.runs -ne 1) {
    throw "S97 requires exactly one guest run; inner record reports $($record.runs)"
}
[ordered]@{
    schema = 'ntdos64.t198.s97.post-bootstrap-native-probe.v1'
    reusedHarness = 'tools/probe/Invoke-T198S94SourceBuiltNormalReturnNativeProbe.ps1'
    inputContract = 'S93 exact source-built COMMAND.COM/SHARE.EXE and v7 profile'
    currentCompositionManifest = 'tools/build/t198-s50-bx-vdm-composition-manifest.json'
    runs = $record.runs
    guestRunExitCode = $record.runExitCode
    innerRecord = 't198-s94-source-built-normal-return-native-probe.json'
    logs = $record.logs
} | ConvertTo-Json -Depth 4 |
    Set-Content -LiteralPath (Join-Path $build 't198-s97-post-bootstrap-native-probe.json') -Encoding utf8
Write-Host "S97 one exact-input post-bootstrap native observation retained at: $build"
