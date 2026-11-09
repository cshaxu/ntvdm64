[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$DriverImage,
 [Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$InstallSource,
 [Parameter(Mandatory)][string]$InstallRoot,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$OutputRoot,
 [switch]$Reuse,
 [switch]$InitializationFailure,
 [switch]$NativeCommand,
 [switch]$InstallViaTool
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../../..").Path
$PackageRoot=(Resolve-Path -LiteralPath $PackageRoot).Path
$Observer=(Resolve-Path -LiteralPath $Observer).Path
if($InstallViaTool){
 $sourceIni=[IO.File]::ReadAllText((Resolve-Path -LiteralPath "$InstallSource/SYSTEM.INI"))
 if($sourceIni -match '(?im)^\s*device\s*=.*(?:ntvdm64\.(?:386|vxd)|insignia\.386)'){
  throw 'Installer witness requires a source without multitask driver entries'
 }
 if((Get-FileHash $DriverImage).Hash -ne (Get-FileHash "$repo/assets/release/ntvdm64.386").Hash){
  throw 'Installer witness requires the released driver image'
 }
}
$copy=& "$PSScriptRoot/New-Win95ProbeCopy.ps1" -DriverImage $DriverImage -InstallSource $InstallSource -InstallRoot $InstallRoot -OutputRoot $OutputRoot
$task=$copy.Task;$source=$copy.Source;$dest=$copy.Destination;$original=$copy.Original
$enc=[Text.Encoding]::GetEncoding(28591)
if($InstallViaTool){
 # The copy helper stages this driver itself. Remove only its exact entry and
 # file to test the actual installer on an otherwise prepared isolated copy.
 $system="$dest/SYSTEM.INI";$text=$enc.GetString([IO.File]::ReadAllBytes($system))
 $line='(?im)^device='+[regex]::Escape("$dest\PATCH\NTVDM64.386")+'\r?\n'
 if([regex]::Matches($text,$line).Count -ne 1){throw 'Expected one copy-helper driver entry'}
 $text=[regex]::Replace($text,$line,'')
 [IO.File]::WriteAllBytes($system,$enc.GetBytes($text))
 Remove-Item -LiteralPath "$dest/PATCH/NTVDM64.386"
 & "$repo/tools/win-multitask/APPLY.CMD" $dest
 if($LASTEXITCODE){throw "Multitask installation failed: $LASTEXITCODE"}
 foreach($name in 'NTVDM64.386','INSIGNIA.386'){
  if((Get-FileHash "$dest/PATCH/$name").Hash -ne (Get-FileHash "$repo/assets/release/$name").Hash){throw "Installed driver differs: $name"}
 }
}
if(($Reuse -and $InitializationFailure) -or ($NativeCommand -and ($Reuse -or $InitializationFailure))){throw 'Native command, reuse and initialization failure are separate witnesses'}

if(!$InitializationFailure){

$dosDefines=@('-DWIN95_ORDERED_PROOF=1')
if($NativeCommand){$dosDefines+=@('-DCONTINUATION=1','-DREAL_COMMAND=1','-DPRODUCTION_NATIVE_ONLY=1')}
& nasm -f bin @dosDefines "$PSScriptRoot/dos-identity.asm" -o "$task/identity.com"
if($LASTEXITCODE){throw 'DOS identity witness build failed'}
foreach($id in 'A','B'){
 $bytes=[IO.File]::ReadAllBytes("$task/identity.com")
 $marker=[Text.Encoding]::ASCII.GetString($bytes).IndexOf('@VM_IDENTITY_RESULT@')
 if($marker -lt 0){throw 'No output marker'}
 [Array]::Clear($bytes,$marker,128)
 $path=[Text.Encoding]::ASCII.GetBytes("$dest\PATCH\$id.BIN")
 [Array]::Copy($path,0,$bytes,$marker,$path.Length)
 $marker=[Text.Encoding]::ASCII.GetString($bytes).IndexOf('@VM_PAIR_READY@')
 if($marker -lt 0){throw 'No pair rendezvous marker'}
 [Array]::Clear($bytes,$marker,128)
 $path=[Text.Encoding]::ASCII.GetBytes("$dest\PATCH\PAIR.KEY")
 [Array]::Copy($path,0,$bytes,$marker,$path.Length)
 $marker=[Text.Encoding]::ASCII.GetString($bytes).IndexOf('@VM_ROLE@')
 if($marker -lt 0){throw 'No VM role marker'}
 $bytes[$marker]=[byte][char]$id
 if($NativeCommand){
  foreach($field in @(@('@VM_ENVIRONMENT@',"ROLE=$id`0ROUND=0`0PATH=$PackageRoot\system32;C:\Windows\System32;C:\Windows`0`0",512),@('@VM_DIRECTORY@',"$dest\PATCH",128))){
   $marker=[Text.Encoding]::ASCII.GetString($bytes).IndexOf($field[0])
   $value=[Text.Encoding]::ASCII.GetBytes($field[1])
   if($marker -lt 0 -or $value.Length -ge $field[2]){throw 'Invalid native command field'}
   [Array]::Clear($bytes,$marker,$field[2]);[Array]::Copy($value,0,$bytes,$marker,$value.Length)
  }
 }
 $marker=[Text.Encoding]::ASCII.GetString($bytes).IndexOf('@VM_A_RESULT@')
 if($marker -lt 0){throw 'No peer result marker'}
 [Array]::Clear($bytes,$marker,128)
 $path=[Text.Encoding]::ASCII.GetBytes("$dest\PATCH\A.BIN")
 [Array]::Copy($path,0,$bytes,$marker,$path.Length)
 [IO.File]::WriteAllBytes("$dest/PATCH/$id.COM",$bytes)
 & "$repo/assets/release/PIF.EXE" create "$dest/PATCH/$id.PIF" --title "VM identity $id" --program "$dest\PATCH\$id.COM" --directory "$dest\PATCH" --arguments '' --config '' --autoexec '' --close-on-exit --w386-flags 0x21002
 if($LASTEXITCODE){throw 'DOS VM PIF build failed'}
 & "$PSScriptRoot/Set-Win95ProbePifMemory.ps1" -Path "$dest/PATCH/$id.PIF"
}
$ini="$dest/WIN.INI"
$s=$enc.GetString([IO.File]::ReadAllBytes($ini))
$s=[regex]::Replace($s,'(?im)^load=[^\r\n]*',"load=$dest\PATCH\A.PIF $dest\PATCH\B.PIF")
[IO.File]::WriteAllBytes($ini,$enc.GetBytes($s))

}
else {
 $ini="$dest/WIN.INI"
 $s=$enc.GetString([IO.File]::ReadAllBytes($ini))
 $s=[regex]::Replace($s,'(?im)^load=[^\r\n]*','load=')
 [IO.File]::WriteAllBytes($ini,$enc.GetBytes($s))
}
$defines=@()
if($InitializationFailure){$defines=@('-DINIT_FAILURE=1')}
if($Reuse){
 & nasm -f bin "$PSScriptRoot/dos-reuse.asm" -o "$dest/PATCH/REUSE.COM"
 if($LASTEXITCODE){throw 'Reuse witness assembly failed'}
 & "$repo/assets/release/PIF.EXE" create "$dest/PATCH/REUSE.PIF" --title 'VM reuse' --program "$dest\PATCH\REUSE.COM" --directory "$dest\PATCH" --arguments '' --config '' --autoexec '' --close-on-exit --w386-flags 0x21002
 if($LASTEXITCODE){throw 'Reuse PIF build failed'}
 & "$PSScriptRoot/Set-Win95ProbePifMemory.ps1" -Path "$dest/PATCH/REUSE.PIF"
 $defines=@('-DREUSE=1')
}
& nasm -f bin @defines "$PSScriptRoot/win31-identity.asm" -o "$task/controller.exe"
if($LASTEXITCODE){throw 'Win16 controller build failed'}
$bytes=[IO.File]::ReadAllBytes("$task/controller.exe")
$fields=@(@('@VM_API_PATH@',"$dest\PATCH\VMLOAD.BIN"),@('@VM_A_PATH@',"$dest\PATCH\A.BIN"),@('@VM_B_PATH@',"$dest\PATCH\B.BIN"),@('@W31_BOOT_PATH@',"$dest\PATCH\BOOT.BIN"),@('@W31_READY_PATH@',"$dest\PATCH\READY.BIN"))
if($Reuse){$fields+=@(@('@VM_REUSE_PROGRAM@',"$dest\PATCH\REUSE.PIF"),@('@VM_REUSE_RESULT@',"$dest\PATCH\REUSE.BIN"))}
foreach($field in $fields){
 $image=[Text.Encoding]::ASCII.GetString($bytes)
 $offset=$image.IndexOf($field[0],[StringComparison]::Ordinal)
 $value=[Text.Encoding]::ASCII.GetBytes($field[1])
 if($offset -lt 0 -or $image.LastIndexOf($field[0],[StringComparison]::Ordinal) -ne $offset -or $value.Length -ge 128){throw 'Invalid controller path field'}
 [Array]::Clear($bytes,$offset,128)
 [Array]::Copy($value,0,$bytes,$offset,$value.Length)
}
[IO.File]::WriteAllBytes("$task/controller.exe",$bytes)
$keep=@('SystemRoot','WINDIR','COMSPEC','PATHEXT','SYSTEMDRIVE','NUMBER_OF_PROCESSORS','PROCESSOR_ARCHITECTURE','PROCESSOR_IDENTIFIER','OS','PSExecutionPolicyPreference')
$nasm=(Get-Command nasm.exe -CommandType Application | Select-Object -First 1).Source
Get-ChildItem Env:|Where-Object {$_.Name -notin $keep}|ForEach-Object{Remove-Item -LiteralPath ('Env:'+ $_.Name)}
$env:PATH=(Split-Path $nasm)+';C:\Windows\System32;C:\Windows';$env:TEMP='O:\tmp';$env:TMP='O:\tmp'
try {

 . "$repo/tests/observation/isolated_package_cleanup.ps1"
 $stage=$null;$scope=$null;$process=$null
 New-Item -ItemType Directory "$task/runtime"|Out-Null
 Copy-Item "$task/controller.exe" "$dest/PATCH/VMTEST.EXE"
 $s=$enc.GetString([IO.File]::ReadAllBytes("$dest/WIN.INI"))
 $s=[regex]::Replace($s,'(?im)^run=[^\r\n]*',"run=$dest\PATCH\VMTEST.EXE")
 [IO.File]::WriteAllBytes("$dest/WIN.INI",$enc.GetBytes($s))
 try {
  $stage=New-PhysicalRuntimeStage $PackageRoot -FreshPackageIdentity
  $scope=New-IsolatedPackageScope $stage
  if($InitializationFailure){$env:MVDM_INIT_FAILURE_REPORT="$task/initialization-rollback.bin"}
  $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
  $env:MVDM_OBSERVER_OWN_JOB='1'
  $env:PATH="$stage\system32;C:\Windows\System32;C:\Windows"
  $process=Start-Process $Observer -ArgumentList @("$stage/system32/run16.exe",$dest,"$task/runtime/observer.txt","$dest/PATCH/WIN.PIF",'--observation-timeout-ms','90000') -WindowStyle Hidden -PassThru
  @{pid=$process.Id;start=$process.StartTime.ToUniversalTime().ToString('o');stage=$stage}|ConvertTo-Json|Set-Content "$task/runtime/process.json"
  if(!$process.WaitForExit(105000)){throw 'Owned Win95 observer deadline'}
  $process.WaitForExit()
  $process.ExitCode|Set-Content "$task/runtime/observer.exit"
 } finally {
  if($process){if(!$process.HasExited){$process.Kill($true);$process.WaitForExit()};$process.Dispose()}
  if($scope){Stop-IsolatedPackageScope $scope}
  if($stage){Remove-PhysicalRuntimeStage $stage}
 }
 if(!(Test-Path "$dest/PATCH/VMLOAD.BIN")){throw 'No guest API result'}
 $b=[IO.File]::ReadAllBytes("$dest/PATCH/VMLOAD.BIN")
 Copy-Item "$dest/PATCH/VMLOAD.BIN" "$task/runtime/VMLOAD.BIN"
 $result=[ordered]@{api=[BitConverter]::ToUInt32($b,0);signature=[BitConverter]::ToUInt32($b,4);vm=[BitConverter]::ToUInt32($b,8);driver=(Get-FileHash "$dest/PATCH/NTVDM64.386").Hash}
 $result|ConvertTo-Json|Set-Content "$task/runtime/vxd-result.json"
 if($InitializationFailure){
  $rollback=[IO.File]::ReadAllBytes("$task/initialization-rollback.bin")
  $observation=Get-Content "$task/runtime/observer.txt" -Raw
  if($rollback.Length -ne 4 -or [BitConverter]::ToUInt32($rollback,0) -ne 1 -or $result.signature -eq 0x4e544d56 -or $observation -notmatch '(?m)^result=exited\r?$'){throw 'Initialization failure, IRQ rollback or Windows exit not proved'}
  'PASS injected initialization failure, guest rollback and host IRQ/hook reacquisition; Windows process exited'
  return
 }
 if($result.signature -ne 0x4e544d56 -or !$result.vm){throw 'VxD API identity did not pass'}

 $rows=@(foreach($id in 'A','B'){
  $data=[IO.File]::ReadAllBytes("$dest/PATCH/$id.BIN")
  Copy-Item "$dest/PATCH/$id.BIN" "$task/runtime/$id.BIN"
  $expectedLength=if($NativeCommand){76}else{72}
  if($data.Length -ne $expectedLength -or [Text.Encoding]::ASCII.GetString($data,0,4) -ne 'VMID'){throw 'Invalid witness output'}
  if($NativeCommand -and ([BitConverter]::ToUInt16($data,72) -ne 2 -or [BitConverter]::ToUInt16($data,74))){throw "Real native continuation failed: $id"}
  $row=[ordered]@{name=$id;signature=[BitConverter]::ToUInt32($data,4);vm=[BitConverter]::ToUInt32($data,8);generation=[BitConverter]::ToUInt64($data,12);status=[BitConverter]::ToUInt16($data,20);negativeStatus=[BitConverter]::ToUInt16($data,22);negativeResult=[BitConverter]::ToUInt32($data,24);start=[BitConverter]::ToUInt32($data,28);finish=[BitConverter]::ToUInt32($data,32)}
  if($row.signature -ne 0x4e544d56 -or !$row.vm -or !$row.generation -or ($row.status -band 1) -or !($row.negativeStatus -band 1) -or $row.negativeResult -ne 1){throw "Identity/negative API failed: $id"}
  $row.callback=[BitConverter]::ToUInt32($data,36)
  $row.callbackCount=[BitConverter]::ToUInt16($data,40)
  $row.callbackErrors=[BitConverter]::ToUInt16($data,42)
  if(!$row.callback -or $row.callbackCount -ne 160 -or $row.callbackErrors){throw "Callback identity/stack/register/flags failed: $id"}
  if([BitConverter]::ToUInt16($data,44) -ne 3 -or [BitConverter]::ToUInt16($data,46) -ne 0 -or [BitConverter]::ToUInt16($data,48) -ne 1 -or [BitConverter]::ToUInt16($data,50) -ne 0){throw "Counted/cross-VM wait failed: $id"}
  if(![BitConverter]::ToUInt64($data,52) -or [BitConverter]::ToUInt64($data,60) -lt [BitConverter]::ToUInt64($data,52) -or [BitConverter]::ToUInt16($data,68) -gt 15 -or [BitConverter]::ToUInt16($data,70)){throw "PIC notification failed: $id"}
  [pscustomobject]$row
 })
 $rows|ConvertTo-Json|Set-Content "$task/runtime/vm-pair.json"
 if($rows[0].vm -eq $rows[1].vm -or $rows[0].generation -eq $rows[1].generation -or [Math]::Max($rows[0].start,$rows[1].start) -ge [Math]::Min($rows[0].finish,$rows[1].finish)){throw 'Distinct concurrent VM identity not proved'}
 'PASS two concurrent DOS VMs, distinct identity/generation, negative API and 320 callback identity/stack/register/flags checks'
 if($NativeCommand){'PASS four actual native commands via production host with guest return, register, flags and stack checks'}
 if($Reuse){
  $b=[IO.File]::ReadAllBytes("$dest/PATCH/REUSE.BIN");Copy-Item "$dest/PATCH/REUSE.BIN" "$task/runtime/REUSE.BIN"
  if($b.Length -ne 36 -or [Text.Encoding]::ASCII.GetString($b,0,4) -ne 'VRUS'){throw 'Invalid reuse report'}
  $r=[ordered]@{vm=[BitConverter]::ToUInt32($b,4);generation=[BitConverter]::ToUInt64($b,8);oldGeneration=[BitConverter]::ToUInt64($b,16);error=[BitConverter]::ToUInt32($b,24);rejected=[BitConverter]::ToUInt32($b,28);consumed=[BitConverter]::ToUInt32($b,32)}
  $previous=@($rows|Where-Object{$_.vm -eq $r.vm -and $_.generation -eq $r.oldGeneration})
  if($r.error -or $previous.Count -ne 1 -or $r.generation -le $r.oldGeneration -or $r.rejected -ne 2 -or $r.consumed -ne 1){throw 'Actual handle reuse or stale generation rejection not proved'}
  $r|ConvertTo-Json|Set-Content "$task/runtime/vm-reuse.json"
  'PASS actual VMM handle reuse, old direct/queued wake rejection and new counted completion'
 }
 $observation=Get-Content "$task/runtime/observer.txt" -Raw
 if($observation -notmatch '(?m)^result=exited\r?$'){throw 'Win95 observer did not see exit'}


} finally {
 if(@(Get-ChildItem $source -Force -Recurse -File).Count -ne $original.Count){throw 'Original installation file inventory changed'}
 foreach($row in $original){if((Get-FileHash ($source+$row.path)).Hash -ne $row.sha256){throw "Original changed: $($row.path)"}}
 'Original Windows installation bytes verified unchanged'
}
