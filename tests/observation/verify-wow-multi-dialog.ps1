[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$CompilerDriver,
    [Parameter(Mandatory)][string]$BuildRoot,
    [Parameter(Mandatory)][string]$Guest,
    [Parameter(Mandatory)][ValidateSet("notepad","progman","winmine","calc","write")][string]$Mode,
    [string]$Observer,
    [switch]$VisibleDesktop,
    [ValidateRange(1,32)][int]$Rounds=1,
    [switch]$CaptureFault,
    [switch]$NoTrace,
    [ValidateSet('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe')]
    [string[]]$RuntimeImages=@('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe')
)
$ErrorActionPreference='Stop'
$CompilerDriver=(Resolve-Path -LiteralPath $CompilerDriver).Path
$Guest=(Resolve-Path -LiteralPath $Guest).Path
if (!$VisibleDesktop) { throw "The matrix probe requires the approved visible desktop" }
if($VisibleDesktop -and !$Observer){throw 'Visible desktop requires the console observer'}
if(!$VisibleDesktop -and $Rounds -ne 1){throw 'Repeated Options requires a PID-bound visible-desktop probe'}
if($Observer){$Observer=(Resolve-Path -LiteralPath $Observer).Path}
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)){throw 'Evidence must be below build/'}
if(Test-Path $root){throw 'Use a fresh evidence directory'}
$null=New-Item -ItemType Directory $root
. "$PSScriptRoot\isolated_package_cleanup.ps1"
$source="$PSScriptRoot\wow_multi_dialog_probe.c"
$process=$null;$runtime=$null;$scope=$null
$saved=@{};foreach($name in @('TEMP','TMP','PATH','MVDM_OBSERVER_PRIVATE_DESKTOP','MVDM_OBSERVER_SOL_OPTIONS_PROBE','MVDM_WOW_SOL_OPTIONS_NO_DEBUG','MVDM_WOW_WINDOW_TRACE_PATH','MVDM_WOW_CALLBACK_FRAME_TRACE_PATH','MVDM_WOW_CALLBACK_TRACE_PATH','MVDM_WOW_DIALOG_PROCEDURE_TRACE_PATH','MVDM_WOW_DIALOG_CREATE_TRACE_PATH','MVDM_WOW_CALLPROC_TRACE_PATH')){$saved[$name]=[Environment]::GetEnvironmentVariable($name)}
try {
    Push-Location $root
    try {
        & $CompilerDriver cl.exe /nologo /O2 /MT /W4 /WX /D_CRT_SECURE_NO_WARNINGS "/Fo$root\probe.obj" "/Fe$root\probe.exe" $source /link user32.lib psapi.lib dbghelp.lib *> "$root\compile.txt"
        if($LASTEXITCODE){throw "Probe build failed: $LASTEXITCODE"}
    } finally {Pop-Location}
    $runtime=New-PhysicalRuntimeStage $PackageRoot -RuntimeImages $RuntimeImages
    Save-IsolatedPackageManifest $runtime "$root\manifest.json"
    # Original WOW redirects the Windows-root progman.exe to the native
    # system shell; an isolated test application must not occupy that path.
    $guestFixture=Join-Path $runtime 'GUEST'
    $null=New-Item -ItemType Directory -Path $guestFixture
    $guestCopy=Join-Path $guestFixture ([IO.Path]::GetFileName($Guest))
    Copy-Item -LiteralPath $Guest -Destination $guestCopy -Force
    if ($Mode -eq 'progman') {
        # The isolated copy owns its INI/group files. Never point the guest at
        # the installed writable group paths from the source installation.
        $guestDir=Split-Path -Parent $Guest
        $groups=@('MAIN.GRP','ACCESSOR.GRP','GAMES.GRP','STARTUP.GRP')
        $ini=@('[Settings]','Window=64 48 576 384 1','Order= 1 2 3 4','', '[Groups]')
        for ($i=0; $i -lt $groups.Count; ++$i) {
            $sourceGroup=Join-Path $guestDir $groups[$i]
            if (!(Test-Path -LiteralPath $sourceGroup -PathType Leaf)) { throw "Missing isolated fixture group: $sourceGroup" }
            Copy-Item -LiteralPath $sourceGroup -Destination (Join-Path $runtime $groups[$i]) -Force
            $ini += ('Group'+($i+1)+'='+ (Join-Path $runtime $groups[$i]))
        }
        Set-Content -Encoding ascii -LiteralPath (Join-Path $runtime 'PROGMAN.INI') -Value $ini
    }
    $scope=New-IsolatedPackageScope $runtime
    $env:TEMP=$runtime;$env:TMP=$runtime;$env:PATH="$runtime\system32;"+$env:SystemRoot+'\System32'
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
        $observationMs=60000+($Rounds-1)*70000
        $start.Arguments='"'+$runtime+'\system32\run16.exe" "'+$runtime+'" "'+$root+'\observer.txt" --wait "'+$guestCopy+'" --observation-timeout-ms '+$observationMs
        $process=[Diagnostics.Process]::Start($start);$null=$process.Handle
        $deadline=[DateTime]::UtcNow.AddSeconds(10);$worker=$null
        do {
            $worker=Get-CimInstance Win32_Process -Filter "Name='ntvdm.exe'" | Where-Object {$_.ExecutablePath -eq "$runtime\system32\ntvdm.exe"} | Select-Object -First 1
            if($worker -or $process.HasExited){break};Start-Sleep -Milliseconds 50
        }while([DateTime]::UtcNow -lt $deadline)
        if(!$worker){throw 'No worker in this owned runtime'}
        & "$root\probe.exe" $worker.ProcessId $Mode "$root\probe.txt" $Rounds
        if($LASTEXITCODE){throw "WOW dialog matrix probe failed: $LASTEXITCODE"}
        if((Select-String -LiteralPath "$root\probe.txt" -Pattern '^PASS menu dialog and parent recovery$').Count -ne $Rounds){throw 'Dialog probe did not complete every requested round'}
        if(!$process.WaitForExit($observationMs+5000)){throw 'Observer exceeded bound after SOL Options close'}
        $report=Get-Content "$root\observer.txt" -Raw
        if($process.ExitCode -or $report -notmatch 'result=exited' -or $report -notmatch 'exit=0x00000000'){throw 'WOW dialog launcher did not complete'}
        'PASS WOW dialog and launcher completion' | Set-Content "$root\result.txt"
        return
    }
} finally {
    if($process){if(!$process.HasExited){$process.Kill();$process.WaitForExit(5000)|Out-Null};$process.Dispose()}
    if($scope){Stop-IsolatedPackageScope $scope}
    if($runtime){Remove-PhysicalRuntimeStage $runtime}
    foreach($name in $saved.Keys){[Environment]::SetEnvironmentVariable($name,$saved[$name])}
}
