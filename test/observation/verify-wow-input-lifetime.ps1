[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot,$repo)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned root required'}
$null=New-Item -ItemType Directory $root
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
 Owned build $env:ComSpec ('/d /s /c ""'+$repo+'/tools/build-fixtures.cmd" x86 native-input-contracts"') 300
 $native=Join-Path $repo 'build/.ninja/msvc-x86-release/wow32'
 $originalObject=(Get-Content (Join-Path $native 'input-membership-object.txt') -Raw).Trim()
 $fixture="$repo/test/adapter-mvdm-host-out/wow/wow_input_lifetime_fixture.c"
 $original="$repo/src/compat/wow_input_membership.c"
 Get-FileHash $PSCommandPath,$originalObject,$fixture,$original,"$repo/test/adapter-mvdm-host-out/wow/wow_input_queue_witness.h",
  "$repo/src/compat/wow_input_group_lifetime.c",
  "$repo/src/compat/include/wow_input_group_lifetime.h",
  "$repo/src/compat/include/wow_input_group_bindings.h",
  "$repo/src/compat/include/wow_task_order_bindings.h"|Export-Csv -NoTypeInformation "$root/inputs.csv"
 Owned membership "$native/wow_input_lifetime_fixture.exe" '--verify' 60
 $positive=Get-Content "$root/membership.stderr.txt" -Raw
 if($positive -notmatch 'INPUT_NATIVE_CONTROL private_caret=1 before_group=1'){throw 'Private caret control unavailable before grouping'}
 if($positive -notmatch 'WOW_INPUT_LIFETIME errors=0 original_components=1 private_native=1 allocation_failures=3 idempotent=1 external_reference=1 origin_exit=1 rejoin=1 middle_exit=1 last_exit=1 scratch_escape=0 live=0'){throw 'Missing complete membership/lifetime witness'}
 Owned terminal "$native/wow_input_lifetime_fixture.exe" '--native-failure' 60 91
 if((Get-Content "$root/terminal.stderr.txt" -Raw) -notmatch 'INPUT_TERMINAL stage=input-native-attach error=5 attaches=3'){throw 'Partial native mutation did not reach the terminal boundary'}
 foreach($row in Import-Csv "$root/inputs.csv"){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw 'Input changed during group'}}
 'Supplementary original component + test-owned native attachment/caret-owner test on an owned nonactive private desktop; no visible window, external thread attachment or SendInput. Failure callback is an explicit test seam, not proof of production fail-fast implementation; actual lifecycle/guest and private-desktop focus regressions remain required.'|Set-Content "$root/limits.txt"
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
