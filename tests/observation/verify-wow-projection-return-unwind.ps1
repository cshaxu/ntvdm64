[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$BuildRoot,
 [Parameter(Mandatory)][string]$CompilerDriver,
 [ValidateSet('All','ClassCopy','NativeWaitPublication')][string]$Case='All',
 [string]$SourceFile
)
# Exact-source-body controls only. Invoke in an owned hidden background run.
# Lower-service stand-ins do not prove original scheduling/CCPU/guest behavior.
# A historical source may be selected for a negative; its real exit propagates.
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh repository build-owned evidence required'}
if($SourceFile -and $Case -eq 'All'){throw 'An explicit source requires one named case'}
if($SourceFile){
 $SourceFile=(Resolve-Path -LiteralPath $SourceFile).Path
 if(!$SourceFile.StartsWith($repo+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Only project source or its owned sealed copy may be selected'}
}
$CompilerDriver=(Resolve-Path -LiteralPath $CompilerDriver).Path
$null=New-Item -ItemType Directory $root
function Owned($name,$exe,$line,$seconds){
 $p=Start-Process $exe -ArgumentList $line -WindowStyle Hidden -WorkingDirectory $root -PassThru -RedirectStandardOutput "$root/$name.stdout.txt" -RedirectStandardError "$root/$name.stderr.txt";$null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command="$exe $line";deadlineSeconds=$seconds}|ConvertTo-Json|Set-Content "$root/$name.process.json"
 if(!$p.WaitForExit($seconds*1000)){taskkill.exe /PID $p.Id /T /F;throw "Owned source-body deadline $name"}
 $p.WaitForExit();$code=$p.ExitCode;$p.Dispose();$code|Set-Content "$root/$name.exit.txt";return $code
}
function Region($text,$start,$end){
 $at=$text.IndexOf($start);if($at -lt 0){throw "Exact body start missing: $start"}
 $last=$text.IndexOf($end,$at);if($last -le $at){throw "Exact body end missing: $end"}
 return $text.Substring($at,$last-$at)
}
try{
 $cases=if($Case -eq 'All'){@('ClassCopy','NativeWaitPublication')}else{@($Case)}
 $results=@()
 foreach($selected in $cases){
  $dir="$root/$selected";$null=New-Item -ItemType Directory $dir
  if($selected -eq 'ClassCopy'){
   $source=if($SourceFile){$SourceFile}else{"$repo/src/ntvdm-exe/softpc/mvdm_softpc_wow_page_domain.c"}
   $fixture="$PSScriptRoot/wow_class_copy_unwind_fixture.c";$header='class_copy_body.h'
   $text=[IO.File]::ReadAllText($source).Replace("`r`n","`n")
   $body=(Region $text 'static int __cdecl compare_class_reference(' 'int mvdm_softpc_wow_page_domain_collect_observed_classes(')+(Region $text 'static int copy_current_window_view(' 'int mvdm_softpc_wow_page_domain_copy_window_view(')
   $marker='CLASS_COPY_BODY_UNWIND errors=0 '
  }else{
   $source=if($SourceFile){$SourceFile}else{"$repo/src/wow32-dll/source/wow_user_task_lifecycle.c"}
   $fixture="$PSScriptRoot/wow_native_wait_publication_fixture.c";$header='native_wait_publication_body.h'
   $text=[IO.File]::ReadAllText($source).Replace("`r`n","`n")
   $body=Region $text 'static BOOL publish_guest_return(' 'static BOOL native_call_end('
   $marker='NATIVE_WAIT_PUBLICATION_BODY errors=0 '
  }
  [IO.File]::WriteAllText("$dir/$header",$body,[Text.UTF8Encoding]::new($false))
  Get-FileHash $source,$fixture,"$dir/$header",$CompilerDriver,$PSCommandPath|Export-Csv -NoTypeInformation "$dir/inputs.csv"
  $line='/d /s /c "call "'+$CompilerDriver+'" cl.exe /nologo /TC /O2 /MT /W4 /WX /I"'+$repo+'/src" /I"'+$dir+'" "'+$fixture+'" /Fo"'+$dir+'/unit.obj" /Fe"'+$dir+'/unit.exe" /link user32.lib"'
  $code=Owned ($selected+'-compile') $env:ComSpec $line 120
  if($code){throw "$selected compile failed $code"}
  $bytes=[IO.File]::ReadAllBytes("$dir/unit.exe");$pe=[BitConverter]::ToInt32($bytes,60)
  if([BitConverter]::ToUInt16($bytes,$pe+4) -ne 0x14c){throw 'Source-body fixture must be MSVC Win32/x86'}
  Get-FileHash "$dir/unit.exe","$dir/unit.obj"|Export-Csv -NoTypeInformation "$dir/artifacts.csv"
  $code=Owned $selected "$dir/unit.exe" '' 30
  $results+=[pscustomobject]@{case=$selected;exit=$code;source=$source;scope='Exact helper/control-flow body, not scheduler/CCPU/guest acceptance'}
  $results|ConvertTo-Json|Set-Content "$root/results.json"
  foreach($inputRow in Import-Csv "$dir/inputs.csv"){if((Get-FileHash $inputRow.Path).Hash -ne $inputRow.Hash){throw 'Frozen source-body input changed'}}
  if($code){$code|Set-Content "$root/exit.txt";exit $code}
  if((Get-Content "$root/$selected.stdout.txt" -Raw) -notmatch [regex]::Escape($marker)){throw 'Missing exact positive body marker'}
 }
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
