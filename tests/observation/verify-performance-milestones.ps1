param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$RuntimeRoot,
    [Parameter(Mandatory)][string]$ReportRoot
)
$ErrorActionPreference='Stop'
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$runtime=(Resolve-Path $RuntimeRoot).Path
$observerPath=(Resolve-Path $Observer).Path
$reports=(Resolve-Path $ReportRoot).Path
$build=(Resolve-Path "$PSScriptRoot/../../build").Path+'\'
foreach($path in @($runtime,$observerPath,$reports)){
    if(!$path.StartsWith($build,[StringComparison]::OrdinalIgnoreCase)){throw 'Require isolated build inputs/results'}
}
if(Test-Path Z:\){throw 'Z already in use'}
$scope=New-IsolatedPackageScope $runtime
$mapped=$false
try {
    & subst.exe Z: $runtime
    if($LASTEXITCODE){throw 'SUBST failed'}
    $mapped=$true
    $scope=New-IsolatedPackageScope $runtime 'Z:\'
    foreach($enabled in @($false,$true)){
        $report=Join-Path $reports "milestone-negative-$enabled.txt"
        if(Test-Path $report){throw 'Fresh evidence required'}
        $start=[Diagnostics.ProcessStartInfo]::new($observerPath)
        $start.UseShellExecute=$false
        $start.WindowStyle=[Diagnostics.ProcessWindowStyle]::Hidden
        $start.Environment['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
        $start.Environment.Remove('MVDM_OBSERVER_WINDOW_INPUT')|Out-Null
        $start.Environment.Remove('MVDM_OBSERVER_SHORT_HISTORY')|Out-Null
        $start.Environment.Remove('MVDM_OBSERVER_PERFORMANCE')|Out-Null
        if($enabled){$start.Environment['MVDM_OBSERVER_PERFORMANCE']='1'}
        foreach($argument in @('Z:\system32\run16.exe','Z:\',$report,'cmd.exe','/d','/c','exit','7',
            '--observe-console-input-text',"exit`r",'--observe-console-input-marker','S1-NOT-EMITTED')){
            $start.ArgumentList.Add($argument)
        }
        $process=[Diagnostics.Process]::Start($start)
        try {
            if(!$process.WaitForExit(40000)){throw 'Negative observer timeout'}
            $record=Get-Content $report -Raw
            if($process.ExitCode -or $record -notmatch '(?m)^result=exited\r?$' -or
                $record -notmatch 'exit=0x00000007' -or $record -notmatch 'scripted-console-input-ready=no'){
                throw 'Missing marker incorrectly counted as readiness or changed direct result'
            }
            if($enabled){
                if($record -notmatch 'performance phase=startup-ready .*observed=0 detail=S1-NOT-EMITTED' -or
                    $record -notmatch 'performance phase=direct-completion .*observed=1'){
                    throw 'Milestone failure was hidden'
                }
            }elseif($record -match 'performance phase='){throw 'Disabled timeline emitted observations'}
            Write-Output "PASS absent marker rejected, actual exit 7 retained; instrumentation=$enabled"
        }finally{$process.Dispose();Stop-IsolatedPackageScope $scope}
    }
}finally{
    try{Stop-IsolatedPackageScope $scope}finally{
        if($mapped){& subst.exe Z: /d;if($LASTEXITCODE){throw 'SUBST cleanup failed'}}
    }
}
