[CmdletBinding()]
param([Parameter(Mandatory)][string]$WorkerBuildRoot,
 [Parameter(Mandatory)][string]$CompilerDriver,
 [Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned evidence required'}
$null=New-Item -ItemType Directory $root
$worker=(Resolve-Path $WorkerBuildRoot).Path
$mapping="$repo/src/ntvdm-exe/softpc/mvdm_softpc_physical_mapping.c"
$fixture="$repo/tests/adapter-mvdm-host-out/physical_alias_empty_fixture.c"
function Owned($name,$exe,$line,$seconds){
 $p=Start-Process $exe -ArgumentList $line -WindowStyle Hidden -WorkingDirectory $root -PassThru -RedirectStandardOutput "$root/$name.stdout.txt" -RedirectStandardError "$root/$name.stderr.txt";$null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');image=$exe;command=$line;timeoutSeconds=$seconds}|ConvertTo-Json|Set-Content "$root/$name.process.json"
 if(!$p.WaitForExit($seconds*1000)){taskkill.exe /PID $p.Id /T /F;throw "Owned deadline $name"}
 $p.WaitForExit();$code=$p.ExitCode;$p.Dispose();$code|Set-Content "$root/$name.exit.txt";if($code){throw "$name failed $code"}
}
try{
 # Snapshot the exact selected production object/archive before mutable roots
 # can be rebuilt. Never substitute a successful mock mapping implementation.
 Copy-Item "$worker/obj/adapter-softpc/mvdm_softpc_physical_mapping.obj" "$root/physical-mapping.obj"
 Copy-Item "$worker/session.lib" "$root/session.lib"
 Copy-Item $mapping "$root/mapping-source.c"
 Get-FileHash $PSCommandPath,$fixture,"$repo/tests/adapter-mvdm-host-out/physical_mapping_observation_test.c",$mapping,"$worker/build.ninja",$CompilerDriver,"$root/physical-mapping.obj","$root/session.lib"|Export-Csv -NoTypeInformation "$root/inputs.csv"
 $line='/d /s /c "call "'+$CompilerDriver+'" cl.exe /nologo /c /MT /O2 /W4 /WX /I"'+$repo+'/src" /I"'+$repo+'/src/ntvdm-exe/softpc/include" /Fo"'+$root+'/fixture.obj" "'+$fixture+'""'
 Owned compile $env:ComSpec $line 120
 $line='/d /s /c "call "'+$CompilerDriver+'" link.exe /nologo /MACHINE:X86 /OPT:REF /OUT:"'+$root+'/fixture.exe" "'+$root+'/fixture.obj" "'+$root+'/physical-mapping.obj" "'+$root+'/session.lib" kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib legacy_stdio_definitions.lib"'
 Owned link $env:ComSpec $line 120
 $bytes=[IO.File]::ReadAllBytes("$root/fixture.exe");$pe=[BitConverter]::ToInt32($bytes,60)
 if([BitConverter]::ToUInt16($bytes,$pe+4) -ne 0x14c){throw 'Require x86 production ABI'}
 Owned fixture "$root/fixture.exe" '--verify' 30
 $text=Get-Content "$root/fixture.stdout.txt" -Raw
 if($text -notmatch 'PHYSICAL_ALIAS_EMPTY_CONTRACT errors=0 empty=1 null=1 invalid_owner=1 bounds=1 multi_owner=1 remap=1 teardown=1 physical_retained=1' -or $text -notmatch 'EMPTY_ALIAS_BENCH iterations=2000000 .*invalid_results=0'){throw 'Missing complete contract or benchmark'}
 foreach($row in Import-Csv "$root/inputs.csv"){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw 'Input changed'}}
 'Supplementary actual adapter/session contract and microbenchmark; not CCPU/guest or whole-performance acceptance'|Set-Content "$root/limits.txt"
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
