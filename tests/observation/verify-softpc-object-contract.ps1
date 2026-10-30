[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$BuildRoot,
    [string]$WorkerBuild = 'build/.ninja/msvc-x86-release/worker',
    [ValidateSet('qevnt','ios','gfi','keycode','nls','umb','vdd','niddb')][string]$Contract = 'qevnt',
    [string]$SelectedObject = ''
)
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
if (-not [IO.Path]::IsPathRooted($BuildRoot)) { $BuildRoot = Join-Path $repo $BuildRoot }
if (-not [IO.Path]::IsPathRooted($WorkerBuild)) { $WorkerBuild = Join-Path $repo $WorkerBuild }
$null = New-Item -ItemType Directory -Force -Path $BuildRoot
$BuildRoot = (Resolve-Path -LiteralPath $BuildRoot).Path
$driver = Join-Path $repo 'build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd'
$source = Join-Path $repo "tests/mvdm-host/${Contract}_object_contract_fixture.c"
$headers = @()
$extraObjects = @()
$extraSources = @()
$extraFlags = ''
$extraLibraries = ''
$negativeSize = 4
if ($SelectedObject -and $Contract -ne 'niddb') { throw 'SelectedObject is limited to the NIDDB diagnostic' }
if ($Contract -eq 'vdd') {
    $original = Join-Path $repo 'src/mvdm/softpc.new/host/src/nt_vdd.c'
    $headers = @('src/mvdm/softpc.new/host/inc/insignia.h','src/mvdm/softpc.new/host/inc/host_def.h','src/mvdm/softpc.new/host/inc/nt_vddp.h','src/mvdm/softpc.new/base/inc/ios.h','src/mvdm/softpc.new/base/inc/dma.h','src/opennt-abi/source/public/sdk/inc/nt_vdd.h') | ForEach-Object { Join-Path $repo $_ }
    $object = Join-Path $WorkerBuild 'obj/host/nt_vdd.obj'
    $extraObjects = @('obj/support/ios.obj','obj/ccpu/in.obj','obj/ccpu/out.obj') | ForEach-Object { Join-Path $WorkerBuild $_ }
    $extraSources = @('src/mvdm/softpc.new/base/support/ios.c','src/mvdm/softpc.new/base/ccpu386/in.c','src/mvdm/softpc.new/base/ccpu386/out.c') | ForEach-Object { Join-Path $repo $_ }
    $negativeDefine = 'MVDM_VDD_EXPECT_HANDLER_BYTES'
    $extraFlags = '/DNTVDM /DSPC386 /DANSI /DPROD /I"' + (Join-Path $repo 'src/mvdm/softpc.new/host/inc') + '" /I"' + (Join-Path $repo 'src/mvdm/softpc.new/base/inc') + '" /I"' + (Join-Path $repo 'src/opennt-abi/source/public/sdk/inc') + '" '
} elseif ($Contract -eq 'niddb') {
    $original = Join-Path $repo 'src/mvdm/softpc.new/base/bios/virtual.c'
    $headers = @('src/mvdm/softpc.new/base/inc/virtual.h','src/mvdm/softpc.new/base/inc/sas.h') | ForEach-Object { Join-Path $repo $_ }
    $object = if ($SelectedObject) { (Resolve-Path -LiteralPath $SelectedObject).Path } else { Join-Path $WorkerBuild 'obj/bios/virtual.obj' }
    if ($SelectedObject -and !$object.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase)) { throw 'Diagnostic object must be under this repository build directory' }
    $negativeDefine = 'MVDM_NIDDB_EXPECT_POINTER_BYTES'
    $negativeSize = 8
} elseif ($Contract -eq 'umb') {
    $original = Join-Path $repo 'src/mvdm/softpc.new/host/src/nt_umb.c'
    $headers = @('src/mvdm/softpc.new/host/inc/insignia.h','src/mvdm/softpc.new/host/inc/host_def.h','src/mvdm/softpc.new/base/inc/sas.h','src/mvdm/softpc.new/base/inc/umb.h','src/ntvdm-exe/softpc/include/generated/x86/prod/sas4gen.h') | ForEach-Object { Join-Path $repo $_ }
    $object = Join-Path $WorkerBuild 'obj/host/nt_umb.obj'
    $negativeDefine = 'MVDM_UMB_EXPECT_NODE_BYTES'
    $extraFlags = '/DNTVDM /DSPC386 /DANSI /DCPU_40_STYLE /DCCPU /DPROD /I"' + (Join-Path $repo 'src/mvdm/softpc.new/host/inc') + '" /I"' + (Join-Path $repo 'src/mvdm/softpc.new/base/inc') + '" /I"' + (Join-Path $repo 'src/ntvdm-exe/softpc/include/generated/x86/prod') + '" '
} elseif ($Contract -eq 'qevnt') {
    $original = Join-Path $repo 'src/mvdm/softpc.new/base/system/qevnt.c'
    $headers = @(Join-Path $repo 'src/mvdm/softpc.new/host/inc/host_qev.h')
    $object = Join-Path $WorkerBuild 'obj/system/qevnt.obj'
    $negativeDefine = 'MVDM_QEV_EXPECT_HANDLE_BYTES'
} elseif ($Contract -eq 'nls') {
    $original = Join-Path $repo 'src/mvdm/softpc.new/host/src/nt_nls.c'
    $headers = @('src/mvdm/softpc.new/host/inc/host_rrr.h','src/mvdm/softpc.new/host/inc/nt_uis.h','src/mvdm/softpc.new/obj.vdm/resource.rc') | ForEach-Object { Join-Path $repo $_ }
    $object = Join-Path $WorkerBuild 'obj/host/nt_nls.obj'
    $extraObjects = @(Join-Path $WorkerBuild 'obj/host/softpc-resource.res')
    $extraSources = @(Join-Path $WorkerBuild 'generated/softpc-embedded-roms.rc')
    $negativeDefine = 'MVDM_NLS_EXPECT_CAPACITY'
    $extraFlags = '/I"' + (Join-Path $repo 'src/mvdm/softpc.new/host/inc') + '" '
    $extraLibraries = ' user32.lib'
} elseif ($Contract -eq 'keycode') {
    $original = Join-Path $repo 'src/mvdm/softpc.new/host/src/nt_keycd.c'
    $headers = @('src/mvdm/softpc.new/host/inc/insignia.h','src/mvdm/softpc.new/base/inc/keyboard.h') | ForEach-Object { Join-Path $repo $_ }
    $object = Join-Path $WorkerBuild 'obj/host/nt_keycd.obj'
    $extraObjects = @(Join-Path $WorkerBuild 'obj/host/nt_input.obj')
    $extraSources = @(Join-Path $repo 'src/mvdm/softpc.new/host/src/nt_input.c')
    $negativeDefine = 'MVDM_KEYCODE_EXPECT_EVENT_BYTES'
    $extraFlags = '/DNTVDM /DSPC386 /DANSI /I"' + (Join-Path $repo 'src/mvdm/softpc.new/host/inc') + '" /I"' + (Join-Path $repo 'src/mvdm/softpc.new/base/inc') + '" '
    $extraLibraries = ' user32.lib ntdll.lib'
} elseif ($Contract -eq 'gfi') {
    $original = Join-Path $repo 'src/mvdm/softpc.new/base/disks/gfi.c'
    $headers = @('src/mvdm/softpc.new/host/inc/insignia.h','src/mvdm/softpc.new/base/inc/xt.h','src/mvdm/softpc.new/base/inc/bios.h','src/mvdm/softpc.new/base/inc/config.h','src/mvdm/softpc.new/base/inc/gfi.h') | ForEach-Object { Join-Path $repo $_ }
    $object = Join-Path $WorkerBuild 'obj/disks/gfi.obj'
    $extraObjects = @(Join-Path $WorkerBuild 'obj/disks/gfi_mpty.obj')
    $extraSources = @(Join-Path $repo 'src/mvdm/softpc.new/base/disks/gfi_mpty.c')
    $negativeDefine = 'MVDM_GFI_EXPECT_TABLE_BYTES'
    $extraFlags = '/DNTVDM /DSPC386 /DANSI /I"' + (Join-Path $repo 'src/mvdm/softpc.new/host/inc') + '" /I"' + (Join-Path $repo 'src/mvdm/softpc.new/base/inc') + '" '
} else {
    $original = Join-Path $repo 'src/mvdm/softpc.new/base/support/ios.c'
    $headers = @('src/mvdm/softpc.new/host/inc/insignia.h','src/mvdm/softpc.new/base/inc/base_def.h','src/mvdm/softpc.new/base/inc/ios.h','src/mvdm/softpc.new/base/ccpu386/in.h','src/mvdm/softpc.new/base/ccpu386/out.h') | ForEach-Object { Join-Path $repo $_ }
    $object = Join-Path $WorkerBuild 'obj/support/ios.obj'
    $extraObjects = @('obj/ccpu/in.obj','obj/ccpu/out.obj') | ForEach-Object { Join-Path $WorkerBuild $_ }
    $extraSources = @('src/mvdm/softpc.new/base/ccpu386/in.c','src/mvdm/softpc.new/base/ccpu386/out.c') | ForEach-Object { Join-Path $repo $_ }
    $negativeDefine = 'MVDM_IO_EXPECT_WORD_BYTES'
    $extraFlags = '/DNTVDM /DSPC386 /DANSI /I"' + (Join-Path $repo 'src/mvdm/softpc.new/host/inc') + '" /I"' + (Join-Path $repo 'src/mvdm/softpc.new/base/inc') + '" '
}
$exe = Join-Path $BuildRoot "$Contract-object-contract.exe"
$fixtureObject = Join-Path $BuildRoot 'fixture.obj'
$scope = if ($SelectedObject) {
    'Explicit diagnostic object; controlled host boundaries, no guest or production lifecycle acceptance'
} else {
    'Actual cached original object; controlled host boundaries, no guest or production lifecycle acceptance'
}
$receipt = [ordered]@{
    started = [DateTime]::UtcNow.ToString('o'); owner_pid = $PID
    result = 'running'; contract = $Contract; scope = $scope
    build_root = $BuildRoot; fixture_path = $exe
    inputs = @(); runs = @()
}
try {
    foreach ($path in (@($PSCommandPath, $driver, $source, $original, $object, (Join-Path $WorkerBuild 'build.ninja')) + $headers + $extraObjects + $extraSources)) {
        $receipt.inputs += @{ path = $path; sha256 = (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash }
    }
    $linkedObjects = (@($object) + $extraObjects | ForEach-Object { '"' + $_ + '"' }) -join ' '
    $command = '/d /s /c ""' + $driver + '" cl.exe /nologo /W4 /MT /Od /D_CRT_SECURE_NO_WARNINGS ' + $extraFlags + '"' + $source + '" ' + $linkedObjects + ' /Fo:"' + $fixtureObject + '" /Fe:"' + $exe + '" /link kernel32.lib' + $extraLibraries + '"'
    $build = Start-Process cmd.exe -ArgumentList $command -WorkingDirectory $repo -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $BuildRoot 'build.stdout.txt') -RedirectStandardError (Join-Path $BuildRoot 'build.stderr.txt')
    $receipt.runs += @{ kind = 'build'; pid = $build.Id; command = $command }
    $receipt | ConvertTo-Json -Depth 8 | Set-Content -Encoding utf8 (Join-Path $BuildRoot 'result.json')
    $build.WaitForExit()
    $receipt.runs[-1].exit = $build.ExitCode
    if ($build.ExitCode -ne 0) { throw "Fixture build failed: $($build.ExitCode)" }
    $negativeCommand = '/d /s /c ""' + $driver + '" cl.exe /nologo /MT /c /D' + $negativeDefine + '=' + $negativeSize + ' ' + $extraFlags + '"' + $source + '" /Fo:"' + (Join-Path $BuildRoot 'negative-abi.obj') + '""'
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
    $marker = $Contract.ToUpperInvariant() + '_OBJECT_CONTRACT checks=(\d+) failures=0 callbacks=(\d+)'
    if ($run.ExitCode -ne 0 -or $text -notmatch $marker) { throw 'Original-object contract failed' }
    $receipt.checks = [int]$Matches[1]; $receipt.callbacks = [int]$Matches[2]
    if ($Contract -eq 'niddb') {
        if ($text -notmatch '(?m)^NIDDB_THREE_VM_REUSE active=1 reused=1 callbacks=3 cleanup=1\r?$') {
            throw 'NIDDB three-VM slot-reuse contract was not completed'
        }
        $receipt.three_vm_reuse = 'pass'
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
