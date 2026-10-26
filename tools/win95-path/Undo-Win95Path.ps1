[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$WindowsRoot)
$ErrorActionPreference='Stop';$WindowsRoot=[IO.Path]::GetFullPath($WindowsRoot).TrimEnd('\');$m=Join-Path $WindowsRoot 'PATCH\W95PATH.MANIFEST'
if(-not(Test-Path -LiteralPath $m)){throw "Missing migration journal: $m"}
$prefix=$WindowsRoot+'\'
$items=@(Get-Content -LiteralPath $m | ForEach-Object {$_|ConvertFrom-Json})
if(!$items.Count){throw "Empty migration journal: $m"}
foreach($item in $items){
 $p=[IO.Path]::GetFullPath([string]$item.path)
 if(!$p.StartsWith($prefix,[StringComparison]::OrdinalIgnoreCase)){throw "Journal path escapes Windows root: $p"}
 $b="$p.W95PATH.BAK"
 if(-not(Test-Path -LiteralPath $p) -or -not(Test-Path -LiteralPath $b)){throw "Missing migrated file or recovery copy: $p"}
 if((Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash -ne [string]$item.installedSha256){throw "Preserve changed installed file: $p"}
}
foreach($item in $items){$p=[IO.Path]::GetFullPath([string]$item.path);$b="$p.W95PATH.BAK";Remove-Item -LiteralPath $p -Force;Move-Item -LiteralPath $b -Destination $p}
Remove-Item -LiteralPath $m -Force
Write-Host 'Win95 paths restored.'
