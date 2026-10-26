[CmdletBinding()]
param([Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($OutputRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path -LiteralPath $out)){
 throw 'Fresh repository build evidence required'
}
New-Item -ItemType Directory "$out/WINDOWS/PATCH","$out/menu-source","$out/apps-source","$out/prepared/menu","$out/prepared/apps" -Force|Out-Null
$windows=Join-Path $out 'WINDOWS'
function WriteFixture([string]$path,[string]$text){[IO.File]::WriteAllText($path,$text,[Text.Encoding]::ASCII)}
function Require([bool]$value,[string]$why){if(!$value){throw $why}}
function Refuses([scriptblock]$operation,[string]$message){
 $failed=$false
 try{& $operation|Out-Null}catch{$failed=$true;Require ($_.ToString().Contains($message)) "Unexpected refusal: $_"}
 Require $failed "Expected refusal: $message"
}
# These are independently authored file fixtures, never a real guest tree.
WriteFixture "$windows/WIN.COM" 'fixture only'
WriteFixture "$windows/PATCH/CONFIG.NT" "device=$windows\PATCH\W95COMP.SYS`r`n"
WriteFixture "$windows/PATCH/W95COMP.SYS" 'old fixture driver'
$before=(Get-FileHash "$windows/PATCH/CONFIG.NT").Hash
Refuses {& "$repo/tools/win95-launch/Apply-Compat95.ps1" -WindowsRoot $windows -BackupRoot "$windows/PATCH/BACKUP" -NoLogo} 'Backup outside PATCH'
Require ((Get-FileHash "$windows/PATCH/CONFIG.NT").Hash -eq $before) 'Refusal changed configuration'
& "$repo/tools/win95-launch/Apply-Compat95.ps1" -WindowsRoot $windows -BackupRoot "$out/compat-backup" -NoLogo
$state=Get-Content "$out/compat-backup/install.json" -Raw|ConvertFrom-Json
Require $state.installed 'Missing completed compatibility recovery record'
Require ((Get-FileHash "$out/compat-backup/CONFIG.NT").Hash -eq $before) 'Lost original config'
Require ((Get-FileHash "$windows/PATCH/compat95.sys").Hash -eq (Get-FileHash "$repo/assets/release/compat95.sys").Hash) 'Wrong installed driver'
Refuses {& "$repo/tools/win95-launch/Apply-Compat95.ps1" -WindowsRoot $windows -BackupRoot "$out/compat-backup" -NoLogo} 'Preserve existing backup'
$manifest=Get-Content "$repo/assets/release/addon-manifest.json" -Raw|ConvertFrom-Json
$lnk=@($manifest.utilities|Where-Object id -eq 'lnk')[0]
$files=@()
foreach($role in @('menu','apps')){
 $name=if($role -eq 'menu'){'TEST.LNK'}else{'TEST.EXE'}
 $source=Join-Path "$out/$role-source" $name
 $candidate=Join-Path "$out/prepared/$role" $name
 $dest=Join-Path $(if($role -eq 'menu'){"$windows/STRTMENU"}else{"$windows/APPS95"}) $name
 WriteFixture $source "original $role fixture"
 WriteFixture $candidate "prepared $role fixture"
 $files+=@{role=$role;relative=$name;source=$source;sourceSha256=(Get-FileHash $source).Hash;candidate=$candidate;sha256=(Get-FileHash $candidate).Hash;destination=$dest}
}
@{schema=1;windows=$windows;menuRoot="$windows\STRTMENU";sourceRoot="$out\menu-source";programSourceRoot="$out\apps-source";lnkSha256=$lnk.sha256;files=$files}|ConvertTo-Json -Depth 5|Set-Content "$out/prepared/plan.json"
# Inject only the recovery-journal write boundary; replacement and rollback
# still execute the actual installers' filesystem operations in this fixture.
$installedConfig=(Get-FileHash "$windows/PATCH/CONFIG.NT").Hash
function Set-Content {
 param([Parameter(ValueFromPipeline)]$Value,[Parameter(Position=0)][string]$Path)
 process {throw 'Injected recovery journal failure'}
}
try {
 Refuses {& "$repo/tools/win95-launch/Apply-Compat95.ps1" -WindowsRoot $windows -BackupRoot "$out/compat-failure" -NoLogo} 'Injected recovery journal failure'
 Require ((Get-FileHash "$windows/PATCH/CONFIG.NT").Hash -eq $installedConfig) 'Journal failure lost compatibility configuration'
 Refuses {& "$repo/tools/win95-launch/Apply-StartMenu.ps1" -Plan "$out/prepared/plan.json" -BackupRoot "$out/menu-failure"} 'Injected recovery journal failure'
 Require (!(Test-Path "$windows/STRTMENU/TEST.LNK") -and !(Test-Path "$windows/APPS95/TEST.EXE")) 'Journal failure left installed menu files'
}finally{Remove-Item Function:Set-Content}
& "$repo/tools/win95-launch/Apply-StartMenu.ps1" -Plan "$out/prepared/plan.json" -BackupRoot "$out/menu-backup"
Require (Test-Path "$out/menu-backup/install.json") 'Missing completed menu recovery record'
WriteFixture "$windows/STRTMENU/TEST.LNK" 'user changed fixture'
Refuses {& "$repo/tools/win95-launch/Restore-StartMenu.ps1" -BackupRoot "$out/menu-backup"} 'Preserve changed/missing installed file'
Require (Test-Path "$windows/APPS95/TEST.EXE") 'Refused restoration partially removed files'
Copy-Item "$out/prepared/menu/TEST.LNK" "$windows/STRTMENU/TEST.LNK" -Force
& "$repo/tools/win95-launch/Restore-StartMenu.ps1" -BackupRoot "$out/menu-backup"
Require (!(Test-Path "$windows/STRTMENU/TEST.LNK") -and !(Test-Path "$windows/APPS95/TEST.EXE")) 'Owned files remain'
Require (Test-Path "$out/menu-source/TEST.LNK") 'Original source was altered'
'WIN95_LAUNCH_INSTALL_OK compat_install=1 backup_refusals=2 menu_install=1 changed_file_refusal=1 restore=1 journal_failure_rollbacks=2 fixture_only=1'
