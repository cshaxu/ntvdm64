[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$DriverImage,
 [Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$InstallSource,
 [Parameter(Mandatory)][string]$InstallRoot,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$OutputRoot,
 [string]$RetirementDriver,
 [switch]$Reuse
)
$ErrorActionPreference='Stop'
if($Reuse -and $RetirementDriver){throw 'Reuse and forced retirement are separate witnesses'}
$repo=(Resolve-Path "$PSScriptRoot/../../..").Path
$task=[IO.Path]::GetFullPath($OutputRoot)
$source=(Resolve-Path -LiteralPath $InstallSource).Path.TrimEnd('\')
$dest=[IO.Path]::GetFullPath($InstallRoot).TrimEnd('\')
$DriverImage=(Resolve-Path -LiteralPath $DriverImage).Path
$PackageRoot=(Resolve-Path -LiteralPath $PackageRoot).Path
$Observer=(Resolve-Path -LiteralPath $Observer).Path
if(!$task.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $task)){throw 'Fresh repository build output required'}
if(!$dest.StartsWith('O:\tmp\',[StringComparison]::OrdinalIgnoreCase) -or $dest -match '[^\x21-\x7e]' -or (Test-Path $dest) -or $dest.StartsWith($source+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Fresh, short, independent O:/tmp guest copy required'}
if(!(Test-Path "$source/WIN.COM") -or !(Test-Path "$source/SYSTEM.INI")){throw 'Installed Windows 3.1 source required'}
New-Item -ItemType Directory $task|Out-Null
# This is a background integration runner. It uses the established matrix
# desktop/scope and normal guest shutdown, never foreground input automation.
$original=@(Get-ChildItem $source -Force -Recurse -File | ForEach-Object {[ordered]@{path=$_.FullName.Substring($source.Length);sha256=(Get-FileHash $_.FullName).Hash}})
$original|ConvertTo-Json|Set-Content "$task/original.json"
New-Item -ItemType Directory $dest|Out-Null
Get-ChildItem $source -Force|Copy-Item -Destination $dest -Recurse
[IO.File]::WriteAllText("$dest/.t479-owner",$task)
foreach($file in Get-ChildItem $dest -Recurse -File | Where-Object {$_.Extension -in '.INI','.CMD','.NT'}) {
 $enc=[Text.Encoding]::GetEncoding(28591)
 $s=$enc.GetString([IO.File]::ReadAllBytes($file.FullName))
 $s=[regex]::Replace($s,[regex]::Escape($source),{param($m) $dest},'IgnoreCase')
 [IO.File]::WriteAllBytes($file.FullName,$enc.GetBytes($s))
}
foreach($file in Get-ChildItem $dest -Recurse -Filter '*.PIF'){
 & "$repo/assets/release/PIF.EXE" replace-root $file.FullName --from $source --to $dest
 if($LASTEXITCODE){throw 'PIF rebase failed'}
}
foreach($file in Get-ChildItem $dest -Filter '*.GRP'){
 & "$repo/assets/release/GRP.EXE" replace-root $file.FullName --root $dest
 if($LASTEXITCODE){throw 'GRP rebase failed'}
}
$system="$dest/SYSTEM.INI"
$enc=[Text.Encoding]::GetEncoding(28591)
$s=$enc.GetString([IO.File]::ReadAllBytes($system))
if([regex]::Matches($s,'(?im)^\[386Enh\]').Count -ne 1){throw 'No unique enhanced section'}
$s=[regex]::Replace($s,'(?im)^\[386Enh\]','[386Enh]' + "`r`ndevice=$dest\PATCH\NTVDM64.386")
[IO.File]::WriteAllBytes($system,$enc.GetBytes($s))
Copy-Item -LiteralPath $DriverImage -Destination "$dest/PATCH/NTVDM64.386"
if($RetirementDriver){
 Copy-Item -LiteralPath $RetirementDriver -Destination "$dest/PATCH/RETIRE.386"
 $s=[regex]::Replace($s,'(?im)^\[386Enh\]','[386Enh]' + "`r`ndevice=$dest\PATCH\RETIRE.386")
 [IO.File]::WriteAllBytes($system,$enc.GetBytes($s))
}

$dosWitness=if($RetirementDriver){'dos-retirement.asm'}else{'dos-identity.asm'}
& nasm -f bin "$PSScriptRoot/$dosWitness" -o "$task/identity.com"
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
 [IO.File]::WriteAllBytes("$dest/PATCH/$id.COM",$bytes)
 & "$repo/assets/release/PIF.EXE" create "$dest/PATCH/$id.PIF" --title "VM identity $id" --program "$dest\PATCH\$id.COM" --directory "$dest\PATCH" --arguments '' --config '' --autoexec '' --close-on-exit --w386-flags 0x21002
 if($LASTEXITCODE){throw 'DOS VM PIF build failed'}
}
$ini="$dest/WIN.INI"
$s=$enc.GetString([IO.File]::ReadAllBytes($ini))
$s=[regex]::Replace($s,'(?im)^load=[^\r\n]*',"load=$dest\PATCH\A.PIF $dest\PATCH\B.PIF")
[IO.File]::WriteAllBytes($ini,$enc.GetBytes($s))

$defines=@()
if($Reuse){
 & nasm -f bin "$PSScriptRoot/dos-reuse.asm" -o "$dest/PATCH/REUSE.COM"
 if($LASTEXITCODE){throw 'Reuse witness assembly failed'}
 & "$repo/assets/release/PIF.EXE" create "$dest/PATCH/REUSE.PIF" --title 'VM reuse' --program "$dest\PATCH\REUSE.COM" --directory "$dest\PATCH" --arguments '' --config '' --autoexec '' --close-on-exit --w386-flags 0x21002
 if($LASTEXITCODE){throw 'Reuse PIF build failed'}
 $defines=@('-DREUSE=1')
}
& nasm -f bin @defines "$PSScriptRoot/win31-identity.asm" -o "$task/controller.exe"
if($LASTEXITCODE){throw 'Win16 controller build failed'}
$bytes=[IO.File]::ReadAllBytes("$task/controller.exe")
$fields=@(@('@VM_API_PATH@',"$dest\PATCH\VMLOAD.BIN"),@('@VM_A_PATH@',"$dest\PATCH\A.BIN"),@('@VM_B_PATH@',"$dest\PATCH\B.BIN"))
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
$keep=@('SystemRoot','WINDIR','COMSPEC','PATHEXT','SYSTEMDRIVE','NUMBER_OF_PROCESSORS','PROCESSOR_ARCHITECTURE','PROCESSOR_IDENTIFIER','OS')
$nasm=(Get-Command nasm.exe -CommandType Application | Select-Object -First 1).Source
Get-ChildItem Env:|Where-Object {$_.Name -notin $keep}|ForEach-Object{Remove-Item -LiteralPath ('Env:'+ $_.Name)}
$env:PATH=(Split-Path $nasm)+';C:\Windows\System32;C:\Windows';$env:TEMP='O:\tmp';$env:TMP='O:\tmp'
try {
 & "$repo/test/integration/win31_runtime_matrix_test.ps1" -PackageRoot $PackageRoot -InstallRoot $dest -Observer $Observer -OutputRoot "$task/runtime" -WindowsProbeImage "$task/controller.exe" -TimeoutMs 90000 -Cases 386-dosonly1
 if($LASTEXITCODE){throw 'Guest matrix runner failed'}
 if(!(Test-Path "$dest/PATCH/VMLOAD.BIN")){throw 'No guest API result'}
 $b=[IO.File]::ReadAllBytes("$dest/PATCH/VMLOAD.BIN")
 Copy-Item "$dest/PATCH/VMLOAD.BIN" "$task/runtime/VMLOAD.BIN"
 $result=[ordered]@{api=[BitConverter]::ToUInt32($b,0);signature=[BitConverter]::ToUInt32($b,4);vm=[BitConverter]::ToUInt32($b,8);driver=(Get-FileHash "$dest/PATCH/NTVDM64.386").Hash}
 $result|ConvertTo-Json|Set-Content "$task/runtime/vxd-result.json"
 if($result.signature -ne 0x4e544d56 -or !$result.vm){throw 'VxD API identity did not pass'}

 if($RetirementDriver){
  $rows=@(foreach($id in 'A','B'){
   $data=[IO.File]::ReadAllBytes("$dest/PATCH/$id.BIN")
   Copy-Item "$dest/PATCH/$id.BIN" "$task/runtime/$id.BIN"
   if($data.Length -ne 28 -or [Text.Encoding]::ASCII.GetString($data,0,4) -ne 'VRET'){throw 'Invalid retirement witness output'}
   [pscustomobject]@{name=$id;vm=[BitConverter]::ToUInt32($data,4);generation=[BitConverter]::ToUInt64($data,8);error=[BitConverter]::ToUInt32($data,16);blocked=[BitConverter]::ToUInt32($data,20);rejected=[BitConverter]::ToUInt32($data,24)}
  })
  $rows|ConvertTo-Json|Set-Content "$task/runtime/vm-retirement.json"
  if($rows[0].error -or $rows[1].error -or !$rows[0].vm -or !$rows[1].vm -or $rows[0].vm -eq $rows[1].vm -or $rows[1].blocked -ne 0x400 -or $rows[1].rejected -ne 1){throw 'Forced retirement/stale wake failed'}
  'PASS forced retirement of blocked VM, rejected late wake and normal Windows exit'
  return
 }

 $rows=@(foreach($id in 'A','B'){
  $data=[IO.File]::ReadAllBytes("$dest/PATCH/$id.BIN")
  Copy-Item "$dest/PATCH/$id.BIN" "$task/runtime/$id.BIN"
  if($data.Length -ne 72 -or [Text.Encoding]::ASCII.GetString($data,0,4) -ne 'VMID'){throw 'Invalid witness output'}
  $row=[ordered]@{name=$id;signature=[BitConverter]::ToUInt32($data,4);vm=[BitConverter]::ToUInt32($data,8);generation=[BitConverter]::ToUInt64($data,12);status=[BitConverter]::ToUInt16($data,20);negativeStatus=[BitConverter]::ToUInt16($data,22);negativeResult=[BitConverter]::ToUInt32($data,24);start=[BitConverter]::ToUInt32($data,28);finish=[BitConverter]::ToUInt32($data,32)}
  if($row.signature -ne 0x4e544d56 -or !$row.vm -or !$row.generation -or ($row.status -band 1) -or !($row.negativeStatus -band 1) -or $row.negativeResult -ne 1){throw "Identity/negative API failed: $id"}
  $row.callback=[BitConverter]::ToUInt32($data,36)
  $row.callbackCount=[BitConverter]::ToUInt16($data,40)
  $row.callbackErrors=[BitConverter]::ToUInt16($data,42)
  if(!$row.callback -or $row.callbackCount -ne 160 -or $row.callbackErrors){throw "Callback identity/stack/register/flags failed: $id"}
  $row.waitCount=[BitConverter]::ToUInt16($data,44)
  $row.waitErrors=[BitConverter]::ToUInt16($data,46)
  if($row.waitCount -ne 3 -or $row.waitErrors){throw "Counted precompletion/stale generation failed: $id"}
  $row.pairCount=[BitConverter]::ToUInt16($data,48)
  $row.pairErrors=[BitConverter]::ToUInt16($data,50)
  if($row.pairCount -ne 1 -or $row.pairErrors){throw "Blocked cross-VM wait/wake failed: $id"}
  $row.notificationPublished=[BitConverter]::ToUInt64($data,52)
  $row.notificationSeen=[BitConverter]::ToUInt64($data,60)
  $row.notificationIrq=[BitConverter]::ToUInt16($data,68)
  $row.notificationErrors=[BitConverter]::ToUInt16($data,70)
  if(!$row.notificationPublished -or $row.notificationSeen -lt $row.notificationPublished -or $row.notificationIrq -gt 15 -or $row.notificationErrors){throw "Actual PIC/VPICD notification failed: $id"}
  [pscustomobject]$row
 })
 $rows|ConvertTo-Json|Set-Content "$task/runtime/vm-pair.json"
 if($rows[0].vm -eq $rows[1].vm -or $rows[0].generation -eq $rows[1].generation -or [Math]::Max($rows[0].start,$rows[1].start) -ge [Math]::Min($rows[0].finish,$rows[1].finish)){throw 'Distinct concurrent VM identity not proved'}
 'PASS two concurrent DOS VMs, distinct identity/generation, negative API and 320 callback identity/stack/register/flags checks'
 if($Reuse){
  $b=[IO.File]::ReadAllBytes("$dest/PATCH/REUSE.BIN")
  Copy-Item "$dest/PATCH/REUSE.BIN" "$task/runtime/REUSE.BIN"
  if($b.Length -ne 36 -or [Text.Encoding]::ASCII.GetString($b,0,4) -ne 'VRUS'){throw 'Invalid reuse report'}
  $reuseResult=[ordered]@{vm=[BitConverter]::ToUInt32($b,4);generation=[BitConverter]::ToUInt64($b,8);oldGeneration=[BitConverter]::ToUInt64($b,16);error=[BitConverter]::ToUInt32($b,24);rejected=[BitConverter]::ToUInt32($b,28);consumed=[BitConverter]::ToUInt32($b,32)}
  $reuseResult|ConvertTo-Json|Set-Content "$task/runtime/vm-reuse.json"
  $previous=@($rows|Where-Object{$_.vm -eq $reuseResult.vm -and $_.generation -eq $reuseResult.oldGeneration})
  if($reuseResult.error -or $previous.Count -ne 1 -or $reuseResult.generation -le $reuseResult.oldGeneration -or $reuseResult.rejected -ne 2 -or $reuseResult.consumed -ne 1){throw 'Actual VMM handle reuse or stale generation rejection not proved'}
  'PASS actual VMM handle reuse, old direct/queued wake rejection and new counted completion'
 }

} finally {
 if(@(Get-ChildItem $source -Force -Recurse -File).Count -ne $original.Count){throw 'Original installation file inventory changed'}
 foreach($row in $original){if((Get-FileHash ($source+$row.path)).Hash -ne $row.sha256){throw "Original changed: $($row.path)"}}
 'Original Windows installation bytes verified unchanged'
}
