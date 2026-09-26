[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [string]$PackageRoot='O:\winnt',
    [Parameter(Mandatory)][string]$LogPrefix
)
$ErrorActionPreference='Stop'
if($LogPrefix -notmatch '^[a-z0-9-]+$'){throw 'Invalid log prefix'}
$Observer=(Resolve-Path -LiteralPath $Observer).Path
$PackageRoot=(Resolve-Path -LiteralPath $PackageRoot).Path
$paths=@('run16.exe','ntvdm.exe','basesrv.exe') | ForEach-Object {Join-Path $PackageRoot $_}
function PackageProcesses {
    @(Get-CimInstance Win32_Process -Filter "Name='run16.exe' OR Name='ntvdm.exe' OR Name='basesrv.exe'" |
        Where-Object {$_.ExecutablePath -in $paths})
}
if((PackageProcesses).Count){throw 'Package already in use'}
$oldPrivate=$env:MVDM_OBSERVER_PRIVATE_DESKTOP
$oldControl=$env:MVDM_OBSERVER_SEND_CONTROL
try {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    foreach($control in @('c','break')){
        $report=Join-Path $PackageRoot "logs\$LogPrefix-$control.txt"
        if(Test-Path -LiteralPath $report){throw 'Use a fresh log prefix'}
        $env:MVDM_OBSERVER_SEND_CONTROL=$control
        try {
            & $Observer (Join-Path $PackageRoot 'run16.exe') $PackageRoot $report COMMAND.COM `
                --observation-timeout-ms 20000 --observe-console-input-text "ver`rmem`rexit`r"
            if($LASTEXITCODE){throw "Observer failed: $LASTEXITCODE"}
            $record=Get-Content -LiteralPath $report -Raw
            $screen=Get-Content -LiteralPath ($report+'.console.txt') -Raw
            $event=Get-Content -LiteralPath ($report+'.generated-control.txt') -Raw
            $expected=if($control -eq 'c'){0}else{1}
            if($event.Trim() -ne "generated-control=$expected"){throw 'No real control event witness'}
            if($record -notmatch '(?m)^result=exited' -or
                $record -notmatch '(?m)^exit=0x00000001' -or
                $record -notmatch '(?m)^scripted-console-input-remaining=0'){
                throw 'COMMAND failed to consume commands and return original exit 1'
            }
            $compact=$screen -replace '\s',''
            if($compact -notmatch 'MicrosoftWindows\[Version' -or
                $compact -notmatch 'bytesavailableXMSmemory' -or
                $compact -match 'Badcommandorfilename|illegalinstruction'){
                throw 'Missing successful VER/MEM guest output after control event'
            }
            Write-Output "PASS DOS ${control}: real event, VER/MEM output, original exit 1"
        } finally {
            # Only the observer-recorded children from this invocation are ours.
            if(Test-Path -LiteralPath $report){
                $record=Get-Content -LiteralPath $report -Raw
                $ids=@([regex]::Matches($record,'(?m)^direct-child pid=(\d+)') |
                    ForEach-Object {[int]$_.Groups[1].Value})
                foreach($process in (PackageProcesses | Where-Object {$_.ProcessId -in $ids})){
                    Stop-Process -Id $process.ProcessId -ErrorAction SilentlyContinue
                }
            }
        }
    }
} finally {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP=$oldPrivate
    $env:MVDM_OBSERVER_SEND_CONTROL=$oldControl
}
