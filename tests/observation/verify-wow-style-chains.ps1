param(
 [Parameter(Mandatory)][string]$RuntimeRoot,
 [Parameter(Mandatory)][string]$FixtureRoot,
 [Parameter(Mandatory)][string]$GuestFixtureRoot,
 [Parameter(Mandatory)][string]$BuildRoot,
 [string[]]$Architectures=@('x86','x64'),
 [string[]]$Variants=@('separate-off','separate-on','separate-all','separate-a-off','separate-a-on','separate-a-all','relay-off','relay-on','relay-all'),
 [switch]$Capture,
 [switch]$ActivateDesktop
)
$ErrorActionPreference='Stop'
Import-Module Microsoft.PowerShell.Utility
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($BuildRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh build-owned output required'}
$null=New-Item -ItemType Directory $out
$runtime=(Resolve-Path $RuntimeRoot).Path;$fixtures=(Resolve-Path $FixtureRoot).Path;$guests=(Resolve-Path $GuestFixtureRoot).Path
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$stage=$null;$scope=$null
try {
 @"
@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MT /O2 /W4 /WX "$repo/tests/observation/wow_style_isolation_test.c" /Fo"$out/observer.obj" /Fe"$out/observer.exe" /link user32.lib gdi32.lib dwmapi.lib
"@ | Set-Content "$out/build.cmd" -Encoding ascii
 & $env:ComSpec /d /c "$out/build.cmd" *> "$out/build.txt"
 if($LASTEXITCODE){throw 'Observer build failed'}
 $stage=New-PhysicalRuntimeStage $runtime;$scope=New-IsolatedPackageScope $stage
 $null=New-Item -ItemType Directory -Force "$stage/tests"
 $stage|Set-Content "$out/stage.txt"
 $keep=@('SystemRoot','WINDIR','ComSpec','SystemDrive','USERPROFILE','USERNAME','USERDOMAIN','LOCALAPPDATA','APPDATA','ProgramData','ProgramFiles','ProgramFiles(x86)','ProgramW6432','CommonProgramFiles','CommonProgramFiles(x86)','CommonProgramW6432','NUMBER_OF_PROCESSORS','PROCESSOR_ARCHITECTURE','OS','PATHEXT','PSExecutionPolicyPreference')
 foreach($item in @(Get-ChildItem Env:)){if($item.Name -notin $keep){Remove-Item -LiteralPath ('Env:'+$item.Name)}}
 $env:TEMP='O:\tmp';$env:TMP='O:\tmp';$env:PSModulePath=Join-Path $PSHOME Modules
 $env:PATH=$PSHOME+';'+$env:SystemRoot+'\System32;'+$env:SystemRoot
 foreach($arch in $Architectures){
  if($arch -notin @('x86','x64')){throw 'Invalid fixture architecture'}
  Copy-Item "$fixtures/$arch/system32/native-title-gui.exe" "$stage/tests/STYLENAT.EXE" -Force
  $scope.Paths+=(Resolve-Path "$stage/tests/STYLENAT.EXE").Path
  foreach($variant in $Variants){
   if($variant -notmatch '^(ui(-off|-all)?|off-on|on-off|(separate(-a)?|relay|child)-(off|on|all))$'){throw 'Invalid variant'}
   $case="$out/$arch-$variant";$null=New-Item -ItemType Directory $case
   $parent=if($variant.StartsWith('child-')){'STYLEPAR.EXE'}else{'STYLERET.EXE'}
   Copy-Item "$guests/$parent" "$stage/tests/STYLEPAR.EXE" -Force
   if($Capture){$env:MVDM_STYLE_CAPTURE_DIR=$case}
   if($ActivateDesktop){$env:MVDM_STYLE_ACTIVATE_DESKTOP='1'}
   $p=Start-Process "$out/observer.exe" -ArgumentList ('"'+$stage+'" '+$variant) -WindowStyle Hidden -PassThru -RedirectStandardOutput "$case/result.txt" -RedirectStandardError "$case/error.txt"
   $null=$p.Handle
   if(!$p.WaitForExit(180000)){$p.Kill();$p.WaitForExit();throw "Observer deadline $arch/$variant"}
   $p.WaitForExit();if($p.ExitCode -ne 0){throw "Observer failed $arch/$variant exit=$($p.ExitCode)"}
   Stop-IsolatedPackageScope $scope
  }
 }
 '0'|Set-Content "$out/exit.txt"
}catch{$_|Out-String|Set-Content "$out/failure.txt";'1'|Set-Content "$out/exit.txt";exit 1}
finally{if($scope){Stop-IsolatedPackageScope $scope};if($stage){Remove-PhysicalRuntimeStage $stage}}
