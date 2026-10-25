[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$CompilerDriver,
    [Parameter(Mandatory)][string]$BuildRoot,
    [switch]$VisibleDesktop,
    [switch]$StartupOnly,
    [switch]$RetiringSibling,
    [switch]$ExceptionSibling,
    [switch]$StartupFailureSibling,
    [switch]$ShellExceptionSibling,
    [ValidateRange(1,32)][uint32]$CommonDialogRounds=1,
    [ValidateSet('Close','Ignore')][string]$ShellExceptionResponse='Close',
    [ValidateSet('Preparation','Returned')][string]$CallbackAbortPhase,
    [string]$StartupDiagnosticManifest,
    [string]$Nasm='nasm.exe',
    [ValidateSet('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe')]
    [string[]]$RuntimeImages=@('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe')
)
$ErrorActionPreference='Stop'
if(([int]$RetiringSibling.IsPresent+[int]$ExceptionSibling.IsPresent+[int]$StartupFailureSibling.IsPresent+[int]$ShellExceptionSibling.IsPresent) -gt 1){throw 'Select one sibling contract'}
$withSibling=$RetiringSibling -or $ExceptionSibling -or $StartupFailureSibling -or $ShellExceptionSibling
if($VisibleDesktop -and $withSibling){throw 'Ordinary desktop acceptance selects only the exact owned game'}
if($CallbackAbortPhase -and !$ShellExceptionSibling){throw 'Callback abort requires the isolated Shell sibling contract'}
if($CommonDialogRounds -ne 1 -and !$RetiringSibling){throw 'CommonDialogRounds requires the retiring sibling contract'}
if($StartupFailureSibling -or $ShellExceptionSibling){
    $expectedTarget=if($ShellExceptionSibling){'SHLPROBE'}else{'GENPROBE'}
    $diagnostic=Get-Content $StartupDiagnosticManifest -Raw|ConvertFrom-Json
    if($diagnostic.kind -ne 'diagnostic-only' -or $diagnostic.publishable -ne $false -or
        $diagnostic.target -ne $expectedTarget -or
        (Get-FileHash "$PackageRoot/system32/WOW32.DLL").Hash -ne $diagnostic.diagnosticProvider){throw 'Startup failure requires exact isolated diagnostic provider'}
    if($CallbackAbortPhase -and $diagnostic.fault -ne 'actual CallBack16 resolver Preparation/Returned'){throw 'Require actual callback-body diagnostic'}
}
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)){throw 'Evidence must be below build/'}
if(Test-Path $root){throw 'Use a fresh evidence directory'}
$null=New-Item -ItemType Directory $root
. "$PSScriptRoot\isolated_package_cleanup.ps1"
$source="$PSScriptRoot\wow_mine_gameplay_probe.c"
$process=$null;$runtime=$null;$scope=$null
$stressRounds=0
$stressValue=[Environment]::GetEnvironmentVariable('MVDM_WOW_MINE_STRESS_ROUNDS')
if($stressValue){
    if(![uint32]::TryParse($stressValue,[ref]$stressRounds) -or !$stressRounds -or $stressRounds -gt 1000){
        throw 'MVDM_WOW_MINE_STRESS_ROUNDS must be 1..1000'
    }
}
$observerTimeout=[Math]::Max(30000,10000+50*$stressRounds)
$saved=@{};foreach($name in @('TEMP','TMP','PATH','MVDM_OBSERVER_PRIVATE_DESKTOP',
    'MVDM_WOW_CALLBACK_FRAME_TRACE_PATH','MVDM_WOW_WINDOW_TRACE_PATH',
    'MVDM_WOW_REGISTRATION_TRACE_PATH','MVDM_TEST_STARTUP_IDENTITY',
    'MVDM_TEST_SHELL_EXCEPTION','MVDM_TEST_CALLBACK_ABORT')){$saved[$name]=[Environment]::GetEnvironmentVariable($name)}
try {
    Push-Location $root
    try {
        & $CompilerDriver cl.exe /nologo /O2 /MT /W4 /WX /D_CRT_SECURE_NO_WARNINGS "/Fo$root\probe.obj" "/Fe$root\probe.exe" $source /link user32.lib gdi32.lib dwmapi.lib *> "$root\compile.txt"
        if($LASTEXITCODE){throw "Probe build failed: $LASTEXITCODE"}
    } finally {Pop-Location}
    $runtime=New-PhysicalRuntimeStage $PackageRoot -RuntimeImages $RuntimeImages
    $scope=New-IsolatedPackageScope $runtime
    if($withSibling){
        $siblingName=if($ExceptionSibling -or $StartupFailureSibling){'GENPROBE'}else{'CDLPROBE'}
        $siblingSource=if($ExceptionSibling -or $StartupFailureSibling){"$PSScriptRoot\win16_generic_probe.asm"}else{"$PSScriptRoot\win16_common_dialog_probe.asm"}
        if($ShellExceptionSibling){$siblingName='SHLPROBE';$siblingSource="$PSScriptRoot\win16_shell_probe.asm"}
        $assemblyArgs=@('-f','bin','-o',"$root\$siblingName.EXE",'-l',"$root\sibling.lst")
        if($RetiringSibling){$assemblyArgs+=('-DCOMMON_DIALOG_ROUNDS='+$CommonDialogRounds)}
        if($ExceptionSibling){$assemblyArgs+='-DFAULT'}
        if($ShellExceptionSibling -and $ShellExceptionResponse -eq 'Ignore'){$assemblyArgs+='-DIGNORE_EXCEPTION'}
        if($CallbackAbortPhase){$assemblyArgs+='-DCALLBACK_ABORT'}
        & $Nasm @assemblyArgs $siblingSource *> "$root\assemble.txt"
        if($LASTEXITCODE){throw 'Independent sibling NE assembly failed'}
        $null=New-Item -ItemType Directory "$runtime\tests" -Force
        Copy-Item "$root\$siblingName.EXE" "$runtime\tests\$siblingName.EXE"
        if($ShellExceptionSibling){
            & $Nasm -f bin -DCHILD -o "$root/SHLCHILD.EXE" $siblingSource *> "$root/child-assemble.txt"
            if($LASTEXITCODE){throw 'Shell child assembly failed'}
            Copy-Item "$root/SHLCHILD.EXE" "$runtime/tests/SHLCHILD.EXE"
            $env:MVDM_TEST_SHELL_EXCEPTION="$root/shell-exception.txt"
            if($CallbackAbortPhase){$env:MVDM_TEST_CALLBACK_ABORT=$CallbackAbortPhase}
        }
        if($ExceptionSibling){
            $nativeSource="$PSScriptRoot\wow_generic_callback_probe.c"
            & $CompilerDriver cl.exe /nologo /LD /MT /O2 /DTEST_GUEST_EXCEPTION "/Fo$root\generic.obj" $nativeSource /link "/OUT:$root\GENHOST.DLL" "/IMPLIB:$root\GENHOST.lib" /EXPORT:RunGenericProbe=_RunGenericProbe@12 /EXPORT:ResolveGenericSegment=_ResolveGenericSegment@4 *> "$root\native-build.txt"
            if($LASTEXITCODE){throw 'Exception witness DLL build failed'}
            Copy-Item "$root\GENHOST.DLL" "$runtime\tests\GENHOST.DLL"
            @($nativeSource,"$root\GENHOST.DLL")|Get-FileHash|Select-Object Path,Hash|ConvertTo-Json|Set-Content "$root\native-inputs.json"
        }
        Get-FileHash -LiteralPath @($siblingSource,"$root\$siblingName.EXE","$runtime\system32\COMMDLG.DLL",
            "$runtime\system32\KRNL386.EXE","$runtime\system32\USER.EXE") |
            Select-Object Path,Hash | ConvertTo-Json | Set-Content "$root\sibling-inputs.json"
        $env:MVDM_WOW_CALLBACK_FRAME_TRACE_PATH="$root\lifetime.txt"
        $env:MVDM_WOW_WINDOW_TRACE_PATH="$root\lifetime.txt"
        if($StartupFailureSibling){
            $env:MVDM_WOW_REGISTRATION_TRACE_PATH="$root\registration.txt"
            $env:MVDM_TEST_STARTUP_IDENTITY="$root\startup-identity.txt"
        }
    }
    @('WINMINE.EXE','system32\ntvdm.exe','system32\WOW32.DLL','system32\run16.exe') | ForEach-Object {
        [pscustomobject]@{File=$_;SHA256=(Get-IsolatedFileSha256 (Join-Path $runtime $_))}
    } | ConvertTo-Json | Set-Content "$root\inputs.json"
    $env:TEMP=$runtime;$env:TMP=$runtime;$env:PATH="$runtime\system32;"+$env:SystemRoot+'\System32'
    if($VisibleDesktop){Remove-Item Env:MVDM_OBSERVER_PRIVATE_DESKTOP -ErrorAction SilentlyContinue}
    else{$env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'}
    $start=[Diagnostics.ProcessStartInfo]::new($Observer)
    $start.UseShellExecute=$false;$start.CreateNoWindow=$true
    $waitArgument=if($StartupOnly){''}else{' --wait'}
    $start.Arguments='"'+$runtime+'\system32\run16.exe" "'+$runtime+'" "'+$root+'\observer.txt"'+$waitArgument+' "'+$runtime+'\WINMINE.EXE" --observation-timeout-ms '+$observerTimeout
    $process=[Diagnostics.Process]::Start($start)
    $null=$process.Handle
    $process.Id | Set-Content "$root\observer.pid"
    $deadline=[DateTime]::UtcNow.AddSeconds(10);$worker=$null
    do {
        $worker=Get-CimInstance Win32_Process -Filter "Name='ntvdm.exe'" | Where-Object {$_.ExecutablePath -eq "$runtime\system32\ntvdm.exe"} | Select-Object -First 1
        if($worker -or $process.HasExited){break};Start-Sleep -Milliseconds 50
    }while([DateTime]::UtcNow -lt $deadline)
    if(!$worker){throw 'No worker in this owned runtime'}
    if($StartupOnly){
        if(!$process.WaitForExit(10000)){throw 'Default Win16 launcher did not return after startup'}
        $startupReport=Get-Content "$root\observer.txt" -Raw
        if($process.ExitCode -or $startupReport -notmatch 'exit=0x00000000'){
            throw 'Default Win16 launcher failed before gameplay'
        }
        'PASS launcher returned successfully before any gameplay or close input' |
            Set-Content "$root\startup-result.txt"
    }
    $marked='0'
    $desktopName=if($VisibleDesktop){'-'}else{"NTVDMConsoleTest-$($process.Id)"}
    $probeArguments=@($worker.ProcessId,$desktopName,"$root\mine",$marked)
    if($withSibling){$probeArguments+=@("$runtime\system32\run16.exe","$runtime\tests\$siblingName.EXE")}
    if($ShellExceptionSibling -and $ShellExceptionResponse -eq 'Ignore'){$probeArguments+='exception-ignore'}
    elseif($ExceptionSibling -or $ShellExceptionSibling){$probeArguments+='exception'}
    if($StartupFailureSibling){$probeArguments+="$root\startup-identity.txt"}
    & "$root\probe.exe" @probeArguments *> "$root\probe.txt"
    $probeExit=$LASTEXITCODE
    # Preserve the guest's completed checkpoints before propagating a host
    # probe failure.  This is diagnostic evidence only; the normal contract
    # below still requires the entire sibling/gameplay run to succeed.
    if($RetiringSibling -and (Test-Path "$runtime\tests\CDLPROBE.BIN")){
        Copy-Item "$runtime\tests\CDLPROBE.BIN" "$root\sibling-report.partial.bin"
    }
    if($ExceptionSibling -and (Test-Path "$runtime\tests\GENPROBE.BIN")){
        Copy-Item "$runtime\tests\GENPROBE.BIN" "$root\sibling-report.bin"
    }
    if($probeExit){throw "Gameplay probe failed: $probeExit"}
    if(!$process.WaitForExit($observerTimeout+5000)){throw 'Observer exceeded bound after window close'}
    $report=Get-Content "$root\observer.txt" -Raw
    if($process.ExitCode -or $report -notmatch 'result=exited' -or $report -notmatch 'exit=0x00000000'){
        throw 'Game actions passed but launcher completion did not; inspect observer.txt'
    }
    if($ExceptionSibling){
        Copy-Item "$runtime\tests\GENPROBE.BIN" "$root\sibling-report.bin"
        $bytes=[IO.File]::ReadAllBytes("$root\sibling-report.bin")
        if($bytes.Length -ne 18 -or [Text.Encoding]::ASCII.GetString($bytes,0,4) -ne 'GEN1' -or
            [BitConverter]::ToUInt16($bytes,4) -ne 0){throw 'Faulting guest continued or did not initialize'}
        $task=[BitConverter]::ToUInt16($bytes,16)
        $trace=Get-Content "$root\lifetime.txt" -Raw
        $task8='{0:X8}' -f [int]$task
        $complete=[regex]::Matches($trace,'(?m)^'+$worker.ProcessId+' TaskRetirement .*task='+$task8+' stage=complete status=0\r?$')
        if(!$task -or $complete.Count -ne 1){throw 'Exception task did not retire once in surviving worker'}
        $task4='{0:X4}' -f [int]$task
        $afterRetirement=$trace.Substring($complete[0].Index+$complete[0].Length)
        if($afterRetirement -match ('(?m)^'+$worker.ProcessId+' (before|simulate|after) ret=\w+ .*task='+$task4+' ')){
            throw 'Guest callback after exception-task retirement'
        }
        'PASS original exception Close, no guest continuation, same-worker exactly-once retirement and surviving WINMINE gameplay'|Set-Content "$root\sibling-result.txt"
    }elseif($ShellExceptionSibling){
        Copy-Item "$runtime/tests/SHLPROBE.BIN" "$root/sibling-report.bin"
        $bytes=[IO.File]::ReadAllBytes("$root/sibling-report.bin")
        if($ShellExceptionResponse -eq 'Close'){
            if($bytes.Length -ne 12 -or [Text.Encoding]::ASCII.GetString($bytes,0,4) -ne 'SHL1' -or
                [BitConverter]::ToUInt16($bytes,4) -ne 0){throw 'Shell guest continued after injected callback exception'}
        }else{
            if($bytes.Length -ne 36){throw 'Ignore did not reach all three guest checkpoints'}
            for($record=0;$record -lt 3;$record++){
                $offset=12*$record
                if([Text.Encoding]::ASCII.GetString($bytes,$offset,4) -ne 'SHL1' -or
                    [BitConverter]::ToUInt16($bytes,$offset+4) -ne $record -or
                    [BitConverter]::ToUInt16($bytes,$offset+6) -ne 0){throw 'Ignore guest stack/continuation/child failure'}
            }
            if([BitConverter]::ToUInt16($bytes,34) -gt 32){throw 'Post-Ignore missing target did not fail normally'}
            # Original handled-exception policy does not promise a successful
            # ShellExecute return. Verify continuation and the next API, not
            # an invented success return for the interrupted call.
            if(!$CallbackAbortPhase){
                Copy-Item "$runtime/tests/SHLCHILD.BIN" "$root/child-report.bin"
                $child=[IO.File]::ReadAllBytes("$root/child-report.bin")
                if($child.Length -ne 12 -or [BitConverter]::ToUInt16($child,4) -ne 3){throw 'Original Shell child did not complete'}
            }
        }
        if($CallbackAbortPhase){
            $boundary=@(Get-Content "$root/shell-exception.txt.frame")
            $pattern='^(\d+) (\d+) (\d+) (enter|unwind) resolve=(\d+) entry=([0-9A-F]{8}) logical=([0-9A-F]{8}) cpu=([0-9A-F]{8}) entrycpu=([0-9A-F]{8}) cs=([0-9A-F]{4})/([0-9A-F]{4}) ip=([0-9A-F]{4})/([0-9A-F]{4})$'
            if($boundary.Count -ne 2 -or $boundary[0] -notmatch $pattern -or $Matches[4] -ne 'enter'){throw 'Missing actual original callback entry'}
            $pidValue=[int]$Matches[1];$tid=[int]$Matches[2];$task=[int]$Matches[3];$entry=$Matches[6]
            if($pidValue -ne $worker.ProcessId -or !$task -or $boundary[1] -notmatch $pattern -or
                $Matches[4] -ne 'unwind' -or [int]$Matches[1] -ne $pidValue -or [int]$Matches[2] -ne $tid -or [int]$Matches[3] -ne $task -or
                [int]$Matches[5] -ne $(if($CallbackAbortPhase -eq 'Preparation'){2}else{3}) -or
                $Matches[6] -ne $entry -or $Matches[7] -ne $entry -or $Matches[8] -ne $entry -or
                $Matches[10] -ne $Matches[11] -or $Matches[12] -ne $Matches[13]){throw 'Actual callback frame/control was not restored at its unwind boundary'}
        }else{
        $boundary=@(Get-Content "$root/shell-exception.txt")
        if(($ShellExceptionResponse -eq 'Close' -and $boundary.Count -ne 2) -or
            $boundary.Count -lt 2 -or $boundary[0] -notmatch '^(\d+) (\d+) (\d+) original-return (\d+)$'){throw 'Missing original Shell callback return identity'}
        $pidValue=[int]$Matches[1];$tid=[int]$Matches[2];$task=[int]$Matches[3];$result=[int]$Matches[4]
        if($pidValue -ne $worker.ProcessId -or $result -le 32 -or
            $boundary[1] -ne "$pidValue $tid $task tls-clear 1"){throw 'Wrong Shell return or adapter TLS not restored'}
        if($ShellExceptionResponse -eq 'Ignore' -and ($boundary.Count -lt 3 -or
            @($boundary[1..($boundary.Count-1)]|Where-Object {$_ -ne "$pidValue $tid $task tls-clear 1"}).Count)){
            throw 'Post-Ignore adapter call did not restore the same task TLS'
        }
        }
        $trace=Get-Content "$root/lifetime.txt" -Raw
        $task4='{0:X4}' -f $task;$task8='{0:X8}' -f $task
        $callbackStage=if($CallbackAbortPhase){'before'}else{'after'}
        # Preparation fails before the original optional "before" trace edge;
        # its exact whole-body entry/unwind identity is proved above instead.
        if($CallbackAbortPhase -ne 'Preparation' -and
            $trace -notmatch ('(?m)^'+$pidValue+' '+$callbackStage+' ret=0037 .*task='+$task4+' ')){throw 'Original RET_WINEXEC not observed'}
        $complete=[regex]::Matches($trace,'(?m)^'+$pidValue+' TaskRetirement tid='+$tid+' task='+$task8+' stage=complete status=0\r?$')
        if($complete.Count -ne 1){throw 'Shell exception task did not retire once'}
        $after=$trace.Substring($complete[0].Index+$complete[0].Length)
        if($after -match ('(?m)^'+$pidValue+' (before|simulate|after) ret=\w+ .*task='+$task4+' ')){throw 'Callback after Shell task retirement'}
        "PASS original Shell callback return, outer exception $ShellExceptionResponse, TLS restore, exact-once retirement and surviving WINMINE gameplay"|Set-Content "$root/sibling-result.txt"
    }elseif($StartupFailureSibling){
        if(Test-Path "$runtime\tests\GENPROBE.BIN"){throw 'Startup-rejected guest reached its checkpoint'}
        $registration=Get-Content "$root\registration.txt" -Raw
        $prefix='(?m)^'+$worker.ProcessId+' '
        $failures=[regex]::Matches($registration,$prefix+'InitTaskError result=00000008\r?$')
        $rejected=[regex]::Matches($registration,$prefix+'InitTask result=00000000\r?$')
        if($failures.Count -ne 7 -or $rejected.Count -ne 7){throw 'Original seven initialization allocation failures not reached'}
        $tail=$registration.Substring($failures[-1].Index+$failures[-1].Length)
        if($tail -notmatch ($prefix+'WOWCleanup result=00000001\r?$')){throw 'No successful registered cleanup after startup rejection'}
        $identities=@(Get-Content "$root\startup-identity.txt")
        if($identities.Count -ne 7 -or @($identities|Select-Object -Unique).Count -ne 1){throw 'Failed InitTask retries did not retain one task/thread identity'}
        $native=Get-Content "$root\probe.txt" -Raw
        if($native -notmatch 'startup-thread-live=\d+ worker=\d+' -or
            $native -notmatch 'startup-thread-exit=\d+ wait=0 code=1'){throw 'Actual startup thread retirement missing'}
        'PASS real original InitTask seven failures, registered cleanup, actual host thread exit1 and surviving WINMINE gameplay'|Set-Content "$root\sibling-result.txt"
    }elseif($RetiringSibling){
        Copy-Item "$runtime\tests\CDLPROBE.BIN" "$root\sibling-report.bin"
        $bytes=[IO.File]::ReadAllBytes("$root\sibling-report.bin")
        # CDL2 records the guest-observed DestroyWindow and IsWindow results.
        # It writes initialization, owner creation, one record for each
        # explicitly destroyed dialog, and the live dialog left for task exit.
        $recordSize=24
        $recordCount=[int]$CommonDialogRounds+3
        if($bytes.Length -ne ($recordCount*$recordSize)){throw 'Unexpected CDL2 lifetime record count'}
        $records=@();$names=@('phase','errors','created','closed','hooks','leaving','task','dialog','destroy_ax','iswindow_ax')
        for($record=0;$record -lt $recordCount;$record++){
            $offset=$recordSize*$record
            if([Text.Encoding]::ASCII.GetString($bytes,$offset,4) -ne 'CDL2'){throw 'Bad sibling signature'}
            $row=[ordered]@{}
            for($i=0;$i -lt $names.Count;$i++){$row[$names[$i]]=[BitConverter]::ToUInt16($bytes,$offset+4+2*$i)}
            $records+=[pscustomobject]$row
        }
        $records|ConvertTo-Json|Set-Content "$root\sibling-report.json"
        $last=$records[-1]
        $destroyed=@($records[2..($recordCount-2)])
        $destroyFailures=0
        for($i=0;$i -lt $destroyed.Count;$i++){
            $expected=$i+1;$row=$destroyed[$i]
            if($row.phase -ne 1 -or $row.created -ne $expected -or $row.closed -ne $expected -or
                $row.hooks -ne $expected -or $row.destroy_ax -ne 1 -or $row.iswindow_ax -ne 0){++$destroyFailures}
        }
        if($records[0].phase -ne 0 -or $records[1].phase -ne 1 -or $last.phase -ne 2 -or
            @($records|Where-Object errors -ne 0).Count -or $destroyFailures -or
            $last.created -ne ($CommonDialogRounds+1) -or $last.closed -ne $CommonDialogRounds -or $last.hooks -ne ($CommonDialogRounds+1) -or
            $last.leaving -ne 1 -or !$last.task -or !$last.dialog){throw 'Sibling operation/exit assertions failed'}
        $trace=Get-Content "$root\lifetime.txt" -Raw
        $task4='{0:X4}' -f [int]$last.task;$task8='{0:X8}' -f [int]$last.task
        $prefix='(?m)^'+$worker.ProcessId+' '
        $complete=[regex]::Matches($trace,$prefix+'TaskRetirement .*task='+$task8+' stage=complete status=0\r?$')
        if($complete.Count -ne 1){throw 'Sibling did not retire exactly once in the WINMINE worker'}
        foreach($ret in @('0014','0012')){
            $callbacks=[regex]::Matches($trace,$prefix+'after ret='+$ret+' .*task='+$task4+' .*\r?$')
            if($callbacks.Count -lt 2 -or @($callbacks|Where-Object Index -gt $complete[0].Index).Count){
                throw 'Sibling resource callbacks missing or after retirement'
            }
        }
        'PASS same-worker sibling normal exit and resource cleanup; surviving WINMINE flag/reset/open/close' |
            Set-Content "$root\sibling-result.txt"
    }
    'PASS flag/counter, reset, open cell, WM_CLOSE and run16 completion' | Set-Content "$root\result.txt"
} finally {
    if($process){if(!$process.HasExited){$process.Kill();$process.WaitForExit(5000)|Out-Null};$process.Dispose()}
    if($scope){Stop-IsolatedPackageScope $scope}
    if($runtime){Remove-PhysicalRuntimeStage $runtime}
    foreach($name in $saved.Keys){[Environment]::SetEnvironmentVariable($name,$saved[$name])}
}
