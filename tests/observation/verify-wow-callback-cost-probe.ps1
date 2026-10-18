[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,
 [string]$CompilerDriver='O:\repos.hobby\ntvdm64\build\M0-T445\S6\provider\msvc-x86.cmd')
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'));$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned diagnostic unit required'}
$null=New-Item -ItemType Directory $root
function Owned($name,$exe,$line,$seconds){
 $p=Start-Process $exe -ArgumentList $line -WindowStyle Hidden -WorkingDirectory $root -PassThru -RedirectStandardOutput "$root/$name.stdout.txt" -RedirectStandardError "$root/$name.stderr.txt";$null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$line;deadlineSeconds=$seconds}|ConvertTo-Json|Set-Content "$root/$name.process.json"
 if(!$p.WaitForExit($seconds*1000)){taskkill.exe /PID $p.Id /T /F;throw "Owned callback diagnostic unit deadline $name"}
 $p.WaitForExit();$code=$p.ExitCode;$p.Dispose();$code|Set-Content "$root/$name.exit.txt";if($code){throw "$name failed $code"}
}
try{
 $inputs=@($PSCommandPath,$CompilerDriver,"$PSScriptRoot/wow_callback_cost_fixture.c","$PSScriptRoot/wow_callback_cost_data.h","$PSScriptRoot/wow_callback_cost_instrument.h","$PSScriptRoot/wow_callback_cost_dispatch.h","$PSScriptRoot/wow_callback_cost_probe.h")
 Get-FileHash $inputs|Export-Csv -NoTypeInformation "$root/inputs.csv"
 Owned compile $env:ComSpec ('/d /s /c "call "'+$CompilerDriver+'" cl.exe /nologo /MT /O2 /W4 /D_CRT_SECURE_NO_WARNINGS /Fo"'+$root+'/fixture.obj" /Fe"'+$root+'/fixture.exe" "'+$PSScriptRoot+'/wow_callback_cost_fixture.c" /link kernel32.lib"') 120
 Owned fixture "$root/fixture.exe" '--control' 30
 if((Get-Content "$root/fixture.stdout.txt" -Raw) -notmatch 'CALLBACK_COST_UNIT errors=0 nested=1 exact_result_error=1 rejection=1 exception=1 untracked=1 readonly=1 overflow=1 negative_states=9 negative_identities=3 handles=([0-9]+)/\1 phase_tls=1'){throw 'Incomplete callback recorder/reader controls'}
 foreach($row in Import-Csv "$root/inputs.csv"){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw 'Frozen diagnostic unit source changed'}}
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
