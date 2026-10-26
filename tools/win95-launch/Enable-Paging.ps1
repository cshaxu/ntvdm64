[CmdletBinding()]
param([Parameter(Mandatory)][string]$BackupRoot,
 [string]$WindowsRoot='O:\WINDOWS',[switch]$Restore)
$ErrorActionPreference='Stop'
$path=Join-Path ([IO.Path]::GetFullPath($WindowsRoot)) 'SYSTEM.INI'
$backup=[IO.Path]::GetFullPath($BackupRoot)
if(@(Get-Process -Name ntvdm -ErrorAction SilentlyContinue).Count){throw 'Close running VDM sessions before changing Win95 configuration'}
if((Get-Item -LiteralPath $path -Force).Attributes -band [IO.FileAttributes]::ReparsePoint){throw 'INI must be a real file'}
if($Restore){
 $record=Get-Content -LiteralPath "$backup/install.json" -Raw|ConvertFrom-Json
 if($record.path -ine $path -or (Get-FileHash -LiteralPath $path).Hash -ne $record.installedSha256 -or (Get-FileHash -LiteralPath "$backup/SYSTEM.INI").Hash -ne $record.originalSha256){throw 'Configuration changed; automatic restore refused'}
 [IO.File]::SetAttributes($path,[IO.FileAttributes]::Normal)
 [IO.File]::Copy("$backup/SYSTEM.INI",$path,$true)
 [IO.File]::SetAttributes($path,[IO.FileAttributes]$record.attributes)
 'PASS restored exact original Win95 paging configuration'
 return
}
if(Test-Path -LiteralPath $backup){throw 'Fresh external backup directory required'}
if($backup.StartsWith([IO.Path]::GetFullPath($WindowsRoot).TrimEnd('\')+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Backup must be outside the installation'}
$encoding=[Text.Encoding]::GetEncoding(936)
$original=[IO.File]::ReadAllBytes($path)
$text=$encoding.GetString($original)
if([Convert]::ToBase64String($encoding.GetBytes($text)) -cne [Convert]::ToBase64String($original)){throw 'INI encoding must round-trip without changing bytes'}
$section=[regex]::Match($text,'(?ims)^\[386Enh\][^\r\n]*\r?\n(?<body>.*?)(?=^\[|\z)')
if(!$section.Success){throw 'Expected one 386Enh section'}
$body=$section.Groups['body']
$settings=[regex]::Matches($body.Value,'(?im)^Paging=off(?=\r?$)')
if($settings.Count -ne 1 -or [regex]::Matches($text,'(?im)^Paging=').Count -ne 1){throw 'Expected exactly one Paging=off setting'}
$at=$body.Index+$settings[0].Index
$changed=$text.Substring(0,$at)+'Paging=On'+$text.Substring($at+$settings[0].Length)
$attributes=(Get-Item -LiteralPath $path -Force).Attributes
$before=(Get-FileHash -LiteralPath $path).Hash
New-Item -ItemType Directory $backup|Out-Null
[IO.File]::WriteAllBytes("$backup/SYSTEM.INI",$original)
try{
 [IO.File]::SetAttributes($path,[IO.FileAttributes]::Normal)
 [IO.File]::WriteAllBytes($path,$encoding.GetBytes($changed))
 [IO.File]::SetAttributes($path,$attributes)
 @{path=$path;originalSha256=$before;installedSha256=(Get-FileHash -LiteralPath $path).Hash;attributes=[int]$attributes;change='386Enh Paging=off -> Paging=On';utc=[DateTime]::UtcNow.ToString('o')}|ConvertTo-Json|Set-Content "$backup/install.json"
}catch{
 [IO.File]::SetAttributes($path,[IO.FileAttributes]::Normal)
 [IO.File]::WriteAllBytes($path,$original)
 [IO.File]::SetAttributes($path,$attributes)
 throw
}
'PASS enabled normal Win95 paging; exact INI backup retained'
