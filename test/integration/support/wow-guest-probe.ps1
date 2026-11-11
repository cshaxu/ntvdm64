# Shared setup for self-reporting, independently authored WOW guest probes.
# Case-specific record/exit assertions stay with the caller.
. "$PSScriptRoot/../../observation/isolated_package_cleanup.ps1"
function Invoke-WowGuestProbe {
 param([string]$PackageRoot,[string]$OutputRoot,[string]$Observer,
       [string]$Source,[string]$Name,[switch]$RequireNoRegistry)
 $ErrorActionPreference='Stop'
 $repo=(Resolve-Path "$PSScriptRoot/../../..").Path
 Set-Location $repo
 $out=[IO.Path]::GetFullPath((Join-Path $repo $OutputRoot))
 if(!$out.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Require fresh build evidence directory'}
 $null=New-Item -ItemType Directory $out
 & nasm -f bin (Join-Path $repo $Source) -o "$out/$Name.EXE"
 if($LASTEXITCODE){throw 'NE fixture assembly failed'}
 $runtime=$null;$scope=$null;$proc=$null;$saved=@{}
 foreach($key in @('TEMP','TMP','PATH','MVDM_OBSERVER_PRIVATE_DESKTOP')){$saved[$key]=[Environment]::GetEnvironmentVariable($key)}
 try {
  $runtime=New-PhysicalRuntimeStage (Resolve-Path $PackageRoot).Path -FreshPackageIdentity
  $scope=New-IsolatedPackageScope $runtime
  $binary=Get-PackageBinaryRoot $runtime
  if($RequireNoRegistry -and (Test-Path "$binary/NTVDM.REG")){throw 'Require absent shadow file: no classes, hence no native profile writes'}
  $tests=Join-Path $runtime 'tests';$null=New-Item -ItemType Directory $tests -Force
  Copy-Item "$out/$Name.EXE" $tests
  $env:TEMP=$runtime;$env:TMP=$runtime;$env:PATH="$binary;$env:SystemRoot\System32";$env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
  $start=[Diagnostics.ProcessStartInfo]::new((Resolve-Path $Observer).Path);$start.UseShellExecute=$false;$start.CreateNoWindow=$true
  $log="$out/observer.txt"
  foreach($arg in @("$binary/run16.exe",$tests,$log,'--wait',"$tests/$Name.EXE",'--observation-timeout-ms','30000')){$start.ArgumentList.Add($arg)}
  $proc=[Diagnostics.Process]::Start($start);$null=$proc.Handle
  if(!$proc.WaitForExit(45000)){throw 'Owned observer timeout'}
  $result="$tests/$Name.BIN"
  if(!(Test-Path $result)){throw 'Missing guest report'}
  Copy-Item $result "$out/report.bin"
  [pscustomobject]@{Bytes=[IO.File]::ReadAllBytes("$out/report.bin");LogText=(Get-Content $log -Raw);ExitCode=$proc.ExitCode;Output=$out;
   Package=(Get-FileHash "$PackageRoot/product-system32-manifest.json").Hash;
   Wow=(Get-FileHash "$binary/WOW32.DLL").Hash;Worker=(Get-FileHash "$binary/ntvdm.exe").Hash}
 } finally {
  if($proc){if(!$proc.HasExited){$proc.Kill();$proc.WaitForExit()};$proc.Dispose()}
  try {if($scope){Stop-IsolatedPackageScope $scope}}finally{
   try {if($runtime){Remove-PhysicalRuntimeStage $runtime}}finally{
    foreach($key in $saved.Keys){[Environment]::SetEnvironmentVariable($key,$saved[$key])}
   }
  }
 }
}
