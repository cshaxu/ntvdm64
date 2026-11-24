[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$BuildRoot,
 [ValidateSet('All','ClassCopy','NativeWaitPublication')][string]$Case='All'
)
# Exact-source-body controls only. Invoke in an owned hidden background run.
# Lower-service stand-ins do not prove original scheduling/CCPU/guest behavior.
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh repository build-owned evidence required'}
$null=New-Item -ItemType Directory $root
function Owned($name,$exe,$line,$seconds){
 if($line){
  $p=Start-Process $exe -ArgumentList $line -WindowStyle Hidden -WorkingDirectory $root -PassThru -RedirectStandardOutput "$root/$name.stdout.txt" -RedirectStandardError "$root/$name.stderr.txt"
 }else{
  $p=Start-Process $exe -WindowStyle Hidden -WorkingDirectory $root -PassThru -RedirectStandardOutput "$root/$name.stdout.txt" -RedirectStandardError "$root/$name.stderr.txt"
 };$null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command="$exe $line";deadlineSeconds=$seconds}|ConvertTo-Json|Set-Content "$root/$name.process.json"
 if(!$p.WaitForExit($seconds*1000)){taskkill.exe /PID $p.Id /T /F;throw "Owned source-body deadline $name"}
 $p.WaitForExit();$code=$p.ExitCode;$p.Dispose();$code|Set-Content "$root/$name.exit.txt";return $code
}
try{
 & "$repo/tools/build-fixtures.cmd" x86 wow-return-unwind *> "$root/build.log"
 if($LASTEXITCODE){throw 'Native return unwind build failed'}
 $native="$repo/build/.ninja/fixtures-x86"
 $cases=if($Case -eq 'All'){@('ClassCopy','NativeWaitPublication')}else{@($Case)}
 $results=@()
 foreach($selected in $cases){
  $dir="$root/$selected";$null=New-Item -ItemType Directory $dir
  if($selected -eq 'ClassCopy'){
   $source="$repo/src/compat/mvdm_softpc_wow_page_domain.c"
   $fixture="$PSScriptRoot/wow_class_copy_unwind_fixture.c";$header='class_copy_body.h';$target='class_copy'
   $marker='CLASS_COPY_BODY_UNWIND errors=0 '
  }else{
   $source="$repo/src/compat/wow_user_task_lifecycle.c"
   $fixture="$PSScriptRoot/wow_native_wait_publication_fixture.c";$header='native_wait_publication_body.h';$target='native_wait_publication'
   $marker='NATIVE_WAIT_PUBLICATION_BODY errors=0 '
  }
  Get-FileHash $source,$fixture,"$native/$header",$PSCommandPath|Export-Csv -NoTypeInformation "$dir/inputs.csv"
  Copy-Item "$native/wow-unwind-$target.exe" "$dir/unit.exe"
  $bytes=[IO.File]::ReadAllBytes("$dir/unit.exe");$pe=[BitConverter]::ToInt32($bytes,60)
  if([BitConverter]::ToUInt16($bytes,$pe+4) -ne 0x14c){throw 'Source-body fixture must be MSVC Win32/x86'}
  Get-FileHash "$dir/unit.exe"|Export-Csv -NoTypeInformation "$dir/artifacts.csv"
  $code=Owned $selected "$dir/unit.exe" '' 30
  $results+=[pscustomobject]@{case=$selected;exit=$code;source=$source;scope='Exact helper/control-flow body, not scheduler/CCPU/guest acceptance'}
  $results|ConvertTo-Json|Set-Content "$root/results.json"
  foreach($inputRow in Import-Csv "$dir/inputs.csv"){if((Get-FileHash $inputRow.Path).Hash -ne $inputRow.Hash){throw 'Frozen source-body input changed'}}
  if($code){$code|Set-Content "$root/exit.txt";exit $code}
  if((Get-Content "$root/$selected.stdout.txt" -Raw) -notmatch [regex]::Escape($marker)){throw 'Missing exact positive body marker'}
 }
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
