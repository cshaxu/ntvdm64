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
 [Parameter(Mandatory)][string]$OutputRoot,
 [ValidateSet('Native','Dos')][string]$FirstExit='Native',
 [switch]$SaveWitness,
 [switch]$DosOnly,
 [switch]$MouseMenu,
 [switch]$FirstDosBeforeSecond,
 [switch]$ExplicitVmd,
 [string]$MouseCalloutBridge,
 [string]$RegistrySnapshotMonitor,
 [string]$InstanceDriver
)
$ErrorActionPreference='Stop'
if($DosOnly -and $SaveWitness){throw 'DOSONLY save witness is a separate case'}
if($DosOnly -and $FirstDosBeforeSecond){throw 'The first-editor sequence requires DOSONLY=0'}
$repo=(Resolve-Path "$PSScriptRoot/../../..").Path
if(!$InstanceDriver){$InstanceDriver="$repo/assets/release/insignia.386"}
$PackageRoot=(Resolve-Path -LiteralPath $PackageRoot).Path
$Observer=(Resolve-Path -LiteralPath $Observer).Path
$copy=& "$PSScriptRoot/New-Win31ProbeCopy.ps1" -DriverImage $DriverImage -InstallSource $InstallSource -InstallRoot $InstallRoot -OutputRoot $OutputRoot -InterruptObserver $InterruptObserver -InstanceDriver $InstanceDriver
$task=$copy.Task; $dest=$copy.Destination
try {
$enc=[Text.Encoding]::GetEncoding(28591)
if($ExplicitVmd){
 $system="$dest/SYSTEM.INI"
 $iniText=$enc.GetString([IO.File]::ReadAllBytes($system))
 $iniText=[regex]::Replace($iniText,'(?im)^\[386Enh\]','[386Enh]'+"`r`ndevice=*vmd")
 [IO.File]::WriteAllBytes($system,$enc.GetBytes($iniText))
}
if($MouseCalloutBridge){
 Copy-Item -LiteralPath $MouseCalloutBridge -Destination "$dest/PATCH/MCALL.COM"
 $auto="$dest/PATCH/AUTOEXEC.NT"
 [IO.File]::AppendAllText($auto,"`r`n$dest\PATCH\MCALL.COM`r`n",$enc)
}
if($SaveWitness){foreach($name in 'A.TXT','B.TXT'){[IO.File]::WriteAllBytes("$dest/PATCH/$name",[byte[]]@())}}
Copy-Item -LiteralPath "$PackageRoot/system32/COMMAND.COM" -Destination "$dest/PATCH/COMMAND.COM"
if((Get-FileHash "$PackageRoot/system32/COMMAND.COM").Hash -ne (Get-FileHash "$dest/PATCH/COMMAND.COM").Hash){throw 'COMMAND copy mismatch'}
foreach($id in 'A','B'){
 & "$repo/assets/release/PIF.EXE" create "$dest/PATCH/$id.PIF" --title "Prompt probe $id" --program "$dest\PATCH\COMMAND.COM" --directory "$dest\PATCH" --arguments '' --config '' --autoexec '' --close-on-exit --w386-flags 0x21002 --min-memory 128 --max-memory 640
 if($LASTEXITCODE){throw 'PIF failed'}

}
$ini="$dest/WIN.INI";$s=$enc.GetString([IO.File]::ReadAllBytes($ini))
$load=if($FirstDosBeforeSecond){"$dest\PATCH\B.PIF"}else{"$dest\PATCH\A.PIF $dest\PATCH\B.PIF"}
$s=[regex]::Replace($s,'(?im)^load=[^\r\n]*',"load=$load")
[IO.File]::WriteAllBytes($ini,$enc.GetBytes($s))
$buildLine='/d /s /c ""'+$repo+'/tools/build-win31-editor.cmd" "'+$task+'/controller.exe" "'+$dest+'" '+[int][bool]$MouseMenu+' '+[int][bool]$DosOnly+' '+[int][bool]$SaveWitness+' '+[int]($FirstExit -eq 'Dos')+' '+[int][bool]$FirstDosBeforeSecond+'"'
$producer=Start-Process $env:ComSpec -ArgumentList $buildLine -WindowStyle Hidden -PassThru -RedirectStandardOutput "$task/controller-build.stdout.txt" -RedirectStandardError "$task/controller-build.stderr.txt"
$null=$producer.Handle
@{pid=$producer.Id;start=$producer.StartTime.ToUniversalTime().ToString('o');command=$buildLine}|ConvertTo-Json|Set-Content "$task/controller-build.process.json"
try {
 if(!$producer.WaitForExit(300000)){taskkill.exe /PID $producer.Id /T /F;throw 'Controller build deadline'}
 $producer.WaitForExit();$producer.ExitCode|Set-Content "$task/controller-build.exit.txt"
 if($producer.ExitCode){throw 'Controller build failed'}
}finally{$producer.Dispose()}
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
if($FirstDosBeforeSecond -and $MouseMenu){$env:MVDM_MENU_SEQUENCE="$dest/PATCH/MENUSTG.BIN"}
$env:MVDM_EDITORS_READY="$dest/PATCH/EDITORS.BIN"
$env:MVDM_REAL_COMMAND='1'
if($DosOnly){
 $env:MVDM_GUEST_SCREEN="$task/screens.txt"
 $env:MVDM_READ_TOTAL="$dest/PATCH/TOTAL.BIN"
}
$runtimeCase=if($DosOnly){'386-dosonly1'}else{'386-dosonly0'}
$env:MVDM_CONTINUATION_TRACE="$task/trace.txt"
$registryArguments=@{}
if($RegistrySnapshotMonitor){
 if(!$DosOnly){throw 'No-payload registry assertion requires DOSONLY'}
 $registryArguments=@{RegistrySnapshotMonitor=$RegistrySnapshotMonitor;RegistrySnapshotMarker="$dest/PATCH/EDITORS.BIN"}
}
& "$repo/test/integration/win31_runtime_matrix_test.ps1" -PackageRoot $PackageRoot -InstallRoot $dest -Observer $Observer -OutputRoot "$task/runtime" -WindowsProbeImage "$task/controller.exe" -TimeoutMs 90000 -Cases $runtimeCase @registryArguments
if($LASTEXITCODE){throw 'Prompt matrix failed'}

} finally {
 Get-ChildItem Env:|Where-Object {!$savedEnvironment.ContainsKey($_.Name)}|ForEach-Object{Remove-Item -LiteralPath ('Env:'+ $_.Name)}
 foreach($name in $savedEnvironment.Keys){Set-Item -LiteralPath ('Env:'+ $name) -Value $savedEnvironment[$name]}
 if(@(Get-ChildItem $copy.Source -Force -Recurse -File).Count -ne $copy.Original.Count){throw 'Original installation inventory changed'}
 foreach($row in $copy.Original){if((Get-FileHash ($copy.Source+$row.path)).Hash -ne $row.sha256){throw "Original changed: $($row.path)"}}
}
$frames=@((Get-Content "$task/screens.txt" -Raw) -split '(?m)(?=^VM=)' | Where-Object {$_ -match '^VM='})
if($DosOnly){
 if($RegistrySnapshotMonitor){
  $registry=@(Get-Content "$task/runtime/$runtimeCase.registry.json" -Raw|ConvertFrom-Json)
  $workers=@($registry|Where-Object category -eq 2)
  if($workers.Count -ne 1 -or $workers[0].kind -ne 0 -or @($registry|Where-Object {$_.category -notin @(1,2)}).Count){throw 'DOSONLY created an unexpected host worker/task/VM frontend'}
  if(@($registry|Where-Object category -eq 1).Count -ne 1){throw 'Expected the root NTCON frontend'}
 }
 if(!(($frames -join "`n") -match 'Save As') -or !(($frames -join "`n") -match 'Exit')){throw 'Guest DOS editor menu missing'}
 if((Get-Content "$task/trace.txt" -Raw) -match '(?m)^11 '){throw 'DOSONLY admitted a host command'}
 if((Get-Content "$task/screens.txt" -Raw) -notmatch '(?i)(cannot be run in DOS mode|requires Microsoft Windows|must be run under Win32)'){throw 'Expected native executable DOS rejection missing'}
 [ordered]@{dosonly=1;guestEditorMenu=$true;mouseMenu=[bool]$MouseMenu;hostCommandAdmissions=0;hostRegistryVerified=[bool]$RegistrySnapshotMonitor;normalExit=$true}|
  ConvertTo-Json|Set-Content "$task/editor-result.json"
 Write-Output 'PASS DOSONLY guest EDIT and native rejection; no host command admitted'
 return
}
$native=@($frames|Where-Object {$_ -match 'New File' -and $_ -match 'Exit'})
$dos=@($frames|Where-Object {$_ -match 'Save As' -and $_ -match 'Exit' -and $_ -notmatch 'New File'})
if(!$native.Count -or !$dos.Count){throw 'Both editor File menus must be observed'}
$nativeVm=[regex]::Match($native[0],'^VM=(\w+)').Groups[1].Value
$dosVm=[regex]::Match($dos[0],'^VM=(\w+)').Groups[1].Value
if($nativeVm -eq $dosVm){throw 'Editor menus must belong to distinct guest VMs'}
if($FirstDosBeforeSecond -and $MouseMenu){
 $menuStage="$dest/PATCH/MENUSTG.BIN"
 if(!(Test-Path -LiteralPath $menuStage) -or
    [BitConverter]::ToUInt32([IO.File]::ReadAllBytes($menuStage),0) -lt 3){
  throw 'First DOS editor menu did not reopen after the second VM appeared'
 }
}
if([BitConverter]::ToUInt32([IO.File]::ReadAllBytes("$dest/PATCH/DONE.BIN"),0) -ne 2){throw 'Both commands must complete'}
if($SaveWitness){
 foreach($pair in @(@('A.TXT','aaa'),@('B.TXT','bbb'))){
  $path="$dest/PATCH/$($pair[0])"
  if((Get-Content -LiteralPath $path -Raw).Trim() -cne $pair[1]){throw "Wrong saved contents: $path"}
  Copy-Item -LiteralPath $path -Destination $task
 }
}
[ordered]@{firstExit=$FirstExit;nativeVm=$nativeVm;dosVm=$dosVm;menus=$true;completed=2;mouseMenu=[bool]$MouseMenu;mouseExitVerified=[bool]$MouseMenu;saveVerified=[bool]$SaveWitness;firstDosBeforeSecond=[bool]$FirstDosBeforeSecond;stagedMenuFrames=[bool]($FirstDosBeforeSecond -and $MouseMenu)}|
 ConvertTo-Json|Set-Content "$task/editor-result.json"
Write-Output "PASS dual editor keyboard/menu/normal-exit; saved-content=$([bool]$SaveWitness); mouse-Exit=$([bool]$MouseMenu)"
} finally {
 . "$repo/test/observation/isolated_package_cleanup.ps1"
 Remove-OwnedGuestProbeCopy $dest $task $copy.Source
}
