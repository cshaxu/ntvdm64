[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$DriverImage,
 [Parameter(Mandatory)][string]$InstallSource,
 [Parameter(Mandatory)][string]$InstallRoot,
 [Parameter(Mandatory)][string]$OutputRoot,
 [string]$InstanceDriver,
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
New-Item -ItemType Directory $task|Out-Null
# This is a background integration runner. It uses the established matrix
# desktop/scope and guest-internal input, never foreground input automation.
$original=@(Get-ChildItem $source -Force -Recurse -File | ForEach-Object {[ordered]@{path=$_.FullName.Substring($source.Length);sha256=(Get-FileHash $_.FullName).Hash}})
$original|ConvertTo-Json|Set-Content "$task/original.json"
New-Item -ItemType Directory $dest|Out-Null
Get-ChildItem $source -Force|Copy-Item -Destination $dest -Recurse
# Relocate only the hives and configuration consumed by this isolated case.
# Do not run the broad installation migrator over unrelated Win95 PIF formats.
foreach($hive in 'SYSTEM.DAT','USER.DAT'){
 $path="$dest/$hive"
 $candidate=Join-Path $task "$hive.CANDIDATE"
 & python "$PSScriptRoot/relocate-win95-hive-equal.py" $path $candidate $source $dest | Out-Host
 if($LASTEXITCODE){throw 'Isolated guest hive path relocation failed'}
 [IO.File]::SetAttributes($path,[IO.FileAttributes]::Normal)
 [IO.File]::Copy($candidate,$path,$true)
}
[IO.File]::WriteAllText("$dest/.t479-owner",$task)
foreach($file in Get-ChildItem $dest -Recurse -File | Where-Object {$_.Extension -in '.INI','.CMD','.NT'}) {
 $enc=[Text.Encoding]::GetEncoding(28591)
 $s=$enc.GetString([IO.File]::ReadAllBytes($file.FullName))
 $s=[regex]::Replace($s,[regex]::Escape($source),{param($m) $dest},'IgnoreCase')
 [IO.File]::WriteAllBytes($file.FullName,$enc.GetBytes($s))
}
foreach($file in Get-Item "$dest/PATCH/WIN.PIF"){
 & "$repo/assets/release/PIF.EXE" replace-root $file.FullName --from $source --to $dest | Out-Host
 if($LASTEXITCODE){throw 'PIF rebase failed'}
}
foreach($file in Get-ChildItem $dest -Filter '*.GRP'){
 & "$repo/assets/release/GRP.EXE" replace-root $file.FullName --root $dest | Out-Host
 if($LASTEXITCODE){throw 'GRP rebase failed'}
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
