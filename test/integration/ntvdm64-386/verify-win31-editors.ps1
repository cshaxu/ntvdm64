# Real Win3.1 dual COMMAND -> host/DOS editor keyboard/menu contract.
# Run in a background modern pwsh process with a diagnostic host package.
[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$DriverImage,
 [Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$InterruptObserver,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$InstallSource,
 [Parameter(Mandatory)][string]$InstallRoot,
 [Parameter(Mandatory)][string]$OutputRoot
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../../..").Path
$PackageRoot=(Resolve-Path -LiteralPath $PackageRoot).Path
$Observer=(Resolve-Path -LiteralPath $Observer).Path
$copy=& "$PSScriptRoot/New-Win31ProbeCopy.ps1" -DriverImage $DriverImage -InstallSource $InstallSource -InstallRoot $InstallRoot -OutputRoot $OutputRoot -InterruptObserver $InterruptObserver
$task=$copy.Task; $dest=$copy.Destination
$enc=[Text.Encoding]::GetEncoding(28591)
Copy-Item -LiteralPath "$PackageRoot/system32/COMMAND.COM" -Destination "$dest/PATCH/COMMAND.COM"
if((Get-FileHash "$PackageRoot/system32/COMMAND.COM").Hash -ne (Get-FileHash "$dest/PATCH/COMMAND.COM").Hash){throw 'COMMAND copy mismatch'}
foreach($id in 'A','B'){
 & "$repo/assets/release/PIF.EXE" create "$dest/PATCH/$id.PIF" --title "Prompt probe $id" --program "$dest\PATCH\COMMAND.COM" --directory "$dest\PATCH" --arguments '' --config '' --autoexec '' --close-on-exit --w386-flags 0x21002
 if($LASTEXITCODE){throw 'PIF failed'}
 $pif=[IO.File]::ReadAllBytes("$dest/PATCH/$id.PIF");$t=[Text.Encoding]::ASCII.GetString($pif)
 $std=$t.IndexOf('MICROSOFT PIFEX');$hdr=$t.IndexOf('WINDOWS 386 3.0');$body=[BitConverter]::ToUInt16($pif,$hdr+18)
 foreach($offset in @(32,$body)){[Array]::Copy([BitConverter]::GetBytes([uint16]640),0,$pif,$offset,2)}
 foreach($offset in @(34,($body+2))){[Array]::Copy([BitConverter]::GetBytes([uint16]128),0,$pif,$offset,2)}
 $sum=0;for($at=2;$at -lt $std;++$at){$sum=($sum+$pif[$at]) -band 255};$pif[1]=[byte]$sum
 [IO.File]::WriteAllBytes("$dest/PATCH/$id.PIF",$pif)
}
$ini="$dest/WIN.INI";$s=$enc.GetString([IO.File]::ReadAllBytes($ini))
$s=[regex]::Replace($s,'(?im)^load=[^\r\n]*',"load=$dest\PATCH\A.PIF $dest\PATCH\B.PIF")
[IO.File]::WriteAllBytes($ini,$enc.GetBytes($s))
$asm=Get-Content "$repo/tests/observation/win31_startup_contract.asm" -Raw
$actions=Get-Content "$PSScriptRoot/editor-actions.inc" -Raw
$asm=$asm.Replace('push word 0                      ; DWORD reserved',$actions+"`n"+'push word 0                      ; DWORD reserved')
$asm=$asm.Replace('code_end:',(Get-Content "$PSScriptRoot/editor-routines.inc" -Raw)+"`ncode_end:")
$asm=$asm.Replace('data_end:',"timer_id: dw 0`ntimer_msg: times 18 db 0`ncompleted_count: dd 0`nwanted_count: dd 0`nread_count: dw 0`neditors_name: db '$dest\PATCH\EDITORS.BIN',0`nreaders_name: db '$dest\PATCH\READERS.BIN',0`ncompletion_name: db '$dest\PATCH\DONE.BIN',0`ninput_api: dd 0`nstamp: dw 0`nactive: dw 0`nwhich: dw 0`ntitle_a: db 'Prompt probe A',0`ntitle_b: db 'Prompt probe B',0`n"+(Get-Content "$PSScriptRoot/editor-events.inc" -Raw)+"`ndata_end:")
[IO.File]::WriteAllText("$task/controller.asm",$asm,[Text.UTF8Encoding]::new($false))
& nasm -f bin "$task/controller.asm" -o "$task/controller.exe"
if($LASTEXITCODE){throw 'Controller assembly failed'}
$savedEnvironment=@{}
Get-ChildItem Env:|ForEach-Object{$savedEnvironment[$_.Name]=$_.Value}
try {
$keep=@('SystemRoot','WINDIR','COMSPEC','PATHEXT','SYSTEMDRIVE','NUMBER_OF_PROCESSORS','PROCESSOR_ARCHITECTURE','PROCESSOR_IDENTIFIER','OS')
$nasm=(Get-Command nasm.exe).Source
Get-ChildItem Env:|Where-Object {$_.Name -notin $keep}|ForEach-Object{Remove-Item -LiteralPath ('Env:'+ $_.Name)}
$env:PATH=(Split-Path $nasm)+';C:\Windows\System32;C:\Windows';$env:TEMP='O:\tmp';$env:TMP='O:\tmp'
$env:MVDM_COMPLETION_COUNT="$dest/PATCH/DONE.BIN"
$env:MVDM_PROMPT_READERS="$dest/PATCH/READERS.BIN"
$env:MVDM_SCREEN_TRACE="$task/screens.txt"
$env:MVDM_EDITORS_READY="$dest/PATCH/EDITORS.BIN"
$env:MVDM_REAL_COMMAND='1'
$env:MVDM_CONTINUATION_TRACE="$task/trace.txt"
& "$repo/test/integration/win31_runtime_matrix_test.ps1" -PackageRoot $PackageRoot -InstallRoot $dest -Observer $Observer -OutputRoot "$task/runtime" -WindowsProbeImage "$task/controller.exe" -TimeoutMs 90000 -Cases 386-dosonly0
if($LASTEXITCODE){throw 'Prompt matrix failed'}

} finally {
 Get-ChildItem Env:|Where-Object {!$savedEnvironment.ContainsKey($_.Name)}|ForEach-Object{Remove-Item -LiteralPath ('Env:'+ $_.Name)}
 foreach($name in $savedEnvironment.Keys){Set-Item -LiteralPath ('Env:'+ $name) -Value $savedEnvironment[$name]}
 if(@(Get-ChildItem $copy.Source -Force -Recurse -File).Count -ne $copy.Original.Count){throw 'Original installation inventory changed'}
 foreach($row in $copy.Original){if((Get-FileHash ($copy.Source+$row.path)).Hash -ne $row.sha256){throw "Original changed: $($row.path)"}}
}
$frames=@((Get-Content "$task/screens.txt" -Raw) -split '(?m)(?=^VM=)' | Where-Object {$_ -match '^VM='})
$native=@($frames|Where-Object {$_ -match 'New File' -and $_ -match 'Exit'})
$dos=@($frames|Where-Object {$_ -match 'Save As' -and $_ -match 'Exit' -and $_ -notmatch 'New File'})
if(!$native.Count -or !$dos.Count){throw 'Both editor File menus must be observed'}
$nativeVm=[regex]::Match($native[0],'^VM=(\w+)').Groups[1].Value
$dosVm=[regex]::Match($dos[0],'^VM=(\w+)').Groups[1].Value
if($nativeVm -eq $dosVm){throw 'Editor menus must belong to distinct guest VMs'}
if([BitConverter]::ToUInt32([IO.File]::ReadAllBytes("$dest/PATCH/DONE.BIN"),0) -ne 2){throw 'Both commands must complete'}
[ordered]@{nativeVm=$nativeVm;dosVm=$dosVm;menus=$true;completed=2;mouseVerified=$false;saveVerified=$false}|
 ConvertTo-Json|Set-Content "$task/editor-result.json"
Write-Output 'PASS dual editor keyboard/menu/normal-exit contract; mouse/save are separate acceptance'
