param([Parameter(Mandatory)][string]$PackageRoot,
      [Parameter(Mandatory)][string]$OutputRoot,
      [string]$Observer='build/.ninja/fixtures-x64/console-startup-observer.exe')
$ErrorActionPreference='Stop'
. "$PSScriptRoot/support/wow-guest-probe.ps1"
$probe=Invoke-WowGuestProbe -PackageRoot $PackageRoot -OutputRoot $OutputRoot -Observer $Observer -Name CTLPROBE
$b=$probe.Bytes
$names=@('phase','errors','created','button','single','multi','list','combos','menus','destroyed','leaving','task','wordCalls','wordNested','wordActions','scroll','drop')
if($b.Length -ne 342){throw 'Incomplete controls report'}
$rows=@(for($offset=0;$offset -lt $b.Length;$offset+=38){
 if([Text.Encoding]::ASCII.GetString($b,$offset,4) -ne 'CTL1'){throw 'Invalid controls report'}
 $r=[ordered]@{}
 for($i=0;$i -lt $names.Count;$i++){$r[$names[$i]]=[BitConverter]::ToUInt16($b,$offset+4+2*$i)}
 [pscustomobject]$r
})
$last=$rows[-1]
$rows|ConvertTo-Json|Set-Content "$($probe.Output)/records.json"
if(($rows.phase -join ',') -ne '0,1,2,3,4,5,6,7,8' -or @($rows|Where-Object errors -ne 0).Count -or
 $last.list -ne 1 -or $last.scroll -ne 1 -or $last.combos -ne 3 -or $last.leaving -ne 2 -or !$last.task){throw 'Controls behavior incomplete'}
if($probe.ExitCode -or $probe.LogText -notmatch '(?m)^result=exited\r?$' -or $probe.LogText -notmatch '(?m)^exit=0x00000000\r?$'){throw 'Controls guest failed ordinary exit'}
@{pass=$true;package=$probe.Package;wow=$probe.Wow;worker=$probe.Worker;records=$rows}|ConvertTo-Json -Depth 4|Set-Content "$($probe.Output)/result.json"
'PASS original Win16 controls, lists, scrolling and three combo forms'
