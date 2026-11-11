[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,
 [Parameter(Mandatory)][string]$CompilerDriver)
# Pure native USER32 reference; no WOW provider, guest or product acceptance.
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'));$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned reference root required'}
$CompilerDriver=(Resolve-Path $CompilerDriver).Path
$null=New-Item -ItemType Directory $root
function Owned($name,$exe,$line,$seconds){
 $job=Start-Process $exe -ArgumentList $line -WorkingDirectory $root -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/$name.stdout.txt" -RedirectStandardError "$root/$name.stderr.txt";$null=$job.Handle
 @{pid=$job.Id;start=$job.StartTime.ToUniversalTime().ToString('o');command="$exe $line";deadlineSeconds=$seconds}|ConvertTo-Json|Set-Content "$root/$name.process.json"
 if(!$job.WaitForExit($seconds*1000)){taskkill.exe /PID $job.Id /T /F;throw "Owned native reference deadline $name"}
 $job.WaitForExit();$code=$job.ExitCode;$job.Dispose();$code|Set-Content "$root/$name.exit.txt";if($code){throw "$name failed $code"}
}
try{
 $source="$PSScriptRoot/wow_native_queue_order_reference.c"
 Get-FileHash $source,$CompilerDriver,$PSCommandPath|Export-Csv -NoTypeInformation "$root/inputs.csv"
 Owned compile $env:ComSpec ('/d /s /c "call "'+$CompilerDriver+'" cl.exe /nologo /TC /MT /O2 /W4 /WX "'+$source+'" /Fo"'+$root+'/reference.obj" /Fe"'+$root+'/reference.exe" /link user32.lib"') 120
 $bytes=[IO.File]::ReadAllBytes("$root/reference.exe");$pe=[BitConverter]::ToInt32($bytes,60)
 if([BitConverter]::ToUInt16($bytes,$pe+4) -ne 0x14c){throw 'MSVC Win32/x86 reference required'}
 Owned body "$root/reference.exe" '' 30
 $output=Get-Content "$root/body.stdout.txt"
 foreach($wide in 0..1){foreach($producer in 0..1){
  if(!($output -match ('^NATIVE_QUEUE_REFERENCE wide='+$wide+' post_quit='+$producer+' ordinary_before_quit=1 sentinel=1 bound=64$'))){throw 'One native A/W producer mode was not proved'}
 }}
 if(!($output -match '^NATIVE_QUEUE_REFERENCE errors=0 old_first_call_quit_assumption_rejected=4$')){throw 'Native order/reference completion marker missing'}
 foreach($row in Import-Csv "$root/inputs.csv"){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw 'Frozen native reference source changed'}}
 Get-FileHash "$root/reference.exe","$root/reference.obj"|Export-Csv -NoTypeInformation "$root/artifacts.csv"
 @{nativeOnly=$true;encodings=2;quitProducers=2;ordinaryBeforeQuit=$true;guestUsed=$false;productAcceptance=$false}|ConvertTo-Json|Set-Content "$root/result.json"
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
