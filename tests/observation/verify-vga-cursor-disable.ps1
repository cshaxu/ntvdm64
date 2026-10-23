[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$OutputRoot
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath((Join-Path $repo $OutputRoot))
if(!$out.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)) {
    throw 'Require a fresh repository-build evidence directory'
}
$observerPath=(Resolve-Path $Observer).Path
. "$PSScriptRoot/isolated_package_cleanup.ps1"
New-Item -ItemType Directory -Path $out | Out-Null
& nasm -f bin "$PSScriptRoot/vga-cursor-registers.asm" -o "$out/CURSOR.COM"
if($LASTEXITCODE){throw 'Cursor probe assembly failed'}
$runtime=$null;$scope=$null;$process=$null
$names=@('PATH','MVDM_OBSERVER_PRIVATE_DESKTOP','MVDM_OBSERVER_INITIAL_GEOMETRY',
    'MVDM_OBSERVER_WINDOW_INPUT','MVDM_OBSERVER_TEXT_CURSOR','MVDM_TEST_CONSOLE_WIRE_PATH')
$saved=@{};foreach($name in $names){$saved[$name]=[Environment]::GetEnvironmentVariable($name)}
try {
    # Package must contain ntvdm-console-wire-observer.exe under the product
    # name ntvdm.exe; its real production path records text frames for tests.
    $runtime=New-PhysicalRuntimeStage $PackageRoot
    $scope=New-IsolatedPackageScope $runtime
    Copy-Item "$out/CURSOR.COM" "$runtime/CURSOR.COM"
    $env:PATH="$runtime/system32;"+(Join-Path $env:SystemRoot 'System32')
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    $env:MVDM_OBSERVER_INITIAL_GEOMETRY='80,30'
    $env:MVDM_OBSERVER_WINDOW_INPUT='1'
    $env:MVDM_OBSERVER_TEXT_CURSOR='1'
    $env:MVDM_TEST_CONSOLE_WIRE_PATH="$out/cursor"
    $arguments=@("$runtime/system32/run16.exe",$runtime,"$out/report.txt",'--observation-timeout-ms','60000',"$runtime/CURSOR.COM")
    $process=Start-Process -FilePath $observerPath -ArgumentList $arguments -WorkingDirectory $runtime -WindowStyle Hidden -PassThru
    if(!$process.WaitForExit(75000)){ $process.Kill();$process.WaitForExit();throw 'Observer timeout' }
    if($process.ExitCode -ne 0 -or (Get-Content "$out/report.txt" -Raw) -notmatch 'result=exited' -or
       (Get-Content "$out/report.txt.console.txt" -Raw) -notmatch 'CURSOR-REGISTERS-PASS' -or
       (Get-Content "$out/report.txt.caf.txt" -Raw) -notmatch 'caf-visible-window=1') { throw 'Guest or window witness failed' }
    $frames=@(Get-ChildItem "$out/cursor-*.frame" | Sort-Object { [int]($_.BaseName.Split('-')[-1]) })
    $stage=0;$grid=''
    foreach($file in $frames) {
        $b=[IO.File]::ReadAllBytes($file.FullName)
        if($b.Length -lt 21468 -or [BitConverter]::ToInt32($b,0) -ne 80){throw 'Unexpected frame format'}
        $x=[BitConverter]::ToInt32($b,1056);$y=[BitConverter]::ToInt32($b,1060)
        $start=[BitConverter]::ToInt32($b,1064);$height=[BitConverter]::ToInt32($b,1068)
        $visible=[BitConverter]::ToUInt32($b,1080);$cells=[Convert]::ToBase64String($b,17468,4000)
        if($stage -eq 0 -and $x -eq 20 -and $y -eq 10 -and $start -eq 2 -and $height -eq 3 -and $visible -eq 1){$stage=1;$grid=$cells}
        elseif($stage -eq 1 -and $x -eq 20 -and $y -eq 10 -and $visible -eq 0 -and $cells -eq $grid){$stage=2}
        elseif($stage -eq 2 -and $x -eq 21 -and $y -eq 10 -and $start -eq 2 -and $height -eq 3 -and $visible -eq 1 -and $cells -eq $grid){$stage=3}
    }
    if($stage -ne 3){throw "Missing ordered cursor-frame states (stage=$stage)"}
    "PASS register readback and three ordered unchanged-text frames; count=$($frames.Count)" | Set-Content "$out/result.txt"
    Get-Content "$out/result.txt"
} finally {
    if($process){$process.Dispose()}
    if($scope){Stop-IsolatedPackageScope $scope}
    if($runtime){Remove-PhysicalRuntimeStage $runtime}
    foreach($name in $names){[Environment]::SetEnvironmentVariable($name,$saved[$name])}
}
