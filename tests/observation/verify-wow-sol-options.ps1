[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$CompilerDriver,
    [Parameter(Mandatory)][string]$BuildRoot,
    [switch]$CaptureFault,
    [ValidateSet('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe')]
    [string[]]$RuntimeImages=@('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe')
)
$ErrorActionPreference='Stop'
$CompilerDriver=(Resolve-Path -LiteralPath $CompilerDriver).Path
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)){throw 'Evidence must be below build/'}
if(Test-Path $root){throw 'Use a fresh evidence directory'}
$null=New-Item -ItemType Directory $root
. "$PSScriptRoot\isolated_package_cleanup.ps1"
$source="$PSScriptRoot\wow_sol_options_probe.c"
$process=$null;$runtime=$null;$scope=$null
$saved=@{};foreach($name in @('TEMP','TMP','PATH','MVDM_OBSERVER_PRIVATE_DESKTOP','MVDM_OBSERVER_SOL_OPTIONS_PROBE','MVDM_WOW_SOL_OPTIONS_NO_DEBUG','MVDM_WOW_WINDOW_TRACE_PATH','MVDM_WOW_CALLBACK_FRAME_TRACE_PATH','MVDM_WOW_CALLBACK_TRACE_PATH','MVDM_WOW_DIALOG_PROCEDURE_TRACE_PATH','MVDM_WOW_DIALOG_CREATE_TRACE_PATH','MVDM_WOW_CALLPROC_TRACE_PATH')){$saved[$name]=[Environment]::GetEnvironmentVariable($name)}
try {
    Push-Location $root
    try {
        & $CompilerDriver cl.exe /nologo /O2 /MT /W4 /WX /D_CRT_SECURE_NO_WARNINGS "/Fo$root\probe.obj" "/Fe$root\probe.exe" $source /link user32.lib psapi.lib dbghelp.lib *> "$root\compile.txt"
        if($LASTEXITCODE){throw "Probe build failed: $LASTEXITCODE"}
    } finally {Pop-Location}
    $runtime=New-PhysicalRuntimeStage $PackageRoot -RuntimeImages $RuntimeImages
    $scope=New-IsolatedPackageScope $runtime
    $env:TEMP=$runtime;$env:TMP=$runtime;$env:PATH="$runtime\system32;"+$env:SystemRoot+'\System32'
    if($CaptureFault){Remove-Item Env:MVDM_WOW_SOL_OPTIONS_NO_DEBUG -ErrorAction SilentlyContinue}
    else {$env:MVDM_WOW_SOL_OPTIONS_NO_DEBUG='1'}
    $env:MVDM_WOW_WINDOW_TRACE_PATH="$root\windows.txt"
    $env:MVDM_WOW_CALLBACK_FRAME_TRACE_PATH="$root\callbacks.txt"
    $env:MVDM_WOW_CALLBACK_TRACE_PATH="$root\callback-dispatch.txt"
    $env:MVDM_WOW_DIALOG_PROCEDURE_TRACE_PATH="$root\dialog-procedures.txt"
    $env:MVDM_WOW_DIALOG_CREATE_TRACE_PATH="$root\dialog-creates.txt"
    $env:MVDM_WOW_CALLPROC_TRACE_PATH="$root\callproc.txt"
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
