[CmdletBinding()]
param([Parameter(Mandatory)][string]$CompilerDriver,
 [Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned root required'}
$null=New-Item -ItemType Directory $root
$driver=(Resolve-Path $CompilerDriver).Path
function Owned($name,$image,$line,$seconds,$expected=0){
 $p=Start-Process $image -ArgumentList $line -WindowStyle Hidden -WorkingDirectory $root -PassThru -RedirectStandardOutput "$root/$name.stdout.txt" -RedirectStandardError "$root/$name.stderr.txt";$null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');image=$image;command=$line;timeoutSeconds=$seconds}|ConvertTo-Json|Set-Content "$root/$name.process.json"
 try{
  if(!$p.WaitForExit($seconds*1000)){taskkill.exe /PID $p.Id /T /F;throw "Owned $name deadline"}
  $p.WaitForExit();$p.ExitCode|Set-Content "$root/$name.exit.txt"
  if($p.ExitCode -ne $expected){throw "$name returned $($p.ExitCode), expected $expected"}
 }finally{$p.Dispose()}
}
try{
 $fixture="$repo/tests/adapter-mvdm-host-out/wow/wow_input_lifetime_fixture.c"
 $original="$repo/src/opennt-host/windows/core/ntuser/kernel/input.c"
 Get-FileHash $PSCommandPath,$driver,$fixture,$original,
  "$repo/src/wow32-dll/source/wow_input_group_lifetime.c",
  "$repo/src/wow32-dll/include/wow_input_group_lifetime.h",
  "$repo/src/wow32-dll/include/wow_input_group_bindings.h",
  "$repo/src/wow32-dll/include/wow_task_order_bindings.h"|Export-Csv -NoTypeInformation "$root/inputs.csv"
 foreach($spec in @(@('fixture',$fixture),@('original-input',$original))){
  $line='/d /s /c ""'+$driver+'" cl.exe /nologo /c /MT /O2 /Gz /W4 /WX /I"'+$repo+'/src" /I"'+$repo+'/src/wow32-dll/include" /Fo"'+$root+'/'+$spec[0]+'.obj" "'+$spec[1]+'""'
  Owned ('compile-'+$spec[0]) $env:ComSpec $line 120
 }
 $line='/d /s /c ""'+$driver+'" link.exe /nologo /MACHINE:X86 /OPT:REF /OUT:"'+$root+'/fixture.exe" "'+$root+'/fixture.obj" "'+$root+'/original-input.obj" kernel32.lib user32.lib"'
 Owned link $env:ComSpec $line 120
 Owned membership "$root/fixture.exe" '--verify' 60
 $positive=Get-Content "$root/membership.stderr.txt" -Raw
 if($positive -notmatch 'INPUT_NATIVE_CONTROL own_Z=128 before_group=1'){throw 'Native keyboard control unavailable before grouping'}
 if($positive -notmatch 'WOW_INPUT_LIFETIME errors=0 original_components=1 private_native=1 allocation_failures=3 idempotent=1 external_reference=1 origin_exit=1 rejoin=1 middle_exit=1 last_exit=1 scratch_escape=0 live=0'){throw 'Missing complete membership/lifetime witness'}
 Owned terminal "$root/fixture.exe" '--native-failure' 60 91
 if((Get-Content "$root/terminal.stderr.txt" -Raw) -notmatch 'INPUT_TERMINAL stage=input-native-attach error=5 attaches=3'){throw 'Partial native mutation did not reach the terminal boundary'}
 foreach($row in Import-Csv "$root/inputs.csv"){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw 'Input changed during group'}}
 'Supplementary original component + test-owned native attachment/keyboard-state test on the current desktop; no visible window, external thread attachment or SendInput. Failure callback is an explicit test seam, not proof of production fail-fast implementation; actual lifecycle/guest and private-desktop focus regressions remain required.'|Set-Content "$root/limits.txt"
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
