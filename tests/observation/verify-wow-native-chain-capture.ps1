[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,
 [Parameter(Mandatory)][string]$CompilerDriver,[switch]$Reacquire)
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
function Body([string]$Text,[string]$Name){
 $found=[regex]::Matches($Text,'(?m)^(?:static )?(?:BOOL|ULONG \*|HWND)\s*(?:WINAPI\s+)?'+[regex]::Escape($Name)+'\([^;{}]*\)\s*\{')
 if($found.Count -ne 1){throw "One body required: $Name"}
 $at=$found[0].Index+$found[0].Length;$depth=1
 while($depth -and $at -lt $Text.Length){if($Text[$at] -eq '{'){++$depth}elseif($Text[$at] -eq '}'){--$depth};++$at}
 if($depth){throw "Unbalanced body: $Name"}
 $Text.Substring($found[0].Index,$at-$found[0].Index)
}
try{
 $owner="$repo/src/wow32-dll/source/wow_user_object_bindings.c"
 $source=[IO.File]::ReadAllText($owner)
 $begin=$source.IndexOf('static ULONG *observation_edge(')
 $end=$source.IndexOf('static BOOL project_graph_attempt(',$begin)
 if($begin -lt 0 -or $end -le $begin){throw 'Exact body bounds absent'}
 [IO.File]::WriteAllText("$out/chain_exact_body.h",$source.Substring($begin,$end-$begin))
 $fixture="$PSScriptRoot/wow_native_chain_capture_fixture.c"
 if($Reacquire){
  $names=@('wow_user_window_sample_links','capture_window','put_observation','observation_edge','normalize_observation_chains','project_graph_attempt','project_graph')
  $current=@($names|ForEach-Object {Body $source $_}) -join "`n"
  $old=(& git -C $repo show '4b8a7c8ae6eaaca3350d640650dd9d57201fd581:src/wow32-dll/source/wow_user_object_bindings.c') -join "`n"
  if($LASTEXITCODE){throw 'Immutable counterfactual missing'}
  $baseline=@($names|Where-Object {$_ -ne 'project_graph_attempt'}|ForEach-Object {Body $old $_}) -join "`n"
  foreach($name in $names){$baseline=[regex]::Replace($baseline,('\b'+$name+'\b'),('baseline_'+$name))}
  [IO.File]::WriteAllText("$out/capture_exact_body.h",$current+"`n"+$baseline)
  $fixture="$PSScriptRoot/wow_native_capture_reacquire_fixture.c"
 }
 & $CompilerDriver cl.exe /nologo /TC /O2 /MT /W4 /WX "/I$out" $fixture "/Fo$out/control.obj" "/Fe$out/control.exe" /link user32.lib *> "$out/build.log"
 if($LASTEXITCODE){throw 'Native counterexample build failed'}
 & "$out/control.exe" *> "$out/result.txt"
 if($LASTEXITCODE){throw 'Native counterexample failed'}
 $text=Get-Content "$out/result.txt" -Raw
 if($Reacquire){
  if($text -notmatch 'CAPTURE_REACQUIRE cases=21 errors=0 production-cause-proven=0'){throw 'Incomplete capture transaction matrix'}
 }else{
  foreach($edge in 0..2){
   if($text -notmatch "(?m)^edge=$edge same-pid=1 valid-native-interleave=1 mixed-rejected=1 error=13 live-accepted=1\r?`$"){throw "Missing native relationship witness $edge"}
  }
  if($text -notmatch 'NATIVE_CHAIN_CONTROL errors=0 production-cause-proven=0'){throw 'Missing evidence boundary'}
 }
 Get-FileHash $owner,$fixture,$PSCommandPath,$CompilerDriver,"$out/chain_exact_body.h","$out/control.exe","$out/result.txt"|Export-Csv -NoTypeInformation "$out/inputs.csv"
 0|Set-Content "$out/exit.txt"
}catch{$_|Out-String|Set-Content "$out/failure.txt";1|Set-Content "$out/exit.txt";throw}
