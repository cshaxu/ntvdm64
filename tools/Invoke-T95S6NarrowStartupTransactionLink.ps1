[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$BuildRoot,
    [switch]$EnableUnmatchedUdDiagnostic,
    [switch]$EnableBopCatalogListener,
    [switch]$EnableCpuResultBridge,
    [int]$TimeoutSeconds = 180
)

$ErrorActionPreference = 'Stop'

if ($TimeoutSeconds -lt 1) {
    throw 'TimeoutSeconds must be positive.'
}

$build = [IO.Path]::GetFullPath($BuildRoot)
$rootRecordPath = Join-Path $build 'narrow-startup-transaction-build-root.json'
$shim = Join-Path $build 'ntdos64-native-startup-transaction.mak'
$wrapper = Join-Path $build 'run-narrow-startup-transaction-link.cmd'
$log = Join-Path $build 'narrow-startup-transaction-build.log'
$exitRecord = Join-Path $build 'narrow-startup-transaction-build.exitcode'

if (-not (Test-Path -LiteralPath $rootRecordPath -PathType Leaf)) {
    throw "Missing narrow startup build-root record: $rootRecordPath"
}
if (-not (Test-Path -LiteralPath $shim -PathType Leaf)) {
    throw "Missing narrow startup shim: $shim"
}
if ((Test-Path -LiteralPath $wrapper) -or
    (Test-Path -LiteralPath $log) -or
    (Test-Path -LiteralPath $exitRecord)) {
    throw 'Refusing to overwrite an existing narrow startup link record.'
}

$record = Get-Content -LiteralPath $rootRecordPath -Raw | ConvertFrom-Json
if ($record.schema -ne 'ntdos64.t95-s6.narrow-startup-transaction-build-root.v1') {
    throw 'Unsupported narrow startup build-root record.'
}
if ($record.closureObjects.Count -ne (20 + $(if ($EnableBopCatalogListener) { 1 } else { 0 }) + $(if ($EnableCpuResultBridge) { 13 } else { 0 }))) {
    throw 'Unexpected narrow startup closure object count.'
}
$expectedMacros = @(
    'BX_NTVDM_ENABLE_EXECUTION_PLAN=1',
    'BX_NTVDM_ENABLE_STARTUP_TRANSACTION=1'
)
if ($EnableUnmatchedUdDiagnostic) {
    $expectedMacros += 'BX_NTVDM_ENABLE_UNMATCHED_UD_DIAGNOSTIC=1'
}
if ($EnableBopCatalogListener) {
    $expectedMacros += 'BX_NTVDM_ENABLE_BOP_CATALOG_LISTENER=1'
}
if ($EnableCpuResultBridge) {
    $expectedMacros += 'BX_NTVDM_ENABLE_CPU_RESULT_BRIDGE=1'
}
if (($record.rootLocalOptInMacros -join ',') -ne ($expectedMacros -join ',')) {
    throw 'Unexpected narrow startup macro set.'
}
if ([bool]$record.unmatchedUdDiagnosticEnabled -ne [bool]$EnableUnmatchedUdDiagnostic) {
    throw 'Narrow startup diagnostic opt-in does not match its build-root record.'
}
if ([bool]$record.bopCatalogListenerEnabled -ne [bool]$EnableBopCatalogListener) {
    throw 'Narrow startup BOP listener opt-in does not match its build-root record.'
}
if ([bool]$record.cpuResultBridgeEnabled -ne [bool]$EnableCpuResultBridge) {
    throw 'Narrow startup CPU-result bridge opt-in does not match its build-root record.'
}
if ($record.absentMacro -ne 'BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT') {
    throw 'Unexpected broad-interceptor assertion.'
}

$vsdev = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (-not (Test-Path -LiteralPath $vsdev -PathType Leaf)) {
    throw "Missing BuildTools environment script: $vsdev"
}

$wrapperText = @"
@echo off
setlocal EnableExtensions EnableDelayedExpansion
call "$vsdev" -arch=x86 -host_arch=x86
if errorlevel 1 goto record
cd /d "$build"
nmake.exe /f ntdos64-native-startup-transaction.mak ntdos64-native-startup-transaction.exe > "$log" 2>&1
:record
set "NTDOS64_EXITCODE=!ERRORLEVEL!"
> "$exitRecord" echo !NTDOS64_EXITCODE!
exit /b !NTDOS64_EXITCODE!
"@
[IO.File]::WriteAllText($wrapper, $wrapperText, [Text.UTF8Encoding]::new($false))

$process = Start-Process -FilePath $env:ComSpec -ArgumentList @('/d', '/c', $wrapper) -WindowStyle Hidden -PassThru
if (-not $process.WaitForExit($TimeoutSeconds * 1000)) {
    $process.Kill()
    $process.WaitForExit()
    throw "Narrow startup link timed out after $TimeoutSeconds seconds; exact wrapper PID $($process.Id) was terminated."
}
if (-not (Test-Path -LiteralPath $exitRecord -PathType Leaf)) {
    throw 'Narrow startup wrapper did not produce an exit record.'
}
$recordedExit = [int](Get-Content -LiteralPath $exitRecord -Raw).Trim()
if ($process.ExitCode -ne $recordedExit) {
    throw "Narrow startup process/recorded exit mismatch: $($process.ExitCode)/$recordedExit"
}
if ($recordedExit -ne 0) {
    throw "Narrow startup named target failed with exit $recordedExit; see $log"
}
Write-Output "Narrow startup link completed: $build"
