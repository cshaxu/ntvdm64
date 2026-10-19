[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,
 [string]$CompilerDriver=(Join-Path $PSScriptRoot '../../build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd'))
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'));$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned fixture root required'}
$null=New-Item -ItemType Directory $root
function Owned($name,$exe,$line,$seconds){
 $p=Start-Process $exe -ArgumentList $line -WindowStyle Hidden -WorkingDirectory $root -PassThru -RedirectStandardOutput "$root/$name.stdout.txt" -RedirectStandardError "$root/$name.stderr.txt";$null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$line;deadlineSeconds=$seconds}|ConvertTo-Json|Set-Content "$root/$name.process.json"
 if(!$p.WaitForExit($seconds*1000)){taskkill.exe /PID $p.Id /T /F;throw "Owned boundary deadline $name"}
 $p.WaitForExit();$code=$p.ExitCode;$p.Dispose();$code|Set-Content "$root/$name.exit.txt";if($code){throw "$name failed $code"}
}
try{
 $source="$repo/tests/adapter-mvdm-host-out/wow/wow_hook_native_binding_fixture.c"
 Get-FileHash $source,$PSCommandPath,$CompilerDriver,"$repo/src/wow32-dll/source/wow_hook_native_binding.c","$repo/src/wow32-dll/include/wow_hook_native_binding.h"|Export-Csv -NoTypeInformation "$root/inputs.csv"
 Owned compile $env:ComSpec ('/d /s /c "call "'+$CompilerDriver+'" cl.exe /nologo /MT /O2 /W4 /I"'+$repo+'/src" /I"'+$repo+'/src/wow32-dll/include" /Fo"'+$root+'/fixture.obj" /Fe"'+$root+'/fixture.exe" "'+$source+'" /link kernel32.lib"') 120
 Owned fixture "$root/fixture.exe" '--boundary' 30
 if((Get-Content "$root/fixture.stdout.txt" -Raw) -notmatch 'WOW_HOOK_NATIVE_BOUNDARY errors=0 calls=24 local=17 global=1 invalid=1 null=1 mismatch=1 callback=1 dead=1 foreign=1 handles=([0-9]+)/\1'){throw 'Incomplete hook input/handle/error proof'}
 foreach($row in Import-Csv "$root/inputs.csv"){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw 'Frozen boundary input changed'}}
 'Unit argument seam only; real native/USER16 hook execution remains required'|Set-Content "$root/limits.txt"
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
