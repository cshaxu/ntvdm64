[CmdletBinding()]
param([Parameter(Mandatory)][string]$OutputRoot,
      [Parameter(Mandatory)][string]$Wlink)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../../..").Path
$out=[IO.Path]::GetFullPath($OutputRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){
 throw 'Fresh repository build output required'
}
$linker=(Resolve-Path -LiteralPath $Wlink).Path
New-Item -ItemType Directory $out|Out-Null
Push-Location $out
try {
 & nasm -f obj "$PSScriptRoot/interrupt-observer.asm" -o observer.obj -l observer.lst
 if($LASTEXITCODE){throw 'Observer assembly failed'}
 $link=@'
format windows vxd static
option quiet
option map=observer.map
name observer.386
file observer.obj
segment _LTEXT shared resident
segment _LDATA shared resident
export observe_ddb.1
'@
 [IO.File]::WriteAllText("$out/observer.lnk",$link.Replace("`r`n","`n"))
 & $linker '@observer.lnk'
 if($LASTEXITCODE){throw 'Observer link failed'}
 @{source=(Get-FileHash "$PSScriptRoot/interrupt-observer.asm").Hash;
   image=(Get-FileHash "$out/observer.386").Hash;
   linker=(Get-FileHash $linker).Hash}|ConvertTo-Json|Set-Content "$out/build.json"
} finally {Pop-Location}
