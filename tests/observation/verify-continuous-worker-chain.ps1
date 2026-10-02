[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$BuildRoot,
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$LogPrefix,
    [string]$LogRoot='O:\winnt\Logs2'
)
$ErrorActionPreference='Stop'
if($LogPrefix -notmatch '^[a-z0-9-]+$'){throw 'Invalid log prefix'}
$Observer=(Resolve-Path $Observer).Path
$BuildRoot=(Resolve-Path $BuildRoot).Path
$PackageRoot=(Resolve-Path $PackageRoot).Path.TrimEnd('\')+'\'
$LogRoot=(Resolve-Path $LogRoot).Path
if($PackageRoot -notmatch '^[A-Za-z]:\\$'){throw 'Use the existing short-drive candidate mapping'}
if(@(Get-CimInstance Win32_Process -Filter "Name='ntsrv.exe'").Count){throw 'Broker already active'}
$events=Join-Path $LogRoot "$LogPrefix.events.txt"
$report=Join-Path $LogRoot "$LogPrefix.observer.txt"
if((Test-Path $events) -or (Test-Path $report)){throw 'Use fresh evidence names'}
$plan=Join-Path $PackageRoot 'tests\CHAIN.INI'
$launcher=Join-Path $PackageRoot 'run16.exe'
$probe=Join-Path $PackageRoot 'tests\CTEST.EXE'
Copy-Item (Join-Path $BuildRoot 'frontend-chain-cui.exe') $probe
Copy-Item (Join-Path $BuildRoot 'frontend-chain-input.exe') (Join-Path $PackageRoot 'tests\CINPUT.EXE')
$kinds='DDWWDDWW'; $commands=@{}; $expected=@{}
for($stage=8;$stage -ge 1;$stage--){
    $expected[$stage]=if($kinds[$stage-1] -eq 'D'){0}elseif($stage -eq 8){37}else{$expected[$stage+1]}
    $commands[$stage]=if($kinds[$stage-1] -eq 'D'){
        "$launcher $($PackageRoot)COMMAND.COM /c $($PackageRoot)tests\C$('{0:00}' -f $stage).BAT"
    }else{"$launcher $probe $plan $stage"}
}
$ini=@('[chain]',"kinds=$kinds","report=$events")
for($stage=1;$stage -le 8;$stage++){
    $ini+=@("[$stage]","command=$($commands[$stage])")
    if($kinds[$stage-1] -ne 'D'){continue}
    $batch=@('@echo off',"echo GUEST-STAGE-$stage-ENTER","$launcher $probe $plan $stage ENTER",
        'if errorlevel 1 goto fail',"echo READY-$stage-ENTER",'pause',"echo ACK-$stage-ENTER",'pause',
        $commands[$stage+1],"if errorlevel $($expected[$stage+1]+1) goto fail",
        "if not errorlevel $($expected[$stage+1]) goto fail",
        "$launcher $probe $plan $stage RETURN",'if errorlevel 1 goto fail',
        "echo READY-$stage-RETURN",'pause',"echo ACK-$stage-RETURN",'pause',
        "echo GUEST-STAGE-$stage-RETURN",'goto end',':fail',"echo CHAIN-FAIL-$stage",':end')
    [IO.File]::WriteAllText((Join-Path $PackageRoot ('tests\C{0:00}.BAT' -f $stage)),
        ($batch -join "`r`n")+"`r`n",[Text.Encoding]::ASCII)
}
[IO.File]::WriteAllText($plan,($ini -join "`r`n")+"`r`n",[Text.Encoding]::ASCII)
$oldPrivate=$env:MVDM_OBSERVER_PRIVATE_DESKTOP
try {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    & $Observer $launcher $PackageRoot $report --observation-timeout-ms 60000 COMMAND.COM /c ($PackageRoot+'tests\C01.BAT')
    if($LASTEXITCODE){throw 'Observer failed'}
    $result=Get-Content $report -Raw
    if($result -notmatch '(?m)^result=exited\r?$' -or $result -notmatch '(?m)^exit=0x00000000\r?$'){
        throw 'Eight-target chain did not return normally'
    }
    $rows=@(Get-Content $events | ForEach-Object {
        if($_ -notmatch '^(ENTER|RETURN) (\d+) ([DW]) (\d+) (\d+) (\d+) (\d+) (\d+)$'){throw "Invalid event: $_"}
        [pscustomobject]@{Phase=$Matches[1];Stage=[int]$Matches[2];Kind=$Matches[3];
            Owner=[uint32]$Matches[5];Generation=[uint32]$Matches[6];Result=[uint32]$Matches[8]}
    })
    $sequence=@(1..8 | ForEach-Object {"ENTER-$_"})+@(8..1 | ForEach-Object {"RETURN-$_"})
    if($rows.Count -ne 16 -or (($rows | ForEach-Object {"$($_.Phase)-$($_.Stage)"}) -join ',') -ne ($sequence -join ',')){
        throw 'Incomplete or reordered eight-target nesting'
    }
    $identities=@($rows | ForEach-Object {"$($_.Owner):$($_.Generation)"} | Select-Object -Unique)
    if($identities.Count -ne 1 -or !$rows[0].Owner -or !$rows[0].Generation){throw 'Frontend changed within continuous chain'}
    foreach($row in $rows){
        if($row.Kind -ne [string]$kinds[$row.Stage-1] -or
            ($row.Phase -eq 'RETURN' -and $row.Result -ne $expected[$row.Stage])){throw 'Kind or direct result mismatch'}
        $io=Get-Content ($events+".io-$($row.Stage)-$($row.Phase)") -Raw
        if($io -match 'FAIL' -or $io -notmatch '(?m)^PASS visible-output-and-input' -or
            $io -notmatch "(?m)^READY READY-$($row.Stage)-$($row.Phase)" -or
            $io -notmatch "(?m)^ACK ACK-$($row.Stage)-$($row.Phase)"){throw 'Missing real input/output checkpoint'}
    }
    $epoch=$null; $dosSequence=$null; $nativeSequence=$null; $tasks=@{}
    foreach($stage in @(1,2,5,6)){
        $depth=@(1..$stage | Where-Object {$kinds[$_-1] -eq 'D'}).Count
        foreach($phase in @('ENTER','RETURN')){
            $lines=Get-Content "$events.records-$stage-$phase"
            if($lines[0] -notmatch '^EPOCH (\d+)$'){throw 'Missing epoch'}
            if(!$epoch){$epoch=$Matches[1]}elseif($epoch -ne $Matches[1]){throw 'Broker changed'}
            $records=@(foreach($line in $lines | Select-Object -Skip 1){
                if($line -notmatch '^RECORD (\d+) (\d+) (\d+) (\d+) (\d+) (.*)$'){throw 'Invalid record'}
                [pscustomobject]@{Id=$Matches[1];Task=$Matches[2];Kind=[int]$Matches[3];State=[int]$Matches[4];Depth=[int]$Matches[5];Image=$Matches[6]}
            })
            $dos=@($records | Where-Object Kind -eq 1)
            $native=@($records | Where-Object Kind -eq 4)
            if($dos.Count -ne 1 -or $native.Count -ne 1 -or $dos[0].Depth -ne $depth -or
                $dos[0].State -ne 2 -or $dos[0].Image -notmatch '(?i)\\COMMAND\.COM$'){throw 'Worker count or original DOS stack mismatch'}
            if(!$dosSequence){$dosSequence=$dos[0].Id;$nativeSequence=$native[0].Id}
            if($dosSequence -ne $dos[0].Id -or $nativeSequence -ne $native[0].Id){throw 'Worker was replaced across DOS/native intervals'}
            if($phase -eq 'ENTER'){$tasks[$stage]=$dos[0].Task}
            elseif($tasks[$stage] -ne $dos[0].Task){throw 'Parent DOS task was not restored'}
        }
    }
    # Root DOS RETURN is recorded before its final PAUSE/exit. Verify empty
    # records separately after the observed root launcher has actually exited.
    $final=$events+'.completed'
    & $probe --snapshot $final
    if($LASTEXITCODE){throw 'Final service snapshot failed'}
    $lines=Get-Content "$final.records-1-RETURN"
    if($lines[0] -ne "EPOCH $epoch"){throw 'Final broker identity changed'}
    foreach($line in $lines | Select-Object -Skip 1){
        if($line -notmatch '^RECORD (\d+) (\d+) (\d+) (\d+) (\d+) (.*)$'){throw 'Invalid final record'}
        if([int]$Matches[3] -in @(1,4) -and ([int]$Matches[5] -ne 0 -or [int]$Matches[4] -ne 8 -or $Matches[6] -ne '<EMPTY>')){
            throw 'Unfinished final worker record'
        }
    }
    $frontend=Get-Process -Id $rows[0].Owner -ErrorAction SilentlyContinue
    if($frontend){try {if(!$frontend.WaitForExit(15000)){throw 'Empty frontend did not retire'}}finally{$frontend.Dispose()}}
    'PASS DDWWDDWW: sixteen input/output checkpoints, one frontend, same DOS/native workers, original DOS depth/task restore, direct results and empty completion'
    # Empty independent workers may remain resident. This is test housekeeping,
    # never evidence that frontend exit should terminate a worker.
    $image=Join-Path $PackageRoot 'ntw32.exe'
    foreach($entry in @(Get-CimInstance Win32_Process -Filter "Name='ntw32.exe'")){
        $process=Get-Process -Id $entry.ProcessId -ErrorAction Stop
        try {
            $null=$process.Handle
            if($process.Path -ne $image -or (Get-FileHash $process.Path).Hash -ne (Get-FileHash $image).Hash){throw 'Unrelated worker; do not clean'}
            $process.Kill()
            if(!$process.WaitForExit(10000)){throw 'Test worker cleanup timed out'}
        } finally {$process.Dispose()}
    }
    $deadline=[DateTime]::UtcNow.AddSeconds(20)
    do {
        $broker=@(Get-CimInstance Win32_Process -Filter "Name='ntsrv.exe'")
        if(!$broker.Count){break}
        Start-Sleep -Milliseconds 100
    } while([DateTime]::UtcNow -lt $deadline)
    if($broker.Count){throw 'Broker remained after fixture worker cleanup'}
} finally {$env:MVDM_OBSERVER_PRIVATE_DESKTOP=$oldPrivate}
