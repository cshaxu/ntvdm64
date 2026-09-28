[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$ProcessPackageRoot,
    [Parameter(Mandatory)][string]$LogPrefix,
    [string]$LogRoot='O:\winnt\Logs2'
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$physical=(Resolve-Path $ProcessPackageRoot).Path
if(!$physical.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase)){
    throw 'Use an isolated build candidate'
}
if($LogPrefix -notmatch '^[a-z0-9-]+$'){throw 'Invalid log prefix'}
$paths=@()
foreach($name in @('run16.exe','ntkvm.exe','ntsrv.exe','ntmon.exe')){
    $launch=Join-Path $PackageRoot $name;$actual=Join-Path $physical $name
    if((Get-FileHash $launch).Hash -ne (Get-FileHash $actual).Hash){throw 'Candidate identity mismatch'}
    $paths+=@($launch,$actual)
}
if(@(Get-CimInstance Win32_Process -Filter "Name='ntsrv.exe'").Count){throw 'Broker already active'}
$oldPrivate=$env:MVDM_OBSERVER_PRIVATE_DESKTOP
$oldWindow=$env:MVDM_OBSERVER_WINDOW_INPUT
$oldColumns=$env:MVDM_OBSERVER_BUFFER_COLUMNS
try {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    # Match the original 120-column native-TUI acceptance buffer. Keep the
    # physical viewport/font unchanged, including narrow RDP scrollbars.
    $env:MVDM_OBSERVER_BUFFER_COLUMNS='120'
    foreach($mode in @('console','window')){
        $report=Join-Path $LogRoot "$LogPrefix-$mode.txt"
        if(Test-Path $report){throw 'Use fresh evidence'}
        if($mode -eq 'window'){$env:MVDM_OBSERVER_WINDOW_INPUT='1'}
        else{Remove-Item Env:MVDM_OBSERVER_WINDOW_INPUT -ErrorAction SilentlyContinue}
        try {
            & $Observer (Join-Path $PackageRoot 'run16.exe') $PackageRoot $report `
                --observation-timeout-ms 15000 --observe-console-input-marker 'NTVDM Task Monitor' `
                --observe-console-function-key 3 (Join-Path $PackageRoot 'ntmon.exe')
            if($LASTEXITCODE){throw 'Observer failed'}
            $record=Get-Content $report -Raw
            $screen=Get-Content ($report+'.pre-input-console.txt.console.txt') -Raw
            if($record -notmatch '(?m)^result=exited\r?$' -or
               $record -notmatch '(?m)^exit=0x00000000\r?$' -or
               $record -notmatch '(?m)^scripted-console-input=delivered\r?$' -or
               $screen -notmatch 'NTVDM Task Monitor'){
                throw "Actual NTMon title/F3/exit failed in $mode"
            }
            if($mode -eq 'window'){
                $caf=Get-Content ($report+'.caf.txt') -Raw
                if($caf -notmatch 'caf-visible-window=1' -or $caf -notmatch 'window-input-owner=\d+' -or
                   $caf -notmatch '(?m)^result=pass error=0\r?$'){throw 'Missing actual Window/input routing witness'}
                $inputRecord=Get-Content ($report+'.window-input.txt') -Raw
                if($inputRecord -notmatch 'frontend=\d+ input-delivered=1 '){throw 'Window did not receive the function key'}
            }
            Write-Output "PASS actual NTMon $mode title, F3 input and direct exit 0 through ConPTY"
        } finally {
            Get-CimInstance Win32_Process | Where-Object {$_.ExecutablePath -in $paths} | ForEach-Object {
                $process=Get-Process -Id $_.ProcessId -ErrorAction SilentlyContinue
                if($process){try{$null=$process.Handle;$process.Kill();$null=$process.WaitForExit(5000)}finally{$process.Dispose()}}
            }
        }
    }
} finally {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP=$oldPrivate
    $env:MVDM_OBSERVER_WINDOW_INPUT=$oldWindow
    $env:MVDM_OBSERVER_BUFFER_COLUMNS=$oldColumns
}
