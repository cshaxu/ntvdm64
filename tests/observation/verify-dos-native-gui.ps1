param([Parameter(Mandatory)][string]$Observer,
      [Parameter(Mandatory)][string]$PackageRoot,
      [Parameter(Mandatory)][string]$ReportPath)
$ErrorActionPreference='Stop'
$Observer=(Resolve-Path $Observer).Path
$ReportPath=[IO.Path]::GetFullPath($ReportPath)
$build=(Resolve-Path "$PSScriptRoot/../../build").Path+'\'
if(!$ReportPath.StartsWith($build,[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $ReportPath)){throw 'Require fresh build evidence'}
if(!(Test-Path (Join-Path $PackageRoot 'GGUI.EXE'))){throw 'Require controlled GUI fixture under DOS-safe alias GGUI.EXE'}
$marker=Join-Path $PackageRoot 'S9GUI.OK'
if(Test-Path $marker){throw 'Use a fresh marker'}
$text="ggui.exe --mark $marker`rmem`rexit`r"
$start=[Diagnostics.ProcessStartInfo]::new($Observer)
$start.UseShellExecute=$false
$start.WindowStyle=[Diagnostics.ProcessWindowStyle]::Hidden
$start.Environment['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
foreach($argument in @((Join-Path $PackageRoot 'run16.exe'),$PackageRoot,$ReportPath,'command',
    '--observe-console-input-text',$text,'--observe-console-line-delay-ms','1600','--observation-timeout-ms','20000')){$start.ArgumentList.Add($argument)}
$process=[Diagnostics.Process]::Start($start)
try {
    if(!$process.WaitForExit(30000)){throw "Live observer: $($process.Id)"}
    $report=Get-Content $ReportPath -Raw
    $screen=Get-Content ($ReportPath+'.line-02.console.txt') -Raw
    if($process.ExitCode -or $report -notmatch 'result=exited' -or $report -notmatch 'exit=0x00000001' -or
        $screen -notmatch 'bytes total conventional memory' -or
        (Get-Content $marker -Raw) -ne 'S9-GUI-TARGET-EXECUTED') {throw 'DOS -> actual GUI -> DOS MEM failed'}
    'PASS DOS -> actual GUI marker -> DOS MEM -> launcher completion'
}finally{$process.Dispose()}
