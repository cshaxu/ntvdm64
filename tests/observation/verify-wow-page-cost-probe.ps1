[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,
 [string]$CompilerDriver='O:\repos.hobby\ntvdm64\build\M0-T445\S6\provider\msvc-x86.cmd')
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned fixture root required'}
$null=New-Item -ItemType Directory $root
function Owned($name,$exe,$line,$seconds){
 $p=Start-Process $exe -ArgumentList $line -WindowStyle Hidden -WorkingDirectory $root -PassThru -RedirectStandardOutput "$root/$name.stdout.txt" -RedirectStandardError "$root/$name.stderr.txt";$null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$line;deadlineSeconds=$seconds}|ConvertTo-Json|Set-Content "$root/$name.process.json"
 if(!$p.WaitForExit($seconds*1000)){taskkill.exe /PID $p.Id /T /F;throw "Owned fixture deadline $name"}
 $p.WaitForExit();$code=$p.ExitCode;$p.Dispose();$code|Set-Content "$root/$name.exit.txt";if($code){throw "$name failed $code"}
}
try{
 Get-FileHash $PSCommandPath,$CompilerDriver,"$PSScriptRoot/wow_page_cost_fixture.c","$PSScriptRoot/wow_page_cost_data.h","$PSScriptRoot/wow_page_cost_probe.h","$PSScriptRoot/wow_page_cost_instrument.h"|Export-Csv -NoTypeInformation "$root/inputs.csv"
 Owned compile $env:ComSpec ('/d /s /c "call "'+$CompilerDriver+'" cl.exe /nologo /MT /O2 /W4 /D_CRT_SECURE_NO_WARNINGS /Fo"'+$root+'/fixture.obj" /Fe"'+$root+'/ntvdm.exe" "'+$PSScriptRoot+'/wow_page_cost_fixture.c" /link kernel32.lib"') 120
 Owned fixture "$root/ntvdm.exe" '--control' 15
 if((Get-Content "$root/fixture.stdout.txt" -Raw) -notmatch 'PASS nested inclusive counters, LastError, real read-only snapshot and five rejected diagnostic identities/states'){throw 'Missing complete observer control marker'}
 foreach($input in Import-Csv "$root/inputs.csv"){if((Get-FileHash $input.Path).Hash -ne $input.Hash){throw 'Frozen fixture input changed'}}
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
