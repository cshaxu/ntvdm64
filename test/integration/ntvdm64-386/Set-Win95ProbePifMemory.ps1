[CmdletBinding()]
param([Parameter(Mandatory)][string]$Path)
$ErrorActionPreference='Stop'
$pif=[IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $Path).Path)
$text=[Text.Encoding]::ASCII.GetString($pif)
$standard=$text.IndexOf('MICROSOFT PIFEX')
$header=$text.IndexOf('WINDOWS 386 3.0')
if($standard -lt 36 -or $header -lt $standard -or $header+22 -gt $pif.Length){throw 'Invalid generated PIF headers'}
$body=[BitConverter]::ToUInt16($pif,$header+18)
if($body -lt $header+22 -or $body+4 -gt $pif.Length){throw 'Invalid generated PIF memory fields'}
foreach($offset in @(32,$body)){[Array]::Copy([BitConverter]::GetBytes([uint16]640),0,$pif,$offset,2)}
foreach($offset in @(34,($body+2))){[Array]::Copy([BitConverter]::GetBytes([uint16]128),0,$pif,$offset,2)}
$sum=0;for($i=2;$i -lt $standard;$i++){$sum=($sum+$pif[$i]) -band 255};$pif[1]=[byte]$sum
[IO.File]::WriteAllBytes((Resolve-Path -LiteralPath $Path).Path,$pif)
