[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$ReportPath
)
$ErrorActionPreference='Stop'
if($env:MVDM_OBSERVER_WINDOW_INPUT){
    throw 'This Ctrl+Q fixture uses real Console input records, not synthetic Window modifier state'
}
$Observer=(Resolve-Path -LiteralPath $Observer).Path
$PackageRoot=(Resolve-Path -LiteralPath $PackageRoot).Path
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$ReportPath=[IO.Path]::GetFullPath($ReportPath)
$build=(Resolve-Path (Join-Path $PSScriptRoot '../../build')).Path+'\'
if(!$ReportPath.StartsWith($build,[StringComparison]::OrdinalIgnoreCase)){
    throw 'Reports must remain under repository build/'
}
if(Test-Path -LiteralPath $ReportPath){throw 'Use a fresh report'}
$edit=Join-Path $env:WINDIR 'System32\edit.exe'
if(!(Test-Path -LiteralPath $edit)){throw 'Installed modern EDIT is required'}
$stage=$null
$scope=$null
$process=$null
try {
    # The immutable guest sees the launch path.  A full evidence path can
    # fail before either carrier starts, which is not a handoff result.  Use
    # the existing checked real short stage, then retain its exact identity
    # for the cleanup boundary below.
    $stage=New-PhysicalRuntimeStage $PackageRoot
    $scope=New-IsolatedPackageScope $stage
    $binary=Get-PackageBinaryRoot $stage
    $start=[Diagnostics.ProcessStartInfo]::new($Observer)
    $start.UseShellExecute=$false
    $start.WindowStyle=[Diagnostics.ProcessWindowStyle]::Hidden
    $start.EnvironmentVariables['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
    $mem=Join-Path $binary 'MEM.EXE'
    $text="cmd.exe /d`r$edit`r$([char]17)`recho S40-NATIVE-RETURN-OK`rexit`r$mem`rexit`r"
    foreach($argument in @((Join-Path $binary 'run16.exe'),$stage,$ReportPath,
        (Join-Path $binary 'COMMAND.COM'),'--observe-console-input-text',$text,
        '--observe-console-line-delay-ms','1800','--observation-timeout-ms','35000')){
        $start.ArgumentList.Add($argument)
    }
    $process=[Diagnostics.Process]::Start($start)
    if(!$process.WaitForExit(60000)){throw 'EDIT return observer timed out'}
    $report=Get-Content -LiteralPath $ReportPath -Raw
    $editor=Get-Content -LiteralPath ($ReportPath+'.line-02.console.txt') -Raw
    $snapshots=Get-ChildItem -LiteralPath (Split-Path $ReportPath) |
        Where-Object {$_.Name.StartsWith([IO.Path]::GetFileName($ReportPath)+'.line-')}
    $text=($snapshots | ForEach-Object {Get-Content -LiteralPath $_.FullName -Raw}) -join "`n"
    if($process.ExitCode -or $report -notmatch 'result=exited' -or
       $report -notmatch 'exit=0x00000001' -or
       $report -notmatch 'scripted-console-input=delivered' -or
       $editor -notmatch 'File  Edit  View  Help' -or
       $editor -notmatch 'Untitled-1.txt' -or
       $text -notmatch '(?m)^\[\d+\] S40-NATIVE-RETURN-OK\r?$' -or
       $text -notmatch 'bytes total conventional memory'){
        throw "COMMAND -> native EDIT -> CMD -> DOS did not pass: $ReportPath"
    }
    'PASS real modern EDIT screen, Ctrl+Q, CMD echo, DOS MEM and launcher completion'
} finally {
    if($process){
        if(!$process.HasExited){$process.Kill();$process.WaitForExit()}
        $process.Dispose()
    }
    try {if($scope){Stop-IsolatedPackageScope $scope}} finally {
        if($stage){Remove-PhysicalRuntimeStage $stage}
    }
}
