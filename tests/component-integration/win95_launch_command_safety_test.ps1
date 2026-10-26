[CmdletBinding()]
param([Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($OutputRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path -LiteralPath $out)){
 throw 'Fresh repository build evidence required'
}
New-Item -ItemType Directory -Force -Path "$out/WINDOWS/SYSTEM","$out/WINDOWS/PATCH"|Out-Null
$windows=Join-Path $out 'WINDOWS'
foreach($file in @('WIN.COM','SYSTEM/VMM32.VXD','SYSTEM.DAT','SYSTEM.INI')){
 [IO.File]::WriteAllText((Join-Path $windows $file),'fixture',[Text.Encoding]::ASCII)
}
$owned=Join-Path $windows 'PATCH/WIN.CMD'
[IO.File]::WriteAllText($owned,'owner content',[Text.Encoding]::ASCII)
function Require([bool]$value,[string]$why){if(!$value){throw $why}}
function Refuses([scriptblock]$operation,[string]$message){
 $failed=$false
 try{& $operation|Out-Null}catch{$failed=$true;Require ($_.ToString().Contains($message)) "Unexpected refusal: $_"}
 Require $failed "Expected refusal: $message"
}
Refuses {& "$repo/tools/win95-launch/Apply-Win95Launch.ps1" -WindowsRoot $windows} 'Refusing to overwrite pre-existing PATCH content: WIN.CMD'
Require (([IO.File]::ReadAllText($owned,[Text.Encoding]::ASCII)) -eq 'owner content') 'Preflight altered existing PATCH content'
Refuses {& "$repo/tools/win95-launch/Undo-Win95Launch.ps1" -WindowsRoot $windows} 'Missing launch journal'
Require (Test-Path -LiteralPath $owned) 'Journal-less undo removed existing PATCH content'
$migrated=Join-Path $windows 'TEST.INI'
[IO.File]::WriteAllText($migrated,'original',[Text.Encoding]::ASCII)
Copy-Item -LiteralPath $migrated -Destination "$migrated.W95PATH.BAK"
[IO.File]::WriteAllText($migrated,'installed',[Text.Encoding]::ASCII)
$installedHash=(Get-FileHash -LiteralPath $migrated -Algorithm SHA256).Hash
@{path=$migrated;installedSha256=$installedHash}|ConvertTo-Json -Compress|Set-Content -LiteralPath "$windows/PATCH/W95PATH.MANIFEST" -NoNewline
[IO.File]::WriteAllText($migrated,'user changed',[Text.Encoding]::ASCII)
Refuses {& "$repo/tools/win95-path/Undo-Win95Path.ps1" -WindowsRoot $windows} 'Preserve changed installed file'
Require (([IO.File]::ReadAllText($migrated,[Text.Encoding]::ASCII)) -eq 'user changed') 'Path undo overwrote user content'
[IO.File]::WriteAllText($migrated,'installed',[Text.Encoding]::ASCII)
& "$repo/tools/win95-path/Undo-Win95Path.ps1" -WindowsRoot $windows
Require (([IO.File]::ReadAllText($migrated,[Text.Encoding]::ASCII)) -eq 'original') 'Path undo failed to restore exact backup'
$launchTarget=Join-Path $windows 'SYSTEM/VMM32.VXD'
Copy-Item -LiteralPath $launchTarget -Destination "$launchTarget.W95LAUNCH.BAK"
[IO.File]::WriteAllText($launchTarget,'installed',[Text.Encoding]::ASCII)
$launchHash=(Get-FileHash -LiteralPath $launchTarget -Algorithm SHA256).Hash
@{path=$launchTarget;kind='replace';installedSha256=$launchHash}|ConvertTo-Json -Compress|Set-Content -LiteralPath "$windows/PATCH/W95LAUNCH.MANIFEST" -NoNewline
[IO.File]::WriteAllText($launchTarget,'user changed',[Text.Encoding]::ASCII)
Refuses {& "$repo/tools/win95-launch/Undo-Win95Launch.ps1" -WindowsRoot $windows} 'Preserve changed installed file'
Require (([IO.File]::ReadAllText($launchTarget,[Text.Encoding]::ASCII)) -eq 'user changed') 'Launch undo overwrote user content'
[IO.File]::WriteAllText($launchTarget,'installed',[Text.Encoding]::ASCII)
& "$repo/tools/win95-launch/Undo-Win95Launch.ps1" -WindowsRoot $windows
Require (([IO.File]::ReadAllText($launchTarget,[Text.Encoding]::ASCII)) -eq 'fixture') 'Launch undo failed to restore exact backup'
'WIN95_LAUNCH_COMMAND_SAFETY_OK preexisting_patch=refused journal_less_undo=refused changed_path_preserved=1 exact_path_restore=1 changed_launch_preserved=1 exact_launch_restore=1'
