[CmdletBinding()]
param([Parameter(Mandatory)][string]$ProbePath,
      [Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../../..").Path
. "$repo/tests/observation/isolated_package_cleanup.ps1"
$out=[IO.Path]::GetFullPath((Join-Path $repo $OutputRoot))
if(!$out.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path -LiteralPath $out)) { throw 'Require a fresh build-owned evidence directory' }
$null=New-Item -ItemType Directory -Path $out
$probe=(Resolve-Path -LiteralPath $ProbePath).Path
$runtime=$null;$scope=$null;$proc=$null;$saved=@{}
foreach($name in @('TEMP','TMP','PATH','MVDM_OBSERVER_PRIVATE_DESKTOP')){$saved[$name]=[Environment]::GetEnvironmentVariable($name)}
try {
 $runtime=New-PhysicalRuntimeStage (Resolve-Path "$repo/build/package-uuid/package-c").Path -FreshPackageIdentity
 $scope=New-IsolatedPackageScope $runtime
 $tests=Join-Path $runtime 'tests';$null=New-Item -ItemType Directory -Path $tests -Force
 Copy-Item -LiteralPath $probe -Destination (Join-Path $tests 'TABTEST.EXE')
 $env:TEMP=$runtime;$env:TMP=$runtime;$env:PATH="$(Get-PackageBinaryRoot $runtime);$env:SystemRoot\System32";$env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
 $start=[Diagnostics.ProcessStartInfo]::new((Resolve-Path "$repo/build/.ninja/observations-x64/console-startup-observer.exe").Path)
 $start.UseShellExecute=$false;$start.CreateNoWindow=$true
 $log=Join-Path $out 'tabstop-guest.observer.txt'
 foreach($arg in @((Join-Path (Get-PackageBinaryRoot $runtime) 'run16.exe'),$tests,$log,'--wait',"$tests/TABTEST.EXE",'--observation-timeout-ms','40000')){$start.ArgumentList.Add($arg)}
 $proc=[Diagnostics.Process]::Start($start);$null=$proc.Handle
 if(!$proc.WaitForExit(45000)){throw 'Observer supervisor exceeded bounded interval'}
 $proc.Refresh();$code=$proc.ExitCode
 $result=Join-Path $tests 'TABTEST.BIN'
 if(!(Test-Path -LiteralPath $result -PathType Leaf)){throw 'Guest report is missing'}
 $observation=Get-Content -LiteralPath $log -Raw
 if($observation -notmatch '(?m)^result=exited\r?$' -or $observation -notmatch '(?m)^exit=0x00000000\r?$'){throw 'Guest did not exit successfully'}
 Copy-Item -LiteralPath $result -Destination (Join-Path $out 'tabstop-guest.report.bin')
 @{observerExit=$code;probeSha256=(Get-IsolatedFileSha256 $probe);runtime=$runtime;observerLog=$log}|ConvertTo-Json|Set-Content (Join-Path $out 'tabstop-guest.run.json')
 if($code){throw "Observer exit $code"}
} catch { $_|Out-String|Set-Content (Join-Path $out 'tabstop-guest.failure.txt');throw } finally {
 foreach($name in $saved.Keys){[Environment]::SetEnvironmentVariable($name,$saved[$name])}
 if($proc){$proc.Dispose()}
 try {if($scope){Stop-IsolatedPackageScope $scope}} finally {if($runtime){Remove-PhysicalRuntimeStage $runtime}}
}
