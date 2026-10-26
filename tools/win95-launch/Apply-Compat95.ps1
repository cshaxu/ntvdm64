[CmdletBinding()]
param([string]$WindowsRoot='O:\WINDOWS',
      [Parameter(Mandatory)][string]$BackupRoot,
      [string]$Logo='', [switch]$NoLogo)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$windows=(Resolve-Path -LiteralPath $WindowsRoot).Path
$patch=Join-Path $windows 'PATCH'
$config=Join-Path $patch 'CONFIG.NT'
if(!(Test-Path -LiteralPath "$windows/WIN.COM") -or !(Test-Path -LiteralPath $config)){throw 'Existing prepared Win95 installation required'}
if($windows -match '\s' -or $Logo -and $NoLogo){throw 'Use the existing short DOS installation path and one logo choice'}
if(@(Get-Process -Name ntvdm -ErrorAction SilentlyContinue).Count){throw 'Close running VDMs before changing the installed guest profile'}
$release=Join-Path $repo 'assets/release'
$manifest=Get-Content "$release/addon-manifest.json" -Raw|ConvertFrom-Json
$entry=@($manifest.drivers|Where-Object id -eq 'win95-compat')
if($manifest.schema -ne 1 -or $entry.Count -ne 1 -or $entry[0].file -ne 'compat95.sys'){throw 'Invalid compat95 release manifest'}
$driver=Join-Path $release 'compat95.sys'
if((Get-FileHash $driver).Hash -ne $entry[0].sha256){throw 'Release driver identity mismatch'}
if(!$NoLogo -and !$Logo){throw 'Supply a local LOGO.SYS or explicitly select -NoLogo'}
if($Logo){
 $logoPath=(Resolve-Path -LiteralPath $Logo).Path
 $bytes=[IO.File]::ReadAllBytes($logoPath)
 if($bytes.Length -ne 129078 -or $bytes[0] -ne 66 -or $bytes[1] -ne 77 -or
    [BitConverter]::ToInt32($bytes,18) -ne 320 -or [BitConverter]::ToInt32($bytes,22) -ne 400 -or
    [BitConverter]::ToUInt16($bytes,28) -ne 8 -or [BitConverter]::ToUInt32($bytes,30) -ne 0){throw 'Expected complete 320x400 8-bit uncompressed logo'}
}
$backup=[IO.Path]::GetFullPath($BackupRoot)
if(Test-Path -LiteralPath $backup){throw 'Preserve existing backup; select a fresh backup directory'}
if($backup -ieq $patch -or $backup.StartsWith($patch+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Backup outside PATCH required'}
$text=[IO.File]::ReadAllText($config)
$pattern='(?im)^device=.*(?:W95COMP|COMPAT|COMPAT95)\.SYS[^\r\n]*'
if([regex]::Matches($text,$pattern).Count -ne 1){throw 'Expected one compatibility-driver configuration entry'}
$option=if($NoLogo){''}else{" /LOGO=$patch\LOGO.SYS"}
$candidate=[regex]::Replace($text,$pattern,"device=$patch\compat95.sys$option")
New-Item -ItemType Directory $backup|Out-Null
$saved=@()
foreach($name in @('CONFIG.NT','compat95.sys','LOGO.SYS','W95COMP.SYS','COMPAT.SYS')){
 $path=Join-Path $patch $name;$row=@{name=$name;path=$path;exists=[IO.File]::Exists($path)}
 if($row.exists){$row.hash=(Get-FileHash -LiteralPath $path).Hash;$row.attributes=(Get-Item -LiteralPath $path -Force).Attributes;[IO.File]::Copy($path,(Join-Path $backup $name),$true)}
 $saved+=,$row
}
$done=$false
try {
 Copy-Item -LiteralPath $driver -Destination "$patch/compat95.sys" -Force
 if($Logo){Copy-Item -LiteralPath $logoPath -Destination "$patch/LOGO.SYS" -Force}
 [IO.File]::WriteAllText($config,$candidate,[Text.Encoding]::ASCII)
 if((Get-FileHash "$patch/compat95.sys").Hash -ne $entry[0].sha256){throw 'Installed driver mismatch'}
 foreach($old in @('W95COMP.SYS','COMPAT.SYS')){if([IO.File]::Exists("$patch/$old")){Remove-Item -LiteralPath "$patch/$old" -Force}}
 @{installed=$true;driver='compat95.sys';sha256=$entry[0].sha256;logoEnabled=![bool]$NoLogo;backup=$backup;before=$saved}|ConvertTo-Json -Depth 5|Set-Content "$backup/install.json"
 $done=$true
} finally {
 if(!$done){foreach($row in $saved){if($row.exists){[IO.File]::Copy((Join-Path $backup $row.name),$row.path,$true);[IO.File]::SetAttributes($row.path,$row.attributes)}elseif([IO.File]::Exists($row.path)){Remove-Item -LiteralPath $row.path -Force}}}
}
'PASS installed compat95.sys and updated existing profile; old names backed up outside PATCH'
