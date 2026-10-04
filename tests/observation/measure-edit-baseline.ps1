[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$RuntimeRoot,
    [Parameter(Mandatory)][string]$ReportRoot,
    [ValidateRange(2,20)][int]$Iterations=5
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$observerPath=(Resolve-Path $Observer).Path
$runtime=(Resolve-Path $RuntimeRoot).Path
$reports=(Resolve-Path $ReportRoot).Path
$build=(Resolve-Path "$repo/build").Path+'\'
foreach($path in @($runtime,$reports,$observerPath)){
    if(!$path.StartsWith($build,[StringComparison]::OrdinalIgnoreCase)){throw 'Disposable inputs/results must be below build/'}
}
if(Test-Path Z:\){throw 'Z already in use'}
if(Get-ChildItem $reports){throw 'Fresh report directory required'}
$scope=New-IsolatedPackageScope $runtime
$names=@('MVDM_OBSERVER_PRIVATE_DESKTOP','MVDM_OBSERVER_MILESTONE_INPUT',
    'MVDM_OBSERVER_SHORT_HISTORY','MVDM_OBSERVER_WINDOW_INPUT','MVDM_OBSERVER_PERFORMANCE')
$saved=@{};foreach($name in $names){$saved[$name]=[Environment]::GetEnvironmentVariable($name)}
$rows=@();$mapped=$false
try {
    & subst.exe Z: $runtime
    if($LASTEXITCODE){throw 'SUBST failed'}
    $mapped=$true
    $scope=New-IsolatedPackageScope $runtime 'Z:\'
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    $env:MVDM_OBSERVER_MILESTONE_INPUT='1'
    Remove-Item Env:MVDM_OBSERVER_SHORT_HISTORY -ErrorAction SilentlyContinue
    foreach($mode in @('Console','Window')){
        if($mode -eq 'Window'){$env:MVDM_OBSERVER_WINDOW_INPUT='1'}
        else{Remove-Item Env:MVDM_OBSERVER_WINDOW_INPUT -ErrorAction SilentlyContinue}
        # Warm-up and disabled runs remain real assertion-bearing cases. Every
        # sample starts a clean isolated package, so this measures cold product
        # startup with warmed host filesystem, not resident-worker startup.
        foreach($iteration in -1..$Iterations){
            $enabled=$iteration -ne -1
            if($enabled){$env:MVDM_OBSERVER_PERFORMANCE='1'}
            else{Remove-Item Env:MVDM_OBSERVER_PERFORMANCE -ErrorAction SilentlyContinue}
            foreach($case in @('empty','edit','direct-seven')){
                $prefix='baseline-'+$mode.ToLowerInvariant()+'-'+($iteration+1)+'-'+$case
                $watch=[Diagnostics.Stopwatch]::StartNew()
                & "$repo/tools/audit/Verify-CommandExitStatus.ps1" -Observer $observerPath -PackageRoot 'Z:\' -ProcessPackageRoot $runtime -LogRoot $reports -LogPrefix $prefix -Cases $case -OrdinaryFrontend
                $watch.Stop()
                $report=Get-Content (Join-Path $reports "$prefix-$case.txt") -Raw
                if($enabled -and ($report -notmatch 'performance-samples=\d+ overflow=0' -or $report -match 'performance .*observed=0')){throw 'Missing/failed performance milestone'}
                if(!$enabled -and $report -match 'performance phase='){throw 'Disabled observer emitted samples'}
                $samples=@([regex]::Matches($report,'(?m)^performance phase=(\S+) elapsed-ms=(\d+) wait-ms=(\d+) observed=(\d+) detail=(.*)\r?$') | ForEach-Object {
                    [pscustomobject]@{Phase=$_.Groups[1].Value;ElapsedMs=[long]$_.Groups[2].Value;WaitMs=[long]$_.Groups[3].Value;Detail=$_.Groups[5].Value.TrimEnd("`r")}
                })
                $rows += [pscustomobject]@{Mode=$mode;Route=$(if($case -eq 'direct-seven'){'native-no-window-control'}else{$mode});Case=$case;Iteration=$iteration;Instrumentation=$enabled;WallMs=$watch.ElapsedMilliseconds;Samples=$samples;Report="$prefix-$case.txt"}
            }
        }
    }
    $rows | ConvertTo-Json -Depth 6 | Set-Content (Join-Path $reports 'measurements.json') -Encoding UTF8
} finally {
    try {Stop-IsolatedPackageScope $scope} finally {
        try {
            if($mapped){& subst.exe Z: /d;if($LASTEXITCODE){throw 'SUBST cleanup failed'}}
        } finally {
            foreach($name in $names){
                if($null -eq $saved[$name]){Remove-Item -LiteralPath "Env:$name" -ErrorAction SilentlyContinue}
                else{Set-Item -LiteralPath "Env:$name" -Value $saved[$name]}
            }
        }
    }
}
