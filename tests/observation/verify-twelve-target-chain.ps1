[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$BuildRoot,
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$EvidenceRoot,
    [ValidateSet('WDW-DWD','DDW-WWD')][string[]]$Cases = @('WDW-DWD','DDW-WWD')
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$BuildRoot=(Resolve-Path $BuildRoot).Path
$Observer=(Resolve-Path $Observer).Path
$EvidenceRoot=[IO.Path]::GetFullPath($EvidenceRoot)
if(!$EvidenceRoot.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase)){
    throw 'Evidence must be under repository build'
}
if(Test-Path $EvidenceRoot){throw 'Use a new evidence directory'}
if($PackageRoot -notmatch '^[A-Za-z]:\\?$'){throw 'Use the isolated package short-drive mapping'}
$PackageRoot=$PackageRoot.TrimEnd('\')+'\'
if(!(Test-Path (Join-Path $PackageRoot 'tests'))){throw 'Existing candidate tests directory required'}
if(@(Get-CimInstance Win32_Process -Filter "Name='basesrv.exe'").Count){
    throw 'A broker is already active; preserve its session and run this isolated suite when the endpoint is free'
}
foreach($case in $Cases){if($case -notmatch '^[DW]{3}-[DW]{3}$'){throw "Invalid case $case"}}
$null=New-Item -ItemType Directory -Path $EvidenceRoot
Copy-Item (Join-Path $BuildRoot 'frontend-chain-cui.exe') (Join-Path $PackageRoot 'tests\CTEST.EXE')
Copy-Item (Join-Path $BuildRoot 'frontend-chain-gui.exe') (Join-Path $PackageRoot 'tests\GTEST.EXE')
Copy-Item (Join-Path $BuildRoot 'frontend-chain-input.exe') (Join-Path $PackageRoot 'tests\CINPUT.EXE')
$oldPrivate=$env:MVDM_OBSERVER_PRIVATE_DESKTOP
$summary=[Collections.Generic.List[object]]::new()
try {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    foreach($case in $Cases){
        $parts=$case.Split('-'); $kinds='GGG'+$parts[0]+'GGG'+$parts[1]
        $events=Join-Path $EvidenceRoot "$case.events.txt"
        $observation=Join-Path $EvidenceRoot "$case.observer.txt"
        $plan=Join-Path $PackageRoot 'tests\CHAIN.INI'
        $launcher=Join-Path $PackageRoot 'run16.exe'
        $cui=Join-Path $PackageRoot 'tests\CTEST.EXE'
        $gui=Join-Path $PackageRoot 'tests\GTEST.EXE'
        $commands=@{}; $expected=@{}
        for($stage=12;$stage -ge 1;$stage--){
            $kind=$kinds[$stage-1]
            $expected[$stage]=if($kind -eq 'D'){0}elseif($stage -eq 12){37}else{$expected[$stage+1]}
            $commands[$stage]=if($kind -eq 'D'){
                "$launcher $($PackageRoot)COMMAND.COM /c $($PackageRoot)tests\C$('{0:00}' -f $stage).BAT"
            }else{"$launcher $(if($kind -eq 'G'){$gui}else{$cui}) $plan $stage"}
        }
        $ini=@('[chain]',"kinds=$kinds","report=$events")
        for($stage=1;$stage -le 12;$stage++){
            $ini+=@("[$stage]","command=$($commands[$stage])")
            if($kinds[$stage-1] -ne 'D'){continue}
            $batch=@('@echo off',"echo GUEST-STAGE-$stage-ENTER","$launcher $cui $plan $stage ENTER",
                'if errorlevel 1 goto fail',"echo READY-$stage-ENTER",'pause',"echo ACK-$stage-ENTER",'pause')
            if($stage -lt 12){
                $batch+=@($commands[$stage+1],"if errorlevel $($expected[$stage+1]+1) goto fail",
                    "if not errorlevel $($expected[$stage+1]) goto fail")
            }
            $batch+=@("$launcher $cui $plan $stage RETURN",'if errorlevel 1 goto fail',
                "echo READY-$stage-RETURN",'pause',"echo ACK-$stage-RETURN",'pause',
                "echo GUEST-STAGE-$stage-RETURN",'goto end',':fail',"echo CHAIN-FAIL-$stage",':end')
            [IO.File]::WriteAllText((Join-Path $PackageRoot ('tests\C{0:00}.BAT' -f $stage)),
                ($batch -join "`r`n")+"`r`n",[Text.Encoding]::ASCII)
            Copy-Item (Join-Path $PackageRoot ('tests\C{0:00}.BAT' -f $stage)) `
                (Join-Path $EvidenceRoot ("$case-C{0:00}.BAT" -f $stage))
        }
        [IO.File]::WriteAllText($plan,($ini -join "`r`n")+"`r`n",[Text.Encoding]::ASCII)
        # Seal the generated caller inputs beside the evidence for each case.
        Copy-Item $plan (Join-Path $EvidenceRoot "$case.ini")
        & $Observer $launcher $PackageRoot $observation --observation-timeout-ms 60000 $gui $plan 1
        if($LASTEXITCODE){throw "Observer failed for $case"}
        $observed=Get-Content $observation -Raw
        if($observed -notmatch '(?m)^result=exited' -or
            $observed -notmatch ('(?m)^exit=0x{0:x8}\r?$' -f $expected[1])){
            throw "Chain $case did not complete with expected direct result; inspect $observation"
        }
        $rows=@(Get-Content $events | ForEach-Object {
            $f=$_ -split ' '
            if($f.Count -ne 8 -or $f[0] -notin @('ENTER','RETURN')){throw "Invalid chain event: $_"}
            [pscustomobject]@{Phase=$f[0];Stage=[int]$f[1];Kind=$f[2];Pid=[uint32]$f[3];
                Owner=[uint32]$f[4];Generation=[uint32]$f[5];Child=[uint32]$f[6];Result=[uint32]$f[7]}
        })
        $sequence=@(1..12 | ForEach-Object {"ENTER-$_"})+@(12..1 | ForEach-Object {"RETURN-$_"})
        if($rows.Count -ne 24 -or (($rows | ForEach-Object {"$($_.Phase)-$($_.Stage)"}) -join ',') -ne ($sequence -join ',')){
            throw "Incomplete or reordered twelve-target nesting: $case"
        }
        $owners=@{}
        foreach($row in $rows){
            if($row.Kind -ne [string]$kinds[$row.Stage-1]){throw 'Target type changed'}
            if($row.Kind -eq 'G'){
                if($row.Owner -or $row.Generation){throw 'GUI inherited frontend membership'}
            }else{
                if(!$row.Owner -or !$row.Generation){throw 'Missing authenticated character identity'}
                $group=if($row.Stage -le 6){1}else{2}
                $identity="$($row.Owner):$($row.Generation)"
                if($owners.ContainsKey($group) -and $owners[$group] -ne $identity){throw 'Within-group frontend changed'}
                $owners[$group]=$identity
            }
            if($row.Phase -eq 'RETURN' -and $row.Result -ne $expected[$row.Stage]){throw 'Unexpected direct-target result'}
        }
        if($owners[1] -eq $owners[2]){throw 'Two character groups share a frontend'}
        foreach($stage in @(4,5,6,10,11,12)){
            foreach($phase in @('ENTER','RETURN')){
                $io=Get-Content ($events+".io-$stage-$phase") -Raw
                if($io -match 'FAIL' -or $io -notmatch '(?m)^PASS visible-output-and-input' -or
                    $io -notmatch "(?m)^READY READY-$stage-$phase" -or $io -notmatch "(?m)^ACK ACK-$stage-$phase"){
                    throw "Missing visible output/input assertion at stage $stage $phase"
                }
            }
        }
        $topology=@(Get-Content ($events+'.topology') | ForEach-Object {
            $f=$_ -split ' '
            if($f.Count -ne 4 -or $f[0] -ne 'TOPOLOGY'){throw 'Invalid topology observation'}
            [pscustomobject]@{Stage=[int]$f[1];Count=[int]$f[2];First=[uint32]$f[3]}
        })
        foreach($stage in 1..12){
            $seen=@($topology | Where-Object Stage -eq $stage)
            if(!$seen.Count){throw "Missing live topology observation at layer $stage"}
            foreach($entry in $seen){
                if($stage -le 3 -and $entry.Count){throw 'Pure GUI prefix created a frontend'}
                if($stage -ge 7 -and $entry.First -ne [uint32]($owners[1].Split(':')[0])){
                    throw 'First frontend was not alive during second segment execution'
                }
            }
        }
        # Observe natural retirement before the caller's failure-cleanup step.
        # These PIDs are fixture observations, never production authorization.
        $frontendIds=@($rows | Where-Object Owner | Select-Object -ExpandProperty Owner -Unique)
        $helperIds=@(Get-CimInstance Win32_Process -Filter "Name='frontend.exe'" |
            Where-Object {$_.ParentProcessId -in $frontendIds} | Select-Object -ExpandProperty ProcessId)
        foreach($processId in @($frontendIds)+@($helperIds)){
            $process=Get-Process -Id $processId -ErrorAction SilentlyContinue
            if(!$process){continue}
            try {
                $null=$process.Handle
                if(!$process.WaitForExit(10000)){throw "Frontend/helper $processId did not retire naturally"}
            } finally {$process.Dispose()}
        }
        $summary.Add([pscustomobject]@{Case=$case;Nesting='pass';Identity='pass';Results='pass';
            Frontend1=$owners[1];Frontend2=$owners[2];LiveTopology='pass';InteractiveIO='pass';Retirement='pass'})
        $summary | ConvertTo-Json -Depth 4 | Set-Content (Join-Path $EvidenceRoot 'summary.json')
        & "$PSScriptRoot/verify-twelve-chain-records.ps1" -EvidenceRoot $EvidenceRoot
        Write-Output "PASS twelve-target nesting/identity/results/topology/visible-input-output/retirement $case"
    }
} finally {$env:MVDM_OBSERVER_PRIVATE_DESKTOP=$oldPrivate}
