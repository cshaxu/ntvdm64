[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$BuildRoot = '',
    [string]$ManifestPath = ''
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $scriptPath = $MyInvocation.MyCommand.Path
    if ([string]::IsNullOrWhiteSpace($scriptPath)) { throw 'RepositoryRoot was not supplied and script path is unavailable.' }
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $scriptPath)
}
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
if ([string]::IsNullOrWhiteSpace($BuildRoot)) { $BuildRoot = Join-Path $repository 'artifacts\build\bochs-2.6-native-intact-msvc-r2' }
if ([string]::IsNullOrWhiteSpace($ManifestPath)) { $ManifestPath = Join-Path $repository 'artifacts\analysis\t95-s6-intact-native-tuple-manifest-001-20260811-001\intact-native-tuple-manifest.json' }
$build = [IO.Path]::GetFullPath($BuildRoot)
if (Test-Path -LiteralPath $build) { throw "Refusing to launch into existing build directory: $build" }

$prepare = Join-Path $repository 'tools\build\New-T95S6IntactNativeBuildRoot.ps1'
& $prepare -RepositoryRoot $repository -BuildRoot $build -ManifestPath $ManifestPath

$recordPath = Join-Path $build 'intact-native-build-observation.json'
if (Test-Path -LiteralPath $recordPath) { throw "Observation record already exists: $recordPath" }
$wrapperPath = Join-Path $build 'run-intact-native-container.cmd'
$logPath = Join-Path $build 'intact-native-container-build-002.log'
$exitPath = Join-Path $build 'intact-native-container-build-002.exitcode'
$exitTemp = "$exitPath.tmp"
$wrapper = @"
@echo off
setlocal
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x86
if errorlevel 1 goto record
cd /d "$build"
nmake.exe /f ntdos64-native-container.mak ntdos64-native-container.exe > "$logPath" 2>&1
:record
set "NTDOS64_EXITCODE=%ERRORLEVEL%"
> "$exitTemp" echo %NTDOS64_EXITCODE%
move /y "$exitTemp" "$exitPath" >nul
exit /b %NTDOS64_EXITCODE%
"@
[IO.File]::WriteAllText($wrapperPath, $wrapper, [Text.UTF8Encoding]::new($false))
$arguments = '/d /c ""' + $wrapperPath + '""'
$process = Start-Process -FilePath $env:ComSpec -ArgumentList $arguments -WindowStyle Hidden -PassThru
$record = [ordered]@{
    schema = 'ntdos64.t95-s6.intact-native-detached-observation.v1'
    buildRoot = $build
    manifest = [IO.Path]::GetFullPath($ManifestPath)
    wrapper = $wrapperPath
    wrapperSha256 = (Get-FileHash -LiteralPath $wrapperPath -Algorithm SHA256).Hash
    log = $logPath
    exitCodeFile = $exitPath
    processId = $process.Id
    processStartTimeUtc = $process.StartTime.ToUniversalTime().ToString('o')
    command = 'nmake /f ntdos64-native-container.mak ntdos64-native-container.exe'
    retryPolicy = 'none'
}
[IO.File]::WriteAllText($recordPath, ($record | ConvertTo-Json -Depth 4), [Text.UTF8Encoding]::new($false))
Write-Host "Started intact native build observation PID $($process.Id): $build"
