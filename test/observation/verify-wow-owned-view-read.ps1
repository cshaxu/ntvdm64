[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'));$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned exact-body evidence required'}
$SourceFile="$repo/src/compat/mvdm_softpc_wow_page_domain.c"
$null=New-Item -ItemType Directory $root
function Owned($name,$exe,$line,$seconds){
 $job=Start-Process $exe -ArgumentList $line -WorkingDirectory $root -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/$name.stdout.txt" -RedirectStandardError "$root/$name.stderr.txt";$null=$job.Handle
 @{pid=$job.Id;start=$job.StartTime.ToUniversalTime().ToString('o');command="$exe $line";deadlineSeconds=$seconds}|ConvertTo-Json|Set-Content "$root/$name.process.json"
 if(!$job.WaitForExit($seconds*1000)){taskkill.exe /PID $job.Id /T /F;throw "Owned read-body deadline $name"}
 $job.WaitForExit();$code=$job.ExitCode;$job.Dispose();$code|Set-Content "$root/$name.exit.txt";if($code){throw "$name failed $code"}
}
try{
 & "$repo/tools/build-fixtures.cmd" x86 wow-owned-read *> "$root/build.log"
 if($LASTEXITCODE){throw 'Native owned read build failed'}
 $native="$repo/build/.ninja/fixtures-x86"
 $header="$native/owned_view_read_body.h";$fixture="$PSScriptRoot/wow_owned_view_read_fixture.c"
 Get-FileHash $SourceFile,$fixture,$header,$PSCommandPath|Export-Csv -NoTypeInformation "$root/inputs.csv"
 Copy-Item "$native/wow-owned-read.exe" "$root/unit.exe"
 $bytes=[IO.File]::ReadAllBytes("$root/unit.exe");$pe=[BitConverter]::ToInt32($bytes,60)
 if([BitConverter]::ToUInt16($bytes,$pe+4) -ne 0x14c){throw 'MSVC Win32/x86 required'}
 $mode='fast'
 Owned body "$root/unit.exe" '--fast' 30
 if((Get-Content "$root/body.stdout.txt" -Raw) -notmatch ('OWNED_VIEW_READ errors=0 selected='+$mode+' ')){throw 'Complete read control marker missing'}
 foreach($row in Import-Csv "$root/inputs.csv"){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw 'Frozen exact body changed'}}
 Get-FileHash "$root/unit.exe"|Export-Csv -NoTypeInformation "$root/artifacts.csv"
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
