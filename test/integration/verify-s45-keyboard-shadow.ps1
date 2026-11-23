[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [string]$BuildRoot,
    [string]$RuntimeRoot = 'O:\winnt'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$repository = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..\..')).Path
$build = (Resolve-Path -LiteralPath $BuildRoot).Path
$runtime = (Resolve-Path -LiteralPath $RuntimeRoot).Path
$logs = Join-Path $runtime 'logs'
$tests = Join-Path $runtime 'tests'
$fixture = Join-Path $repository 'test\mvdm-host\fixtures\S45-KEYBOARD-NTVDM.REG'
$builder = Join-Path $build 's45-keyboard-profile-builder.exe'
$observer = Join-Path $build 's45-pif-detached-observer.exe'
$reader = Join-Path $build 's45-cmdkeyb-shadow-fixture.exe'
$report = Join-Path $logs 's45-keyboard-pif-console.txt'
$rpcTrace = Join-Path $logs 's45-keyboard-pif-rpc.log'
$registryTrace = Join-Path $logs 's45-keyboard-pif-registry.log'
$readerTrace = Join-Path $build 's45-cmdkeyb-shadow-fixture.trace'
$shadow = Join-Path $runtime 'NTVDM.REG'
$pif = Join-Path $tests 'S45KEY.PIF'

if (@(Get-Process ntvdm,basesrv,run16 -ErrorAction SilentlyContinue).Count -ne 0) {
    throw 'S45 keyboard test requires no existing run16/basesrv/ntvdm session.'
}
if (!(Test-Path -LiteralPath $fixture)) { throw "Required input is absent: $fixture" }
# Keep the compiler environment in its own child; original COMMAND must not
# inherit toolchain variables from construction of these test-only helpers.
$producer=Start-Process -FilePath $env:ComSpec -ArgumentList ('/d /s /c ""'+(Join-Path $repository 'tools/build-fixtures.cmd')+'" x86 command-keyboard-shadow"') -WorkingDirectory $repository -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $build 'keyboard-build.log') -RedirectStandardError (Join-Path $build 'keyboard-build.stderr.log')
try {
    "pid=$($producer.Id) started=$($producer.StartTime.ToUniversalTime().ToString('o'))" | Set-Content (Join-Path $build 'keyboard-producer.txt')
    if(!$producer.WaitForExit(300000)){
        $producer.Kill($true);$producer.WaitForExit()
        throw 'Keyboard producer timed out'
    }
    "exit=$($producer.ExitCode)" | Add-Content (Join-Path $build 'keyboard-producer.txt')
    if($producer.ExitCode){throw 'Keyboard producer failed'}
} finally {$producer.Dispose()}
$cache=Join-Path $repository 'build/.ninja/msvc-x86-release/worker'
foreach($destination in @($builder,$observer,$reader)) {
    $source=Join-Path $cache ([IO.Path]::GetFileName($destination))
    if(![string]::Equals($source,$destination,[StringComparison]::OrdinalIgnoreCase)) {
        Copy-Item -LiteralPath $source -Destination $destination
    }
}
New-Item -ItemType Directory -Force -Path $logs,$tests | Out-Null
foreach ($name in @('run16.exe','ntsrv.exe','ntvdm.exe','ntmon.exe')) {
    Copy-Item -LiteralPath (Join-Path $build $name) -Destination (Join-Path $runtime $name) -Force
}

try {
    & $builder $tests 'MEM.EXE'
    if ($LASTEXITCODE -ne 0) { throw 'S45 PIF fixture generation failed.' }
    Copy-Item -LiteralPath $fixture -Destination $shadow -Force
    Remove-Item -LiteralPath $report,$rpcTrace,$registryTrace,$readerTrace -Force -ErrorAction SilentlyContinue
    $env:MVDM_S34_TRACE_PATH = $rpcTrace
    $env:NTVDM_SHADOW_REGISTRY_TRACE = $registryTrace
    & $observer $report 'tests\S45KEY.PIF'
    if ($LASTEXITCODE -ne 0) { throw 'Real PIF/MEM Console observation failed.' }
    $screen = Get-Content -LiteralPath $report -Raw
    $registry = @(Get-Content -LiteralPath $registryTrace)
    $required = @(
        'OPEN_SECTION:HKEY_LOCAL_MACHINE\System\CurrentControlSet\Control\Keyboard Layout\DosKeybCodes',
        'OPEN_SECTION:HKEY_LOCAL_MACHINE\System\CurrentControlSet\Control\Keyboard Layout\DosKeybIDs',
        'QUERY_OK',
        'CLOSE'
    )
    if ($screen -notmatch 'bytes total conventional mem' -or
        $screen -match 'Bad command or filename' -or
        @($required | Where-Object { $registry -notcontains $_ }).Count -ne 0) {
        throw 'Real PIF/MEM keyboard path lacks required Console or original-reader evidence.'
    }

    $env:NTVDM_SHADOW_REGISTRY_TRACE = $readerTrace
    & $reader
    if ($LASTEXITCODE -ne 0) { throw 'Original cmdkeyb configured/absent/malformed fixture failed.' }
    $readerText = Get-Content -LiteralPath $readerTrace -Raw
    if ($readerText -notmatch 'QUERY_OK' -or $readerText -notmatch 'QUERY_MALFORMED' -or
        $readerText -notmatch 'QUERY_ABSENT') {
        throw 'Original cmdkeyb fixture lacks one required provider disposition.'
    }
    Write-Output 'S45_COMMAND_KEYBOARD_LAYERED_SHADOW_OK'
}
finally {
    Get-Process ntvdm,basesrv,run16 -ErrorAction SilentlyContinue | Stop-Process -Force
    Remove-Item -LiteralPath $shadow,$pif -Force -ErrorAction SilentlyContinue
}
