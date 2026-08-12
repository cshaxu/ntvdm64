[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$BuildRoot = ''
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
}
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
if ([string]::IsNullOrWhiteSpace($BuildRoot)) {
    $BuildRoot = Join-Path $repository 'artifacts\build\bochs-2.6-native-adapter-msvc-r1'
}
$build = [IO.Path]::GetFullPath($BuildRoot)
$rootRecordPath = Join-Path $build 'narrow-companion-build-root.json'
$shim = Join-Path $build 'ntdos64-native-adapter-observation.mak'
$recordPath = Join-Path $build 'narrow-companion-link-observation.json'
if (-not (Test-Path -LiteralPath $rootRecordPath -PathType Leaf)) { throw "Missing build-root record: $rootRecordPath" }
if (-not (Test-Path -LiteralPath $shim -PathType Leaf)) { throw "Missing companion shim: $shim" }
if (Test-Path -LiteralPath $recordPath) { throw "Observation already recorded: $recordPath" }
$rootRecord = Get-Content -LiteralPath $rootRecordPath -Raw | ConvertFrom-Json
if ($rootRecord.schema -ne 'ntdos64.t95-s6.narrow-companion-build-root.v1') { throw 'Unsupported companion build-root record.' }
if ((Get-FileHash -LiteralPath $shim -Algorithm SHA256).Hash -ne $rootRecord.shimSha256) { throw 'Companion shim hash mismatch.' }

$log = Join-Path $build 'narrow-companion-link.log'
$exitCodePath = Join-Path $build 'narrow-companion-link.exitcode'
$wrapper = Join-Path $build 'run-narrow-companion-link.cmd'
$wrapperText = @"
@echo off
setlocal
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x86
if errorlevel 1 goto record
cd /d "$build"
nmake.exe /f ntdos64-native-adapter-observation.mak ntdos64-native-adapter-observation.exe > "$log" 2>&1
:record
set "NTDOS64_EXITCODE=%ERRORLEVEL%"
> "$exitCodePath" echo %NTDOS64_EXITCODE%
exit /b %NTDOS64_EXITCODE%
"@
[IO.File]::WriteAllText($wrapper, $wrapperText, [Text.UTF8Encoding]::new($false))
$process = Start-Process -FilePath $env:ComSpec -ArgumentList @('/d', '/c', "`"$wrapper`"") -WindowStyle Hidden -PassThru -Wait
if (-not (Test-Path -LiteralPath $exitCodePath -PathType Leaf)) { throw "Wrapper did not produce exit code: $exitCodePath" }
$exitCode = [int](Get-Content -LiteralPath $exitCodePath -Raw).Trim()
$output = Join-Path $build 'ntdos64-native-adapter-observation.exe'
$map = Join-Path $build 'ntdos64-native-adapter-observation.map'
$record = [ordered]@{
    schema = 'ntdos64.t95-s6.narrow-companion-link-observation.v1'
    buildRoot = $build
    rootRecord = $rootRecordPath
    command = $rootRecord.permittedCommand
    processExitCode = $process.ExitCode
    recordedExitCode = $exitCode
    wrapperSha256 = (Get-FileHash -LiteralPath $wrapper -Algorithm SHA256).Hash
    log = $log
    logSha256 = if (Test-Path -LiteralPath $log -PathType Leaf) { (Get-FileHash -LiteralPath $log -Algorithm SHA256).Hash } else { $null }
    outputExists = Test-Path -LiteralPath $output -PathType Leaf
    outputSha256 = if (Test-Path -LiteralPath $output -PathType Leaf) { (Get-FileHash -LiteralPath $output -Algorithm SHA256).Hash } else { $null }
    mapExists = Test-Path -LiteralPath $map -PathType Leaf
    mapSha256 = if (Test-Path -LiteralPath $map -PathType Leaf) { (Get-FileHash -LiteralPath $map -Algorithm SHA256).Hash } else { $null }
    retryPolicy = 'none'
}
[IO.File]::WriteAllText($recordPath, ($record | ConvertTo-Json -Depth 4), [Text.UTF8Encoding]::new($false))
if ($exitCode -ne 0) { throw "Narrow companion link failed; see $log" }
Write-Host "Narrow companion link completed: $output"
