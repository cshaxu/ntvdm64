[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$BuildRoot,
    [ValidateSet('qevnt','ios','gfi','keycode','nls','umb','vdd','niddb')][string]$Contract = 'qevnt'
)
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
if (-not [IO.Path]::IsPathRooted($BuildRoot)) { $BuildRoot = Join-Path $repo $BuildRoot }
$WorkerBuild = Join-Path $repo 'build/.ninja/msvc-x86-release/worker'
$null = New-Item -ItemType Directory -Force -Path $BuildRoot
$BuildRoot = (Resolve-Path -LiteralPath $BuildRoot).Path
$driver = Join-Path $WorkerBuild 'run-ninja-parallel.cmd'
$source = Join-Path $repo "test/mvdm-host/${Contract}_object_contract_fixture.c"
$headers = @()
$extraObjects = @()
$extraSources = @()
if ($Contract -eq 'vdd') {
    $original = Join-Path $repo 'src/mvdm-host/softpc.new/host/src/nt_vdd.c'
    $headers = @('src/mvdm-host/softpc.new/host/inc/insignia.h','src/mvdm-host/softpc.new/host/inc/host_def.h','src/mvdm-host/softpc.new/host/inc/nt_vddp.h','src/mvdm-host/softpc.new/base/inc/ios.h','src/mvdm-host/softpc.new/base/inc/dma.h','src/non-mvdm/public/sdk/inc/nt_vdd.h') | ForEach-Object { Join-Path $repo $_ }
    $object = Join-Path $WorkerBuild 'obj/host/nt_vdd.obj'
    $extraObjects = @('obj/support/ios.obj','obj/ccpu/in.obj','obj/ccpu/out.obj') | ForEach-Object { Join-Path $WorkerBuild $_ }
    $extraSources = @('src/mvdm-host/softpc.new/base/support/ios.c','src/mvdm-host/softpc.new/base/ccpu386/in.c','src/mvdm-host/softpc.new/base/ccpu386/out.c') | ForEach-Object { Join-Path $repo $_ }
} elseif ($Contract -eq 'niddb') {
    $original = Join-Path $repo 'src/mvdm-host/softpc.new/base/bios/virtual.c'
    $headers = @('src/mvdm-host/softpc.new/base/inc/virtual.h','src/mvdm-host/softpc.new/base/inc/sas.h') | ForEach-Object { Join-Path $repo $_ }
    $object = Join-Path $WorkerBuild 'obj/bios/virtual.obj'
} elseif ($Contract -eq 'umb') {
    $original = Join-Path $repo 'src/mvdm-host/softpc.new/host/src/nt_umb.c'
    $headers = @('src/mvdm-host/softpc.new/host/inc/insignia.h','src/mvdm-host/softpc.new/host/inc/host_def.h','src/mvdm-host/softpc.new/base/inc/sas.h','src/mvdm-host/softpc.new/base/inc/umb.h','src/compat/include/generated/x86/prod/sas4gen.h') | ForEach-Object { Join-Path $repo $_ }
    $object = Join-Path $WorkerBuild 'obj/host/nt_umb.obj'
} elseif ($Contract -eq 'qevnt') {
    $original = Join-Path $repo 'src/mvdm-host/softpc.new/base/system/qevnt.c'
    $headers = @(Join-Path $repo 'src/mvdm-host/softpc.new/host/inc/host_qev.h')
    $object = Join-Path $WorkerBuild 'obj/system/qevnt.obj'
} elseif ($Contract -eq 'nls') {
    $original = Join-Path $repo 'src/mvdm-host/softpc.new/host/src/nt_nls.c'
    $headers = @('src/mvdm-host/softpc.new/host/inc/host_rrr.h','src/mvdm-host/softpc.new/host/inc/nt_uis.h','src/mvdm-host/softpc.new/obj.vdm/resource.rc') | ForEach-Object { Join-Path $repo $_ }
    $object = Join-Path $WorkerBuild 'obj/host/nt_nls.obj'
    $extraObjects = @(Join-Path $WorkerBuild 'obj/host/softpc-resource.res')
    $extraSources = @(Join-Path $WorkerBuild 'generated/softpc-embedded-roms.rc')
} elseif ($Contract -eq 'keycode') {
    $original = Join-Path $repo 'src/mvdm-host/softpc.new/host/src/nt_keycd.c'
    $headers = @('src/mvdm-host/softpc.new/host/inc/insignia.h','src/mvdm-host/softpc.new/base/inc/keyboard.h') | ForEach-Object { Join-Path $repo $_ }
    $object = Join-Path $WorkerBuild 'obj/host/nt_keycd.obj'
    $extraObjects = @(Join-Path $WorkerBuild 'obj/host/nt_input.obj')
    $extraSources = @(Join-Path $repo 'src/mvdm-host/softpc.new/host/src/nt_input.c')
} elseif ($Contract -eq 'gfi') {
    $original = Join-Path $repo 'src/mvdm-host/softpc.new/base/disks/gfi.c'
    $headers = @('src/mvdm-host/softpc.new/host/inc/insignia.h','src/mvdm-host/softpc.new/base/inc/xt.h','src/mvdm-host/softpc.new/base/inc/bios.h','src/mvdm-host/softpc.new/base/inc/config.h','src/mvdm-host/softpc.new/base/inc/gfi.h') | ForEach-Object { Join-Path $repo $_ }
    $object = Join-Path $WorkerBuild 'obj/disks/gfi.obj'
    $extraObjects = @(Join-Path $WorkerBuild 'obj/disks/gfi_mpty.obj')
    $extraSources = @(Join-Path $repo 'src/mvdm-host/softpc.new/base/disks/gfi_mpty.c')
} else {
    $original = Join-Path $repo 'src/mvdm-host/softpc.new/base/support/ios.c'
    $headers = @('src/mvdm-host/softpc.new/host/inc/insignia.h','src/mvdm-host/softpc.new/base/inc/base_def.h','src/mvdm-host/softpc.new/base/inc/ios.h','src/mvdm-host/softpc.new/base/ccpu386/in.h','src/mvdm-host/softpc.new/base/ccpu386/out.h') | ForEach-Object { Join-Path $repo $_ }
    $object = Join-Path $WorkerBuild 'obj/support/ios.obj'
    $extraObjects = @('obj/ccpu/in.obj','obj/ccpu/out.obj') | ForEach-Object { Join-Path $WorkerBuild $_ }
    $extraSources = @('src/mvdm-host/softpc.new/base/ccpu386/in.c','src/mvdm-host/softpc.new/base/ccpu386/out.c') | ForEach-Object { Join-Path $repo $_ }
}
$exe = Join-Path $BuildRoot "$Contract-object-contract.exe"
$scope = 'Actual cached original object; controlled host boundaries, no guest or production lifecycle acceptance'
$receipt = [ordered]@{
    started = [DateTime]::UtcNow.ToString('o'); owner_pid = $PID
    result = 'running'; contract = $Contract; scope = $scope
    build_root = $BuildRoot; fixture_path = $exe
    inputs = @(); runs = @()
}
try {
    $command = '/d /s /c ""' + (Join-Path $repo 'tools/build-fixtures.cmd') + '" x86 softpc-contracts"'
    $build = Start-Process cmd.exe -ArgumentList $command -WorkingDirectory $repo -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $BuildRoot 'build.stdout.txt') -RedirectStandardError (Join-Path $BuildRoot 'build.stderr.txt')
    $receipt.runs += @{kind='native-build';pid=$build.Id;command=$command}
    try {
        if(!$build.WaitForExit(120000)){throw 'Native fixture build deadline exceeded'}
        $build.WaitForExit();$receipt.runs[-1].exit=$build.ExitCode
        if($build.ExitCode){throw 'Native fixture build failed'}
    } finally {if(!$build.HasExited){$build.Kill($true);$build.WaitForExit()};$build.Dispose()}
    Copy-Item (Join-Path $WorkerBuild "$Contract-object-contract.exe") $exe
    foreach ($path in (@($PSCommandPath, $driver, $source, $original, $object, (Join-Path $WorkerBuild 'build.ninja')) + $headers + $extraObjects + $extraSources)) {
        $receipt.inputs += @{ path = $path; sha256 = (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash }
    }
    $negativeCommand = '/d /s /c ""' + $driver + '" softpc-' + $Contract + '-negative"'
    $negative = Start-Process cmd.exe -ArgumentList $negativeCommand -WorkingDirectory $repo -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $BuildRoot 'negative-abi.stdout.txt') -RedirectStandardError (Join-Path $BuildRoot 'negative-abi.stderr.txt')
    $receipt.runs += @{kind='negative-abi-compile';pid=$negative.Id;command=$negativeCommand}
    try {
        if(!$negative.WaitForExit(120000)){throw 'Negative compile deadline exceeded'}
        $negative.WaitForExit();$receipt.runs[-1].exit=$negative.ExitCode
        if($negative.ExitCode -eq 0 -or (Get-Content -Raw (Join-Path $BuildRoot 'negative-abi.stdout.txt')) -notmatch 'C2118'){throw 'Wrong-width ABI negative did not fail at the declared type check'}
    } finally {if(!$negative.HasExited){$negative.Kill($true);$negative.WaitForExit()};$negative.Dispose()}
    $run = Start-Process -FilePath $exe -WorkingDirectory $BuildRoot -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $BuildRoot 'run.stdout.txt') -RedirectStandardError (Join-Path $BuildRoot 'run.stderr.txt')
    $null = $run.Handle
    $receipt.runs += @{ kind = 'fixture'; pid = $run.Id; diagnostic_budget_ms = 30000 }
    $receipt | ConvertTo-Json -Depth 8 | Set-Content -Encoding utf8 (Join-Path $BuildRoot 'result.json')
    if (-not $run.WaitForExit(30000)) { $run.Kill(); $run.WaitForExit(); throw 'Owned fixture exceeded diagnostic budget' }
    $run.WaitForExit()
    $receipt.runs[-1].exit = $run.ExitCode
    $text = Get-Content -Raw (Join-Path $BuildRoot 'run.stdout.txt')
    $marker = $Contract.ToUpperInvariant() + '_OBJECT_CONTRACT checks=(\d+) failures=0 callbacks=(\d+)'
    if ($run.ExitCode -ne 0 -or $text -notmatch $marker) { throw 'Original-object contract failed' }
    $receipt.checks = [int]$Matches[1]; $receipt.callbacks = [int]$Matches[2]
    if ($Contract -eq 'niddb') {
        if ($text -notmatch '(?m)^NIDDB_THREE_VM_REUSE active=1 reused=1 callbacks=3 cleanup=1\r?$') {
            throw 'NIDDB three-VM slot-reuse contract was not completed'
        }
        $receipt.three_vm_reuse = 'pass'
        foreach ($mode in @('stale','out-of-range','active-stale')) {
            $invalid = Start-Process -FilePath $exe -ArgumentList "--$mode" -WorkingDirectory $BuildRoot -WindowStyle Hidden -PassThru `
                -RedirectStandardOutput (Join-Path $BuildRoot "$mode.stdout.txt") -RedirectStandardError (Join-Path $BuildRoot "$mode.stderr.txt")
            $null = $invalid.Handle
            $receipt.runs += @{ kind = "invalid-$mode"; pid = $invalid.Id }
            if (!$invalid.WaitForExit(30000)) { $invalid.Kill(); $invalid.WaitForExit(); throw "NIDDB $mode control exceeded diagnostic budget" }
            $invalid.WaitForExit()
            $code = [BitConverter]::ToUInt32([BitConverter]::GetBytes([int]$invalid.ExitCode),0)
            $receipt.runs[-1].exit = ('0x{0:X8}' -f $code)
            if ($code -ne [uint32]3221226528) { throw "NIDDB $mode did not fail through its explicit test seam: $($receipt.runs[-1].exit)" }
        }
        $receipt.invalid_identity_controls = 'pass (fixture termination seam)'
    }
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
exit 0
