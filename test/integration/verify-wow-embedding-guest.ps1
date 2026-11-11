param([string]$PackageRoot,[string]$OutputRoot,[string]$Observer='build/t478-s24/observer.exe',[switch]$ExpectStall)
$ErrorActionPreference='Stop'
. "$PSScriptRoot/support/wow-guest-probe.ps1"
$probe=Invoke-WowGuestProbe -PackageRoot $PackageRoot -OutputRoot $OutputRoot -Observer $Observer -Source 'test/integration/wow_embedding_guest.asm' -Name EMBTEST -RequireNoRegistry
$b=$probe.Bytes;$out=$probe.Output
 if($b.Length -notin @(12,24) -or [Text.Encoding]::ASCII.GetString($b,0,4) -ne 'EMB1'){throw 'Invalid guest record'}
 $last=$b.Length-12
 $row=@{phase=[BitConverter]::ToUInt16($b,$last+4);errors=[BitConverter]::ToUInt16($b,$last+6);returned=[BitConverter]::ToUInt16($b,$last+8);registryBytes=[BitConverter]::ToUInt16($b,$last+10);recordBytes=$b.Length;expectedStall=[bool]$ExpectStall;package=$probe.Package;wow=$probe.Wow;worker=$probe.Worker}
 $row|ConvertTo-Json|Set-Content "$out/result.json";$row|ConvertTo-Json
 $logText=$probe.LogText
 if($ExpectStall){
  if($row.phase -ne 1 -or $row.errors -or $b.Length -ne 12 -or $logText -notmatch '(?m)^result=timeout\r?$'){throw 'Expected pre-fix enumeration stall was not observed'}
 } else {
  if($row.registryBytes -ne 6 -or $row.phase -ne 2 -or $row.errors -or $b.Length -ne 24 -or $probe.ExitCode -or $logText -notmatch '(?m)^result=exited\r?$' -or $logText -notmatch '(?m)^exit=0x00000000\r?$'){throw 'Embedding guest did not complete'}
 }
