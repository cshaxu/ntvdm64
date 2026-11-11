[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$CompilerDriver,
    [Parameter(Mandatory)][string]$BuildRoot,
    [string]$Observer,
    [switch]$VisibleDesktop,
    [ValidateRange(1,32)][int]$Rounds=1,
    [switch]$CaptureFault,
    [switch]$NoTrace,
    [ValidateRange(10000,120000)][int]$DialogWaitMs=10000,
    [ValidateRange(0,300)][int]$ExternalWindows=0,
    [ValidateSet('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe')]
    [string[]]$RuntimeImages=@('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe')
)
$ErrorActionPreference='Stop'
$CompilerDriver=(Resolve-Path -LiteralPath $CompilerDriver).Path
if($VisibleDesktop -and !$Observer){throw 'Visible desktop requires the console observer'}
if(!$VisibleDesktop -and $Rounds -ne 1){throw 'Repeated Options requires a PID-bound visible-desktop probe'}
$observationMs=[Math]::Max(60000+($Rounds-1)*15000, ($Rounds*2+1)*$DialogWaitMs)
if($VisibleDesktop -and $observationMs -gt 600000){throw 'Requested rounds and dialog wait exceed the observer 600-second limit'}
if($Observer){$Observer=(Resolve-Path -LiteralPath $Observer).Path}
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)){throw 'Evidence must be below build/'}
if(Test-Path $root){throw 'Use a fresh evidence directory'}
$null=New-Item -ItemType Directory $root
. "$PSScriptRoot\isolated_package_cleanup.ps1"
$source="$PSScriptRoot\wow_sol_options_probe.c"
$process=$null;$runtime=$null;$scope=$null
$saved=@{};foreach($name in @('TEMP','TMP','PATH','MVDM_OBSERVER_PRIVATE_DESKTOP','MVDM_OBSERVER_SOL_OPTIONS_PROBE','MVDM_WOW_SOL_OPTIONS_NO_DEBUG','MVDM_WOW_SOL_DIALOG_WAIT_MS','MVDM_SOL_EXTERNAL_WINDOWS','MVDM_WOW_WINDOW_TRACE_PATH','MVDM_WOW_CALLBACK_FRAME_TRACE_PATH','MVDM_WOW_CALLBACK_TRACE_PATH','MVDM_WOW_DIALOG_PROCEDURE_TRACE_PATH','MVDM_WOW_DIALOG_CREATE_TRACE_PATH','MVDM_WOW_CALLPROC_TRACE_PATH')){$saved[$name]=[Environment]::GetEnvironmentVariable($name)}
try {
    Push-Location $root
    try {
        & $CompilerDriver cl.exe /nologo /O2 /MT /W4 /WX /D_CRT_SECURE_NO_WARNINGS "/Fo$root\probe.obj" "/Fe$root\probe.exe" $source /link user32.lib psapi.lib dbghelp.lib *> "$root\compile.txt"
        if($LASTEXITCODE){throw "Probe build failed: $LASTEXITCODE"}
    } finally {Pop-Location}
    $runtime=New-PhysicalRuntimeStage $PackageRoot -RuntimeImages $RuntimeImages
    Save-IsolatedPackageManifest $runtime "$root\manifest.json"
    $scope=New-IsolatedPackageScope $runtime
    $env:TEMP=$runtime;$env:TMP=$runtime;$env:PATH="$runtime\system32;"+$env:SystemRoot+'\System32'
    if($ExternalWindows -and $Rounds -lt 3){throw "External window measurement requires at least three rounds"}
    $env:MVDM_SOL_EXTERNAL_WINDOWS=[string]$ExternalWindows
    $env:MVDM_WOW_SOL_DIALOG_WAIT_MS=[string]$DialogWaitMs
    if($CaptureFault){Remove-Item Env:MVDM_WOW_SOL_OPTIONS_NO_DEBUG -ErrorAction SilentlyContinue}
    else {$env:MVDM_WOW_SOL_OPTIONS_NO_DEBUG='1'}
    if (!$NoTrace) {
    $env:MVDM_WOW_WINDOW_TRACE_PATH="$root\windows.txt"
    $env:MVDM_WOW_CALLBACK_FRAME_TRACE_PATH="$root\callbacks.txt"
    $env:MVDM_WOW_CALLBACK_TRACE_PATH="$root\callback-dispatch.txt"
    $env:MVDM_WOW_DIALOG_PROCEDURE_TRACE_PATH="$root\dialog-procedures.txt"
    $env:MVDM_WOW_DIALOG_CREATE_TRACE_PATH="$root\dialog-creates.txt"
    $env:MVDM_WOW_CALLPROC_TRACE_PATH="$root\callproc.txt"
    } else {
        foreach ($name in @("MVDM_WOW_WINDOW_TRACE_PATH","MVDM_WOW_CALLBACK_FRAME_TRACE_PATH","MVDM_WOW_CALLBACK_TRACE_PATH","MVDM_WOW_DIALOG_PROCEDURE_TRACE_PATH","MVDM_WOW_DIALOG_CREATE_TRACE_PATH","MVDM_WOW_CALLPROC_TRACE_PATH")) { [Environment]::SetEnvironmentVariable($name,$null) }
    }
    if($VisibleDesktop){
        Remove-Item Env:MVDM_OBSERVER_PRIVATE_DESKTOP -ErrorAction SilentlyContinue
        $start=[Diagnostics.ProcessStartInfo]::new($Observer)
        $start.UseShellExecute=$false;$start.CreateNoWindow=$true
        $start.Arguments='"'+$runtime+'\system32\run16.exe" "'+$runtime+'" "'+$root+'\observer.txt" --wait "'+$runtime+'\SOL.EXE" --observation-timeout-ms '+$observationMs
        $process=[Diagnostics.Process]::Start($start);$null=$process.Handle
        $deadline=[DateTime]::UtcNow.AddSeconds(10);$worker=$null
        do {
            $worker=Get-CimInstance Win32_Process -Filter "Name='ntvdm.exe'" | Where-Object {$_.ExecutablePath -eq "$runtime\system32\ntvdm.exe"} | Select-Object -First 1
            if($worker -or $process.HasExited){break};Start-Sleep -Milliseconds 50
        }while([DateTime]::UtcNow -lt $deadline)
        if(!$worker){throw 'No worker in this owned runtime'}
        & "$root\probe.exe" attach-pid $worker.ProcessId "$root\probe.txt" $Rounds
        if($LASTEXITCODE){throw "SOL Options probe failed: $LASTEXITCODE"}
        if ([Environment]::GetEnvironmentVariable('MVDM_SOL_SAMPLE_PC')) {
            $probeReport = Get-Content "$root\probe.txt" -Raw
            $expectedWow32 = [regex]::Escape((Join-Path $runtime 'system32\WOW32.DLL'))
            if ($probeReport -notmatch ('(?im)^SOL module-path: module=WOW32\.DLL path=' + $expectedWow32 + '\s*$')) {
                throw 'Loaded WOW32 path does not match the owned isolated package'
            }
        }
        if(!$process.WaitForExit($observationMs+5000)){throw 'Observer exceeded bound after SOL Options close'}
        [string]$process.ExitCode | Set-Content "$root\observer-exit.txt"
        $report=Get-Content "$root\observer.txt" -Raw
        if($process.ExitCode -or $report -notmatch 'result=exited' -or $report -notmatch 'exit=0x00000000'){throw "SOL Options launcher did not complete; observer exit=$($process.ExitCode)"}
        'PASS SOL Options modal route and launcher completion' | Set-Content "$root\result.txt"
        return
    }
    # The probe owns a fresh private desktop itself.  Running it as an
    # observer child races SOL's first application window against a second
    # observer timeout and is not needed for this message-only witness.
    $start=[Diagnostics.ProcessStartInfo]::new("$root\probe.exe")
    $start.UseShellExecute=$false;$start.CreateNoWindow=$true
    $start.Arguments='"'+$runtime+'" "'+$root+'\probe.txt"'
    $process=[Diagnostics.Process]::Start($start)
    $null=$process.Handle
    if(!$process.WaitForExit(140000)){throw 'Observer exceeded bound after SOL Options probe'}
    $report=Get-Content "$root\probe.txt" -Raw
    if($process.ExitCode -or $report -notmatch 'PASS SOL Options command, modal dialog, close and parent recovery'){
        throw 'SOL Options probe failed; inspect retained report'
    }
    'PASS SOL Options modal route and launcher completion' | Set-Content "$root\result.txt"
} finally {
    if($process){if(!$process.HasExited){$process.Kill();$process.WaitForExit(5000)|Out-Null};$process.Dispose()}
    if($scope){Stop-IsolatedPackageScope $scope}
    if($runtime){Remove-PhysicalRuntimeStage $runtime}
    foreach($name in $saved.Keys){[Environment]::SetEnvironmentVariable($name,$saved[$name])}
}
