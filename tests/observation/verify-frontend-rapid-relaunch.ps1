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
if(@(Get-CimInstance Win32_Process -Filter "Name='ntsrv.exe'").Count) {
    throw 'An existing broker must not be controlled'
}
# The outer native CMD waits for each real launcher and immediately launches
# the next request. No input pacing or delay masks the acquisition race.
# DOS /C and native /C exercise real startup/completion, not interactive input.
$batch=[IO.Path]::ChangeExtension($ReportPath,'.cmd')
$lines=@('@echo off')
for($round=1;$round -le $Rounds;$round++) {
    $lines+=@(
        "run16 cmd /d /c echo S10-NATIVE-$round",
        'if errorlevel 1 exit /b 81',
        'run16 command /c mem',
        'if errorlevel 1 exit /b 82'
    )
}
$lines+=@('run16 cmd /d /c echo S10-NATIVE-FINAL','if errorlevel 1 exit /b 83',
    'echo S10-RAPID-COMPLETE','exit /b 19')
[IO.File]::WriteAllLines($batch,$lines,[Text.Encoding]::ASCII)
$start=[Diagnostics.ProcessStartInfo]::new((Resolve-Path $Observer).Path)
$start.UseShellExecute=$false;$start.WindowStyle=[Diagnostics.ProcessWindowStyle]::Hidden
$start.EnvironmentVariables['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
foreach($argument in @((Join-Path $env:WINDIR 'System32/cmd.exe'),$PackageRoot,$ReportPath,
    '/d','/c',$batch,'--observation-timeout-ms','60000')){$start.ArgumentList.Add($argument)}
$process=[Diagnostics.Process]::Start($start)
try {
    if(!$process.WaitForExit(75000)){throw "Rapid relaunch observer remains live: $($process.Id)"}
    $report=Get-Content $ReportPath -Raw
    if($process.ExitCode -or $report -notmatch '(?m)^result=exited\r?$' -or
        $report -notmatch '(?m)^exit=0x00000013\r?$') {
        throw 'Rapid native/DOS alternation did not complete with outer CMD exit 19'
    }
    # Native and DOS are verified against the real console capture, not only
    # the outer exit. The product contract has no host scrollback guarantee.
    $screen=Get-Content ($ReportPath+'.console.txt') -Raw
    if($screen -notmatch 'S10-NATIVE-FINAL' -or
        $screen -notmatch 'bytes total conventional memory' -or
        $screen -notmatch 'S10-RAPID-COMPLETE') {
        throw 'Final native/DOS output evidence missing'
    }
    "PASS $Rounds immediate native/DOS relaunch pairs, final output and outer CMD exit 19"
} finally {
    if(!$process.HasExited){$process.Kill();$null=$process.WaitForExit(5000)}
    $process.Dispose()
}
