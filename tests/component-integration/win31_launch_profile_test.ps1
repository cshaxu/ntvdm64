param([Parameter(Mandatory)][string]$InstallRoot,
      [Parameter(Mandatory)][string]$ProfileDirectory,
      [Parameter(Mandatory)][string]$BeforeHashes,
      [ValidateSet('Standard','Enhanced')][string]$Mode='Standard')
$ErrorActionPreference='Stop'
$root=(Resolve-Path $InstallRoot).Path.TrimEnd('\');$patch="$root\PATCH"
$bytes=[IO.File]::ReadAllBytes((Join-Path $ProfileDirectory 'WIN31.PIF'))
function Field([int]$offset,[int]$length){[Text.Encoding]::ASCII.GetString($bytes,$offset,$length).Trim([char]0)}
$arguments=if($Mode -eq 'Enhanced'){'/3'}else{'/S'}
if((Field 36 63) -ne "$root\WIN.COM" -or (Field 101 64) -ne $root -or (Field 165 64) -ne $arguments){throw 'Wrong main PIF fields'}
$pos=369;$nt=$false;$extended=$false
while($pos -ne 65535){
    $sig=Field $pos 16;$data=[BitConverter]::ToUInt16($bytes,$pos+18)
    if($sig -eq 'WINDOWS 386 3.0'){if((Field ($data+40) 64) -ne $arguments){throw 'Wrong extended arguments'};$extended=$true}
    if($sig -eq 'WINDOWS NT  3.1'){
        if((Field ($data+12) 64) -ne "$patch\CONFIG.NT" -or (Field ($data+76) 64) -ne "$patch\AUTOEXEC.NT"){throw 'Wrong profile paths'};$nt=$true
    }
    $pos=[BitConverter]::ToUInt16($bytes,$pos+16)
}
if(!$nt -or !$extended){throw 'Missing extensions'}
$sum=0;for($i=2;$i -lt 369;$i++){$sum=($sum+$bytes[$i]) -band 255};if($sum -ne $bytes[1]){throw 'Bad PIF checksum'}
$config=Get-Content (Join-Path $ProfileDirectory 'CONFIG.NT') -Raw
$auto=Get-Content (Join-Path $ProfileDirectory 'AUTOEXEC.NT') -Raw
if($config -notmatch '(?m)^dosonly\s*$' -or $auto -match '(?im)^\s*(lh\s+)?[^\r\n]*\\dosx(\.exe)?\s*$'){throw 'Wrong DOS-only/DOSX profile'}
if(!$auto.Contains("SET TEMP=$patch\TEMP") -or !$auto.Contains("$root\SYSTEM")){throw 'Wrong environment'}
$launch=Get-Content (Join-Path $ProfileDirectory 'win.cmd') -Raw
if(!$launch.Contains('call run16 "%~dp0WIN31.PIF"')){throw 'Launcher does not use PATH/PIF'}
$before=Get-Content $BeforeHashes -Raw|ConvertFrom-Json
foreach($p in $before.PSObject.Properties){if((Get-FileHash $p.Name).Hash -ne $p.Value){throw "Existing input changed: $($p.Name)"}}
"PASS $Mode PIF/config/launcher and unchanged guest/default profiles; no runtime execution"
