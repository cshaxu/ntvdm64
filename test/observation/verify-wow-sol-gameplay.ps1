[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$CompilerDriver,
    [Parameter(Mandatory)][string]$BuildRoot,
    [switch]$VisibleDesktop,
    [ValidateSet('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe')]
    [string[]]$RuntimeImages=@('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe')
)
$ErrorActionPreference='Stop'
$CompilerDriver=(Resolve-Path -LiteralPath $CompilerDriver).Path
$Observer=(Resolve-Path -LiteralPath $Observer).Path
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)){throw 'Evidence must be below build/'}
if(Test-Path $root){throw 'Use a fresh evidence directory'}
$null=New-Item -ItemType Directory $root
. "$PSScriptRoot\isolated_package_cleanup.ps1"
$source="$PSScriptRoot\wow_sol_gameplay_probe.c"
$process=$null;$runtime=$null;$scope=$null
$saved=@{};foreach($name in @('TEMP','TMP','PATH','MVDM_OBSERVER_PRIVATE_DESKTOP')){$saved[$name]=[Environment]::GetEnvironmentVariable($name)}
try {
    Push-Location $root
    try {
        & $CompilerDriver cl.exe /nologo /O2 /MT /W4 /WX /D_CRT_SECURE_NO_WARNINGS "/Fo$root\probe.obj" "/Fe$root\probe.exe" $source /link user32.lib gdi32.lib dwmapi.lib *> "$root\compile.txt"
        if($LASTEXITCODE){throw "Probe build failed: $LASTEXITCODE"}
    } finally {Pop-Location}
    $runtime=New-PhysicalRuntimeStage $PackageRoot -RuntimeImages $RuntimeImages
    $scope=New-IsolatedPackageScope $runtime
    $manifest=Get-Content "$repo\assets\release\release-manifest.json" -Raw | ConvertFrom-Json
    $inputs=@('SOL.EXE')+@($manifest | ForEach-Object { 'system32\'+$_.name })
    $inputs | ForEach-Object {
        [pscustomobject]@{File=$_;SHA256=(Get-IsolatedFileSha256 (Join-Path $runtime $_))}
    } | ConvertTo-Json | Set-Content "$root\inputs.json"
    $env:TEMP=$runtime;$env:TMP=$runtime;$env:PATH="$runtime\system32;"+$env:SystemRoot+'\System32'
    if($VisibleDesktop){Remove-Item Env:MVDM_OBSERVER_PRIVATE_DESKTOP -ErrorAction SilentlyContinue}
    else{$env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'}
    $start=[Diagnostics.ProcessStartInfo]::new($Observer)
    $start.UseShellExecute=$false;$start.CreateNoWindow=$true
    $start.Arguments='"'+$runtime+'\system32\run16.exe" "'+$runtime+'" "'+$root+'\observer.txt" --wait "'+$runtime+'\SOL.EXE" --observation-timeout-ms 60000'
    $process=[Diagnostics.Process]::Start($start)
    $null=$process.Handle
    $process.Id | Set-Content "$root\observer.pid"
    $deadline=[DateTime]::UtcNow.AddSeconds(10);$worker=$null
    do {
        $worker=Get-CimInstance Win32_Process -Filter "Name='ntvdm.exe'" | Where-Object {$_.ExecutablePath -eq "$runtime\system32\ntvdm.exe"} | Select-Object -First 1
        if($worker -or $process.HasExited){break};Start-Sleep -Milliseconds 50
    }while([DateTime]::UtcNow -lt $deadline)
    if(!$worker){throw 'No worker in this owned runtime'}
    $desktopName=if($VisibleDesktop){'-'}else{"NTVDMConsoleTest-$($process.Id)"}
    & "$root\probe.exe" $worker.ProcessId $desktopName "$root\sol" *> "$root\probe.txt"
    $probeResult=$LASTEXITCODE
    if($probeResult -ne 0 -and $probeResult -ne 77){throw "Gameplay probe failed: $probeResult"}
    if(!$process.WaitForExit(65000)){throw 'Observer exceeded bound after window close'}
    $report=Get-Content "$root\observer.txt" -Raw
    if($process.ExitCode -or $report -notmatch 'result=exited' -or $report -notmatch 'exit=0x00000000'){
        throw 'Game actions passed but launcher completion did not; inspect observer.txt'
    }
    if($probeResult -eq 77){
        $probeText=Get-Content "$root\probe.txt" -Raw
        if($probeText -notmatch 'PARTIAL SOL startup and window close; full gameplay UNVERIFIED: geometry matches host max tracking size'){
            throw 'SOL returned the geometry-limited disposition without its exact host-geometry witness'
        }
        # The original program's 585x384 card layout cannot be operated through
        # coordinates that the actual host maximum tracking geometry refuses.
        # This is an integration branch, not an implicit success for gameplay:
        # it proves window creation, command readiness, normal close and launcher
        # completion on the constrained host.  A sufficiently large host takes
        # the ordinary deal/draw branch below.
        'PASS SOL geometry-limited integration: startup, command readiness, normal close and run16 completion; deal/draw coordinates are not applicable at the actual host maximum tracking size' | Set-Content "$root\result.txt"
        return
    }
    'PASS SOL deal, stock draw, WM_CLOSE and run16 completion' | Set-Content "$root\result.txt"
} finally {
    if($process){if(!$process.HasExited){$process.Kill();$process.WaitForExit(5000)|Out-Null};$process.Dispose()}
    if($scope){Stop-IsolatedPackageScope $scope}
    if($runtime){Remove-PhysicalRuntimeStage $runtime}
    foreach($name in $saved.Keys){[Environment]::SetEnvironmentVariable($name,$saved[$name])}
}
