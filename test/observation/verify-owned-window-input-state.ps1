[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$RuntimeRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$LogRoot
)
$ErrorActionPreference='Stop'
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$runtime=(Resolve-Path $RuntimeRoot).Path
$observerPath=(Resolve-Path $Observer).Path
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$log=[IO.Path]::GetFullPath($LogRoot)
if(!$log.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $log)){
    throw 'Require fresh build-only input evidence'
}
$scope=New-IsolatedPackageScope $runtime
$null=New-Item -ItemType Directory -Path $log
$variables=@('MVDM_OBSERVER_PRIVATE_DESKTOP','MVDM_OBSERVER_SHORT_HISTORY',
    'MVDM_OBSERVER_WINDOW_INPUT','MVDM_OBSERVER_MILESTONE_INPUT',
    'MVDM_OBSERVER_WINDOW_CAPS_DIAGNOSTIC')
$saved=@{};foreach($name in $variables){$saved[$name]=[Environment]::GetEnvironmentVariable($name)}
$mapped=$false
try{
    if(Test-Path Z:\){throw 'Z occupied'}
    & subst.exe Z: $runtime
    if($LASTEXITCODE){throw 'Cannot map owned Z'}
    $mapped=$true
    $scope.Paths+=@($scope.Paths|ForEach-Object {Join-Path Z:\ $_.Substring($runtime.Length+1)})
    foreach($name in $variables[0..3]){[Environment]::SetEnvironmentVariable($name,'1')}
    foreach($mode in 'caps-negative','normal'){
        if($mode -eq 'caps-negative'){$env:MVDM_OBSERVER_WINDOW_CAPS_DIAGNOSTIC='1'}
        else{Remove-Item Env:MVDM_OBSERVER_WINDOW_CAPS_DIAGNOSTIC -ErrorAction SilentlyContinue}
        $failure=$null
        try{
            & "$repo/tools/audit/Verify-CommandExitStatus.ps1" -Observer $observerPath `
                -PackageRoot Z:\ -ProcessPackageRoot $runtime -LogRoot $log `
                -LogPrefix $mode -Cases native-zero -OrdinaryFrontend
        }catch{$failure=$_.Exception.Message}
        finally{Stop-IsolatedPackageScope $scope}
        $report=Get-Content "$log/$mode-native-zero.txt" -Raw
        $screen=Get-Content "$log/$mode-native-zero.txt.console.txt" -Raw
        if($mode -eq 'caps-negative'){
            if(!$failure -or $report -notmatch '(?m)^result=timeout\r?$' -or
                $report -notmatch 'milestone-waits=1 ' -or
                $report -notmatch 'window-enter-count=0' -or $screen -notmatch '>VER'){
                throw "Wrong strict pre-Enter Caps failure: $failure"
            }
            'PASS wrong Caps: actual VER, exact echo rejection, no Enter; not completion timeout'
        }elseif($failure -or $report -notmatch 'window-enter-count=2'){
            throw "Normal exact Window input failed: $failure"
        }
    }
}finally{
    try{Stop-IsolatedPackageScope $scope}finally{if($mapped){& subst.exe Z: /d}}
    foreach($name in $variables){
        if($null -eq $saved[$name]){Remove-Item "Env:$name" -ErrorAction SilentlyContinue}
        else{[Environment]::SetEnvironmentVariable($name,$saved[$name])}
    }
}
