[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$LogPrefix,
    [string]$FixtureRoot='O:\winnt\tests',
    [string]$LogRoot='O:\winnt\Logs2'
)
$ErrorActionPreference='Stop'
if($LogPrefix -notmatch '^[a-z0-9-]+$'){throw 'Invalid log prefix'}
$Observer=(Resolve-Path -LiteralPath $Observer).Path
$PackageRoot=(Resolve-Path -LiteralPath $PackageRoot).Path
$LogRoot=(Resolve-Path -LiteralPath $LogRoot).Path
if(@(Get-CimInstance Win32_Process -Filter "Name='ntsrv.exe'").Count){throw 'Existing broker'}
$names=@('MVDM_OBSERVER_PRIVATE_DESKTOP','MVDM_OBSERVER_MOUSE_HOOK',
    'MVDM_OBSERVER_MOUSE_BURST','MVDM_OBSERVER_MOUSE_RETIRE')
$saved=@{}
foreach($name in $names){$saved[$name]=[Environment]::GetEnvironmentVariable($name)}
try {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    $env:MVDM_OBSERVER_MOUSE_HOOK=Join-Path $FixtureRoot 'S7MOUSE.dll'
    foreach($case in @(@{Name='burst';Count=1000},@{Name='retire';Count=1000;Retire=$true},@{Name='latency';Count=200})){
        $env:MVDM_OBSERVER_MOUSE_BURST=[string]$case.Count
        if($case.Retire){$env:MVDM_OBSERVER_MOUSE_RETIRE='1'}
        else{Remove-Item Env:MVDM_OBSERVER_MOUSE_RETIRE -ErrorAction SilentlyContinue}
        $report=Join-Path $LogRoot ($LogPrefix+'-'+$case.Name+'.txt')
        if(Test-Path -LiteralPath $report){throw 'Evidence already exists'}
        & $Observer (Join-Path $PackageRoot 'run16.exe') $PackageRoot $report --observation-timeout-ms 60000 (Join-Path $FixtureRoot 'WMS7.COM')
        if($LASTEXITCODE -or !(Test-Path -LiteralPath $report)){throw 'Observer failed'}
        $record=Get-Content -LiteralPath $report -Raw
        $screen=Get-Content -LiteralPath ($report+'.console.txt') -Raw
        $hook=Get-Content -LiteralPath ($report+'.mouse-window.txt') -Raw
        if($record -notmatch '(?m)^result=exited\r?$' -or
           $record -notmatch '(?m)^exit=0x00000000\r?$' -or
           $screen -notmatch 'WINDOW-MOUSE-PASS' -or
           $hook -notmatch ('(?m)^burst-records='+$case.Count+'\r?$') -or
           $hook -notmatch 'posted=pass'){throw ('Mouse pressure failed: '+$case.Name)}
        Write-Output ('PASS real guest mouse pressure '+$case.Name+' count='+$case.Count)
    }
} finally {
    foreach($name in $names){
        if($null -eq $saved[$name]){Remove-Item -LiteralPath ('Env:'+$name) -ErrorAction SilentlyContinue}
        else{Set-Item -LiteralPath ('Env:'+$name) -Value $saved[$name]}
    }
}
