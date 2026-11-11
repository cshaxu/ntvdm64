[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$ProcessPackageRoot,
    [Parameter(Mandatory)][string]$LogPrefix,
    [ValidateSet('WIN32','WIN64')][string]$ExpectedKind='WIN32',
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
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$scope=New-IsolatedPackageScope $physical $PackageRoot
$binary=Get-PackageBinaryRoot $PackageRoot
$physicalBinary=Get-PackageBinaryRoot $physical
foreach($name in @('run16.exe','ntcon.exe','ntsrv.exe','ntmon.exe','ntvwm.exe')){
    $launch=Join-Path $binary $name;$actual=Join-Path $physicalBinary $name
    if((Get-FileHash $launch).Hash -ne (Get-FileHash $actual).Hash){throw 'Candidate identity mismatch'}
    $paths+=@($launch,$actual)
}
$image=[IO.File]::ReadAllBytes((Join-Path $binary 'ntmon.exe'))
$pe=[BitConverter]::ToInt32($image,60)
$expectedMachine=if($ExpectedKind -eq 'WIN64'){0x8664}else{0x14c}
if([BitConverter]::ToUInt16($image,$pe+4) -ne $expectedMachine){throw 'Monitor kind expectation differs from actual target machine'}
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
            & $Observer (Join-Path $binary 'run16.exe') $PackageRoot $report `
                --observation-timeout-ms 15000 --observe-console-input-marker 'CONSOLE' `
                --observe-console-input-text ([string][char]27) (Join-Path $binary 'ntmon.exe')
            if($LASTEXITCODE){throw 'Observer failed'}
            $record=Get-Content $report -Raw
            if(!(Test-Path ($report+'.pre-input-console.txt.console.txt'))){
                throw "NTMon never reached the authoritative tree/input gate in $mode; see $report"
            }
            $screen=Get-Content ($report+'.pre-input-console.txt.console.txt') -Raw
            if($record -notmatch '(?m)^result=exited\r?$' -or
               $record -notmatch '(?m)^exit=0x00000000\r?$' -or
               $record -notmatch '(?m)^scripted-console-input=delivered\r?$' -or
               # This observer captures only the physical viewport, which may
               # clip the centered title on narrow RDP/private desktops. The
               # formal layout fixture checks all 80 title/footer cells.
               $screen -notmatch 'NTVDM Task' -or
               $screen -notmatch 'CONSOLE' -or $screen -notmatch $ExpectedKind -or
               $screen -match 'UNBOUND|MEMBERS='){
                throw "Actual NTMon tree/ESC/exit failed in $mode"
            }
            if($mode -eq 'window'){
                $caf=Get-Content ($report+'.caf.txt') -Raw
                if($caf -notmatch 'caf-visible-window=1' -or $caf -notmatch 'window-input-owner=\d+' -or
                   $caf -notmatch '(?m)^result=pass error=0\r?$'){throw 'Missing actual Window/input routing witness'}
                $inputRecord=Get-Content ($report+'.window-input.txt') -Raw
                if($inputRecord -notmatch 'frontend=\d+ input-delivered=1 '){throw 'Window did not receive ESC'}
            }
            Write-Output "PASS actual NTMon $mode title, ESC input and direct exit 0 through NTVWM"
        } finally {
            Stop-IsolatedPackageScope $scope
        }
    }
} finally {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP=$oldPrivate
    $env:MVDM_OBSERVER_WINDOW_INPUT=$oldWindow
    $env:MVDM_OBSERVER_BUFFER_COLUMNS=$oldColumns
}
