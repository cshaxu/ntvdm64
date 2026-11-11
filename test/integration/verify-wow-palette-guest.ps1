param([string]$PackageRoot,[string]$OutputRoot,[string]$Observer='build/t478-s23/observer.exe')
$ErrorActionPreference='Stop'
. "$PSScriptRoot/support/wow-guest-probe.ps1"
$probe=Invoke-WowGuestProbe -PackageRoot $PackageRoot -OutputRoot $OutputRoot -Observer $Observer -Source 'test/integration/wow_palette_guest.asm' -Name PALTEST
$b=$probe.Bytes;$out=$probe.Output
 if($b.Length -ne 12 -or [Text.Encoding]::ASCII.GetString($b,0,4) -ne 'PAL1'){throw 'Invalid guest record'}
 $row=@{errors=[BitConverter]::ToUInt16($b,4);count=[BitConverter]::ToUInt16($b,6);requested=[BitConverter]::ToUInt16($b,8);returned=[BitConverter]::ToUInt16($b,10);package=$probe.Package;wow=$probe.Wow}
 $row|ConvertTo-Json|Set-Content "$out/result.json";$row|ConvertTo-Json
 if($row.errors -or !$row.count -or $row.returned -ne $row.count -or $row.requested -ne $row.count+3){throw 'Guest palette output failed'}
 $logText=$probe.LogText
 if($probe.ExitCode -or $logText -notmatch '(?m)^result=exited\r?$' -or $logText -notmatch '(?m)^exit=0x00000000\r?$'){throw 'Guest failed ordinary exit'}
