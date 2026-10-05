[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$RuntimeRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$LogRoot,
    [ValidateSet('Console','Window')][string[]]$Modes=@('Console','Window'),
    [ValidateSet('nested-mem-typeahead','dos-native-typeahead')]
    [ValidateCount(1,2)][string[]]$Cases=@('nested-mem-typeahead','dos-native-typeahead')
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$runtime=(Resolve-Path $RuntimeRoot).Path
$observerPath=(Resolve-Path $Observer).Path
if(@($Cases | Select-Object -Unique).Count -ne $Cases.Count){throw 'Duplicate typeahead cases'}
$log=[IO.Path]::GetFullPath($LogRoot)
if(!$log.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $log)){
    throw 'Require a fresh build-root typeahead evidence directory'
}
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$scope=New-IsolatedPackageScope $runtime
if(Test-Path Z:\){throw 'Z: already exists; no mapping changed'}
$binary=Get-PackageBinaryRoot $runtime
$names=@(Get-PackageImageNames $runtime)
$manifest=@($names | ForEach-Object {[pscustomobject]@{Name=$_;Sha256=(Get-FileHash (Join-Path $binary $_)).Hash}})
$variables=@('MVDM_OBSERVER_PRIVATE_DESKTOP','MVDM_OBSERVER_WINDOW_INPUT',
    'MVDM_OBSERVER_SHORT_HISTORY','MVDM_OBSERVER_MILESTONE_INPUT',
    'MVDM_OBSERVER_AFTER_FIRST_LINE_EVENT','MVDM_OBSERVER_WAIT_PROMPT_AFTER_FIRST_LINE')
$old=@{};foreach($name in $variables){$old[$name]=[Environment]::GetEnvironmentVariable($name)}
$mapped=$false
$timings=@()
$null=New-Item -ItemType Directory -Path $log
$manifest | ConvertTo-Json | Set-Content "$log/runtime-manifest.json"
try {
    & subst.exe Z: $runtime
    if($LASTEXITCODE){throw 'SUBST Z: failed'}
    $mapped=$true
    $scope.Paths+=@($scope.Paths | ForEach-Object {Join-Path 'Z:\' $_.Substring($runtime.Length+1)})
    foreach($name in $variables){[Environment]::SetEnvironmentVariable($name,$null)}
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    $env:MVDM_OBSERVER_SHORT_HISTORY='1'
    foreach($mode in $Modes){
        if($mode -eq 'Window'){$env:MVDM_OBSERVER_WINDOW_INPUT='1'}else{Remove-Item Env:MVDM_OBSERVER_WINDOW_INPUT -ErrorAction SilentlyContinue}
        $watch=[Diagnostics.Stopwatch]::StartNew();$passed=$false
        try {
            & "$repo/tools/audit/Verify-CommandExitStatus.ps1" -Observer $observerPath -PackageRoot Z:\ -ProcessPackageRoot $runtime -LogRoot $log -LogPrefix "$($mode.ToLower())-typeahead" -Cases $Cases -OrdinaryFrontend -ObservationTimeoutMs 60000
            $summary=Get-Content "$log/$($mode.ToLower())-typeahead-summary.json" -Raw | ConvertFrom-Json
            if($summary.Count -ne $Cases.Count -or @($summary | Where-Object {$_.Case -notin $Cases -or $_.Actual -ne $_.Expected}).Count){
                throw 'Missing or incorrect typeahead case verdict'
            }
            $passed=$true
        }finally{
            Stop-IsolatedPackageScope $scope
            $timings+=[pscustomobject]@{Mode=$mode;Passed=$passed;ElapsedMs=$watch.ElapsedMilliseconds}
        }
    }
}finally{
    try {if($mapped){Stop-IsolatedPackageScope $scope}}
    finally {
        if($mapped){& subst.exe Z: /d}
        foreach($name in $variables){[Environment]::SetEnvironmentVariable($name,$old[$name])}
        $timings | ConvertTo-Json | Set-Content "$log/timings.json"
        foreach($entry in $manifest){
            if((Get-FileHash (Join-Path $binary $entry.Name)).Hash -ne $entry.Sha256){throw "Runtime changed: $($entry.Name)"}
        }
    }
}
