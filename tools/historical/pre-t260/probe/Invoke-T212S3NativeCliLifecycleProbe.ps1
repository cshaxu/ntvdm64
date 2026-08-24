param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$BuildRoot,
    [Parameter(Mandatory = $true)][string]$CompositionBuildRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = [IO.Path]::GetFullPath($BuildRoot)
& (Join-Path $root 'tools\probe\Invoke-T200S27NativeCliProbe.ps1') `
    -RepositoryRoot $root -BuildRoot $build -CompositionBuildRoot $CompositionBuildRoot `
    -AdditionalCliSources @('src\cli\ntdos64_lifecycle_v1.c') -ExpectedRunExitCode 3
$baseRecord = Get-Content -LiteralPath (Join-Path $build 't200-s27-native-cli.json') -Raw | ConvertFrom-Json
if (-not $baseRecord.passed -or $baseRecord.runExitCode -ne 3 -or
    $baseRecord.expectedRunExitCode -ne 3) {
    throw 'T212 S3 native CLI lifecycle probe did not produce its declared composition rejection.'
}
$runLog = Get-Content -LiteralPath (Join-Path $build 'run.log') -Raw
if ($runLog -notmatch 'terminal=.+lifecycle=.+presentation=.+cancellation=') {
    throw 'T212 S3 native CLI did not emit lifecycle classification fields'
}
$record = [ordered]@{
    schema = 'ntdos64.t212.s3.native-cli-lifecycle.v1'
    architecture = 'x64'
    runtimeLibrary = '/MT'
    sourceClosure = @('src/cli/ntdos64_native_cli.c', 'src/cli/ntdos64_lifecycle_v1.[hc]', 'existing current composition objects')
    engineAbiChanged = $false
    bochsChanged = $false
    runExitCode = 3
    expectedRunExitCode = 3
    passed = $true
}
$record | ConvertTo-Json -Depth 3 | Set-Content -LiteralPath (Join-Path $build 't212-s3-native-cli-lifecycle.json') -Encoding utf8
Write-Host "T212 S3 native CLI lifecycle probe passed: $build"
