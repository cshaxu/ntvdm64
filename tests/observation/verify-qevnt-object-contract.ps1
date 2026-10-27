[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$BuildRoot,
    [string]$WorkerBuild = 'build/.ninja/msvc-x86-release/worker'
)
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
if (-not [IO.Path]::IsPathRooted($BuildRoot)) { $BuildRoot = Join-Path $repo $BuildRoot }
if (-not [IO.Path]::IsPathRooted($WorkerBuild)) { $WorkerBuild = Join-Path $repo $WorkerBuild }
$null = New-Item -ItemType Directory -Force -Path $BuildRoot
$BuildRoot = (Resolve-Path -LiteralPath $BuildRoot).Path
$driver = Join-Path $repo 'build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd'
$source = Join-Path $repo 'tests/mvdm-host/qevnt_object_contract_fixture.c'
$original = Join-Path $repo 'src/mvdm/softpc.new/base/system/qevnt.c'
$header = Join-Path $repo 'src/mvdm/softpc.new/host/inc/host_qev.h'
$object = Join-Path $WorkerBuild 'obj/system/qevnt.obj'
$exe = Join-Path $BuildRoot 'qevnt-object-contract.exe'
$fixtureObject = Join-Path $BuildRoot 'fixture.obj'
$receipt = [ordered]@{
    started = [DateTime]::UtcNow.ToString('o'); owner_pid = $PID
    result = 'running'; scope = 'Actual cached qevnt object; controlled host boundaries, no guest or production lifecycle acceptance'
    inputs = @(); runs = @()
}
try {
    foreach ($path in @($PSCommandPath, $driver, $source, $original, $header, $object, (Join-Path $WorkerBuild 'build.ninja'))) {
        $receipt.inputs += @{ path = $path; sha256 = (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash }
    }
    $command = '/d /s /c ""' + $driver + '" cl.exe /nologo /W4 /MT /Od /D_CRT_SECURE_NO_WARNINGS "' + $source + '" "' + $object + '" /Fo:"' + $fixtureObject + '" /Fe:"' + $exe + '" /link kernel32.lib"'
    $build = Start-Process cmd.exe -ArgumentList $command -WorkingDirectory $repo -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $BuildRoot 'build.stdout.txt') -RedirectStandardError (Join-Path $BuildRoot 'build.stderr.txt')
    $receipt.runs += @{ kind = 'build'; pid = $build.Id; command = $command }
    $receipt | ConvertTo-Json -Depth 8 | Set-Content -Encoding utf8 (Join-Path $BuildRoot 'result.json')
    $build.WaitForExit()
    $receipt.runs[-1].exit = $build.ExitCode
    if ($build.ExitCode -ne 0) { throw "Fixture build failed: $($build.ExitCode)" }
    $negativeCommand = '/d /s /c ""' + $driver + '" cl.exe /nologo /MT /c /DMVDM_QEV_EXPECT_HANDLE_BYTES=4 "' + $source + '" /Fo:"' + (Join-Path $BuildRoot 'negative-abi.obj') + '""'
    $negative = Start-Process cmd.exe -ArgumentList $negativeCommand -WorkingDirectory $repo -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $BuildRoot 'negative-abi.stdout.txt') -RedirectStandardError (Join-Path $BuildRoot 'negative-abi.stderr.txt')
    $receipt.runs += @{ kind = 'negative-abi-compile'; pid = $negative.Id; command = $negativeCommand }
    $receipt | ConvertTo-Json -Depth 8 | Set-Content -Encoding utf8 (Join-Path $BuildRoot 'result.json')
    $negative.WaitForExit()
    $receipt.runs[-1].exit = $negative.ExitCode
    if ($negative.ExitCode -eq 0 -or (Get-Content -Raw (Join-Path $BuildRoot 'negative-abi.stdout.txt')) -notmatch 'C2118') { throw 'Wrong-width ABI negative did not fail at the declared type check' }
    $run = Start-Process -FilePath $exe -WorkingDirectory $BuildRoot -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $BuildRoot 'run.stdout.txt') -RedirectStandardError (Join-Path $BuildRoot 'run.stderr.txt')
    $receipt.runs += @{ kind = 'fixture'; pid = $run.Id; diagnostic_budget_ms = 30000 }
    $receipt | ConvertTo-Json -Depth 8 | Set-Content -Encoding utf8 (Join-Path $BuildRoot 'result.json')
    if (-not $run.WaitForExit(30000)) { $run.Kill(); $run.WaitForExit(); throw 'Owned fixture exceeded diagnostic budget' }
    $run.WaitForExit()
    $receipt.runs[-1].exit = $run.ExitCode
    $text = Get-Content -Raw (Join-Path $BuildRoot 'run.stdout.txt')
    if ($run.ExitCode -ne 0 -or $text -notmatch 'QEVNT_OBJECT_CONTRACT checks=(\d+) failures=0 callbacks=(\d+)') { throw 'Original-object contract failed' }
    $receipt.checks = [int]$Matches[1]; $receipt.callbacks = [int]$Matches[2]
    foreach ($inputRecord in $receipt.inputs) {
        if ((Get-FileHash -LiteralPath $inputRecord.path -Algorithm SHA256).Hash -ne $inputRecord.sha256) { throw "Input changed during fixture: $($inputRecord.path)" }
    }
    $receipt.fixture_sha256 = (Get-FileHash -LiteralPath $exe -Algorithm SHA256).Hash
    $receipt.result = 'pass'
} catch {
    $receipt.result = 'fail'; $receipt.error = $_.Exception.Message
} finally {
    $receipt.finished = [DateTime]::UtcNow.ToString('o')
    $receipt | ConvertTo-Json -Depth 8 | Set-Content -Encoding utf8 (Join-Path $BuildRoot 'result.json')
}
$receipt | ConvertTo-Json -Depth 8
if ($receipt.result -ne 'pass') { exit 1 }
