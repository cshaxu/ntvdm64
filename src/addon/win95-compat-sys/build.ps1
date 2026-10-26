[CmdletBinding()]
param([Parameter(Mandatory)][string]$OutputRoot,[string]$Nasm='nasm',
      [switch]$CompatibilityOnly)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../../..").Path
$out=[IO.Path]::GetFullPath($OutputRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh repository build/ output required'}
New-Item -ItemType Directory $out|Out-Null
$defines=@(if($CompatibilityOnly){'-DCOMPAT95_NO_LOGO=1'})
& $Nasm @defines -I "$PSScriptRoot/" -f bin "$PSScriptRoot/compat.asm" -o "$out/compat95.sys" -l "$out/compat95.lst"
if($LASTEXITCODE){throw 'NASM failed'}
$sources=@('compat.asm','bootlogo.inc','root.inc')|ForEach-Object {@{name=$_;sha256=(Get-FileHash (Join-Path $PSScriptRoot $_)).Hash}}
@{file='compat95.sys';driverSha256=(Get-FileHash "$out/compat95.sys").Hash;bytes=(Get-Item "$out/compat95.sys").Length;sourceSha256=(Get-FileHash "$PSScriptRoot/compat.asm").Hash;sources=@($sources);compatibilityOnly=[bool]$CompatibilityOnly;nasm=(& $Nasm -v)}|ConvertTo-Json -Depth 4|Set-Content "$out/build.json"
Get-Content "$out/build.json"
