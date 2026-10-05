[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$RuntimeRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$MonitorClient,
    [Parameter(Mandatory)][string]$LogRoot,
    [ValidateSet('I386','AMD64')][string]$NativeMachine='AMD64',
    [string]$GuiTarget,
    [switch]$Reentry
)
$ErrorActionPreference='Stop'
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$runtime=(Resolve-Path -LiteralPath $RuntimeRoot).Path
$observerPath=(Resolve-Path -LiteralPath $Observer).Path
$monitor=(Resolve-Path -LiteralPath $MonitorClient).Path
$log=[IO.Path]::GetFullPath($LogRoot)
$build=(Resolve-Path "$PSScriptRoot/../../build").Path+'\'
if(!$log.StartsWith($build,[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $log)){throw 'Require fresh build-only evidence'}
if(Test-Path Z:\){throw 'Z: in use'}
$scope=New-IsolatedPackageScope $runtime
$cmd=Join-Path $env:WINDIR $(if($NativeMachine -eq 'AMD64'){'System32\cmd.exe'}else{'SysWOW64\cmd.exe'})
$bytes=[IO.File]::ReadAllBytes($cmd)
$machine=[BitConverter]::ToUInt16($bytes,[BitConverter]::ToInt32($bytes,60)+4)
$expectedMachine=if($NativeMachine -eq 'AMD64'){0x8664}else{0x14c}
$expectedKind=if($NativeMachine -eq 'AMD64'){3}else{2}
if($machine -ne $expectedMachine){throw 'CMD image machine mismatch'}
if($NativeMachine -eq 'AMD64'){$cmd=Join-Path $env:WINDIR 'Sysnative\cmd.exe'}
$expectedImage='cmd.exe'
if($Reentry -and $GuiTarget){throw 'Select GUI projection or native reentry, not both'}
$phasePaths=@();$batch=''
if($Reentry){
    $leaf=Split-Path $log -Leaf
    if($leaf -notmatch '^[a-zA-Z0-9-]+$'){throw 'Reentry requires a bounded fixture name'}
    $phasePaths=@(Join-Path $runtime "tests\$leaf-child.txt";Join-Path $runtime "tests\$leaf-parent.txt")
    $batch=Join-Path $runtime "tests\$leaf.cmd"
    foreach($file in @($phasePaths)+@($batch)){if(Test-Path $file){throw 'Use fresh reentry fixtures'}}
    $child=Join-Path $env:WINDIR $(if($NativeMachine -eq 'AMD64'){'SysWOW64\cmd.exe'}else{'Sysnative\cmd.exe'})
    $lines=@('@echo off',
        ('"Z:\system32\run16.exe" "'+$child+'" /d /c "echo CHILD-PHASE>Z:\tests\'+$leaf+'-child.txt & ping -n 6 127.0.0.1 >nul & echo REENTRY-CHILD-DONE"'),
        'if errorlevel 1 exit /b 81',
        ('echo PARENT-PHASE>Z:\tests\'+$leaf+'-parent.txt'),
        'ping -n 6 127.0.0.1 >nul','echo REENTRY-PARENT-DONE','exit /b 23')
    [IO.File]::WriteAllLines($batch,$lines,[Text.Encoding]::ASCII)
}
if($GuiTarget){
    $gui=(Resolve-Path -LiteralPath $GuiTarget).Path
    if(!$gui.StartsWith($build,[StringComparison]::OrdinalIgnoreCase)){throw 'GUI fixture must be build-only'}
    $bytes=[IO.File]::ReadAllBytes($gui);$pe=[BitConverter]::ToInt32($bytes,60)
    if([BitConverter]::ToUInt16($bytes,$pe+4) -ne $expectedMachine -or
       [BitConverter]::ToUInt16($bytes,$pe+24+68) -ne 2){throw 'GUI fixture machine/subsystem mismatch'}
    $expectedImage='GUI'+$NativeMachine+'-'+(Split-Path $log -Leaf)+'.exe'
    $fixture=Join-Path $runtime ('tests\'+$expectedImage)
    if(Test-Path $fixture){throw 'GUI fixture destination already exists'}
    Copy-Item -LiteralPath $gui -Destination $fixture
    $scope.Paths=@($scope.Paths)+@($fixture)
}
$null=New-Item -ItemType Directory -Path $log
$report=Join-Path $log 'direct.txt'
$savedPrivate=$env:MVDM_OBSERVER_PRIVATE_DESKTOP
$savedHistory=$env:MVDM_OBSERVER_SHORT_HISTORY
$process=$null
& subst.exe Z: $runtime
if($LASTEXITCODE){throw 'SUBST failed'}
$scope.Paths=@($scope.Paths)+@($scope.Paths|ForEach-Object {Join-Path Z:\ $_.Substring($runtime.Length+1)})
try {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    $env:MVDM_OBSERVER_SHORT_HISTORY='1'
    # Ping only holds this direct CMD long enough for read-only snapshots;
    # it does not replace native completion or add a production wait.
    $arguments='"Z:\system32\run16.exe" "Z:\." "'+$report+'" "'+$cmd+'" /d /c "echo MACHINE-BEGIN & ping -n 9 127.0.0.1 >nul & echo MACHINE-END" --observation-timeout-ms 20000'
    if($Reentry){$arguments='"Z:\system32\run16.exe" "Z:\." "'+$report+'" "'+$cmd+'" /d /c "Z:\tests\'+$leaf+'.cmd" --observation-timeout-ms 25000'}
    if($GuiTarget){$arguments='"Z:\system32\run16.exe" "Z:\." "'+$report+'" "Z:\tests\'+$expectedImage+'" held-helper --observation-timeout-ms 20000'}
    $process=Start-Process -FilePath $observerPath -ArgumentList $arguments -WindowStyle Hidden -PassThru
    $deadline=[DateTime]::UtcNow.AddSeconds(15)
    $hit=$false;$index=0;$workerPid=0;$seen=@{}
    do {
        $snapshot=(& $monitor --tree-json 2> (Join-Path $log ('snapshot-'+$index+'.err'))) -join "`n"
        $status=$LASTEXITCODE
        $snapshot | Set-Content (Join-Path $log ('snapshot-'+$index+'.json'))
        ++$index
        if(!$status){
            $rows=@($snapshot|ConvertFrom-Json)
            $wanted=$expectedKind;$phase=''
            if($Reentry){
                if(Test-Path $phasePaths[1]){$phase='parent'}
                elseif(Test-Path $phasePaths[0]){$phase='child';$wanted=if($expectedKind -eq 3){2}else{3}}
            }
            $selected=@($rows|Where-Object {$_.kind -eq $wanted -and
                $_.image -match ('(?i)'+[regex]::Escape($expectedImage)+'$') -and
                (!$GuiTarget -or $_.category -eq 4)})
            $hit=$selected.Count -eq 1
            if($Reentry){
                $workers=@($rows|Where-Object {$_.category -eq 2 -and $_.kind -in @(2,3)})
                if($workers.Count -gt 1){throw 'Bitness-only reentry created another NTVWM worker'}
                if($hit -and $phase){
                    if($workerPid -and $workerPid -ne $selected[0].pid){throw 'Worker changed across actual native reentry'}
                    $workerPid=$selected[0].pid;$seen[$phase]=$wanted
                }
                $hit=$seen.ContainsKey('child') -and $seen.ContainsKey('parent')
            }
        }
        if($hit){break}
        $null=$process.WaitForExit(100)
    }while((!$process.HasExited -or $GuiTarget) -and [DateTime]::UtcNow -lt $deadline)
    if(!$hit){throw 'Actual native direct task machine not projected by authenticated NTSRV snapshot'}
    if(!$process.WaitForExit(25000)){throw 'Direct observer did not finish'}
    if($process.ExitCode){throw 'Direct observer failed'}
    $result=Get-Content $report -Raw
    if($GuiTarget){
        if($result -notmatch 'result=exited' -or $result -notmatch 'exit=0x00000000'){throw 'GUI startup-only completion failed'}
        & $monitor --close-node 4 ([string]$selected[0].pid) *> (Join-Path $log 'gui-close.txt')
        if($LASTEXITCODE){throw 'Authenticated exact GUI close failed'}
        "PASS actual $NativeMachine GUI: authenticated detached kind=$expectedKind, startup-only result and exact management close"
        return
    }
    $screen=Get-Content ($report+'.console.txt') -Raw
    if($Reentry){
        if($result -notmatch 'result=exited' -or $result -notmatch 'exit=0x00000017' -or
           $screen.IndexOf('REENTRY-CHILD-DONE') -lt 0 -or
           $screen.IndexOf('REENTRY-PARENT-DONE') -le $screen.IndexOf('REENTRY-CHILD-DONE')){throw 'Native reentry output/real parent receipt failed'}
        "PASS $NativeMachine opposite-width reentry: same NTVWM PID=$workerPid, child kind=$($seen.child), parent kind=$($seen.parent), output order and real exit23"
        return
    }
    if($result -notmatch 'result=exited' -or $result -notmatch 'exit=0x00000000' -or
       $screen.IndexOf('MACHINE-BEGIN') -lt 0 -or $screen.IndexOf('MACHINE-END') -le $screen.IndexOf('MACHINE-BEGIN')){throw 'Direct completion/output order failed'}
    "PASS actual $NativeMachine direct target: authenticated kind=$expectedKind and real completion/output order"
}finally{
    if($process){if(!$process.HasExited){$process.Kill();$null=$process.WaitForExit(5000)};$process.Dispose()}
    try{Stop-IsolatedPackageScope $scope}finally{
        & subst.exe Z: /d
        $env:MVDM_OBSERVER_PRIVATE_DESKTOP=$savedPrivate
        $env:MVDM_OBSERVER_SHORT_HISTORY=$savedHistory
    }
}
