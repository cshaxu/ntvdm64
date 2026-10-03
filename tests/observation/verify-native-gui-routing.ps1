param([Parameter(Mandatory)][string]$Observer,
      [Parameter(Mandatory)][string]$PackageRoot,
      [Parameter(Mandatory)][string]$ReportPrefix)
$ErrorActionPreference='Stop'
$Observer=(Resolve-Path $Observer).Path
$PackageRoot=(Resolve-Path $PackageRoot).Path
$ReportPrefix=[IO.Path]::GetFullPath($ReportPrefix)
$build=(Resolve-Path "$PSScriptRoot/../../build").Path+'\'
if(!$ReportPrefix.StartsWith($build,[StringComparison]::OrdinalIgnoreCase)){throw 'Evidence must remain under build'}
if(@(Get-CimInstance Win32_Process -Filter "Name='ntsrv.exe'").Count){throw 'Existing service must not be controlled'}
function Wait-IsolatedService {
    foreach($row in @(Get-CimInstance Win32_Process -Filter "Name='ntsrv.exe'")) {
        if($row.ExecutablePath -ne (Join-Path $PackageRoot 'ntsrv.exe')){throw 'Foreign service present'}
        try {$process=[Diagnostics.Process]::GetProcessById($row.ProcessId)}catch{continue}
        try {if(!$process.WaitForExit(20000)){throw 'Isolated service did not retire'}}finally{$process.Dispose()}
    }
}
function Observe([string]$Name,[string]$Image,[string[]]$Arguments,[string]$Expected,[string]$Marker='') {
    $report=$ReportPrefix+'-'+$Name+'.txt'
    if(Test-Path $report){throw 'Require fresh evidence'}
    $start=[Diagnostics.ProcessStartInfo]::new($Observer)
    $start.UseShellExecute=$false
    $start.WindowStyle=[Diagnostics.ProcessWindowStyle]::Hidden
    $start.Environment['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
    foreach($argument in @($Image,$PackageRoot,$report)+$Arguments+@('--observation-timeout-ms','35000')){$start.ArgumentList.Add($argument)}
    $process=[Diagnostics.Process]::Start($start)
    try {
        if(!$process.WaitForExit(45000)){throw "Live observer: $($process.Id)"}
        $content=Get-Content $report -Raw
        $screen=Get-Content ($report+'.console.txt') -Raw
        if($process.ExitCode -or $content -notmatch 'result=exited' -or !$content.Contains($Expected) -or
            ($Marker -and !$screen.Contains($Marker))){throw "GUI case failed: $Name; $report"}
        "PASS GUI $Name $Expected $Marker"
    }finally{$process.Dispose()}
    Wait-IsolatedService
}
$probe=Join-Path $PackageRoot 'native-gui-startup-probe.exe'
$launcher=Join-Path $PackageRoot 'run16.exe'
Observe 'startup' $launcher @($probe) 'exit=0x00000000'
Observe 'wait' $launcher @('--wait',$probe) 'exit=0x00000025'
Observe 'gui-fresh-text' $launcher @('--wait',$probe,'--spawn-text') 'exit=0x00000025'
$survivalArguments=@('-NoProfile','-File',(Join-Path $PSScriptRoot 'native_gui_route_fixture.ps1'),'-PackageRoot',$PackageRoot,'-WaitWorkerRetirement')
Observe 'carrier-retirement' (Get-Command pwsh).Source $survivalArguments 'exit=0x00000000' 'PASS GUI survives launcher/request release; actual exit=37; worker-retirement=True'
$command='"'+$launcher+'" --wait "'+$probe+'" & echo S9-GUI-TEXT-RESUMED & exit /b 19'
Observe 'text-gui-text' $launcher @('cmd','/d','/c',$command) 'exit=0x00000013' 'S9-GUI-TEXT-RESUMED'
