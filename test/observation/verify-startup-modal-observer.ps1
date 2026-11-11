[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$Fixture,
    [Parameter(Mandatory)][string]$Report
)
$ErrorActionPreference='Stop'
$Observer=(Resolve-Path -LiteralPath $Observer).Path
$Fixture=(Resolve-Path -LiteralPath $Fixture).Path
if(Test-Path -LiteralPath $Report){throw 'Do not overwrite existing evidence'}
$previous=$env:MVDM_OBSERVER_PRIVATE_DESKTOP
try {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    & $Observer $Fixture (Split-Path $Fixture) $Report --observation-timeout-ms 10000
    if($LASTEXITCODE){throw 'Observer failed'}
    $result=Get-Content -LiteralPath $Report
    if($result -notcontains 'result=timeout'){throw 'Expected an intentional modal timeout'}
    $pidLine=$result | Where-Object {$_ -match '^pid=\d+$'}
    if(!$pidLine){throw 'Missing fixture identity'}
    $fixtureId=$pidLine.Substring(4)
    $live=Get-Content -LiteralPath ($Report+'.timeout-live.txt')
    if(!($live -match '^windows-desktop=NTVDMConsoleTest-\d+$') -or
        !($live -match ('^window pid='+$fixtureId+' visible=1 class=#32770 title=NTVDM-STARTUP-MODAL-WITNESS$'))){
        throw 'Modal window and live fixture identity not observed before termination'
    }
    Write-Output 'PASS private-desktop modal caption/PID captured before watchdog termination'
} finally {$env:MVDM_OBSERVER_PRIVATE_DESKTOP=$previous}
