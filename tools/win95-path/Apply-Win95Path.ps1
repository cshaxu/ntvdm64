[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$WindowsRoot)
$ErrorActionPreference='Stop'
function Fail([string]$m){Write-Error $m;exit 1}
function Copy-Replace([string]$p,[scriptblock]$writer){
  $candidate="$p.W95NEW"; if(Test-Path -LiteralPath $candidate){Fail "Stale candidate: $candidate"}
  & $writer $candidate; if($LASTEXITCODE -ne 0 -or -not(Test-Path -LiteralPath $candidate)){Fail "Could not prepare $p"}
  $before=(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash
  $after=(Get-FileHash -LiteralPath $candidate -Algorithm SHA256).Hash
  if($before -eq $after){Remove-Item -LiteralPath $candidate -Force;return}
  $backup="$p.W95PATH.BAK"; if(Test-Path -LiteralPath $backup){Fail "Recovery copy already exists: $backup"}
  Move-Item -LiteralPath $p -Destination $backup
  try {
    Move-Item -LiteralPath $candidate -Destination $p
    @{path=$p;installedSha256=$after}|ConvertTo-Json -Compress|Add-Content -LiteralPath $script:manifest
  } catch {
    if(Test-Path -LiteralPath $backup){Remove-Item -LiteralPath $p -Force -ErrorAction SilentlyContinue;Move-Item -LiteralPath $backup -Destination $p -Force}
    throw
  }
}
$WindowsRoot=[IO.Path]::GetFullPath($WindowsRoot).TrimEnd('\')
if(-not(Test-Path -LiteralPath "$WindowsRoot\WIN.COM")){Fail "WIN.COM was not found below $WindowsRoot"}
if(((Get-Item -LiteralPath $WindowsRoot -Force).Attributes -band [IO.FileAttributes]::ReparsePoint)){Fail "Refusing Windows root reparse point: $WindowsRoot"}
if(@(Get-ChildItem -LiteralPath $WindowsRoot -Recurse -Force | Where-Object { $_.Attributes -band [IO.FileAttributes]::ReparsePoint }).Count){Fail "Refusing tree containing reparse point: $WindowsRoot"}
$tool=Join-Path $PSScriptRoot 'WIN95PATH.EXE';$release=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\..\assets\release'))
foreach($f in @($tool,"$release\PIF.EXE","$release\GRP.EXE","$release\LNK.EXE")){if(-not(Test-Path -LiteralPath $f)){Fail "Missing required tool: $f"}}
$line=(& $tool root "$WindowsRoot\SYSTEM.DAT" | Where-Object {$_ -like 'OLD_ROOT=*'} | Select-Object -First 1)
if(-not $line){Fail 'SYSTEM.DAT did not provide a unique SystemRoot'}
$old=$line.Substring(9)
if($old -ieq $WindowsRoot){Write-Host 'Win95 paths already match.';return}
$patch=Join-Path $WindowsRoot 'PATCH';New-Item -ItemType Directory -Force -Path $patch|Out-Null
$script:manifest=Join-Path $patch 'W95PATH.MANIFEST';if(Test-Path -LiteralPath $script:manifest){Fail "Existing migration journal: $script:manifest (run UNAPPLY.CMD first)"}
New-Item -ItemType File -Path $script:manifest|Out-Null
try {
  foreach($h in @('SYSTEM.DAT','USER.DAT')){ $p=Join-Path $WindowsRoot $h;if(Test-Path $p){Copy-Replace $p {param($o) & $tool replace $p $o $old $WindowsRoot}}}
  Get-ChildItem -LiteralPath $WindowsRoot -Recurse -File -Include *.INI | ForEach-Object { $p=$_.FullName;Copy-Replace $p {param($o) & $tool replace $p $o $old $WindowsRoot} }
  Get-ChildItem -LiteralPath $WindowsRoot -Recurse -File -Include *.PIF | ForEach-Object { $p=$_.FullName;Copy-Replace $p {param($o) Copy-Item -LiteralPath $p -Destination $o; & "$release\PIF.EXE" replace-root $o --from $old --to $WindowsRoot; if($LASTEXITCODE -ne 0){throw 'PIF rewrite failed'}} }
  Get-ChildItem -LiteralPath $WindowsRoot -Recurse -File -Include *.GRP | ForEach-Object { $p=$_.FullName;Copy-Replace $p {param($o) Copy-Item -LiteralPath $p -Destination $o; & "$release\GRP.EXE" replace-root $o --root $WindowsRoot; if($LASTEXITCODE -ne 0){throw 'GRP rewrite failed'}} }
  Get-ChildItem -LiteralPath $WindowsRoot -Recurse -File -Include *.LNK | ForEach-Object {
    $p=$_.FullName;$info=@{}; (& "$release\LNK.EXE" show $p --codepage 936) | ForEach-Object {if($_ -match '^([^=]+)=(.*)$'){$info[$matches[1]]=$matches[2]}}
    if($LASTEXITCODE -ne 0){throw "Unsupported LNK: $p"};$target=$info['TARGET'];if(-not $target -or -not $target.StartsWith($old,[StringComparison]::OrdinalIgnoreCase)){return}
    $dir=$info['DIRECTORY'];$icon=$info['ICON'];Copy-Replace $p {param($o) Copy-Item $p $o; $args=@('update',$o,'--target',$target.Replace($old,$WindowsRoot),'--codepage','936');if($dir -and $dir.StartsWith($old,[StringComparison]::OrdinalIgnoreCase)){$args+=@('--directory',$dir.Replace($old,$WindowsRoot))};if($icon -and $icon.StartsWith($old,[StringComparison]::OrdinalIgnoreCase)){$args+=@('--icon',$icon.Replace($old,$WindowsRoot))}; & "$release\LNK.EXE" @args;if($LASTEXITCODE -ne 0){throw 'LNK rewrite failed'}}
  }
  Write-Host "Migrated Win95 path: $old -> $WindowsRoot"
} catch { Write-Error $_; & "$PSScriptRoot\Undo-Win95Path.ps1" -WindowsRoot $WindowsRoot; exit 1 }
