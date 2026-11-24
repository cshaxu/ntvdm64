[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot)
# Exact project_graph/put/normalization body; lower services are stand-ins.
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'));$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned evidence required'}
$null=New-Item -ItemType Directory $root
function Owned($name,$exe,$line,$seconds){
 $job=Start-Process $exe -ArgumentList $line -WorkingDirectory $root -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/$name.stdout.txt" -RedirectStandardError "$root/$name.stderr.txt";$null=$job.Handle
 @{pid=$job.Id;start=$job.StartTime.ToUniversalTime().ToString('o');command="$exe $line";deadlineSeconds=$seconds}|ConvertTo-Json|Set-Content "$root/$name.process.json"
 if(!$job.WaitForExit($seconds*1000)){taskkill.exe /PID $job.Id /T /F;throw "Owned root-body deadline $name"}
 $job.WaitForExit();$code=$job.ExitCode;$job.Dispose();$code|Set-Content "$root/$name.exit.txt";return $code
}
try{
 & "$repo/tools/build-fixtures.cmd" x86 wow-capture-required-root *> "$root/build.log"
 if($LASTEXITCODE){throw 'Native required-root contract build failed'}
 $native="$repo/build/.ninja/fixtures-x86"
 $sources=@("$repo/src/compat/wow_user_object_bindings.c","$repo/src/compat/wow_window_capture.c")
 $fixture="$PSScriptRoot/wow_required_root_capture_fixture.c";$header="$native/required_root_capture_body.h"
 Get-FileHash ($sources+@($fixture,$header,$PSCommandPath))|Export-Csv -NoTypeInformation "$root/inputs.csv"
 Copy-Item "$native/wow-capture-required-root.exe" "$root/unit.exe"
 $bytes=[IO.File]::ReadAllBytes("$root/unit.exe");$pe=[BitConverter]::ToInt32($bytes,60)
 if([BitConverter]::ToUInt16($bytes,$pe+4) -ne 0x14c){throw 'MSVC Win32/x86 required'}
 Get-FileHash "$root/unit.exe"|Export-Csv -NoTypeInformation "$root/artifacts.csv"
 $code=Owned body "$root/unit.exe" '' 30
 foreach($row in Import-Csv "$root/inputs.csv"){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw 'Frozen body input changed'}}
 $code|Set-Content "$root/exit.txt"
 if($code){exit $code}
 if((Get-Content "$root/body.stdout.txt" -Raw) -notmatch 'REQUIRED_ROOT_CAPTURE errors=0 '){throw 'Body success marker missing'}
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
