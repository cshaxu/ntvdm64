[CmdletBinding()]
param([Parameter(Mandatory)][string]$CompilerDriver,
 [Parameter(Mandatory)][string]$BuildRoot)
# Existing native/original controls. Not WOW task or guest acceptance.
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned root required'}
$null=New-Item -ItemType Directory $root
$driver=(Resolve-Path $CompilerDriver).Path
function Owned($name,$exe,$line,$seconds){
 $p=Start-Process $exe -ArgumentList $line -WindowStyle Hidden -WorkingDirectory $root -PassThru -RedirectStandardOutput "$root/$name.stdout.txt" -RedirectStandardError "$root/$name.stderr.txt";$null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');image=$exe;command=$line;timeoutSeconds=$seconds}|ConvertTo-Json|Set-Content "$root/$name.process.json"
 try{
  if(!$p.WaitForExit($seconds*1000)){taskkill.exe /PID $p.Id /T /F;throw "Owned $name deadline"}
  $p.WaitForExit();$p.ExitCode|Set-Content "$root/$name.exit.txt";if($p.ExitCode){throw "$name failed $($p.ExitCode)"}
 }finally{$p.Dispose()}
}
try{
 $original="$repo/src/opennt-host/windows/core/ntuser/kernel/input.c"
 $sources=@('wow_native_input_group_fixture','wow_native_input_focus_fixture','wow_input_membership_fixture')
 $inputs=@($PSCommandPath,$driver,$original,"$repo/src/wow32-dll/include/wow_input_group_bindings.h","$repo/src/wow32-dll/include/wow_task_order_bindings.h")
 $inputs+=@($sources|ForEach-Object {"$repo/tests/adapter-mvdm-host-out/wow/$_.c"})
 Get-FileHash $inputs|Export-Csv -NoTypeInformation "$root/inputs.csv"
 $flags='/nologo /c /MT /O2 /Gz /W4 /I"'+$repo+'/src/wow32-dll/include"'
 Owned original-compile $env:ComSpec ('/d /s /c ""'+$driver+'" cl.exe '+$flags+' /Fo"'+$root+'/original.obj" "'+$original+'""') 120
 foreach($name in $sources){
  $source="$repo/tests/adapter-mvdm-host-out/wow/$name.c"
  Owned ($name+'-compile') $env:ComSpec ('/d /s /c ""'+$driver+'" cl.exe '+$flags+' /Fo"'+$root+'/'+$name+'.obj" "'+$source+'""') 120
  $extra=if($name -eq 'wow_input_membership_fixture'){' "'+$root+'/original.obj"'}else{''}
  Owned ($name+'-link') $env:ComSpec ('/d /s /c ""'+$driver+'" link.exe /nologo /MACHINE:X86 /OPT:REF /OUT:"'+$root+'/'+$name+'.exe" "'+$root+'/'+$name+'.obj"'+$extra+' kernel32.lib user32.lib"') 120
  Owned $name "$root/$name.exe" '--verify' 30
 }
 if((Get-Content "$root/wow_native_input_group_fixture.stdout.txt" -Raw) -notmatch 'WOW_NATIVE_INPUT_GROUP errors=0 .*naive_join_split=1 maintained_join=1 repeated=1'){throw 'Missing native group/counterexample witness'}
 if((Get-Content "$root/wow_native_input_focus_fixture.stdout.txt" -Raw) -notmatch 'WOW_NATIVE_INPUT_FOCUS errors=0 private_desktop=1 capture=1 callbacks=0'){throw 'Missing private native focus/capture/callback witness'}
 if((Get-Content "$root/wow_input_membership_fixture.stdout.txt" -Raw) -notmatch 'WOW_INPUT_MEMBERSHIP errors=0 .*queue_selection=1 allocation_failure=1 partial_failure=1'){throw 'Missing original grouping/allocation-path witness'}
 foreach($row in Import-Csv "$root/inputs.csv"){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw 'Input changed'}}
 'Supplementary native services/original grouping, not production lifecycle, guest acceptance or universal NT4 foreground precedence. Keyboard fixture touches only its own thread tables; focus/capture fixture owns an invisible private desktop.'|Set-Content "$root/limits.txt"
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
