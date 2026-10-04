param([Parameter(Mandatory)][string]$PackageRoot,
      [Parameter(Mandatory)][string]$Probe,
      [Parameter(Mandatory)][string]$Observer,
      [Parameter(Mandatory)][string]$RunRoot)
$ErrorActionPreference='Stop'
$PackageRoot=(Resolve-Path $PackageRoot).Path
$Probe=(Resolve-Path $Probe).Path;$Observer=(Resolve-Path $Observer).Path
$RunRoot=[IO.Path]::GetFullPath($RunRoot)
$build=(Resolve-Path "$PSScriptRoot/../../build").Path+'\'
if(!$RunRoot.StartsWith($build,[StringComparison]::OrdinalIgnoreCase) -or
   (Test-Path $RunRoot) -or (Test-Path Z:\)){throw 'Require fresh build run root and free Z:'}
$null=New-Item -ItemType Directory -Path $RunRoot
$runtime=Join-Path $RunRoot 'runtime'
Copy-Item -LiteralPath $PackageRoot -Destination $runtime -Recurse
$test=Join-Path $runtime 'tests'
if(!(Test-Path $test)){$null=New-Item -ItemType Directory -Path $test}
Copy-Item -LiteralPath $Probe -Destination (Join-Path $test 'SEARCH.EXE')
# Authored native batch, never a replacement guest/configuration.
[IO.File]::WriteAllText((Join-Path $test 'STREAM.CMD'),"@echo off`r`necho S3_INTERNAL_STDOUT`r`necho S3_INTERNAL_STDERR 1>&2`r`nexit /b 0`r`n",[Text.Encoding]::ASCII)
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$scope=New-IsolatedPackageScope $runtime
& subst.exe Z: $runtime
if($LASTEXITCODE){throw 'SUBST failed'}
$scope.Paths+=@($scope.Paths|ForEach-Object {Join-Path Z:\ $_.Substring($runtime.Length+1)})
function Observe([string]$Name,[string[]]$Tail){
    $report=Join-Path $RunRoot ($Name+'.txt')
    $start=[Diagnostics.ProcessStartInfo]::new($Observer)
    $start.UseShellExecute=$false;$start.WindowStyle=[Diagnostics.ProcessWindowStyle]::Hidden
    $start.Environment['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
    $start.Environment['MVDM_OBSERVER_SHORT_HISTORY']='1'
    $start.Environment['PATH']=Join-Path $env:SystemRoot 'System32'
    $start.Environment['SEARCH_IDENTITY_REPORT']=Join-Path $RunRoot ($Name+'.image.txt')
    foreach($item in (@('Z:\system32\run16.exe','Z:\tests',$report)+$Tail+@('--observation-timeout-ms','20000'))){$start.ArgumentList.Add($item)}
    $process=[Diagnostics.Process]::Start($start)
    try{
        if(!$process.WaitForExit(30000)){throw "Owned observer timeout: $($process.Id)"}
        $result=Get-Content $report -Raw
        if($process.ExitCode -or $result -notmatch '(?m)^result=exited\r?$' -or
            $result -notmatch '(?m)^exit=0x00000000\r?$'){throw "Handoff failed: $Name $result"}
        Get-Content ($report+'.console.txt') -Raw
    }finally{$process.Dispose();Stop-IsolatedPackageScope $scope}
}
try{
    $screen=Observe 'internal-interpreter' @('Z:\system32\COMMAND.COM','/c','cmd','/c','Z:\tests\STREAM.CMD')
    if(!$screen.Contains('S3_INTERNAL_STDOUT') -or !$screen.Contains('S3_INTERNAL_STDERR')){throw 'Missing native stream witnesses'}
    'PASS internal interpreter/native streams outside package CWD/PATH'
    $screen=Observe 'dos-selected-native' @('Z:\system32\COMMAND.COM','/c','SEARCH.EXE')
    $image=[IO.File]::ReadAllText((Join-Path $RunRoot 'dos-selected-native.image.txt'),[Text.Encoding]::Unicode)
    if($image -ine 'Z:\tests\SEARCH.EXE'){throw "Wrong DOS-selected native image: $image"}
    'PASS DOS-to-native selected current-directory image with package absent from PATH'
}finally{
    try{Stop-IsolatedPackageScope $scope}finally{& subst.exe Z: /d}
}
