param(
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$BuildRoot,
    [Parameter(Mandatory)][string]$Observer,
    [string]$ReadinessProbe,
    [switch]$RequireCarrierCleanup,
    [ValidateSet('native','wow')][string]$Kind='native'
)
# Real admitted-client failure, not a stubbed receipt or synthetic task result.
$ErrorActionPreference='Stop'
if($Kind -eq 'wow' -and (!$ReadinessProbe -or !(Test-Path -LiteralPath $ReadinessProbe))){throw 'WOW requires the compiled private-desktop readiness probe'}
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
    (Test-Path $root)){throw 'Fresh repository build evidence directory required'}
$null=New-Item -ItemType Directory $root
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$runtime=$null;$scope=$null;$observerProcess=$null;$nativeTarget=$null;$nativeCarrier=$null
$nativeDescendant=$null
$saved=@{}
foreach($name in @('TEMP','TMP','PATH','MVDM_OBSERVER_PRIVATE_DESKTOP')){
    $saved[$name]=[Environment]::GetEnvironmentVariable($name)
}
try {
    $runtime=New-PhysicalRuntimeStage (Resolve-Path $PackageRoot).Path
    $scope=New-IsolatedPackageScope $runtime
    $env:TEMP=$runtime;$env:TMP=$runtime
    $env:PATH="$runtime\system32;"+$env:SystemRoot+'\System32'
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    $start=[Diagnostics.ProcessStartInfo]::new((Resolve-Path $Observer).Path)
    $start.UseShellExecute=$false;$start.CreateNoWindow=$true
    $targetArgs=if($Kind -eq 'wow'){@('--wait',"$runtime\WINMINE.EXE")}else{
        @("$env:SystemRoot\System32\cmd.exe",'/d','/c','ping -n 30 127.0.0.1 >nul')
    }
    foreach($arg in (@("$runtime\system32\run16.exe",$runtime,"$root\observer.txt")+
        $targetArgs+@('--observation-timeout-ms','30000'))){
        $start.ArgumentList.Add($arg)
    }
    $observerProcess=[Diagnostics.Process]::Start($start)
    $null=$observerProcess.Handle
    $deadline=[DateTime]::UtcNow.AddSeconds(10)
    $service=$null;$carrier=$null
    do {
        $peers=@(Get-CimInstance Win32_Process -Filter "Name='ntsrv.exe' OR Name='ntvwm.exe' OR Name='ntvdm.exe'")
        $service=$peers|Where-Object ExecutablePath -EQ "$runtime\system32\ntsrv.exe"|Select-Object -First 1
        $carrierImage=if($Kind -eq 'wow'){'ntvdm.exe'}else{'ntvwm.exe'}
        $carrier=$peers|Where-Object ExecutablePath -EQ "$runtime\system32\$carrierImage"|Select-Object -First 1
        if($service -and $carrier){break}
        if($observerProcess.HasExited){throw 'Launcher ended before admitted worker appeared'}
        Start-Sleep -Milliseconds 20
    }while([DateTime]::UtcNow -lt $deadline)
    if(!$service -or !$carrier){throw 'No exact-package admitted worker'}
    $nativeCarrier=[Diagnostics.Process]::GetProcessById($carrier.ProcessId)
    $null=$nativeCarrier.Handle
    if($Kind -eq 'wow') {
        # MainWindowTitle observes the caller desktop, not our isolated one.
        & $ReadinessProbe $carrier.ProcessId "NTVDMConsoleTest-$($observerProcess.Id)" "$root/mine" 0 --ready-only *> "$root/readiness.txt"
        if($LASTEXITCODE){throw "Private-desktop WOW readiness failed: $LASTEXITCODE"}
    } else { do {
        $child=Get-CimInstance Win32_Process -Filter "ParentProcessId=$($carrier.ProcessId) AND Name='cmd.exe'" |
            Where-Object ExecutablePath -EQ "$env:SystemRoot\System32\cmd.exe" | Select-Object -First 1
        if($child){
            $nativeTarget=[Diagnostics.Process]::GetProcessById($child.ProcessId)
            $null=$nativeTarget.Handle
            break
        }
        Start-Sleep -Milliseconds 20
    }while([DateTime]::UtcNow -lt $deadline)
    if(!$nativeTarget){throw 'No pinned native task beneath the owned carrier'}
    # Prove actual CUI execution, not only CreateProcess(CREATE_SUSPENDED).
    # Pin its known test child as well: carrier/window disappearance alone
    # cannot prove every Console client has handled the close broadcast.
    do {
        $child=Get-CimInstance Win32_Process -Filter "ParentProcessId=$($nativeTarget.Id) AND Name='ping.exe'" |
            Where-Object ExecutablePath -EQ "$env:SystemRoot\System32\PING.EXE" | Select-Object -First 1
        if($child){
            $nativeDescendant=[Diagnostics.Process]::GetProcessById($child.ProcessId)
            $null=$nativeDescendant.Handle
            break
        }
        Start-Sleep -Milliseconds 20
    }while([DateTime]::UtcNow -lt $deadline)
    if(!$nativeDescendant){throw 'Native CUI did not execute its pinned test child before injection'}
    }
    $pinned=[Diagnostics.Process]::GetProcessById($service.ProcessId)
    try {
        if($pinned.MainModule.FileName -ine "$runtime\system32\ntsrv.exe"){
            throw 'Service identity changed before injection'
        }
        $identity=@{kind=$Kind;observer=$observerProcess.Id;service=$pinned.Id;worker=$carrier.ProcessId;target=if($nativeTarget){$nativeTarget.Id}else{0};descendant=if($nativeDescendant){$nativeDescendant.Id}else{0};
            serviceStart=$pinned.StartTime.ToUniversalTime();image=$pinned.MainModule.FileName}
        $identity|ConvertTo-Json|Set-Content "$root/injection.json"
        $clock=[Diagnostics.Stopwatch]::StartNew()
        $pinned.Kill()
        if(!$pinned.WaitForExit(10000)){throw 'Injected service did not exit'}
        if(!$observerProcess.WaitForExit(10000)){throw 'Run16 did not return after service death'}
        $report=Get-Content "$root/observer.txt" -Raw
        if($report -notmatch 'result=exited' -or $report -notmatch 'exit=0x00000006'){
            throw 'Run16 did not preserve disconnected-port ERROR_INVALID_HANDLE'
        }
        if($RequireCarrierCleanup) {
            $remaining=[Math]::Max(0,10000-$clock.ElapsedMilliseconds)
            if(!$nativeCarrier.WaitForExit([int]$remaining)){throw 'Carrier did not clean up within ten seconds'}
            $remaining=[Math]::Max(0,10000-$clock.ElapsedMilliseconds)
            if($nativeTarget -and !$nativeTarget.WaitForExit([int]$remaining)){throw 'Native CUI target survived carrier loss cleanup'}
            $remaining=[Math]::Max(0,10000-$clock.ElapsedMilliseconds)
            if($nativeDescendant -and !$nativeDescendant.WaitForExit([int]$remaining)){throw 'Native CUI descendant survived carrier loss cleanup'}
            "PASS actual $Kind carrier exit=$($nativeCarrier.ExitCode) and pinned CUI target/descendant exit before test containment"|Set-Content "$root/carrier-cleanup.txt"
        }
        "PASS authenticated service death: Run16 returned 6; observed elapsed=$($clock.ElapsedMilliseconds)ms; no Console-restoration claim" |
            Set-Content "$root/result.txt"
    } finally {$pinned.Dispose()}
} finally {
    if($observerProcess){
        if(!$observerProcess.HasExited){$observerProcess.Kill();$null=$observerProcess.WaitForExit(10000)}
        $observerProcess.Dispose()
    }
    # Fault injection may orphan the native task before S3 loss cleanup.
    # Closing this pinned test process is containment, never a cleanup pass.
    if($nativeTarget){
        try {
            if(!$nativeTarget.HasExited){$nativeTarget.Kill($true);$null=$nativeTarget.WaitForExit(10000)}
        } finally {$nativeTarget.Dispose()}
    }
    if($nativeCarrier){$nativeCarrier.Dispose()}
    if($nativeDescendant){
        try {
            if(!$nativeDescendant.HasExited){$nativeDescendant.Kill();$null=$nativeDescendant.WaitForExit(10000)}
        } finally {$nativeDescendant.Dispose()}
    }
    if($scope){Stop-IsolatedPackageScope $scope}
    if($runtime){Remove-PhysicalRuntimeStage $runtime}
    foreach($name in $saved.Keys){[Environment]::SetEnvironmentVariable($name,$saved[$name])}
}
