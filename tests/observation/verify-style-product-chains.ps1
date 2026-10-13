param(
 [Parameter(Mandatory)][string]$RuntimeRoot,
 [Parameter(Mandatory)][string]$FixtureRoot,
 [Parameter(Mandatory)][string]$BuildRoot,
 [ValidateSet('cui-launcher','dos-leaf','dos-launcher')][string]$Mode='dos-launcher')
$ErrorActionPreference='Stop'
Import-Module Microsoft.PowerShell.Utility
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($BuildRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
 (Test-Path -LiteralPath $out)){throw 'Fresh build-owned output required'}
$fixtures=(Resolve-Path $FixtureRoot).Path
$null=New-Item -ItemType Directory $out
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$stage=$null;$scope=$null
try {
 $stage=New-PhysicalRuntimeStage $RuntimeRoot
 $scope=New-IsolatedPackageScope $stage
 $image=if($Mode -eq 'cui-launcher'){'native-title-test.exe'}else{'native-title-gui.exe'}
 foreach($arch in @('x86','x64')) {
  Copy-Item "$fixtures/$arch/system32/$image" "$stage/tests/STYLE$arch.exe"
  $scope.Paths+=(Resolve-Path "$stage/tests/STYLE$arch.exe").Path
 }
 $stage|Set-Content "$out/stage.txt"
 # Match the Product gate's controlled environment. No global environment
 # changes: callers run this script in their owned background process.
 $keep=@('SystemRoot','WINDIR','ComSpec','SystemDrive','USERPROFILE','USERNAME','USERDOMAIN','LOCALAPPDATA','APPDATA','ProgramData','ProgramFiles','ProgramFiles(x86)','ProgramW6432','CommonProgramFiles','CommonProgramFiles(x86)','CommonProgramW6432','NUMBER_OF_PROCESSORS','PROCESSOR_ARCHITECTURE','OS','PATHEXT','PSExecutionPolicyPreference')
 foreach($item in @(Get-ChildItem Env:)){if($item.Name -notin $keep){Remove-Item -LiteralPath ('Env:'+$item.Name)}}
 $env:TEMP='O:\tmp';$env:TMP='O:\tmp';$env:PSModulePath=Join-Path $PSHOME Modules
 $env:PATH=$PSHOME+';'+$env:SystemRoot+'\System32;'+$env:SystemRoot
 $arguments="$Mode `"$stage\system32\run16.exe`" `"$stage\tests\STYLEx86.exe`" `"$stage\tests\STYLEx64.exe`""
 $p=Start-Process "$fixtures/x64/system32/native-title-test.exe" -WorkingDirectory "$stage/system32" -ArgumentList $arguments -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/result.txt" -RedirectStandardError "$out/error.txt"
 $handle=$p.Handle
 if(!$p.WaitForExit(780000)){$p.Kill();throw 'Style chain observer deadline'}
 if($p.ExitCode -ne 0){throw "Style chain observer exit=$($p.ExitCode)"}
 foreach($arch in @('x86','x64')) {
  $rows=Get-Content "$stage/tests/STYLE$arch.exe.policy.txt"
  $expected=if($Mode -eq 'dos-leaf'){1}else{2}
  foreach($policy in 0..2){
   if(@($rows|Where-Object {$_ -match "policy=$policy complete"}).Count -ne $expected){
    throw "Missing child completion witnesses: $arch policy=$policy"
   }
  }
 }
 '0'|Set-Content "$out/exit.txt"
}catch{$_|Out-String|Set-Content "$out/failure.txt";'1'|Set-Content "$out/exit.txt";exit 1}
finally {
 if($stage){
  foreach($f in @(Get-ChildItem "$stage/tests" -Filter '*.exe.*.txt')){Copy-Item $f.FullName $out}
 }
 if($scope){Stop-IsolatedPackageScope $scope}
 if($stage){Remove-PhysicalRuntimeStage $stage}
}
