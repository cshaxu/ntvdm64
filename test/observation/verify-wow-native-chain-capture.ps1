[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,
 [switch]$Reacquire)
# Invoke as an owned background process. No product package, guest, RPC or
# visible desktop operation; native windows exist only on an isolated desktop.
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($BuildRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
 $out.StartsWith($repo+'\build\.ninja\',[StringComparison]::OrdinalIgnoreCase) -or
 $out.StartsWith($repo+'\build\.ccache\',[StringComparison]::OrdinalIgnoreCase) -or
 (Test-Path $out)){throw 'Fresh disposable evidence directory required'}
$null=New-Item -ItemType Directory $out
try{
 $owner="$repo/src/compat/wow_user_object_bindings.c"
 $captureOwner="$repo/src/compat/wow_window_capture.c"
 & "$repo/tools/build-fixtures.cmd" x86 wow-capture-contracts *> "$out/build.log"
 if($LASTEXITCODE){throw 'Native capture contract build failed'}
 $native="$repo/build/.ninja/fixtures-x86"
 $kind=if($Reacquire){'reacquire'}else{'chain'}
 $fixture=if($Reacquire){"$PSScriptRoot/wow_native_capture_reacquire_fixture.c"}else{"$PSScriptRoot/wow_native_chain_capture_fixture.c"}
 Copy-Item "$native/wow-capture-$kind.exe" "$out/control.exe"
 $p=Start-Process "$out/control.exe" -WorkingDirectory $out -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/result.txt" -RedirectStandardError "$out/error.txt"
 $null=$p.Handle
 @{pid=$p.Id;started=$p.StartTime.ToUniversalTime().ToString('o');deadlineSeconds=30;exe="$out/control.exe"}|ConvertTo-Json|Set-Content "$out/process.json"
 if(!$p.WaitForExit(30000)){& taskkill.exe /PID $p.Id /T /F;throw 'Owned capture deadline'}
 $p.WaitForExit();$p.ExitCode|Set-Content "$out/process.exit"
 if($p.ExitCode){throw 'Native capture contract failed'}
 $text=Get-Content "$out/result.txt" -Raw
 if($Reacquire){
  if($text -notmatch 'CAPTURE_REACQUIRE cases=15 errors=0 production-cause-proven=0'){throw 'Incomplete capture transaction matrix'}
 }else{
  foreach($edge in 0..2){
   if($text -notmatch "(?m)^edge=$edge same-pid=1 valid-native-interleave=1 mixed-rejected=1 error=13 live-accepted=1\r?`$"){throw "Missing native relationship witness $edge"}
  }
  if($text -notmatch 'NATIVE_CHAIN_CONTROL errors=0 production-cause-proven=0'){throw 'Missing evidence boundary'}
 }
 Get-FileHash $owner,$captureOwner,$fixture,$PSCommandPath,"$native/chain_exact_body.h","$native/capture_exact_body.h","$out/control.exe","$out/result.txt"|Export-Csv -NoTypeInformation "$out/inputs.csv"
 0|Set-Content "$out/exit.txt"
}catch{$_|Out-String|Set-Content "$out/failure.txt";1|Set-Content "$out/exit.txt";throw}
