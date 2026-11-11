param([Parameter(Mandatory)][string]$PackageRoot,
      [Parameter(Mandatory)][string]$Probe,
      [Parameter(Mandatory)][string]$Observer,
      [Parameter(Mandatory)][string]$RunRoot,
      [switch]$DiagnoseWindows)
$ErrorActionPreference='Stop'
$PackageRoot=(Resolve-Path $PackageRoot).Path
$Probe=(Resolve-Path $Probe).Path;$Observer=(Resolve-Path $Observer).Path
$RunRoot=[IO.Path]::GetFullPath($RunRoot)
$build=(Resolve-Path "$PSScriptRoot/../../build").Path+'\'
if(!$RunRoot.StartsWith($build,[StringComparison]::OrdinalIgnoreCase) -or
   (Test-Path $RunRoot) -or (Test-Path Z:\)){throw 'Require fresh build root and free Z:'}
$null=New-Item -ItemType Directory -Path $RunRoot
$runtime=Join-Path $RunRoot 'relocated runtime'
Copy-Item -LiteralPath $PackageRoot -Destination $runtime -Recurse
$tests=Join-Path $runtime 'tests'
if(!(Test-Path $tests)){$null=New-Item -ItemType Directory -Path $tests}
Copy-Item -LiteralPath $Probe -Destination (Join-Path $tests 'DIRP.EXE')
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$scope=New-IsolatedPackageScope $runtime
$process=$null;$mapped=$false
try{
    & subst.exe Z: $runtime
    if($LASTEXITCODE){throw 'SUBST failed'}
    $mapped=$true
    $scope.Paths+=@($scope.Paths|ForEach-Object {Join-Path Z:\ $_.Substring($runtime.Length+1)})
    $start=[Diagnostics.ProcessStartInfo]::new($Observer)
    $start.UseShellExecute=$false;$start.WindowStyle=[Diagnostics.ProcessWindowStyle]::Hidden
    $start.Environment['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
    $start.Environment['NtvdmSystemRoot']='C:\wrong-root'
    foreach($argument in @('Z:\system32\run16.exe','Z:\tests',(Join-Path $RunRoot 'observer.txt'),
        '--wait','Z:\tests\DIRP.EXE','--observation-timeout-ms','20000')){$start.ArgumentList.Add($argument)}
    $process=[Diagnostics.Process]::Start($start)
    if($DiagnoseWindows -and !$process.WaitForExit(1500)){
        $windows=Join-Path $PSScriptRoot '../../build/M0-T425/S9/r033/worker-window-snapshot.exe'
        foreach($worker in @(Get-CimInstance Win32_Process -Filter "Name='ntvdm.exe'"|
            Where-Object {$_.ExecutablePath -in $scope.Paths})){
            & $windows $worker.ProcessId ('NTVDMConsoleTest-'+$process.Id)|
                Set-Content (Join-Path $RunRoot 'diagnostic-windows.txt')
        }
    }
    if(!$process.WaitForExit(25000)){throw 'Owned observer exceeded bound'}
    if($process.ExitCode){throw "Observer failure $($process.ExitCode)"}
    if(!(Test-Path (Join-Path $tests 'DIRP.DON'))){throw 'Guest directory calls did not complete'}
    $bytes=[IO.File]::ReadAllBytes((Join-Path $tests 'DIRP.BIN'))
    if($bytes.Length -ne 768){throw 'Incomplete guest directory report'}
    $actual=@(0,256,512|ForEach-Object {[Text.Encoding]::ASCII.GetString($bytes,$_ ,256).Split([char]0)[0]})
    $expected=@('Z:\','Z:\system','Z:\system32\KRNL386.EXE')
    for($i=0;$i -lt 3;++$i){
        # Original KRNL386 appends \SYSTEM to SYSTEMROOT; a drive-root package
        # therefore returns Z:\\SYSTEM. Compare absolute path identity, not
        # separator spelling, without accepting a host or relative directory.
        if(![IO.Path]::IsPathFullyQualified($actual[$i]) -or
           [IO.Path]::GetFullPath($actual[$i]).TrimEnd('\') -ine
           [IO.Path]::GetFullPath($expected[$i]).TrimEnd('\')){throw "Guest directory mismatch $i : $($actual[$i])"}
    }
    [pscustomobject]@{Windows=$actual[0];System=$actual[1];Kernel=$actual[2];ProbeSha256=(Get-FileHash $Probe).Hash}|
        ConvertTo-Json|Set-Content (Join-Path $RunRoot 'directories.json')
    'PASS real Win16 Windows/system/system32 directory APIs in relocated package'
}finally{
    if($process){$process.Dispose()}
    try{Stop-IsolatedPackageScope $scope}finally{if($mapped){& subst.exe Z: /d}}
}
