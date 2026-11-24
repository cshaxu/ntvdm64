[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$DriverImage,
 [Parameter(Mandatory)][string]$InstallSource,
 [Parameter(Mandatory)][string]$InstallRoot,
 [Parameter(Mandatory)][string]$OutputRoot,
 [string]$InstanceDriver,
 [string]$MouseDriver,
 [string]$InterruptObserver
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../../..").Path
$task=[IO.Path]::GetFullPath($OutputRoot)
$source=(Resolve-Path -LiteralPath $InstallSource).Path.TrimEnd('\')
$dest=[IO.Path]::GetFullPath($InstallRoot).TrimEnd('\')
$DriverImage=(Resolve-Path -LiteralPath $DriverImage).Path
if(!$task.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $task)){throw 'Fresh repository build output required'}
if(!$dest.StartsWith('O:\tmp\',[StringComparison]::OrdinalIgnoreCase) -or $dest -match '[^\x21-\x7e]' -or (Test-Path $dest) -or $dest.StartsWith($source+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Fresh, short, independent O:/tmp guest copy required'}
if(!(Test-Path "$source/WIN.COM") -or !(Test-Path "$source/SYSTEM.INI")){throw 'Installed Windows 95 source required'}
if($source.Length -ne $dest.Length){throw 'Hive relocation requires paths of equal length'}
$autoexec=Join-Path $source 'PATCH/AUTOEXEC.NT'
if(Test-Path -LiteralPath $autoexec){
 $text=[IO.File]::ReadAllText($autoexec)
 foreach($match in [regex]::Matches($text,'(?im)^\s*set\s+(?:windir|winbootdir)\s*=\s*([a-z]:\\[^\r\n]*)\s*$')){
  if($match.Groups[1].Value.TrimEnd() -ine $source){throw 'Source AUTOEXEC.NT points at another Windows installation'}
 }
}
New-Item -ItemType Directory $task|Out-Null
# This is a background integration runner. It uses the established matrix
# desktop/scope and guest-internal input, never foreground input automation.
$original=@(Get-ChildItem $source -Force -Recurse -File | ForEach-Object {[ordered]@{path=$_.FullName.Substring($source.Length);sha256=(Get-FileHash $_.FullName).Hash}})
$original|ConvertTo-Json|Set-Content "$task/original.json"
try {
& "$repo/tools/copy-guest-probe.cmd" $source $dest $task *> "$task/copy.log"
if($LASTEXITCODE){throw 'Owned guest installation copy failed'}
# Relocate only the hives and configuration consumed by this isolated case.
# Do not run the broad installation migrator over unrelated Win95 PIF formats.
& "$repo/tools/relocate-guest-hives.cmd" $dest $source $task *> "$task/hive-relocation.log"
if($LASTEXITCODE){throw 'Isolated guest hive path relocation failed'}
& "$repo/tools/rebase-guest-paths.cmd" $dest $source $task 95 *> "$task/path-rebase.log"
if($LASTEXITCODE){throw 'Copied guest path preparation failed'}
$driverArgs=@($DriverImage,$InstanceDriver,$InterruptObserver,$null,$MouseDriver) | ForEach-Object {if($_){(Resolve-Path -LiteralPath $_).Path}else{'-'}}
& "$repo/tools/prepare-guest-ini.cmd" $dest Drivers $task 95 @driverArgs *> "$task/driver-preparation.log"
if($LASTEXITCODE){throw 'Copied guest driver preparation failed'}
$targets=@('PATCH/NTVDM64.386','PATCH/INSIGNIA.386','PATCH/INTS.386','PATCH/RETIRE.386','SYSTEM/MOUSE.DRV')
for($i=0;$i -lt $driverArgs.Count;$i++){
 if($driverArgs[$i] -ne '-' -and (Get-FileHash $driverArgs[$i]).Hash -ne (Get-FileHash (Join-Path $dest $targets[$i])).Hash){throw 'Driver staging mismatch'}
}
if($InstanceDriver){
 [ordered]@{source=$driverArgs[1];sha256=(Get-FileHash $driverArgs[1]).Hash;installed=(Get-FileHash "$dest/PATCH/INSIGNIA.386").Hash}|
  ConvertTo-Json|Set-Content "$task/instance-driver.json"
}
if($MouseDriver){@{source=(Get-FileHash "$source/SYSTEM/MOUSE.DRV").Hash;tested=(Get-FileHash "$dest/SYSTEM/MOUSE.DRV").Hash;asset=(Get-FileHash $driverArgs[4]).Hash}|ConvertTo-Json|Set-Content "$task/mouse-driver.json"}

[pscustomobject]@{Task=$task;Source=$source;Destination=$dest;Original=$original}
} catch {
 $failure=$_
 . "$repo/test/observation/isolated_package_cleanup.ps1"
 try {Remove-OwnedGuestProbeCopy $dest $task $source} catch {Write-Warning "Guest copy retained: $($_.Exception.Message)"}
 throw $failure
}
