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
$fixture = Join-Path $repository 'tests\mvdm-host\fixtures\S45-KEYBOARD-NTVDM.REG'
$pifSource = Join-Path $repository 'tests\observation\s45_keyboard_profile_builder.c'
$observerSource = Join-Path $repository 'tests\observation\pif_detached_observer.c'
$readerSource = Join-Path $repository 'tests\mvdm-host\s45_cmdkeyb_shadow_fixture.c'
$cmdkeybObject = Join-Path $build 'obj\command\cmdkeyb.obj'
$providerObject = Join-Path $build 'obj\adapter-softpc\mvdm_shadow_registry.obj'
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
foreach ($path in @($fixture,$pifSource,$observerSource,$readerSource,$cmdkeybObject,$providerObject)) {
    if (!(Test-Path -LiteralPath $path)) { throw "Required input is absent: $path" }
}

$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$compiler = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.43.34808\bin\HostX86\x86\cl.exe'
$originalEnvironment = @{}
Get-ChildItem Env: | ForEach-Object { $originalEnvironment[$_.Name] = $_.Value }
$envLines = & cmd.exe /d /s /c ('call "' + $vs + '" -arch=x86 -host_arch=x86 >nul && set')
foreach ($line in $envLines) {
    $separator = $line.IndexOf('=')
    if ($separator -gt 0) {
        Set-Item -Path ('env:' + $line.Substring(0,$separator)) -Value $line.Substring($separator + 1)
    }
}

& $compiler /nologo /MT /W4 /I (Join-Path $repository 'src\opennt-abi\source\public\internal\windows\inc') $pifSource "/Fe:$builder"
if ($LASTEXITCODE -ne 0) { throw 'S45 PIF builder compilation failed.' }
& $compiler /nologo /MT /W4 $observerSource user32.lib "/Fe:$observer"
if ($LASTEXITCODE -ne 0) { throw 'S45 Console observer compilation failed.' }
& $compiler /nologo /MT /W4 $readerSource $cmdkeybObject $providerObject advapi32.lib user32.lib legacy_stdio_definitions.lib "/Fe:$reader"
if ($LASTEXITCODE -ne 0) { throw 'S45 cmdkeyb fixture compilation failed.' }

# The original DOS environment is inherited from the caller.  Build Tools
# variables (notably PATH and the VSCMD family) must not become accidental
# PIF/COMMAND test inputs, so restore the pre-build process environment before
# launching run16.
Get-ChildItem Env: | ForEach-Object {
    if (!$originalEnvironment.ContainsKey($_.Name)) { Remove-Item -LiteralPath ('Env:' + $_.Name) }
}
foreach ($name in $originalEnvironment.Keys) {
    Set-Item -LiteralPath ('Env:' + $name) -Value $originalEnvironment[$name]
}

New-Item -ItemType Directory -Force -Path $logs,$tests | Out-Null
foreach ($name in @('run16.exe','basesrv.exe','ntvdm.exe','dtmgr.exe')) {
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
