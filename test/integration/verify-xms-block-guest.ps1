[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$OutputRoot,
    [ValidateSet('Boundaries','Existing','InvalidRange','InvalidDescriptor')][string]$Case='Boundaries'
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath((Join-Path $repo $OutputRoot))
if(!$out.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)) {
    throw 'Require a fresh repository-build evidence directory'
}
$observerPath=(Resolve-Path $Observer).Path
. "$PSScriptRoot/../observation/isolated_package_cleanup.ps1"
New-Item -ItemType Directory -Path $out | Out-Null
$fault=$Case -in @('InvalidRange','InvalidDescriptor')
$source=if($fault){'xms_invalid_transport.asm'}elseif($Case -eq 'Boundaries'){'xms_block_boundaries.asm'}else{'xms_capability.asm'}
$defines=@(); if($Case -eq 'Existing'){$defines+= '-DDEFAULT_INT15'}
if($Case -eq 'InvalidDescriptor'){$defines+='-DINVALID_DESCRIPTOR'}
& nasm @defines -f bin "$PSScriptRoot/../observation/$source" -o "$out/XMSCASE.COM"
if($LASTEXITCODE){throw 'XMS probe assembly failed'}
$runtime=$null;$scope=$null;$process=$null;$worker=$null
$names=@('PATH','MVDM_OBSERVER_PRIVATE_DESKTOP','MVDM_OBSERVER_INITIAL_GEOMETRY')
$saved=@{};foreach($name in $names){$saved[$name]=[Environment]::GetEnvironmentVariable($name)}
try {
    $runtime=New-PhysicalRuntimeStage $PackageRoot
    $scope=New-IsolatedPackageScope $runtime
    New-Item -ItemType Directory "$runtime/tests" -Force | Out-Null
    Copy-Item "$out/XMSCASE.COM" "$runtime/tests/XMSCASE.COM"
    $env:PATH="$runtime/system32;"+(Join-Path $env:SystemRoot 'System32')
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    $env:MVDM_OBSERVER_INITIAL_GEOMETRY='80,30'
    $arguments=@("$runtime/system32/run16.exe","$runtime/tests","$out/report.txt",'--wait',"$runtime/tests/XMSCASE.COM",'--observation-timeout-ms','60000')
    $process=Start-Process -FilePath $observerPath -ArgumentList $arguments -WorkingDirectory $runtime -WindowStyle Hidden -PassThru
    if($fault) {
        $deadline=[DateTime]::UtcNow.AddSeconds(30)
        while(!(Test-Path "$runtime/tests/ENTER.DAT")) {
            if($process.HasExited -or [DateTime]::UtcNow -ge $deadline){throw 'XMS guest did not reach failure handshake'}
            Start-Sleep -Milliseconds 50
        }
        $workerPath=[IO.Path]::GetFullPath((Join-Path $runtime 'system32/ntvdm.exe'))
        $workers=@(Get-Process -Name ntvdm -ErrorAction SilentlyContinue | Where-Object {$_.Path -ieq $workerPath})
        if($workers.Count -ne 1){throw 'Require one owned NTVDM at XMS failure handshake'}
        $worker=$workers[0]
        $null=$worker.Handle # Hold the real process before releasing the guest.
        $workerStart=$worker.StartTime
        [IO.File]::WriteAllBytes("$runtime/tests/GO.DAT",[byte[]]@())
        if(!$worker.WaitForExit(30000)){throw 'XMS failure did not terminate the owned worker'}
        $worker.Refresh()
        @{worker_pid=$worker.Id;worker_start=$workerStart.ToString('o');worker_exit=$worker.ExitCode;
          package_sha256=(Get-FileHash "$runtime/product-system32-manifest.json" -ErrorAction Stop).Hash;
          worker_sha256=(Get-FileHash $workerPath).Hash;entered=$true;returned=(Test-Path "$runtime/tests/AFTER.DAT")
        } | ConvertTo-Json | Set-Content "$out/worker-result.json"
        if($worker.ExitCode -ne 487 -or (Test-Path "$runtime/tests/AFTER.DAT")){throw 'XMS failure did not preserve INVALID_ADDRESS/no-return contract'}
    }
    if(!$process.WaitForExit(75000)){ $process.Kill();$process.WaitForExit();throw 'Observer timeout' }
    $report=Get-Content "$out/report.txt" -Raw
    $screen=Get-Content "$out/report.txt.console.txt" -Raw
    # Worker failure is distinct from the unfinished launcher's broker result.
    $expectedExit=if($fault){'exit=0x0000042b'}else{'exit=0x00000000'}
    if($process.ExitCode -ne 0 -or $report -notmatch $expectedExit) {throw "Guest XMS case failed; expected $expectedExit"}
    if($fault -and $screen -match 'XMS-FAULT-RETURNED') {throw 'Failed XMS transport returned to guest'}
    if($Case -eq 'Boundaries' -and $screen -notmatch 'XMS-BOUNDARIES-PASS'){throw 'Missing boundary assertions'}
    if($Case -eq 'Existing' -and $screen -notmatch 'S35_XMS_ALLOC_MOVE_REALLOC_FREE_A20_OK'){throw 'Missing existing XMS assertions'}
    "PASS guest XMS case=$Case" | Set-Content "$out/result.txt"
    Get-Content "$out/result.txt"
} finally {
    if($worker){$worker.Dispose()}
    if($process){$process.Dispose()}
    if($scope){Stop-IsolatedPackageScope $scope}
    if($runtime){Remove-PhysicalRuntimeStage $runtime}
    foreach($name in $names){[Environment]::SetEnvironmentVariable($name,$saved[$name])}
}
