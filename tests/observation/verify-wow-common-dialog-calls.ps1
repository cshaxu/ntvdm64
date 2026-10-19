[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,
 [Parameter(Mandatory)][string]$CompilerDriver)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'));$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned boundary evidence required'}
$null=New-Item -ItemType Directory $root
function Owned($name,$exe,$line,$seconds){
 $job=Start-Process $exe -ArgumentList $line -WorkingDirectory $root -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/$name.stdout.txt" -RedirectStandardError "$root/$name.stderr.txt";$null=$job.Handle
 @{pid=$job.Id;start=$job.StartTime.ToUniversalTime().ToString('o');command="$exe $line";deadlineSeconds=$seconds}|ConvertTo-Json|Set-Content "$root/$name.process.json"
 if(!$job.WaitForExit($seconds*1000)){taskkill.exe /PID $job.Id /T /F;throw "Owned boundary deadline $name"}
 $job.WaitForExit();$code=$job.ExitCode;$job.Dispose();$code|Set-Content "$root/$name.exit.txt";if($code){throw "$name failed $code"}
}
try{
 $source="$repo/src/wow32-dll/source/wow_common_dialog_calls.c";$fixture="$PSScriptRoot/wow_common_dialog_calls_fixture.c"
 $body=[regex]::Replace([IO.File]::ReadAllText($source),'(?m)^#include[^\r\n]*\r?\n','')
 $header="$root/common_dialog_exact_body.h"
 [IO.File]::WriteAllText($header,$body,[Text.UTF8Encoding]::new($false))
 Get-FileHash $source,$fixture,$header,$CompilerDriver,$PSCommandPath|Export-Csv -NoTypeInformation "$root/inputs.csv"
 Owned compile $env:ComSpec ('/d /s /c "call "'+$CompilerDriver+'" cl.exe /nologo /TC /O2 /MT /W4 /WX /I"'+$root+'" "'+$fixture+'" /Fo"'+$root+'/unit.obj" /Fe"'+$root+'/unit.exe" /link user32.lib"') 180
 Owned body "$root/unit.exe" '' 30
 if((Get-Content "$root/body.stdout.txt" -Raw) -notmatch 'COMMON_DIALOG_BOUNDARIES errors=0 leaves=7 scenarios=42 guest_proof=0'){throw 'Complete boundary proof missing'}
 $reference="$PSScriptRoot/wow_native_common_dialog_reference.c"
 Get-FileHash $reference|Export-Csv -NoTypeInformation "$root/native-reference-input.csv"
 Owned native-compile $env:ComSpec ('/d /s /c "call "'+$CompilerDriver+'" cl.exe /nologo /TC /O2 /MT /W4 /WX "'+$reference+'" /Fo"'+$root+'/native.obj" /Fe"'+$root+'/native.exe" /link user32.lib comdlg32.lib"') 180
 Owned native-reference "$root/native.exe" '' 30
 if((Get-Content "$root/native-reference.stdout.txt" -Raw) -notmatch 'NATIVE_COMMDLG_REFERENCE_RESULT errors=0 modes=2'){throw 'Native standard-template reference failed'}
 foreach($row in Import-Csv "$root/inputs.csv"){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw 'Frozen boundary input changed'}}
 Get-FileHash "$root/unit.exe","$root/unit.obj"|Export-Csv -NoTypeInformation "$root/artifacts.csv"
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
