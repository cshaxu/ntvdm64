[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$ReportPath
)
$ErrorActionPreference='Stop'
$build=(Resolve-Path "$PSScriptRoot/../../build").Path+'\'
$ReportPath=[IO.Path]::GetFullPath($ReportPath)
if(!$ReportPath.StartsWith($build,[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $ReportPath)){
    throw 'Require a fresh build-root report'
}
if(@(Get-CimInstance Win32_Process -Filter "Name='ntsrv.exe'").Count){throw 'An existing broker must not be controlled'}
$start=[Diagnostics.ProcessStartInfo]::new((Resolve-Path $Observer).Path)
$start.UseShellExecute=$false;$start.WindowStyle=[Diagnostics.ProcessWindowStyle]::Hidden
$start.EnvironmentVariables['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$binary=Get-PackageBinaryRoot $PackageRoot
$launcher='"'+(Join-Path $binary 'run16.exe')+'"'
# Match the owner workflow: guest media are in System32, so bare names are
# resolved from that cwd. Do not alter production PATH/search semantics.
$text="$launcher command`rmem`rexit`r$launcher cmd /c ver`r$launcher command`rmem`rexit`rexit /b 19`r"
foreach($argument in @((Join-Path $env:WINDIR 'System32/cmd.exe'),$binary,$ReportPath,
    '/d','/k','--observe-console-input-text',$text,'--observe-console-line-delay-ms','1600',
    '--observation-timeout-ms','30000')){$start.ArgumentList.Add($argument)}
$process=[Diagnostics.Process]::Start($start)
try {
    if(!$process.WaitForExit(45000)){throw 'Same-Console relaunch timed out'}
    $report=Get-Content $ReportPath -Raw
    if($report -notmatch '(?m)^result=exited\r?$' -or $report -notmatch '(?m)^exit=0x00000013\r?$'){
        throw 'Outer CMD failed to complete after nested direct/native/DOS requests'
    }
    foreach($line in @('03','07')){
        $screen=Get-Content ($ReportPath+".line-$line.console.txt") -Raw
        if($screen -notmatch 'bytes total conventional memory'){throw "DOS relaunch did not execute MEM before step $line"}
    }
    $native=Get-Content ($ReportPath+'.line-05.console.txt') -Raw
    if($native -notmatch 'Microsoft Windows'){throw 'Native request did not execute VER'}
    Write-Output 'PASS same outer CMD: DOS MEM/EXIT -> native VER -> DOS MEM/EXIT -> cooked CMD exit 19'
}finally{
    if(!$process.HasExited){$process.Kill();$null=$process.WaitForExit(5000)}
    $process.Dispose()
}
