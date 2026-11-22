[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$Checker,
 [Parameter(Mandatory)][string]$Map,
 [Parameter(Mandatory)][string]$Object,
 [Parameter(Mandatory)][string]$OutputRoot
)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../../..'))
$root=[IO.Path]::GetFullPath($OutputRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){
 throw 'Fresh build-owned output root required'
}
$null=New-Item -ItemType Directory $root
$Checker=[IO.Path]::GetFullPath($Checker)
$Map=[IO.Path]::GetFullPath($Map)
$Object=[IO.Path]::GetFullPath($Object)
$cases=[Collections.Generic.List[object]]::new()
function Check($name,$mapPath,$objectPath,$success){
 & $Checker $mapPath $objectPath *> "$root/$name.log"
 $code=$LASTEXITCODE
 if(($code -eq 0) -ne $success){throw "$name unexpected exit $code"}
 $cases.Add(@{name=$name;exit=$code;expectedSuccess=$success})
}
Check actual-product $Map $Object $true
$mapText=[IO.File]::ReadAllText($Map)
$objectBytes=[IO.File]::ReadAllBytes($Object)
$utf8=[Text.UTF8Encoding]::new($false)
function Map-Case($name,$text){
 if($text -ceq $mapText){throw "Mutation not applied: $name"}
 $path="$root/$name.map"
 [IO.File]::WriteAllText($path,$text,$utf8)
 Check $name $path $Object $false
}
Map-Case wrong-owner ($mapText.Replace('dpmiint.obj','stubs.obj'))
Map-Case missing-startup-binding ($mapText.Replace('_mvdm_monitor_bind_current_thread','_missing_startup_binding'))
$owner=[regex]::Match($mapText,'(?m)^\s+([0-9a-fA-F]+:[0-9a-fA-F]+)\s+_VdmTib\s+([0-9a-fA-F]{8})\s')
if(!$owner.Success){throw 'Actual MAP owner missing'}
Map-Case overlapping-symbol ($mapText+"`n "+$owner.Groups[1].Value+' _test_overlap '+$owner.Groups[2].Value+" data.obj`n")
foreach($length in @(0,19,100)){
 $path="$root/truncated-$length.obj"
 $truncated=[byte[]]::new($length)
 [Array]::Copy($objectBytes,$truncated,$length)
 [IO.File]::WriteAllBytes($path,$truncated)
 Check "truncated-$length" $Map $path $false
}
$bad=[byte[]]$objectBytes.Clone()
$bad[0]=0x64;$bad[1]=0x86
[IO.File]::WriteAllBytes("$root/wrong-machine.obj",$bad)
Check wrong-machine $Map "$root/wrong-machine.obj" $false
$table=[BitConverter]::ToUInt32($objectBytes,8)
$count=[BitConverter]::ToUInt32($objectBytes,12)
$strings=$table+$count*18
$found=$false
for($i=0;$i -lt $count;){
 $at=$table+$i*18
 if([BitConverter]::ToUInt32($objectBytes,$at) -eq 0){
  $nameAt=$strings+[BitConverter]::ToUInt32($objectBytes,$at+4);$limit=$objectBytes.Length
 }else{$nameAt=$at;$limit=$at+8}
 $end=$nameAt
 while($end -lt $limit -and $objectBytes[$end]){$end++}
 $name=[Text.Encoding]::ASCII.GetString($objectBytes,$nameAt,$end-$nameAt)
 if($name -eq '_VdmTib'){
  $bad=[byte[]]$objectBytes.Clone();$bad[$at+12]=0;$bad[$at+13]=0
  [IO.File]::WriteAllBytes("$root/common.obj",$bad)
  Check common-definition $Map "$root/common.obj" $false
  $section=[BitConverter]::ToUInt16($objectBytes,$at+12)
  $header=20+[BitConverter]::ToUInt16($objectBytes,16)+($section-1)*40
  $bad=[byte[]]$objectBytes.Clone()
  [BitConverter]::GetBytes([uint32]0xfc0).CopyTo($bad,$header+16)
  [IO.File]::WriteAllBytes("$root/short-storage.obj",$bad)
  Check short-storage $Map "$root/short-storage.obj" $false
  $found=$true;break
 }
 $i+=1+$objectBytes[$at+17]
}
if(!$found){throw 'Actual COFF definition missing'}
@{checker=(Get-FileHash $Checker).Hash;map=(Get-FileHash $Map).Hash;object=(Get-FileHash $Object).Hash;cases=$cases}|ConvertTo-Json -Depth 5|Set-Content "$root/result.json"
Write-Output "PASS: $($cases.Count) actual-product and rejection cases"
exit 0
