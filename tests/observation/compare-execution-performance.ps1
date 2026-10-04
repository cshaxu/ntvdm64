param(
    [Parameter(Mandatory)][string]$Baseline,
    [Parameter(Mandatory)][string]$Candidate,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$ReportRoot,
    [ValidateRange(2,10)][int]$Iterations=3
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$observerPath=(Resolve-Path $Observer).Path
$roots=@{baseline=(Resolve-Path $Baseline).Path;candidate=(Resolve-Path $Candidate).Path}
$root=[IO.Path]::GetFullPath($ReportRoot)
foreach($path in @($root,$observerPath)+@($roots.Values)){
    if(!$path.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)){throw 'Require build-local inputs/results'}
}
if((Test-Path $root) -or (Test-Path Z:\)){throw 'Fresh report root and free Z: required'}
$identities=@{}
foreach($variant in @('baseline','candidate')){
    $identities[$variant]=@(Get-ChildItem (Join-Path $roots[$variant] 'system32') -File |
        Where-Object {$_.Extension -in @('.exe','.dll','.com','.sys','.pif','.rom','.bin')} |
        ForEach-Object {[pscustomobject]@{Name=$_.Name;Hash=(Get-FileHash $_.FullName).Hash}})
}
foreach($item in $identities.baseline){
    $other=@($identities.candidate | Where-Object Name -eq $item.Name)
    if($other.Count -ne 1 -or ($item.Name -ne 'ntvdm.exe' -and $other[0].Hash -ne $item.Hash)){
        throw "Unmatched comparison input: $($item.Name)"
    }
}
$null=New-Item -ItemType Directory -Path $root
$names=@('MVDM_OBSERVER_PRIVATE_DESKTOP','MVDM_OBSERVER_MILESTONE_INPUT',
    'MVDM_OBSERVER_SHORT_HISTORY','MVDM_OBSERVER_WINDOW_INPUT','MVDM_OBSERVER_PERFORMANCE',
    'MVDM_TEST_WORKER_PERFORMANCE','MVDM_WOW_NT_TRACE_PATH')
$saved=@{};foreach($name in $names){$saved[$name]=[Environment]::GetEnvironmentVariable($name)}
$rows=@();$scope=$null;$mapped=$false
try {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    $env:MVDM_OBSERVER_MILESTONE_INPUT='1'
    $env:MVDM_OBSERVER_PERFORMANCE='1'
    foreach($name in @('MVDM_OBSERVER_SHORT_HISTORY','MVDM_TEST_WORKER_PERFORMANCE','MVDM_WOW_NT_TRACE_PATH')){
        Remove-Item "Env:$name" -ErrorAction SilentlyContinue
    }
    foreach($mode in @('Console','Window')){
        if($mode -eq 'Window'){$env:MVDM_OBSERVER_WINDOW_INPUT='1'}
        else{Remove-Item Env:MVDM_OBSERVER_WINDOW_INPUT -ErrorAction SilentlyContinue}
        foreach($case in @('empty','edit','direct-seven')){
            foreach($iteration in 0..$Iterations){
                # Alternate pair ordering; zero is excluded warmup, not a retry.
                $order=if($iteration%2){@('candidate','baseline')}else{@('baseline','candidate')}
                foreach($variant in $order){
                    $runtime=$roots[$variant]
                    & subst.exe Z: $runtime
                    if($LASTEXITCODE){throw 'SUBST failed'}
                    $mapped=$true;$scope=New-IsolatedPackageScope $runtime 'Z:\'
                    $prefix="$mode-$case-$iteration-$variant"
                    $watch=[Diagnostics.Stopwatch]::StartNew()
                    & "$repo/tools/audit/Verify-CommandExitStatus.ps1" -Observer $observerPath -PackageRoot 'Z:\' -ProcessPackageRoot $runtime -LogRoot $root -LogPrefix $prefix -Cases $case -OrdinaryFrontend
                    $watch.Stop()
                    $report=Get-Content (Join-Path $root "$prefix-$case.txt") -Raw
                    if($report -notmatch 'performance-samples=\d+ overflow=0' -or $report -match 'performance .*observed=0'){
                        throw 'Missing/failed actual performance milestone'
                    }
                    $samples=@([regex]::Matches($report,'(?m)^performance phase=(\S+) elapsed-ms=(\d+) wait-ms=(\d+) observed=(\d+) detail=(.*)\r?$') | ForEach-Object {
                        [pscustomobject]@{Phase=$_.Groups[1].Value;ElapsedMs=[long]$_.Groups[2].Value;WaitMs=[long]$_.Groups[3].Value;Detail=$_.Groups[5].Value.TrimEnd("`r")}
                    })
                    $rows+=[pscustomobject]@{Variant=$variant;Mode=$mode;Case=$case;Iteration=$iteration;Warmup=($iteration -eq 0);WallMs=$watch.ElapsedMilliseconds;Samples=$samples;Report="$prefix-$case.txt"}
                    Stop-IsolatedPackageScope $scope;$scope=$null
                    & subst.exe Z: /d
                    if($LASTEXITCODE){throw 'SUBST cleanup failed'}
                    $mapped=$false
                    Write-Output "PASS $prefix elapsed-ms=$($watch.ElapsedMilliseconds)"
                }
            }
        }
    }
} finally {
    try {if($scope){Stop-IsolatedPackageScope $scope}} finally {
        try {if($mapped){& subst.exe Z: /d;if($LASTEXITCODE){throw 'SUBST cleanup failed'}}} finally {
            foreach($name in $names){[Environment]::SetEnvironmentVariable($name,$saved[$name])}
            [ordered]@{Profile='x86 /MT CCPU40; ordinary Console geometry; private desktop; serial paired cold starts';
                Observer=(Get-FileHash $observerPath).Hash;Inputs=$identities;Cases=$rows} |
                ConvertTo-Json -Depth 7 | Set-Content (Join-Path $root 'measurements.json')
        }
    }
}
