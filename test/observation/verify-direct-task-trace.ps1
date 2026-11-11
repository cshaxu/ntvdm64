[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$ProcessPackageRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$MonitorRpc,
    [Parameter(Mandatory)][string]$LogRoot,
    [ValidateSet('DOS','NestedDOS','Native','Native32')][string]$Case='DOS'
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$scope=New-IsolatedPackageScope $ProcessPackageRoot $PackageRoot
$binary=Get-PackageBinaryRoot $PackageRoot
$observerPath=(Resolve-Path $Observer).Path
$probe=(Resolve-Path $MonitorRpc).Path
$log=[IO.Path]::GetFullPath($LogRoot)
if(!$log.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or
    (Test-Path $log)){throw 'Require fresh build-owned evidence directory'}
New-Item -ItemType Directory -Path $log|Out-Null
$saved=@{}
foreach($name in @('MVDM_OBSERVER_PRIVATE_DESKTOP','MVDM_OBSERVER_SHORT_HISTORY',
    'MVDM_OBSERVER_INPUT_GATE','MVDM_OBSERVER_MILESTONE_INPUT','MVDM_OBSERVER_WINDOW_INPUT')) {
    $saved[$name]=[Environment]::GetEnvironmentVariable($name)
}
$gate=$null;$observerProcess=$null
try {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1';$env:MVDM_OBSERVER_SHORT_HISTORY='1'
    Remove-Item Env:MVDM_OBSERVER_MILESTONE_INPUT -ErrorAction SilentlyContinue
    Remove-Item Env:MVDM_OBSERVER_WINDOW_INPUT -ErrorAction SilentlyContinue
    $gateName='Local\direct-trace-'+[guid]::NewGuid().ToString('N')
    $gate=[Threading.EventWaitHandle]::new($false,[Threading.EventResetMode]::ManualReset,$gateName)
    $env:MVDM_OBSERVER_INPUT_GATE=$gateName
    $report=Join-Path $log 'observer.txt'
    $native=$Case -in @('Native','Native32')
    $target=if($Case -eq 'Native32'){Join-Path $env:SystemRoot 'SysWOW64\cmd.exe'}elseif($native){'cmd.exe'}else{Join-Path $binary 'COMMAND.COM'}
    $command=if($native){"echo DIRECT-TRACE-WITNESS`rexit /b 37`r"}else{"echo DIRECT-TRACE-WITNESS`rver`rexit`r"}
    $arguments=@((Join-Path $binary 'run16.exe'),$PackageRoot,$report,$target)
    if($Case -eq 'NestedDOS'){$arguments+=@('/c','system32\COMMAND.COM')}
    if($native){$arguments+=@('/d','/k')}
    $arguments+=@('--observation-timeout-ms','20000','--observe-console-input-text',('"'+$command+'"'))
    $observerProcess=Start-Process -FilePath $observerPath -ArgumentList $arguments -WindowStyle Hidden -PassThru
    # /C of a known DOS file uses original INT21/EXEC; it is not another
    # broker admission. S4 must observe that guest child, never invent Direct.
    $expected=1
    $kind=if($Case -eq 'Native32'){@(2)}elseif($native){@(3)}else{@(0)}
    $deadline=[DateTime]::UtcNow.AddSeconds(12)
    $worker=$null;$tree=$null
    do {
        $text=& $probe --tree-json 2> (Join-Path $log 'snapshot.err')
        if(!$LASTEXITCODE) {
            $tree=@(($text -join "`n")|ConvertFrom-Json)
            $candidates=@($tree|Where-Object {$_.category -eq 2 -and $_.kind -in $kind -and $_.stack -eq $expected})
            if($candidates.Count -eq 1){$worker=$candidates[0];break}
        }
        if($observerProcess.HasExited){throw 'Launcher exited before Direct records became observable'}
        Start-Sleep -Milliseconds 50
    }while([DateTime]::UtcNow -lt $deadline)
    if(!$worker){
        $tree|ConvertTo-Json -Depth 5|Set-Content (Join-Path $log 'unmet-tree.json')
        foreach($row in @($tree|Where-Object {$_.category -eq 2})) {
            & $probe --trace-json $row.pid | Set-Content (Join-Path $log "unmet-trace-$($row.pid).json")
        }
        # Capture the guest's actual response before test cleanup, but never
        # convert the unmet observation precondition into a passing case.
        $null=$gate.Set();$null=$observerProcess.WaitForExit(12000)
        throw 'Expected Direct stack was not reached; timeout is not a pass'
    }
    $traceText=& $probe --trace-json $worker.pid
    if($LASTEXITCODE){throw 'Actual worker trace RPC failed'}
    $traceText|Set-Content (Join-Path $log 'trace.json')
    $trace=($traceText -join "`n")|ConvertFrom-Json
    $nodes=@($trace.nodes|Where-Object {$_.relation -eq 1})
    if($nodes.Count -ne $expected){throw 'Direct trace length differs from authority stack'}
    $ids=@{}
    foreach($node in $nodes) {
        if(!$node.node -or $ids.ContainsKey([string]$node.node) -or $node.source -ne 1 -or
            $node.kind -notin $kind -or !$node.image){throw 'Missing/duplicate/misclassified Direct source facts'}
        if($node.parent -and !$ids.ContainsKey([string]$node.parent)){throw 'Parent does not precede its Direct child'}
        if(!$native -and $node.image -notmatch '(?i)(^|\\)command\.com$'){
            throw 'Actual DOS Direct image is not COMMAND.COM'
        }
        $ids[[string]$node.node]=$true
    }
    $after=& $probe --tree-json
    if($LASTEXITCODE){throw 'Post-trace snapshot failed'}
    $afterTree=@(($after -join "`n")|ConvertFrom-Json)
    $same=@($afterTree|Where-Object {$_.key -eq $worker.key})
    if($same.Count -ne 1 -or $same[0].stack -ne $expected -or $same[0].state -ne $worker.state){
        throw 'Read-only query changed Direct stack/state'
    }
    $null=$gate.Set()
    if(!$observerProcess.WaitForExit(12000) -or $observerProcess.ExitCode){throw 'Observer/target did not complete'}
    $result=Get-Content $report -Raw
    if($result -notmatch '(?m)^result=exited\r?$' -or $result -notmatch '(?m)^scripted-console-input=delivered\r?$'){
        throw 'Direct task completion/input consumption was not proved'
    }
    $expectedExit=if($native){'00000025'}else{'00000000'}
    if($result -notmatch "(?m)^exit=0x$expectedExit\r?`$" -or
        (Get-Content "$report.console.txt" -Raw) -notmatch '(?m)^\[\d+\] DIRECT-TRACE-WITNESS\s*$'){
        throw 'This invocation did not produce the actual witness and expected target exit code'
    }
    "PASS actual $Case Direct trace/$expected records, unchanged stack/state and target completion"
} finally {
    if($observerProcess){
        if(!$observerProcess.HasExited){$observerProcess.Kill();$null=$observerProcess.WaitForExit(5000)}
        $observerProcess.Dispose()
    }
    if($gate){$gate.Dispose()}
    try {Stop-IsolatedPackageScope $scope} finally {
        foreach($name in $saved.Keys){[Environment]::SetEnvironmentVariable($name,$saved[$name])}
    }
}
