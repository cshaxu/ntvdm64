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
   (Test-Path $RunRoot) -or (Test-Path Z:\)){throw 'Require fresh build root and free Z:'}
$null=New-Item -ItemType Directory -Path $RunRoot
$runtime=Join-Path $RunRoot 'runtime'
Copy-Item -LiteralPath $PackageRoot -Destination $runtime -Recurse
$tests=Join-Path $runtime 'tests'
if(!(Test-Path $tests)){$null=New-Item -ItemType Directory -Path $tests}
Copy-Item -LiteralPath $Probe -Destination (Join-Path $tests 'HROOT.EXE')
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$scope=New-IsolatedPackageScope $runtime
$mapped=$false
try{
    & subst.exe Z: $runtime
    if($LASTEXITCODE){throw 'SUBST failed'}
    $mapped=$true
    $scope.Paths+=@($scope.Paths|ForEach-Object {Join-Path Z:\ ([IO.Path]::GetFileName($_))})
    foreach($case in @('direct','dos-native','nested-dos-native')){
        $tail=switch($case){
            'direct' {@('Z:\tests\HROOT.EXE')}
            'dos-native' {@('Z:\system32\COMMAND.COM','/c','HROOT.EXE')}
            'nested-dos-native' {@('Z:\system32\COMMAND.COM','/c','COMMAND.COM','/c','HROOT.EXE')}
        }
        $report=Join-Path $RunRoot ($case+'.txt')
        $hostReport=Join-Path $RunRoot ($case+'.host.txt')
        $start=[Diagnostics.ProcessStartInfo]::new($Observer)
        $start.UseShellExecute=$false;$start.WindowStyle=[Diagnostics.ProcessWindowStyle]::Hidden
        $start.Environment['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
        $start.Environment['MVDM_OBSERVER_SHORT_HISTORY']='1'
        $start.Environment['HOST_ROOT_REPORT']=$hostReport
        $start.Environment['NtvdmSystemRoot']='C:\wrong-root'
        $start.Environment['PATH']='Z:\system32;'+(Join-Path $env:SystemRoot 'System32')
        foreach($argument in (@('Z:\run16.exe','Z:\tests',$report)+$tail+@('--observation-timeout-ms','20000'))){$start.ArgumentList.Add($argument)}
        $process=[Diagnostics.Process]::Start($start)
        try{
            if(!$process.WaitForExit(30000)){throw "Owned observer timeout $($process.Id)"}
            $result=Get-Content $report -Raw
            if($process.ExitCode -or $result -notmatch '(?m)^result=exited\r?$' -or
                $result -notmatch '(?m)^exit=0x00000000\r?$'){throw "Handoff failed $case : $result"}
            $text=[IO.File]::ReadAllText($hostReport,[Text.Encoding]::Unicode)
            if(!$text.Contains("SYSTEMROOT=$env:SystemRoot`n") -or !$text.Contains("Windows=$env:SystemRoot`n") -or
               !$text.Contains("System=$(Join-Path $env:SystemRoot 'system32')`n")){
                throw "Native host root changed $case : $text"
            }
            if(!(Get-Content ($report+'.console.txt') -Raw).Contains('NATIVE-HOST-ROOT-PASS')){throw 'Missing actual native output'}
            "PASS native host root retained: $case"
        }finally{$process.Dispose();Stop-IsolatedPackageScope $scope}
    }
}finally{try{Stop-IsolatedPackageScope $scope}finally{if($mapped){& subst.exe Z: /d}}}
