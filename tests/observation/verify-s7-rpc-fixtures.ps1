[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$ReportPrefix,
    [Parameter(Mandatory)][ValidateSet('client','bootstrap','startup-rejections',
        'startup-timeout','native-worker-failure','native-completed-worker-loss','native-io-error',
        'workerless-grace','workerless-cancel','monitor-rpc',
        'native-command','native-registry')][string[]]$Cases
)
$ErrorActionPreference='Stop'
$Observer=(Resolve-Path -LiteralPath $Observer).Path
$PackageRoot=(Resolve-Path -LiteralPath $PackageRoot).Path
$ReportPrefix=[IO.Path]::GetFullPath($ReportPrefix)
$build=(Resolve-Path (Join-Path $PSScriptRoot '../../build')).Path+'\'
if(!$ReportPrefix.StartsWith($build,[StringComparison]::OrdinalIgnoreCase)) {
    throw 'Reports must remain under repository build/'
}
foreach($case in $Cases) {
    $report=$ReportPrefix+'-'+$case+'.txt'
    if(Test-Path -LiteralPath $report){throw 'Use a fresh report prefix'}
    $image=if($case -eq 'client'){'frontend-request-client-test.exe'}
        elseif($case -eq 'monitor-rpc'){'monitor-rpc-test.exe'}
        elseif($case -in @('native-command','native-registry')){'basesrv-service-reservation-test.exe'}
        else{'broker-frontend-bootstrap-test.exe'}
    $casePackageRoot=$PackageRoot
    if($case -in @('startup-rejections','startup-timeout','native-io-error')) {
        # This fixture deliberately replaces the frontend with an adversarial
        # peer. Its C-side build-only guard must remain effective even when
        # ordinary product cases execute in a short physical O:\tmp stage.
        # Copy only its two inputs to fresh build-owned System32, retaining
        # the real service's package-root semantics and exact input identity.
        $casePackageRoot=$ReportPrefix+'-'+$case+'-package\system32'
        if(Test-Path -LiteralPath $casePackageRoot){throw 'Require a fresh rejection fixture package'}
        $null=New-Item -ItemType Directory -Path $casePackageRoot
        $inputs=@($image,'ntsrv.exe');if($case -eq 'native-io-error'){$inputs+='ntcon.exe'}
        foreach($name in $inputs) {
            $source=Join-Path $PackageRoot $name
            $destination=Join-Path $casePackageRoot $name
            Copy-Item -LiteralPath $source -Destination $destination
            if((Get-FileHash -LiteralPath $source).Hash -ne
               (Get-FileHash -LiteralPath $destination).Hash){throw 'Rejection fixture input identity mismatch'}
        }
    }
    $start=[Diagnostics.ProcessStartInfo]::new($Observer)
    $start.UseShellExecute=$false
    $start.WindowStyle=[Diagnostics.ProcessWindowStyle]::Hidden
    $start.EnvironmentVariables['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
    $arguments=@((Join-Path $casePackageRoot $image),$casePackageRoot,$report)
    if($case -eq 'native-registry'){$arguments+='--native-worker'}
    elseif($case -notin @('client','bootstrap','monitor-rpc')){$arguments+='--'+$case}
    $arguments+=@('--observation-timeout-ms','45000')
    foreach($argument in $arguments){$start.ArgumentList.Add($argument)}
    $process=[Diagnostics.Process]::Start($start)
    try {
        if(!$process.WaitForExit(55000)){throw "Observer remains live: $($process.Id)"}
        $content=Get-Content -LiteralPath $report -Raw
        $screen=Get-Content -LiteralPath ($report+'.console.txt') -Raw
        if($process.ExitCode -or $content -notmatch 'result=exited' -or
            $content -notmatch 'exit=0x00000000' -or $screen -notmatch 'PASS') {
            throw "RPC fixture failed: $case; report=$report"
        }
        "PASS $case actual fixture exit zero and assertion output"
    } finally {$process.Dispose()}
}
