[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$Probe,
    [Parameter(Mandatory)][string]$LogPrefix,
    [Parameter(Mandatory)][ValidatePattern('^[0-9A-F]{8}$')][string]$FaultIp,
    [Parameter(Mandatory)][string]$PackageRoot,
    [string]$LogRoot,
    [switch]$Nested,
    [switch]$IgnoreFirst
)
$ErrorActionPreference = 'Stop'
$expectedDialog = "X#=0D, CS=[0-9A-F]{4} IP=$FaultIp"
$Observer = (Resolve-Path -LiteralPath $Observer).Path
$Probe = (Resolve-Path -LiteralPath $Probe).Path
$PackageRoot = (Resolve-Path -LiteralPath $PackageRoot).Path
if ($LogPrefix -notmatch '^[a-z0-9-]+$') { throw 'Invalid log prefix' }
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
. "$repo/test/observation/isolated_package_cleanup.ps1"
$out=Join-Path $repo "build/dpmi-fatal-$LogPrefix"
if(Test-Path -LiteralPath $out){throw 'Use a fresh prefix'}
& "$repo/tools/prepare-test-inputs.cmd" $out $Probe D38F.COM
if($LASTEXITCODE){throw 'DPMI probe preparation failed'}
if(!$LogRoot){$LogRoot=$out}
$LogRoot=(Resolve-Path -LiteralPath $LogRoot).Path
$report = Join-Path $LogRoot "$LogPrefix.txt"
if (Test-Path -LiteralPath $report) { throw 'Use a fresh log prefix' }
function PackageProcesses {
    @(Get-CimInstance Win32_Process -Filter "Name='run16.exe' OR Name='ntvdm.exe' OR Name='ntsrv.exe'" |
        Where-Object { $_.ExecutablePath -in $packagePaths })
}
& "$repo/tools/build-fixtures.cmd" x64 fatal-dialog
if($LASTEXITCODE){throw "Native observation helper build failed"}
[void][Reflection.Assembly]::LoadFrom("$repo/build/.ninja/fixtures-x64/fatal-dialog.dll")
$runtime=$null
$scope=$null
try {
$runtime=New-PhysicalRuntimeStage $PackageRoot -TestFiles @("$out/D38F.COM")
$scope=New-IsolatedPackageScope $runtime
$PackageRoot=$runtime
$binaryRoot=Get-PackageBinaryRoot $runtime
$packagePaths = @('run16.exe','ntvdm.exe','ntsrv.exe') | ForEach-Object { Join-Path $binaryRoot $_ }
Save-IsolatedPackageManifest $runtime (Join-Path $out 'manifest.json')
$arguments = @('tests\D38F.COM')
if ($Nested) { $arguments = @('COMMAND.COM','/c','COMMAND.COM','/c') + $arguments }
$observation = $null
$workerId = 0
$dialogText = ''
try {
    $observation = Start-Process -FilePath $Observer -ArgumentList (
        @($packagePaths[0],$PackageRoot,$report,'--wait') + $arguments + @('--observation-timeout-ms','20000')
    ) -WorkingDirectory $PackageRoot -WindowStyle Hidden -PassThru
    $deadline = [DateTime]::UtcNow.AddSeconds(15)
    $dialog = [IntPtr]::Zero
    while ([DateTime]::UtcNow -lt $deadline) {
        $workers = @(PackageProcesses | Where-Object { $_.Name -eq 'ntvdm.exe' })
        if ($workers.Count -gt 1) { throw 'Ambiguous test worker' }
        if ($workers.Count -eq 1) {
            $workerId = [int]$workers[0].ProcessId
            $dialog = [S38FatalDialog]::Find($workerId)
            if ($dialog -ne [IntPtr]::Zero) {
                $dialogText = [S38FatalDialog]::Text($dialog)
                if ($dialogText -match $expectedDialog) { break }
            }
        }
        Start-Sleep -Milliseconds 150
    }
    if ($dialogText -notmatch $expectedDialog) { throw "Expected exact original dialog absent: $dialogText" }
    $worker = Get-Process -Id $workerId
    $firstThread = [S38FatalDialog]::ThreadId($dialog)
    $secondThread = 0
    if ($IgnoreFirst) {
        [S38FatalDialog]::IgnoreAndWait($dialog)
        $deadline = [DateTime]::UtcNow.AddSeconds(5)
        do {
            $dialog = [S38FatalDialog]::Find($workerId)
            $secondThread = [S38FatalDialog]::ThreadId($dialog)
            $dialogText = [S38FatalDialog]::Text($dialog)
            if ($secondThread -ne 0 -and $secondThread -ne $firstThread -and
                $dialogText -match "X#=0D, CS=[0-9A-F]{4} IP=$FaultIp") { break }
            Start-Sleep -Milliseconds 50
        } while ([DateTime]::UtcNow -lt $deadline)
        if ($secondThread -eq 0 -or $secondThread -eq $firstThread -or
            $dialogText -notmatch "X#=0D, CS=[0-9A-F]{4} IP=$FaultIp") { throw 'Ignore did not produce a new exact-fault dialog' }
    }
    if (![S38FatalDialog]::Click($dialog,100)) { throw 'Original dialog action failed' }
    if (!$worker.WaitForExit(10000)) { throw 'Worker survived Terminate' }
    if (!$observation.WaitForExit(10000)) { throw 'Observer/launcher remains live' }
    $record = Get-Content -LiteralPath $report -Raw
    if ($record -notmatch '(?m)^result=exited\r?$') { throw 'Launcher did not exit' }
    @{ Dialog=$dialogText; WorkerId=$workerId; WorkerExited=$true; Nested=[bool]$Nested;
       IgnoreFirst=[bool]$IgnoreFirst; FirstDialogThread=$firstThread; SecondDialogThread=$secondThread;
       ProbeSha256=(Get-FileHash $Probe).Hash; WorkerSha256=(Get-FileHash $packagePaths[1]).Hash;
       Report=$report } | ConvertTo-Json | Set-Content -LiteralPath "$LogRoot\$LogPrefix.result.json"
    Write-Host "PASS S38 original failure dialog and Terminate: $LogPrefix"
} finally {
    if ($observation -and !$observation.HasExited) { $observation.Kill(); $observation.WaitForExit() }
    @{ Dialog=$dialogText; WorkerId=$workerId } | ConvertTo-Json | Set-Content -LiteralPath "$LogRoot\$LogPrefix.dialog.json"
}
} finally {
    try {if($scope){Stop-IsolatedPackageScope $scope}}
    finally {if($runtime){Remove-PhysicalRuntimeStage $runtime}}
}
