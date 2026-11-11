[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,
 [Parameter(Mandatory)][string]$CompilerDriver,[string]$SourceFile)
# Exact project_graph/put/normalization body; lower services are stand-ins.
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'));$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned evidence required'}
if(!$SourceFile){$SourceFile="$repo/src/wow32-dll/source/wow_user_object_bindings.c"}
$SourceFile=(Resolve-Path $SourceFile).Path;$CompilerDriver=(Resolve-Path $CompilerDriver).Path
if(!$SourceFile.StartsWith($repo+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Project or sealed source required'}
$null=New-Item -ItemType Directory $root
function Owned($name,$exe,$line,$seconds){
 $job=Start-Process $exe -ArgumentList $line -WorkingDirectory $root -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/$name.stdout.txt" -RedirectStandardError "$root/$name.stderr.txt";$null=$job.Handle
 @{pid=$job.Id;start=$job.StartTime.ToUniversalTime().ToString('o');command="$exe $line";deadlineSeconds=$seconds}|ConvertTo-Json|Set-Content "$root/$name.process.json"
 if(!$job.WaitForExit($seconds*1000)){taskkill.exe /PID $job.Id /T /F;throw "Owned root-body deadline $name"}
 $job.WaitForExit();$code=$job.ExitCode;$job.Dispose();$code|Set-Content "$root/$name.exit.txt";return $code
}
try{
 $text=[IO.File]::ReadAllText($SourceFile).Replace("`r`n","`n")
 $start=$text.IndexOf('static BOOL put_observation(');$end=$text.IndexOf('BOOL WINAPI wow_user_window_project_graph(',$start)
 if($start -lt 0 -or $end -le $start){throw 'Exact transaction body boundaries missing'}
 $header="$root/required_root_capture_body.h";$fixture="$PSScriptRoot/wow_required_root_capture_fixture.c"
 [IO.File]::WriteAllText($header,$text.Substring($start,$end-$start),[Text.UTF8Encoding]::new($false))
 Get-FileHash $SourceFile,$fixture,$header,$CompilerDriver,$PSCommandPath|Export-Csv -NoTypeInformation "$root/inputs.csv"
 $line='/d /s /c "call "'+$CompilerDriver+'" cl.exe /nologo /TC /O2 /MT /W4 /WX /I"'+$repo+'/src" /I"'+$root+'" "'+$fixture+'" /Fo"'+$root+'/unit.obj" /Fe"'+$root+'/unit.exe" /link user32.lib"'
 if((Owned compile $env:ComSpec $line 120)){throw 'Exact root-body fixture compile failed'}
 $bytes=[IO.File]::ReadAllBytes("$root/unit.exe");$pe=[BitConverter]::ToInt32($bytes,60)
 if([BitConverter]::ToUInt16($bytes,$pe+4) -ne 0x14c){throw 'MSVC Win32/x86 required'}
 Get-FileHash "$root/unit.exe","$root/unit.obj"|Export-Csv -NoTypeInformation "$root/artifacts.csv"
 $code=Owned body "$root/unit.exe" '' 30
 foreach($row in Import-Csv "$root/inputs.csv"){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw 'Frozen body input changed'}}
 $code|Set-Content "$root/exit.txt"
 if($code){exit $code}
 if((Get-Content "$root/body.stdout.txt" -Raw) -notmatch 'REQUIRED_ROOT_CAPTURE errors=0 '){throw 'Body success marker missing'}
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
