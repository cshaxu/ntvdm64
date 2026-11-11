[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$BuildRoot,
 [string]$Nasm='nasm.exe',
 [string]$ToolHelpImage='O:\winnt\tests\w31p\SYSTEM\TOOLHELP.DLL',
 [switch]$ExpectOriginalUnavailable
)
# Actual immutable-guest ToolHelp exercise. Invoke only from an owned hidden
# background supervisor; the stage and copied DLL are test inputs, not media.
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path -LiteralPath $root)){
 throw 'Fresh build-owned output required'
}
$null=New-Item -ItemType Directory -Path $root
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$runtime=$null;$scope=$null;$process=$null
try {
 $assembler=(Get-Command $Nasm -ErrorAction Stop).Source
 $probe="$PSScriptRoot/win16_toolhelp_probe.asm"
 & $assembler -f bin -o "$root/THPROBE.EXE" -l "$root/toolhelp-probe.lst" $probe *> "$root/assemble.txt"
 if($LASTEXITCODE){throw 'Independent ToolHelp probe assembly failed'}
 $toolhelp=(Resolve-Path -LiteralPath $ToolHelpImage).Path
 $runtime=New-PhysicalRuntimeStage (Resolve-Path -LiteralPath $PackageRoot).Path
 $scope=New-IsolatedPackageScope $runtime
 $tests=Join-Path $runtime 'tests';$null=New-Item -ItemType Directory -Path $tests -Force
 Copy-Item -LiteralPath "$root/THPROBE.EXE",$toolhelp -Destination $tests
 Get-FileHash $probe,$PSCommandPath,"$root/THPROBE.EXE",$toolhelp | Export-Csv -NoTypeInformation "$root/inputs.csv"
 $saved=@{};foreach($name in @('TEMP','TMP','PATH','MVDM_OBSERVER_PRIVATE_DESKTOP')){$saved[$name]=[Environment]::GetEnvironmentVariable($name)}
 try {
  $binary=Get-PackageBinaryRoot $runtime
  $env:TEMP=$runtime;$env:TMP=$runtime;$env:PATH="$binary;"+$env:SystemRoot+'\System32';$env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
  $start=[Diagnostics.ProcessStartInfo]::new((Resolve-Path -LiteralPath $Observer).Path)
  $start.UseShellExecute=$false;$start.CreateNoWindow=$true
  foreach($arg in @((Join-Path $binary 'run16.exe'),$tests,"$root/observer.txt",'--wait',"$tests/THPROBE.EXE",'--observation-timeout-ms','40000')){$start.ArgumentList.Add($arg)}
  $process=[Diagnostics.Process]::Start($start);$null=$process.Handle
  if(!$process.WaitForExit(45000)){throw 'ToolHelp observer timeout'};$process.Refresh()
  if($process.ExitCode){throw "ToolHelp observer failed $($process.ExitCode)"}
  $bytes=[IO.File]::ReadAllBytes("$tests/RESULT.BIN")
  if($bytes.Length -ne 18 -or [Text.Encoding]::ASCII.GetString($bytes,0,4) -ne 'THP1'){throw 'Invalid ToolHelp guest report'}
  $row=[ordered]@{version=[BitConverter]::ToUInt16($bytes,4);winFlags=[BitConverter]::ToUInt16($bytes,6);task=[BitConverter]::ToUInt16($bytes,8);moduleBad=[BitConverter]::ToUInt16($bytes,10);module=[BitConverter]::ToUInt16($bytes,12);class=[BitConverter]::ToUInt16($bytes,14);exit=[BitConverter]::ToUInt16($bytes,16)}
  $row|ConvertTo-Json|Set-Content "$root/guest-report.json"
  if($ExpectOriginalUnavailable){
   if($row.version -ne 0x0a03 -or !($row.winFlags -band 1) -or $row.task -ne 0 -or $row.moduleBad -ne 0 -or $row.module -ne 0 -or $row.class -ne 0 -or $row.exit -ne 1){throw 'Original ToolHelp unavailable disposition changed'}
  } elseif($row.task -ne 1 -or $row.moduleBad -ne 0 -or $row.module -ne 1 -or $row.class -ne 0 -or $row.exit -ne 0){throw 'ToolHelp query/failure contract mismatch'}
  $log=Get-Content "$root/observer.txt" -Raw
  if($log -notmatch 'result=exited' -or $log -notmatch 'exit=0x00000000'){throw 'ToolHelp guest did not retire normally'}
  if($ExpectOriginalUnavailable){'PASS original ToolHelp unavailable disposition: compatible Win3.1 version/mode, query exports reject'|Set-Content "$root/result.txt"}
  else{'PASS original TaskFirst/ModuleFirst normal and size-failure paths; ClassFirst original WOW failure'|Set-Content "$root/result.txt"}
  '0'|Set-Content "$root/result.exit"
 } finally {foreach($name in $saved.Keys){[Environment]::SetEnvironmentVariable($name,$saved[$name])}}
} catch { $_|Out-String|Set-Content "$root/failure.txt";'1'|Set-Content "$root/result.exit";throw }
finally {
 if($process){$process.Dispose()}
 try {if($scope){Stop-IsolatedPackageScope $scope}}finally {if($runtime){Remove-PhysicalRuntimeStage $runtime}}
}
