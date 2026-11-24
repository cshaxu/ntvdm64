[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'));$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned boundary evidence required'}
$null=New-Item -ItemType Directory $root
function Owned($name,$exe,$line,$seconds){
 $start=@{FilePath=$exe;WorkingDirectory=$root;WindowStyle='Hidden';PassThru=$true;RedirectStandardOutput="$root/$name.stdout.txt";RedirectStandardError="$root/$name.stderr.txt"}
 if($line){$start.ArgumentList=$line}
 $job=Start-Process @start;$null=$job.Handle
 @{pid=$job.Id;start=$job.StartTime.ToUniversalTime().ToString('o');command="$exe $line";deadlineSeconds=$seconds}|ConvertTo-Json|Set-Content "$root/$name.process.json"
 if(!$job.WaitForExit($seconds*1000)){taskkill.exe /PID $job.Id /T /F;throw "Owned boundary deadline $name"}
 $job.WaitForExit();$code=$job.ExitCode;$job.Dispose();$code|Set-Content "$root/$name.exit.txt";if($code){throw "$name failed $code"}
}
try{
 $source="$repo/src/compat/wow_common_dialog_calls.c";$fixture="$PSScriptRoot/wow_common_dialog_calls_fixture.c"
 & "$repo/tools/build-fixtures.cmd" x86 wow-common-dialog *> "$root/build.log"
 if($LASTEXITCODE){throw 'Native common dialog build failed'}
 $native="$repo/build/.ninja/fixtures-x86"
 $header="$native/common_dialog_exact_body.h"
 Get-FileHash $source,$fixture,$header,$PSCommandPath|Export-Csv -NoTypeInformation "$root/inputs.csv"
 Copy-Item "$native/wow-common-dialog-boundary.exe" "$root/unit.exe"
 Copy-Item "$native/wow-common-dialog-reference.exe" "$root/native.exe"
 Owned body "$root/unit.exe" '' 30
 if((Get-Content "$root/body.stdout.txt" -Raw) -notmatch 'COMMON_DIALOG_BOUNDARIES errors=0 leaves=7 scenarios=42 guest_proof=0'){throw 'Complete boundary proof missing'}
 $reference="$PSScriptRoot/wow_native_common_dialog_reference.c"
 Get-FileHash $reference|Export-Csv -NoTypeInformation "$root/native-reference-input.csv"
 Owned native-reference "$root/native.exe" '' 30
 if((Get-Content "$root/native-reference.stdout.txt" -Raw) -notmatch 'NATIVE_COMMDLG_REFERENCE_RESULT errors=0 modes=2'){throw 'Native standard-template reference failed'}
 foreach($row in Import-Csv "$root/inputs.csv"){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw 'Frozen boundary input changed'}}
 Get-FileHash "$root/unit.exe","$root/native.exe"|Export-Csv -NoTypeInformation "$root/artifacts.csv"
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
