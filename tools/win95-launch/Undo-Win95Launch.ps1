[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$WindowsRoot,[switch]$BestEffort)
$ErrorActionPreference='Stop';$WindowsRoot=[IO.Path]::GetFullPath($WindowsRoot).TrimEnd('\');$patch=Join-Path $WindowsRoot 'PATCH';$j=Join-Path $patch 'W95LAUNCH.MANIFEST'
if(-not(Test-Path -LiteralPath $j)){if($BestEffort){return};throw "Missing launch journal: $j"}
$prefix=$WindowsRoot+'\';$items=@(Get-Content -LiteralPath $j|ForEach-Object {$_|ConvertFrom-Json});if(!$items.Count){throw "Empty launch journal: $j"}
foreach($item in $items){$p=[IO.Path]::GetFullPath([string]$item.path);if(!$p.StartsWith($prefix,[StringComparison]::OrdinalIgnoreCase)){throw "Journal path escapes Windows root: $p"};if(-not(Test-Path -LiteralPath $p)){throw "Missing owned file: $p"};if((Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash -ne [string]$item.installedSha256){throw "Preserve changed installed file: $p"};if($item.kind -eq 'replace' -and -not(Test-Path -LiteralPath "$p.W95LAUNCH.BAK")){throw "Missing recovery copy: $p.W95LAUNCH.BAK"}}
[array]::Reverse($items);foreach($item in $items){$p=[IO.Path]::GetFullPath([string]$item.path);if($item.kind -eq 'replace'){Remove-Item -LiteralPath $p -Force;Move-Item -LiteralPath "$p.W95LAUNCH.BAK" -Destination $p -Force}else{Remove-Item -LiteralPath $p -Force}};Remove-Item -LiteralPath $j -Force
 $pathUndo=Join-Path $PSScriptRoot '..\win95-path\Undo-Win95Path.ps1'
 if((Test-Path -LiteralPath $pathUndo) -and (Test-Path -LiteralPath (Join-Path $patch 'W95PATH.MANIFEST'))){& $pathUndo -WindowsRoot $WindowsRoot}
Write-Host 'Win95 launcher restored.'
