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
if($MouseDriver){
 $enc=[Text.Encoding]::GetEncoding(28591)
 $MouseDriver=(Resolve-Path -LiteralPath $MouseDriver).Path
# The source installation may predate the accepted mouse driver. Exercise the
# asset installed by APPLY.CMD, not an unrelated historical installation byte.
$iniText=$enc.GetString([IO.File]::ReadAllBytes("$dest/SYSTEM.INI"))
$boot=[regex]::Match($iniText,'(?ims)^\[boot\][^\r\n]*\r?\n(.*?)(?=^\[|\z)').Groups[1].Value
$mouse=[regex]::Matches($boot,'(?im)^mouse\.drv\s*=\s*([^\r\n]+)')
if($mouse.Count -ne 1 -or $mouse[0].Groups[1].Value.Trim() -ine 'mouse.drv'){
 throw 'Prepared mouse fixture requires the prepared SYSTEM/MOUSE.DRV boot entry'
}
Copy-Item $MouseDriver "$dest/SYSTEM/MOUSE.DRV"
@{source=(Get-FileHash "$source/SYSTEM/MOUSE.DRV").Hash;tested=(Get-FileHash "$dest/SYSTEM/MOUSE.DRV").Hash;asset=(Get-FileHash $MouseDriver).Hash}|ConvertTo-Json|Set-Content "$task/mouse-driver.json"

}
$system="$dest/SYSTEM.INI"
$enc=[Text.Encoding]::GetEncoding(28591)
$s=$enc.GetString([IO.File]::ReadAllBytes($system))
if([regex]::Matches($s,'(?im)^\[386Enh\]').Count -ne 1){throw 'No unique enhanced section'}
$s=[regex]::Replace($s,'(?im)^[ \t]*device[ \t]*=[^\r\n]*(?:ntvdm64\.(?:386|vxd)|insignia\.386|ints\.386)[ \t]*(?:;[^\r\n]*)?\r?\n','')
$s=[regex]::Replace($s,'(?im)^\[386Enh\]','[386Enh]' + "`r`ndevice=$dest\PATCH\NTVDM64.386")
[IO.File]::WriteAllBytes($system,$enc.GetBytes($s))
Copy-Item -LiteralPath $DriverImage -Destination "$dest/PATCH/NTVDM64.386"
if($InstanceDriver -or $InterruptObserver){
 $entries=''
 if($InstanceDriver){Copy-Item -LiteralPath (Resolve-Path -LiteralPath $InstanceDriver).Path -Destination "$dest/PATCH/INSIGNIA.386";$entries+="`r`ndevice=$dest\PATCH\INSIGNIA.386"}
 if($InterruptObserver){Copy-Item -LiteralPath (Resolve-Path -LiteralPath $InterruptObserver).Path -Destination "$dest/PATCH/INTS.386";$entries+="`r`ndevice=$dest\PATCH\INTS.386"}
 $s=$enc.GetString([IO.File]::ReadAllBytes($system))
 $s=[regex]::Replace($s,'(?im)^\[386Enh\]','[386Enh]'+$entries)
 [IO.File]::WriteAllBytes($system,$enc.GetBytes($s))
}


[pscustomobject]@{Task=$task;Source=$source;Destination=$dest;Original=$original}
} catch {
 $failure=$_
 . "$repo/test/observation/isolated_package_cleanup.ps1"
 try {Remove-OwnedGuestProbeCopy $dest $task $source} catch {Write-Warning "Guest copy retained: $($_.Exception.Message)"}
 throw $failure
}
