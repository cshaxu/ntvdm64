[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$ReportPath,
    [ValidateRange(2,100)][int]$Rounds=12
)
$ErrorActionPreference='Stop'
$build=(Resolve-Path "$PSScriptRoot/../../build").Path+'\'
$ReportPath=[IO.Path]::GetFullPath($ReportPath)
if(!$ReportPath.StartsWith($build,[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $ReportPath)) {
    throw 'Require a fresh build-root report'
}
if(@(Get-CimInstance Win32_Process -Filter "Name='ntsrv.exe'").Count){throw 'An existing broker must not be controlled'}
$batch=[IO.Path]::ChangeExtension($ReportPath,'.cmd')
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$launcher='"'+(Join-Path (Get-PackageBinaryRoot $PackageRoot) 'run16.exe')+'"'
$lines=@('@echo off')
for($round=1;$round -le $Rounds;$round++) {
    $lines+="$launcher cmd /d /k echo S10-INTERACTIVE-$round"
    $lines+='if errorlevel 1 exit /b 81'
}
$lines+=@("$launcher cmd /d /c echo S10-INTERACTIVE-FINAL",'if errorlevel 1 exit /b 82',
    'echo S10-INTERACTIVE-COMPLETE','exit /b 19')
[IO.File]::WriteAllLines($batch,$lines,[Text.Encoding]::ASCII)
$start=[Diagnostics.ProcessStartInfo]::new((Resolve-Path $Observer).Path)
$start.UseShellExecute=$false;$start.WindowStyle=[Diagnostics.ProcessWindowStyle]::Hidden
$start.EnvironmentVariables['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
# All exit lines are queued without per-line delay. Each real interactive CMD
# consumes its own exit; the outer batch immediately starts the next launcher.
$inputText=('exit'+"`r")*$Rounds
foreach($arg in @((Join-Path $env:WINDIR 'System32/cmd.exe'),$PackageRoot,$ReportPath,
    '/d','/c',$batch,'--observe-console-input-text',$inputText,
    '--observe-console-line-delay-ms','0','--observation-timeout-ms','60000')){$start.ArgumentList.Add($arg)}
$process=[Diagnostics.Process]::Start($start)
try {
    if(!$process.WaitForExit(75000)){throw "Live interactive observer: $($process.Id)"}
    $report=Get-Content $ReportPath -Raw
    if($process.ExitCode -or $report -notmatch '(?m)^result=exited\r?$' -or
        $report -notmatch '(?m)^exit=0x00000013\r?$'){throw 'Interactive rapid relaunch did not return outer19'}
    $screen=Get-Content ($ReportPath+'.console.txt') -Raw
    if($screen -notmatch 'S10-INTERACTIVE-FINAL' -or $screen -notmatch 'S10-INTERACTIVE-COMPLETE') {
        throw 'Interactive final output missing'
    }
    "PASS $Rounds immediate interactive CMD exit/relaunches, final output and outer19"
}finally {
    if(!$process.HasExited){$process.Kill();$null=$process.WaitForExit(5000)}
    $process.Dispose()
}
