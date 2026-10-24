[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$LogRoot
)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
Set-Location $repo
$out=[IO.Path]::GetFullPath($LogRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)){throw 'Use repository build evidence'}
$null=New-Item -ItemType Directory $out -Force
$Observer=(Resolve-Path $Observer).Path
. "$repo/tests/observation/isolated_package_cleanup.ps1"
$stage=New-PhysicalRuntimeStage $PackageRoot
$scope=New-IsolatedPackageScope $stage
try {
    $bin=Get-PackageBinaryRoot $stage
    $launcher='"'+(Join-Path $bin 'run16.exe')+'"'
    $file=Join-Path $out 'redirect.txt'
    $pipe=Join-Path $out 'pipe.txt'
    $ver=Join-Path $out 'ver.txt'
    foreach($path in @($file,$pipe,$ver)){if(Test-Path $path){throw 'Preserve old redirection evidence'}}
    $text="$launcher cmd /d /c echo REDIRECT-ONE > `"$file`"`r$launcher cmd /d /c echo APPEND-TWO >> `"$file`"`r$launcher cmd /d /c echo PIPE-THREE | findstr PIPE-THREE > `"$pipe`"`r$launcher cmd /c ver > `"$ver`"`rexit /b 23`r"
    $start=[Diagnostics.ProcessStartInfo]::new($Observer)
    $start.UseShellExecute=$false;$start.WindowStyle=[Diagnostics.ProcessWindowStyle]::Hidden
    # This is an outer CMD Console test, not the preceding DOS Window case.
    # Observation modes must not leak from another suite into this child.
    foreach($key in @($start.Environment.Keys)){
        if($key.StartsWith('MVDM_OBSERVER_')){$null=$start.Environment.Remove($key)}
    }
    $start.Environment['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
    $start.Environment['PATH']=$bin+';'+(Join-Path $env:SystemRoot 'System32')
    foreach($arg in @((Join-Path $env:SystemRoot 'System32/cmd.exe'),$bin,"$out/redirection-observer.txt",'/d','/k','--observe-console-input-text',$text,'--observe-console-line-delay-ms','1800','--observation-timeout-ms','35000')){$start.ArgumentList.Add($arg)}
    $p=[Diagnostics.Process]::Start($start)
    try {
        # The observer types each character with 100 ms pacing BEFORE its
        # 35-second target observation. Account for both, not just the latter.
        $budget=35000+10000+100*$text.Length+1800*5
        if(!$p.WaitForExit($budget)){throw "Redirection observer still live after $budget ms"}
        $report=Get-Content "$out/redirection-observer.txt" -Raw
        if($p.ExitCode -or $report -notmatch '(?m)^result=exited\r?$' -or $report -notmatch '(?m)^exit=0x00000017\r?$'){throw 'Outer CMD failed to return'}
        if((Get-Content $file -Raw) -notmatch '(?s)REDIRECT-ONE\s+APPEND-TWO'){throw 'Redirect/append data missing'}
        if((Get-Content $pipe -Raw).Trim() -ne 'PIPE-THREE'){throw 'Pipeline data mismatch'}
        if((Get-Content $ver -Raw) -notmatch 'Microsoft Windows \[Version'){throw 'cmd /c ver output missing'}
    }finally{if(!$p.HasExited){$p.Kill();$p.WaitForExit()};$p.Dispose()}
    'PASS >, >>, pipeline and cmd /c ver; outer CMD returned 23' | Set-Content "$out/redirection.pass.txt"
}finally{Stop-IsolatedPackageScope $scope}
