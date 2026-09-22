[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [string]$BuildRoot,
    [string]$RuntimeRoot = 'O:\winnt',
    [string]$ShadowFixture = ''
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$build = (Resolve-Path -LiteralPath $BuildRoot).Path
$runtime = (Resolve-Path -LiteralPath $RuntimeRoot).Path
$testRoot = Join-Path $runtime 'tests'
$trace = Join-Path $runtime 'logs\s44-vdd-shadow.log'
$output = Join-Path $runtime 'logs\s44-vdd-shadow-mem.log'
$errorOutput = Join-Path $runtime 'logs\s44-vdd-shadow-mem.stderr.log'
$bopOutput = Join-Path $runtime 'logs\s44-vdd-shadow-bop.log'
$bopErrorOutput = Join-Path $runtime 'logs\s44-vdd-shadow-bop.stderr.log'
$restartOutput = Join-Path $runtime 'logs\s44-vdd-shadow-restart.log'
$restartErrorOutput = Join-Path $runtime 'logs\s44-vdd-shadow-restart.stderr.log'
$shadow = Join-Path $runtime 'NTVDM.REG'
$testVdd = Join-Path $testRoot 's44-entry-hook-vdd.dll'
$bopVdd = Join-Path $testRoot 's44-bop-vdd.dll'
$bopGuest = Join-Path $testRoot 'S44BOP.COM'
$repository = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..\..')).Path
$defaultShadowFixture = Join-Path $PSScriptRoot '..\mvdm-host\fixtures\S44-ENTRY-HOOK-NTVDM.REG'
if (!$ShadowFixture) { $ShadowFixture = $defaultShadowFixture }
$ShadowFixture = (Resolve-Path -LiteralPath $ShadowFixture).Path
$bopBuild = Join-Path $build 's44-vdd-bop'
$processes = @(Get-Process ntvdm,basesrv,run16 -ErrorAction SilentlyContinue)

if ($processes.Count -ne 0) {
    throw 'S44 shadow test requires no existing run16/basesrv/ntvdm session.'
}
New-Item -ItemType Directory -Force -Path $testRoot | Out-Null
New-Item -ItemType Directory -Force -Path (Join-Path $runtime 'logs') | Out-Null
& powershell.exe -NoProfile -ExecutionPolicy Bypass -File (Join-Path $repository 'tools\build\Build-T420S44VddGuestTest.ps1') `
    -RepositoryRoot $repository -BuildRoot $bopBuild
if ($LASTEXITCODE -ne 0) { throw 'Failed to build S44BOP.COM.' }
foreach ($name in @('run16.exe','basesrv.exe','ntvdm.exe','dtmgr.exe')) {
    Copy-Item -LiteralPath (Join-Path $build $name) -Destination (Join-Path $runtime $name) -Force
}
Copy-Item -LiteralPath (Join-Path $build 's44-entry-hook-vdd.dll') -Destination $testVdd -Force
Copy-Item -LiteralPath (Join-Path $build 's44-bop-vdd.dll') -Destination $bopVdd -Force
Copy-Item -LiteralPath (Join-Path $bopBuild 'S44BOP.COM') -Destination $bopGuest -Force
Copy-Item -LiteralPath $ShadowFixture -Destination $shadow -Force
Remove-Item -LiteralPath $trace,$output,$errorOutput,$bopOutput,$bopErrorOutput,$restartOutput,$restartErrorOutput -Force -ErrorAction SilentlyContinue

try {
    $env:S44_VDD_TRACE_PATH = $trace
    $process = Start-Process -FilePath (Join-Path $runtime 'run16.exe') -ArgumentList 'MEM.EXE' `
        -WorkingDirectory $runtime -RedirectStandardOutput $output -RedirectStandardError $errorOutput `
        -WindowStyle Hidden -PassThru
    if (!$process.WaitForExit(30000)) {
        Stop-Process -Id $process.Id -Force
        throw 'run16 MEM.EXE timed out.'
    }
    # Start-Process cannot expose the exit code of this console-parent route
    # reliably after its VDM completion object has been consumed.  Completion
    # is asynchronous with the worker's final Console/notification writes;
    # the transcript and provider markers below are the product contract.
    $process.Refresh()
    if (!$process.HasExited) { throw 'run16 MEM.EXE did not complete.' }
    $deadline = [DateTime]::UtcNow.AddSeconds(10)
    do {
        $markers = if (Test-Path -LiteralPath $trace) { @(Get-Content -LiteralPath $trace) } else { @() }
        $text = if (Test-Path -LiteralPath $output) { Get-Content -LiteralPath $output -Raw } else { '' }
        $ready = $text -match 'bytes total conventional memory' -and
            $text -match 'MS-DOS resident in High Memory Area' -and
            @(@('CREATE','TERMINATE','BLOCK') | Where-Object { $markers -notcontains $_ }).Count -eq 0
        if (!$ready) { Start-Sleep -Milliseconds 100 }
    } while (!$ready -and [DateTime]::UtcNow -lt $deadline)
    if (!$ready) {
        throw 'MEM did not produce the required VDD callback and guest-text evidence.'
    }
    if ($text -match 'Bad command or filename') {
        throw 'MEM reported a DOS command failure.'
    }
    $bop = Start-Process -FilePath (Join-Path $runtime 'run16.exe') -ArgumentList 'tests\S44BOP.COM' `
        -WorkingDirectory $runtime -RedirectStandardOutput $bopOutput -RedirectStandardError $bopErrorOutput `
        -WindowStyle Hidden -PassThru
    if (!$bop.WaitForExit(30000)) {
        Stop-Process -Id $bop.Id -Force
        throw 'run16 S44BOP.COM timed out.'
    }
    $deadline = [DateTime]::UtcNow.AddSeconds(10)
    do {
        $bopText = if (Test-Path -LiteralPath $bopOutput) { Get-Content -LiteralPath $bopOutput -Raw } else { '' }
        $markers = if (Test-Path -LiteralPath $trace) { @(Get-Content -LiteralPath $trace) } else { @() }
        $required = @('BOP_INIT','BOP_DISPATCH','HOOK_INSTALL_OK','HOOK_INVALID_OK',
            'IO_INSTALL_OK','IO_INB','IO_OUTB','TERMINATE','BLOCK','DETACH','HOOK_REMOVE_OK')
        $ready = $bopText -match 'S44BOP OK' -and
            @($required | Where-Object { $markers -notcontains $_ }).Count -eq 0
        if (!$ready) { Start-Sleep -Milliseconds 100 }
    } while (!$ready -and [DateTime]::UtcNow -lt $deadline)
    if (!$ready -or $bopText -match 'S44BOP FAIL') {
        throw 'Original BOP 58 guest registration/dispatch/deregistration failed.'
    }

    Write-Output 'S44_VDD_SHADOW_END_TO_END_OK'
}
finally {
    $created = @(Get-Process ntvdm,basesrv,run16 -ErrorAction SilentlyContinue)
    if ($created.Count -ne 0) { $created | Stop-Process -Force }
    Remove-Item -LiteralPath $shadow,$testVdd,$bopVdd,$bopGuest -Force -ErrorAction SilentlyContinue
}
