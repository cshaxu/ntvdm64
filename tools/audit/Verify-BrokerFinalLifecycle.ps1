[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [string]$PackageRoot='O:\winnt',
    [Parameter(Mandatory)][string]$LogPrefix,
    [string]$BarrierBroker,
    [switch]$BrokerLoss,
    [switch]$WorkerLoss,
    [switch]$LauncherLoss,
    [switch]$FrontendLoss,
    [switch]$NestedWorkerLoss,
    [switch]$NestedInteractive,
    [switch]$MiddleLayerLoss,
    [switch]$MiddleLayerInputProbe,
    [switch]$DosNativeLoss,
    [switch]$NestedDosChild,
    [switch]$NestedDosChildControl,
    [switch]$NativeRoot,
    [switch]$RootTargetLoss,
    [string]$WorkerWindowObserver,
    [switch]$TwoWorkers,
    [switch]$ConsoleClose
)
$ErrorActionPreference='Stop'
if($NestedDosChildControl -and (!$NestedDosChild -or $LauncherLoss)){throw 'NestedDosChildControl requires NestedDosChild without LauncherLoss'}
if($NestedDosChild -and (!$DosNativeLoss -or $NativeRoot -or $NestedWorkerLoss -or $BrokerLoss -or $FrontendLoss -or $WorkerLoss -or $TwoWorkers -or $ConsoleClose -or $RootTargetLoss -or $BarrierBroker)){
    throw 'NestedDosChild requires only DosNativeLoss and optional LauncherLoss'
}
if($NativeRoot -and !$DosNativeLoss){throw 'NativeRoot requires DosNativeLoss'}
if($RootTargetLoss -and (!$NestedWorkerLoss -or $NestedInteractive -or $MiddleLayerLoss -or
    $MiddleLayerInputProbe -or $BrokerLoss -or $WorkerLoss -or $FrontendLoss -or
    $LauncherLoss -or $DosNativeLoss -or $NativeRoot -or $TwoWorkers -or $ConsoleClose -or $BarrierBroker)){
    throw 'RootTargetLoss requires only NestedWorkerLoss'
}
if($MiddleLayerInputProbe -and !$MiddleLayerLoss){throw 'MiddleLayerInputProbe requires MiddleLayerLoss'}
if($MiddleLayerLoss -and (!$NestedWorkerLoss -or !$NestedInteractive -or $BrokerLoss -or $FrontendLoss)){
    throw 'Middle-layer test requires NestedWorkerLoss/NestedInteractive and no broker/frontend loss'
}
$Observer=(Resolve-Path -LiteralPath $Observer).Path
$PackageRoot=(Resolve-Path -LiteralPath $PackageRoot).Path
if($LogPrefix -notmatch '^[a-z0-9-]+$'){throw 'Invalid log prefix'}
$trace=Join-Path $PackageRoot "logs\$LogPrefix.trace"
if(Test-Path -LiteralPath $trace){throw 'Use a fresh log prefix'}
$paths=@('run16.exe','basesrv.exe','ntvdm.exe') | ForEach-Object {Join-Path $PackageRoot $_}
function PackageProcesses {
    @(Get-CimInstance Win32_Process -Filter "Name='run16.exe' OR Name='basesrv.exe' OR Name='ntvdm.exe'" | Where-Object {$_.ExecutablePath -in $paths})
}
function ObservedProcesses {
    $shells=@((Join-Path $env:SystemRoot 'System32\cmd.exe'),(Join-Path $env:SystemRoot 'SysWOW64\cmd.exe'))
    @(Get-CimInstance Win32_Process -Filter "Name='run16.exe' OR Name='basesrv.exe' OR Name='ntvdm.exe' OR Name='cmd.exe' OR Name='console-startup-observer.exe'" |
        Where-Object {$_.ExecutablePath -in $paths -or $_.ExecutablePath -eq $Observer -or $_.ExecutablePath -in $shells})
}
if((PackageProcesses).Count){throw 'Package already in use'}
$owned=[Collections.Generic.HashSet[int]]::new()
$observers=[Collections.Generic.List[object]]::new()
$results=[Collections.Generic.List[string]]::new()
$previous=[Environment]::GetEnvironmentVariable('MVDM_BASESRV_TRACE_PATH')
$previousGate=[Environment]::GetEnvironmentVariable('MVDM_OBSERVER_INPUT_GATE')
$previousClose=[Environment]::GetEnvironmentVariable('MVDM_OBSERVER_CLOSE_CONSOLE')
$previousControlRecord=[Environment]::GetEnvironmentVariable('MVDM_OBSERVER_RECORD_CONTROL')
$inputGate=$null
$postFrontend=@()
function TraceText {if(Test-Path -LiteralPath $trace){Get-Content -LiteralPath $trace -Raw}else{''}}
function CollectOwned {
    foreach($o in $observers){
        if(Test-Path -LiteralPath $o.Report){
            $m=[regex]::Match((Get-Content -LiteralPath $o.Report -Raw),'(?m)^pid=(\d+)')
            if($m.Success){[void]$owned.Add([int]$m.Groups[1].Value)}
        }
    }
    for($depth=0;$depth -lt 8;++$depth){
        $new=@(ObservedProcesses | Where-Object {$owned.Contains([int]$_.ParentProcessId) -and !$owned.Contains([int]$_.ProcessId)})
        foreach($p in $new){[void]$owned.Add([int]$p.ProcessId)}
        if(!$new.Count){break}
    }
}
function StartObserved([string]$name,[string]$target,[string]$inputText=''){
    $report=Join-Path $PackageRoot "logs\$LogPrefix-$name.txt"
    $args=@((Join-Path $PackageRoot 'run16.exe'),$PackageRoot,$report,$target,'--observation-timeout-ms','20000')
    if($inputText){$args+=@('--observe-console-input-text',('"'+$inputText+'"'))}
    $start=[Diagnostics.ProcessStartInfo]::new($Observer,($args -join ' '))
    $start.UseShellExecute=$false
    $start.WindowStyle=[Diagnostics.ProcessWindowStyle]::Hidden
    $start.WorkingDirectory=$PackageRoot
    foreach($key in @('MVDM_OBSERVER_PRIVATE_DESKTOP','MVDM_OBSERVER_INPUT_GATE','MVDM_BASESRV_TRACE_PATH')){
        $value=[Environment]::GetEnvironmentVariable($key)
        if($value){$start.EnvironmentVariables[$key]=$value}
    }
    $p=[Diagnostics.Process]::Start($start)
    [void]$owned.Add($p.Id)
    $o=[pscustomobject]@{Process=$p;Report=$report;Name=$name}
    $observers.Add($o)
    return $o
}
function FinishObserved($o,[long]$expected){
    if(!$o.Process.WaitForExit(55000)){throw "Observer timeout: $($o.Name)"}
    if($o.Process.ExitCode -ne 0){throw "Observer failed before final report: $($o.Name) exit=$($o.Process.ExitCode)"}
    CollectOwned
    $record=Get-Content -LiteralPath $o.Report -Raw
    $match=[regex]::Match($record,'(?m)^exit=0x([0-9a-f]+)')
    if($record -notmatch '(?m)^result=exited' -or !$match.Success){throw "No exit: $($o.Name)"}
    $actual=[Convert]::ToUInt32($match.Groups[1].Value,16)
    if($actual -ne $expected){throw "$($o.Name): $actual expected $expected"}
    $results.Add("PASS $($o.Name): $actual")
}
function StopOwnedWorkers {
    CollectOwned
    foreach($p in (PackageProcesses | Where-Object {$_.Name -eq 'ntvdm.exe' -and $owned.Contains([int]$_.ProcessId)})){
        Stop-Process -Id $p.ProcessId
    }
}
try {
    [Environment]::SetEnvironmentVariable('MVDM_BASESRV_TRACE_PATH',$trace)
    if($ConsoleClose){
        if($env:MVDM_OBSERVER_PRIVATE_DESKTOP -ne '1'){throw 'ConsoleClose requires a private desktop'}
        $inputGate=[Threading.EventWaitHandle]::new($false,[Threading.EventResetMode]::ManualReset,('Local\MvdmConsoleClose-'+$PID))
        $env:MVDM_OBSERVER_INPUT_GATE='Local\MvdmConsoleClose-'+$PID
        $env:MVDM_OBSERVER_CLOSE_CONSOLE='1'
        $env:MVDM_OBSERVER_RECORD_CONTROL='1'
        $target=if($NestedWorkerLoss){'cmd.exe /d /k run16.exe cmd.exe /d /c run16.exe COMMAND.COM'}else{'COMMAND.COM'}
        $closing=StartObserved 'console-close' $target "exit`r"
        $screen=$closing.Report+'.pre-input-console.txt.console.txt'
        $deadline=[DateTime]::UtcNow.AddSeconds(12)
        do {
            if((Test-Path $screen) -and ((Get-Content $screen -Raw) -match '(?i)[a-z]:\\[^\r\n]*>')){break}
            Start-Sleep -Milliseconds 50
        } while([DateTime]::UtcNow -lt $deadline)
        if(!(Test-Path $screen) -or ((Get-Content $screen -Raw) -notmatch '(?i)[a-z]:\\[^\r\n]*>')){throw 'No live DOS prompt before Console close'}
        CollectOwned
        $members=@(ObservedProcesses | Where-Object {$owned.Contains([int]$_.ProcessId) -and $_.Name -in @('run16.exe','ntvdm.exe','cmd.exe')})
        $expectedLaunchers=if($NestedWorkerLoss){3}else{1}
        $expectedShells=if($NestedWorkerLoss){2}else{0}
        if(@($members | Where-Object Name -eq 'run16.exe').Count -ne $expectedLaunchers -or
           @($members | Where-Object Name -eq 'cmd.exe').Count -ne $expectedShells -or
           @($members | Where-Object Name -eq 'ntvdm.exe').Count -ne 1){throw 'Unexpected process chain before close'}
        $members | Select-Object ProcessId,ParentProcessId,Name,ExecutablePath,CommandLine |
            ConvertTo-Json -Depth 3 | Set-Content (Join-Path $PackageRoot "logs\$LogPrefix-close-chain.json") -Encoding utf8
        $retained=@($members | ForEach-Object {$p=Get-Process -Id $_.ProcessId;[void]$p.Handle; $p})
        try {
            [void]$inputGate.Set()
            if(!$closing.Process.WaitForExit(15000)){throw 'Console owner did not close'}
            foreach($p in $retained){
                if(!$p.WaitForExit(10000)){throw "Console-close survivor: $($p.Id)"}
                $results.Add("PASS native Console close: PID=$($p.Id) exit=$($p.ExitCode)")
            }
            $event=[IO.File]::ReadAllBytes($closing.Report+'.control.bin')
            if($event.Length -ne 4 -or [BitConverter]::ToUInt32($event,0) -ne 2){throw 'No native CTRL_CLOSE_EVENT witness'}
            $results.Add('PASS real CTRL_CLOSE_EVENT; all observed launchers, shells and worker completed before test cleanup')
        } finally {foreach($p in $retained){$p.Dispose()}}
    } elseif($DosNativeLoss){
        $fixture=Join-Path $PackageRoot 'tests\S2NLOSS.BAT'
        # Original DOS COMMAND requires CRLF command boundaries. Repository
        # checkout line-ending policy must not turn this authored test into
        # one multiline ECHO argument. No original guest media is changed.
        $batchName=if($NestedDosChild){'dos-native-dos-loss.bat'}else{'dos-native-loss.bat'}
        $batch=Get-Content -LiteralPath (Join-Path $PSScriptRoot "../../tests/observation/$batchName") -Raw
        [IO.File]::WriteAllText($fixture,($batch -replace '\r?\n',"`r`n"),[Text.Encoding]::ASCII)
        $target='COMMAND.COM /c '+$fixture
        if($NativeRoot){
            $inputGate=[Threading.EventWaitHandle]::new($false,[Threading.EventResetMode]::ManualReset,('Local\MvdmNativeDosNative-'+$PID))
            $env:MVDM_OBSERVER_INPUT_GATE='Local\MvdmNativeDosNative-'+$PID
            $target='cmd.exe /d /k run16.exe '+$target
        }
        if($NestedDosChild){
            $inputGate=[Threading.EventWaitHandle]::new($false,[Threading.EventResetMode]::ManualReset,('Local\MvdmDosNativeDos-'+$PID))
            $env:MVDM_OBSERVER_INPUT_GATE='Local\MvdmDosNativeDos-'+$PID
        }
        $nativePattern=if($NestedDosChild){'/c run16.exe COMMAND.COM'}else{'/k echo S2-NATIVE-WAIT'}
        $loss=StartObserved 'dos-native-loss' $target $(if($NativeRoot){"echo OUTER-NATIVE-RECOVERED`rexit /b 23`r"}elseif($NestedDosChild){"exit`r"}else{''})
        $deadline=[DateTime]::UtcNow.AddSeconds(15)
        $native=$null
        do {
            CollectOwned
            $native=@(ObservedProcesses | Where-Object {
                $owned.Contains([int]$_.ProcessId) -and $_.Name -eq 'cmd.exe' -and
                $_.CommandLine -match [regex]::Escape($nativePattern)
            }) | Select-Object -First 1
            if($native -or $loss.Process.HasExited){break}
            Start-Sleep -Milliseconds 100
        } while([DateTime]::UtcNow -lt $deadline)
        if(!$native){throw 'DOS did not create the selected native waiting target'}
        ObservedProcesses | Where-Object {$owned.Contains([int]$_.ProcessId)} |
            Select-Object ProcessId,ParentProcessId,Name,ExecutablePath,CommandLine |
            ConvertTo-Json -Depth 3 | Set-Content -LiteralPath (Join-Path $PackageRoot "logs\$LogPrefix-chain.json") -Encoding utf8
        $pair=@(PackageProcesses | Where-Object {$_.ProcessId -eq $native.ParentProcessId -and $_.Name -eq 'run16.exe'})
        if($pair.Count -ne 1){throw 'Native target has no exact run16 lifetime pair'}
        $targetProcess=Get-Process -Id $native.ProcessId
        $pairProcess=Get-Process -Id $pair[0].ProcessId
        [void]$targetProcess.Handle;[void]$pairProcess.Handle
        $dosParent=$null
        $outerParent=$null
        $nestedLauncher=$null
        try {
        if($NestedDosChild){
            $screen=$loss.Report+'.pre-input-console.txt.console.txt'
            $deadline=[DateTime]::UtcNow.AddSeconds(12)
            do {
                CollectOwned
                $inner=@(PackageProcesses | Where-Object {$_.Name -eq 'run16.exe' -and $_.ParentProcessId -eq $native.ProcessId -and $owned.Contains([int]$_.ProcessId)})
                if($inner.Count -eq 1 -and (Test-Path $screen) -and (Get-Content $screen -Raw) -match '(?i)[a-z]:\\[^\r\n]*>'){break}
                Start-Sleep -Milliseconds 50
            } while([DateTime]::UtcNow -lt $deadline)
            if($inner.Count -ne 1 -or !(Test-Path $screen) -or (Get-Content $screen -Raw) -notmatch '(?i)[a-z]:\\[^\r\n]*>'){throw 'Nested DOS child did not reach prompt'}
            $nestedLauncher=Get-Process -Id $inner[0].ProcessId;[void]$nestedLauncher.Handle
            if($nestedLauncher.HasExited){throw 'Nested DOS launcher already exited'}
        }
        if($NativeRoot){
            $workers=@(PackageProcesses | Where-Object {$owned.Contains([int]$_.ProcessId) -and $_.Name -eq 'ntvdm.exe'})
            if($workers.Count -ne 1){throw 'Expected one DOS worker in native/DOS/native chain'}
            $dosLaunchers=@(PackageProcesses | Where-Object {$_.ProcessId -eq $workers[0].ParentProcessId -and $_.Name -eq 'run16.exe'})
            if($dosLaunchers.Count -ne 1){throw 'Cannot identify DOS command launcher'}
            $outerShells=@(ObservedProcesses | Where-Object {$_.ProcessId -eq $dosLaunchers[0].ParentProcessId -and $_.Name -eq 'cmd.exe' -and $owned.Contains([int]$_.ProcessId)})
            if($outerShells.Count -ne 1){throw 'DOS launcher has no outer native CMD parent'}
            $dosParent=Get-Process -Id $dosLaunchers[0].ProcessId
            $outerParent=Get-Process -Id $outerShells[0].ProcessId
            [void]$dosParent.Handle;[void]$outerParent.Handle
        }
            if($NestedDosChildControl){[void]$inputGate.Set()}
            elseif($LauncherLoss){$pairProcess.Kill()}else{$targetProcess.Kill()}
            if(!$targetProcess.WaitForExit(5000) -or !$pairProcess.WaitForExit(5000)){throw 'Native lifetime pair did not finish'}
            # With a live launcher, target status must propagate exactly.
            # Killing the launcher instead invokes OS Job kill-on-close;
            # that cleanup status is not the launcher's externally set code.
            $results.Add("OBSERVED native=$($targetProcess.ExitCode) launcher=$($pairProcess.ExitCode) launcher-loss=$([bool]$LauncherLoss)")
            if($NestedDosChildControl){
                if($pairProcess.ExitCode -ne $targetProcess.ExitCode){throw 'Normal native completion did not propagate'}
            } elseif($pairProcess.ExitCode -ne -1 -or (!$LauncherLoss -and $targetProcess.ExitCode -ne -1)){throw 'Native termination code was not propagated to its launcher'}
            $results.Add('PASS native target and inner run16 both finish; live launcher propagates target status')
            if($NestedDosChild){
                if(!$NestedDosChildControl){
                    if($nestedLauncher.HasExited){throw 'Middle native pair loss recursively terminated nested DOS launcher'}
                    $results.Add('PASS middle native pair ended while nested DOS launcher remains alive; no recursive kill')
                }
                [void]$inputGate.Set()
                if(!$nestedLauncher.WaitForExit(10000)){throw 'Nested DOS exit input did not complete its launcher'}
                $results.Add("OBSERVED nested DOS launcher exit=$($nestedLauncher.ExitCode) after normal exit input")
            }
            if($NativeRoot){
                if(!$dosParent.WaitForExit(10000) -or $dosParent.ExitCode -ne 0){throw 'DOS parent command did not recover and complete normally'}
                if($outerParent.HasExited){throw 'Outer interactive native CMD died with inner pair'}
                $results.Add('PASS DOS command launcher completes with 0 while outer interactive CMD survives')
                [void]$inputGate.Set()
            }
        } finally {
            $targetProcess.Dispose();$pairProcess.Dispose()
            if($dosParent){$dosParent.Dispose()}
            if($outerParent){$outerParent.Dispose()}
            if($nestedLauncher){$nestedLauncher.Dispose()}
        }
        FinishObserved $loss $(if($NativeRoot){23}else{0})
        $screen=Get-Content -LiteralPath ($loss.Report+'.console.txt') -Raw
        $markers=@('S2-DOS-RECOVERED','bytes total conventional memory','S2-DOS-END')
        if(!$NestedDosChildControl){$markers+= 'S2-NATIVE-EXIT-255'}
        foreach($marker in $markers){
            if($screen -notmatch [regex]::Escape($marker)){throw "DOS parent recovery missing: $marker"}
        }
        $results.Add($(if($NestedDosChildControl){'PASS original DOS parent executes MEM and completes its script after normal nested return'}else{'PASS original DOS parent observes low-byte 255, executes MEM, and completes its script'}))
        if($NativeRoot){
            if($screen -notmatch 'OUTER-NATIVE-RECOVERED'){throw 'Outer native CMD recovery output missing'}
            $results.Add('PASS outer native CMD executes recovery input and root run16 returns 23')
        }
    } elseif($NestedWorkerLoss){
        if(($BrokerLoss -or $FrontendLoss) -and $NestedInteractive){throw 'Interactive recovery selects worker or inner launcher loss only'}
        $failureCode=if($BrokerLoss){1722}else{1067}
        $inputGate=[Threading.EventWaitHandle]::new($false,[Threading.EventResetMode]::ManualReset,('Local\MvdmNestedLoss-'+$PID))
        [Environment]::SetEnvironmentVariable('MVDM_OBSERVER_INPUT_GATE',('Local\MvdmNestedLoss-'+$PID))
        $target=if($NestedInteractive){'cmd.exe /d /k run16.exe cmd.exe /d /c run16.exe COMMAND.COM'}else{'cmd.exe /d /c run16.exe cmd.exe /d /c run16.exe COMMAND.COM'}
        $inputText=if($NestedInteractive){"echo NESTED-RECOVERED`rexit /b 23`r"}else{"exit`r"}
        $loss=StartObserved 'nested-worker-loss' $target $inputText
        $screen=$loss.Report+'.pre-input-console.txt.console.txt'
        $deadline=[DateTime]::UtcNow.AddSeconds(15)
        do {
            if((Test-Path -LiteralPath $screen) -and ((Get-Content -LiteralPath $screen -Raw) -match '(?i)[a-z]:\\[^\r\n]*>')){break}
            Start-Sleep -Milliseconds 50
        } while([DateTime]::UtcNow -lt $deadline)
        if(!(Test-Path -LiteralPath $screen) -or ((Get-Content -LiteralPath $screen -Raw) -notmatch '(?i)[a-z]:\\[^\r\n]*>')){throw 'Nested COMMAND never reached real prompt'}
        CollectOwned
        $chain=@(ObservedProcesses | Where-Object {$owned.Contains([int]$_.ProcessId)})
        $launchers=@($chain | Where-Object {$_.Name -eq 'run16.exe'})
        $shells=@($chain | Where-Object {$_.Name -eq 'cmd.exe'})
        $workers=@($chain | Where-Object {$_.Name -eq 'ntvdm.exe'})
        $servers=@($chain | Where-Object {$_.Name -eq 'basesrv.exe'})
        $chain | Select-Object ProcessId,ParentProcessId,Name,ExecutablePath,CommandLine |
            ConvertTo-Json -Depth 3 | Set-Content -LiteralPath (Join-Path $PackageRoot "logs\$LogPrefix-chain.json") -Encoding utf8
        if($launchers.Count -ne 3 -or $shells.Count -ne 2 -or $workers.Count -ne 1 -or $servers.Count -ne 1){throw 'Expected three run16, two CMD, one worker and one broker'}
        $root=@($launchers | Where-Object {$_.ParentProcessId -notin $shells.ProcessId})
        if($root.Count -ne 1){throw 'Cannot identify unique outer run16'}
        $outerShell=@($shells | Where-Object {$_.ParentProcessId -eq $root[0].ProcessId})
        if($outerShell.Count -ne 1){throw 'Cannot identify outer native shell'}
        $inner=@($launchers | Where-Object {$_.ProcessId -eq $workers[0].ParentProcessId})
        if($inner.Count -ne 1){throw 'Cannot identify inner DOS launcher'}
        $survivors=if($NestedInteractive){@($root[0].ProcessId,$outerShell[0].ProcessId)}else{@()}
        $retained=@($chain | Where-Object {$_.Name -in @('run16.exe','cmd.exe') -or (($BrokerLoss -or $FrontendLoss -or $LauncherLoss -or $MiddleLayerLoss -or $RootTargetLoss) -and $_.Name -eq 'ntvdm.exe')} | ForEach-Object {
            $process=Get-Process -Id $_.ProcessId; [void]$process.Handle
            [pscustomobject]@{Process=$process;Name=$_.Name;Id=$_.ProcessId}
        })
        try {
            if($MiddleLayerLoss){
                $middle=@($launchers | Where-Object {$_.ParentProcessId -eq $outerShell[0].ProcessId})
                if($middle.Count -ne 1){throw 'Cannot identify middle launcher'}
                $middleTarget=@($shells | Where-Object {$_.ParentProcessId -eq $middle[0].ProcessId})
                if($middleTarget.Count -ne 1){throw 'Cannot identify middle native target'}
                $victimId=if($LauncherLoss){$middle[0].ProcessId}else{$middleTarget[0].ProcessId}
                Stop-Process -Id $victimId
                foreach($id in @($middle[0].ProcessId,$middleTarget[0].ProcessId)){
                    $item=@($retained | Where-Object {$_.Id -eq $id})[0]
                    if(!$item.Process.WaitForExit(5000)){throw "Middle pair remains alive: $id"}
                    if((!$LauncherLoss -or $id -eq $victimId) -and $item.Process.ExitCode -ne -1){throw 'Middle pair did not preserve native target termination status'}
                    $results.Add("PASS middle pair exit: $($item.Name) PID=$id code=$($item.Process.ExitCode)")
                }
                foreach($id in @($root[0].ProcessId,$outerShell[0].ProcessId,$inner[0].ProcessId,$workers[0].ProcessId)){
                    $item=@($retained | Where-Object {$_.Id -eq $id})[0]
                    if($item.Process.WaitForExit(300)){throw "Unexpected cross-layer termination: $id"}
                    $results.Add("PASS no recursive cleanup: $($item.Name) PID=$id survives")
                }
                if($MiddleLayerInputProbe){
                    $postFrontend=@($inner[0].ProcessId,$workers[0].ProcessId | ForEach-Object {
                        $p=Get-Process -Id $_;[void]$p.Handle;$p
                    })
                    $results.Add('OBSERVATION: sending recovery input with both outer CMD and descendant DOS alive; no second fault')
                } else {
                # Deliberately release the surviving DOS descendant with a
                # SECOND fault. No input is sent while both shells are live.
                # This cannot certify concurrent input ownership before release.
                Stop-Process -Id $workers[0].ProcessId
                $innerProcess=@($retained | Where-Object {$_.Id -eq $inner[0].ProcessId})[0].Process
                if(!$innerProcess.WaitForExit(5000) -or $innerProcess.ExitCode -ne 1067){throw 'Surviving inner launcher did not report the separate worker fault'}
                $results.Add('PASS separate descendant worker fault returns 1067; parent input recovery follows this release')
                }
            } else {
            $victimId=if($RootTargetLoss){$outerShell[0].ProcessId}elseif($BrokerLoss){$servers[0].ProcessId}elseif($FrontendLoss){$root[0].ProcessId}elseif($LauncherLoss){$inner[0].ProcessId}else{$workers[0].ProcessId}
            Stop-Process -Id $victimId
            foreach($item in @($retained | Where-Object {$_.Id -notin $survivors})){
                if(!$item.Process.WaitForExit(5000)){
                    if($item.Name -eq 'ntvdm.exe' -and $WorkerWindowObserver){
                        & $WorkerWindowObserver $item.Id "NTVDMConsoleTest-$($loss.Process.Id)" |
                            Set-Content -LiteralPath (Join-Path $PackageRoot "logs\$LogPrefix-worker-windows.txt") -Encoding utf8
                    }
                    throw "Nested process still alive: $($item.Name) PID=$($item.Id)"
                }
                $expectedCode=if(($FrontendLoss -and $item.Id -eq $victimId) -or ($LauncherLoss -and $item.Name -ne 'ntvdm.exe')){-1}else{$failureCode}
                if($RootTargetLoss -and $item.Id -in @($root[0].ProcessId,$victimId)){$expectedCode=-1}
                if($item.Process.ExitCode -ne $expectedCode){throw "Wrong propagated code: $($item.Name) PID=$($item.Id) code=$($item.Process.ExitCode) expected=$expectedCode"}
                $results.Add("PASS chain exit: $($item.Name) PID=$($item.Id) code=$expectedCode")
            }
            }
            foreach($item in @($retained | Where-Object {$_.Id -in $survivors})){
                if($item.Process.HasExited){throw "Interactive owner unexpectedly exited: $($item.Name) PID=$($item.Id)"}
                $results.Add("PASS interactive owner survives: $($item.Name) PID=$($item.Id)")
            }
            if(!$BrokerLoss -and !(PackageProcesses | Where-Object {$_.ProcessId -eq $servers[0].ProcessId})){throw 'Broker died with nested worker'}
        } finally {foreach($item in $retained){$item.Process.Dispose()}}
        [void]$inputGate.Set()
        FinishObserved $loss $(if($NestedInteractive){23}elseif($FrontendLoss -or $LauncherLoss -or $RootTargetLoss){[uint32]::MaxValue}else{$failureCode})
        if($MiddleLayerInputProbe){
            foreach($p in $postFrontend){
                if(!$p.WaitForExit(5000)){throw "Descendant outlived completed root frontend: $($p.Id)"}
                if($p.ExitCode -eq 0){throw "Unfinished descendant reported success: $($p.Id)"}
                $results.Add("PASS root frontend completion contains unfinished descendant: PID=$($p.Id) exit=$($p.ExitCode)")
            }
        }
        if($NestedInteractive -and ((Get-Content -LiteralPath ($loss.Report+'.console.txt') -Raw) -notmatch '(?m)^\[\d+\]\s*NESTED-RECOVERED\s*$')){throw 'Interactive CMD did not execute the recovery command'}
        $after=StartObserved 'after-nested-loss' 'MEM.EXE'; FinishObserved $after 0
        if(((Get-Content -LiteralPath ($after.Report+'.console.txt') -Raw) -replace '\s','') -notmatch 'bytesavailableXMSmemory'){throw 'No real MEM output after nested loss'}
    } elseif($BrokerLoss -or $WorkerLoss -or $LauncherLoss -or $FrontendLoss){
        $inputGate=[Threading.EventWaitHandle]::new($false,[Threading.EventResetMode]::ManualReset,'Local\MvdmS10BrokerLost')
        [Environment]::SetEnvironmentVariable('MVDM_OBSERVER_INPUT_GATE','Local\MvdmS10BrokerLost')
        $loss=StartObserved 'broker-loss' 'COMMAND.COM' "exit`r"
        $other=if($TwoWorkers){StartObserved 'unrelated-worker' 'COMMAND.COM' "echo ISOLATION-ALIVE`rexit`r"}else{$null}
        $requiredTasks=if($TwoWorkers){2}else{1}
        $deadline=[DateTime]::UtcNow.AddSeconds(15)
        # Production trace hooks have been retired. The observer captures the
        # real COMMAND prompt before blocking input; require that evidence.
        $ready=@($loss); if($other){$ready+=@($other)}
        do {
            $readyCount=@($ready | Where-Object {
                $screen=$_.Report+'.pre-input-console.txt.console.txt'
                (Test-Path -LiteralPath $screen) -and
                    ((Get-Content -LiteralPath $screen -Raw) -match '(?i)[a-z]:\\[^\r\n]*>')
            }).Count
            if($readyCount -eq $requiredTasks){break}
            Start-Sleep -Milliseconds 50
        } while([DateTime]::UtcNow -lt $deadline)
        if($readyCount -ne $requiredTasks){throw 'No real COMMAND prompt before loss test'}
        CollectOwned
        $servers=@(PackageProcesses | Where-Object {$_.Name -eq 'basesrv.exe' -and $owned.Contains([int]$_.ProcessId)})
        if($servers.Count -ne 1){
            [pscustomobject]@{Owned=@($owned);Processes=@(PackageProcesses |
                Select-Object ProcessId,ParentProcessId,Name,ExecutablePath,CreationDate);
                Observers=@($observers | Select-Object Name,Report);
                ReadyCount=$readyCount;RequiredTasks=$requiredTasks;
                Reports=@($observers | ForEach-Object {Get-Content -LiteralPath $_.Report -Raw -ErrorAction SilentlyContinue})} |
                ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $PackageRoot "logs\$LogPrefix-ownership.json") -Encoding utf8
            throw 'Cannot identify owned broker; see ownership.json'
        }
        $workers=@(PackageProcesses | Where-Object {$_.Name -eq 'ntvdm.exe' -and $owned.Contains([int]$_.ProcessId)})
        $launchers=@(PackageProcesses | Where-Object {$_.Name -eq 'run16.exe' -and $owned.Contains([int]$_.ProcessId)})
        # The final report is written after exit, not while COMMAND waits.
        # Follow only this live observer's verified relay/process ancestry.
        $observedTree=@(ObservedProcesses)
        $launcher=@($launchers | Where-Object {
            $parentId=[int]$_.ParentProcessId
            $belongs=$false
            for($depth=0;$depth -lt 8 -and $parentId;++$depth){
                if($parentId -eq $loss.Process.Id){$belongs=$true;break}
                $parent=@($observedTree | Where-Object {$_.ProcessId -eq $parentId})
                if($parent.Count -ne 1){break}
                $parentId=[int]$parent[0].ParentProcessId
            }
            $belongs
        })
        if($launcher.Count -ne 1){throw 'Cannot identify observer-owned launcher'}
        $launcherId=[int]$launcher[0].ProcessId
        $worker=@($workers | Where-Object {$_.ParentProcessId -eq $launcherId})
        if($workers.Count -ne $requiredTasks -or $worker.Count -ne 1){throw 'Cannot identify owned pairs'}
        $victim=if($WorkerLoss){$worker[0]}elseif($LauncherLoss -or $FrontendLoss){$launcher[0]}else{$servers[0]}
        # Unlike the pre-frontend launcher, the S2 root owns active DOS input
        # and presentation. Observe worker failure while the observer still
        # holds the Console and input gate; Console teardown must not fake it.
        $frontendWorker=if($FrontendLoss -or $LauncherLoss){Get-Process -Id $worker[0].ProcessId}else{$null}
        if($frontendWorker){[void]$frontendWorker.Handle} # retain before process exit
        Stop-Process -Id $victim.ProcessId
        if($TwoWorkers -and !$BrokerLoss){
            Start-Sleep -Milliseconds 500
            $otherWorker=$workers | Where-Object {$_.ProcessId -ne $worker[0].ProcessId}
            if(!(PackageProcesses | Where-Object {$_.ProcessId -eq $otherWorker.ProcessId})){throw 'Unrelated worker was terminated'}
        }
        if($FrontendLoss -or $LauncherLoss){
            if(!$frontendWorker.WaitForExit(5000)){
                if($WorkerWindowObserver){
                    & $WorkerWindowObserver $worker[0].ProcessId "NTVDMConsoleTest-$($loss.Process.Id)" |
                        Set-Content -LiteralPath (Join-Path $PackageRoot "logs\$LogPrefix-worker-windows.txt") -Encoding utf8
                }
                throw 'Active worker still waiting after frontend loss'
            }
            if($null -eq $frontendWorker.ExitCode -or $frontendWorker.ExitCode -ne 1067){
                throw "Frontend loss must return worker error 1067; observed $($frontendWorker.ExitCode)"
            }
            $results.Add("PASS active frontend loss: worker failed before Console observer teardown ($($frontendWorker.ExitCode))")
            $frontendWorker.Dispose()
        }
        [void]$inputGate.Set()
        if(!$loss.Process.WaitForExit(55000)){throw 'Broker-loss observer timeout'}
        $record=Get-Content -LiteralPath $loss.Report -Raw
        if($record -notmatch '(?m)^result=exited' -or $record -match '(?m)^exit=0x00000000'){
            throw 'Process loss did not produce a bounded explicit failure'
        }
        $lossExit=[regex]::Match($record,'(?m)^exit=0x([0-9a-f]+)')
        if(!$lossExit.Success){throw 'Missing explicit failure code'}
        $actualLoss=[Convert]::ToUInt32($lossExit.Groups[1].Value,16)
        if($BrokerLoss -and $actualLoss -ne 1722){throw 'Broker loss must return 1722'}
        if($WorkerLoss -and $actualLoss -ne 1067){throw 'Worker loss must return 1067'}
        if($other){FinishObserved $other $(if($BrokerLoss){1722}else{0})}
        if($other -and !$BrokerLoss){
            $otherText=Get-Content -LiteralPath ($other.Report+'.console.txt') -Raw
            if($otherText -notmatch '(?m)^\[\d+\]\s*ISOLATION-ALIVE\s*$'){
                throw 'Unrelated worker did not execute its post-fault command'
            }
            $results.Add('PASS unrelated worker: actual post-fault command output and normal exit')
        }
        if($BrokerLoss){
            $deadline=[DateTime]::UtcNow.AddSeconds(5)
            while((PackageProcesses | Where-Object {$_.Name -eq 'ntvdm.exe'}) -and [DateTime]::UtcNow -lt $deadline){Start-Sleep -Milliseconds 50}
            if(PackageProcesses | Where-Object {$_.Name -eq 'ntvdm.exe'}){throw 'Worker survived broker loss'}
            $results.Add('PASS broker loss: launcher and worker exit, no replay')
        } else {
            if(!(PackageProcesses | Where-Object {$_.ProcessId -eq $servers[0].ProcessId})){throw 'Unrelated broker terminated'}
            $results.Add("PASS $($victim.Name) loss: bounded parent outcome, broker survives")
            StopOwnedWorkers
        }
        Start-Sleep -Seconds 2
        $after=StartObserved 'after-loss' 'MEM.EXE'; FinishObserved $after 0
        $memText=(Get-Content -LiteralPath ($after.Report+'.console.txt') -Raw) -replace '\s',''
        if($memText -notmatch 'bytesavailableXMSmemory'){throw 'MEM did not print its real memory report after loss'}
    } elseif($BarrierBroker){
        $BarrierBroker=(Resolve-Path -LiteralPath $BarrierBroker).Path
        if($BarrierBroker -notmatch '\\build\\M0-T412\\S10\\'){throw 'Barrier binary must remain in S10 build'}
        $ready=[Threading.EventWaitHandle]::new($false,[Threading.EventResetMode]::ManualReset,'Local\MvdmS10TimerReady')
        $release=[Threading.EventWaitHandle]::new($false,[Threading.EventResetMode]::ManualReset,'Local\MvdmS10TimerRelease')
        $barrier=Start-Process -FilePath $BarrierBroker -WindowStyle Hidden -PassThru
        try {
            if(!$ready.WaitOne(75000)){throw 'Timer callback barrier not reached'}
            $arrival=StartObserved 'callback-arrival' 'MEM.EXE'
            Start-Sleep -Milliseconds 750
            [void]$release.Set()
            FinishObserved $arrival 0
            if(!$barrier.WaitForExit(10000)){throw 'Old empty broker did not exit'}
            if((TraceText) -notmatch 'phase=empty-stop'){throw 'Missing stopped epoch'}
            $results.Add('PASS callback-in-progress: original callback wins; launcher retries against fresh broker')
        } finally {
            [void]$release.Set()
            if(!$barrier.HasExited){Stop-Process -Id $barrier.Id}
            $ready.Dispose();$release.Dispose()
        }
    } else {
        $first=StartObserved 'concurrent-a' 'MEM.EXE'
        $second=StartObserved 'concurrent-b' 'MEM.EXE'
        FinishObserved $first 0; FinishObserved $second 0
        $servers=@(PackageProcesses | Where-Object {$_.Name -eq 'basesrv.exe'})
        if($servers.Count -ne 1 -or !$owned.Contains([int]$servers[0].ProcessId)){throw 'Not exactly one owned broker'}
        $serverId=[int]$servers[0].ProcessId
        $text=TraceText
        if([regex]::Matches($text,'phase=prepare ').Count -ne 2 -or [regex]::Matches($text,'phase=first-yes ').Count -ne 1){throw 'Concurrent command/first identity mismatch'}
        $duplicate=Start-Process -FilePath (Join-Path $PackageRoot 'basesrv.exe') -WindowStyle Hidden -PassThru
        [void]$owned.Add($duplicate.Id)
        if(!$duplicate.WaitForExit(5000) -or $duplicate.ExitCode -ne 1740){throw 'Duplicate broker did not reject endpoint'}
        $results.Add('PASS concurrent startup and singleton: two commands, one broker, one first-VDM')
        StopOwnedWorkers
        Start-Sleep -Seconds 2
        if((TraceText) -notmatch 'phase=worker-process-cleanup'){throw 'No original abrupt-worker cleanup'}
        $again=StartObserved 'after-worker-exit' 'MEM.EXE'; FinishObserved $again 0
        if(!(PackageProcesses | Where-Object {$_.ProcessId -eq $serverId})){throw 'Broker lost on worker cleanup'}
        StopOwnedWorkers
        Start-Sleep -Seconds 2
        $before=TraceText
        if($before -notmatch 'phase=empty-grace'){throw 'No empty grace'}
        # Arrival during the real production 60-second grace cancels this epoch.
        Start-Sleep -Seconds 35
        $arrival=StartObserved 'grace-arrival' 'MEM.EXE'; FinishObserved $arrival 0
        if(!(PackageProcesses | Where-Object {$_.ProcessId -eq $serverId})){throw 'Arrival failed to retain broker'}
        StopOwnedWorkers
        $deadline=[DateTime]::UtcNow.AddSeconds(75)
        while((PackageProcesses | Where-Object {$_.ProcessId -eq $serverId}) -and [DateTime]::UtcNow -lt $deadline){Start-Sleep -Seconds 1}
        if(PackageProcesses | Where-Object {$_.ProcessId -eq $serverId}){throw 'Empty broker did not stop'}
        if((TraceText) -notmatch 'phase=empty-stop'){throw 'No empty-stop witness'}
        $restart=StartObserved 'restart' 'MEM.EXE'; FinishObserved $restart 0
        if([regex]::Matches((TraceText),'phase=first-yes ').Count -ne 2){throw 'Restart did not reset first-VDM exactly once'}
        $results.Add('PASS abrupt cleanup, grace arrival cancellation, empty stop and fresh first-VDM')
        $wow=StartObserved 'wow-boundary' 'system32\WRITE.EXE'
        if(!$wow.Process.WaitForExit(55000)){throw 'WOW observer did not finish'}
        CollectOwned
        $wowRecord=Get-Content -LiteralPath $wow.Report -Raw
        if((TraceText) -notmatch 'phase=get-dispatched state=00000102 status=00000000'){throw 'No successful shared WOW PIF request'}
        $results.Add('PASS real worker shared-WOW PIF acquisition; completion is separately tested through original service fixture')
        if($wowRecord -match '(?m)^result=timeout'){
            $results.Add('LIMIT: WRITE timed out at 20 seconds after successful broker acquisition; not workload acceptance')
        } elseif($wowRecord -match '(?m)^result=exited'){
            $results.Add('OBSERVATION: WRITE returned; see exact exit code in report, not workload acceptance')
        } else {throw 'Unclassified WOW observation'}
    }
} finally {
    CollectOwned
    foreach($p in $postFrontend){$p.Dispose()}
    foreach($p in (PackageProcesses | Where-Object {$owned.Contains([int]$_.ProcessId)})){Stop-Process -Id $p.ProcessId -ErrorAction SilentlyContinue}
    foreach($p in (ObservedProcesses | Where-Object {$_.Name -eq 'cmd.exe' -and $owned.Contains([int]$_.ProcessId)})){Stop-Process -Id $p.ProcessId -ErrorAction SilentlyContinue}
    foreach($o in $observers){if(!$o.Process.HasExited){Stop-Process -Id $o.Process.Id -ErrorAction SilentlyContinue}}
    foreach($entry in @{
        MVDM_BASESRV_TRACE_PATH=$previous
        MVDM_OBSERVER_INPUT_GATE=$previousGate
        MVDM_OBSERVER_CLOSE_CONSOLE=$previousClose
        MVDM_OBSERVER_RECORD_CONTROL=$previousControlRecord
    }.GetEnumerator()){
        # Preserve absence, not an empty variable (which Win32 size queries
        # distinguish and presence-triggered observer switches would enable).
        if($null -eq $entry.Value){[Environment]::SetEnvironmentVariable($entry.Key,[NullString]::Value)}
        else{[Environment]::SetEnvironmentVariable($entry.Key,[string]$entry.Value)}
    }
    if($inputGate){[void]$inputGate.Set();$inputGate.Dispose()}
    $results | Set-Content -LiteralPath (Join-Path $PackageRoot "logs\$LogPrefix-results.txt") -Encoding utf8
    $results | Write-Output
}
