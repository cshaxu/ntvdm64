[CmdletBinding()]
param([Parameter(Mandatory)][string]$Plan,
 [Parameter(Mandatory)][string]$BackupRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$planPath=(Resolve-Path -LiteralPath $Plan).Path
if(!$planPath.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)){throw 'Build-owned prepared plan required'}
$data=Get-Content -LiteralPath $planPath -Raw|ConvertFrom-Json
$root=[IO.Path]::GetFullPath($data.menuRoot).TrimEnd('\')
if($data.schema -ne 1 -or $root -ine (Join-Path $data.windows 'STRTMENU')){throw 'Unexpected prepared menu root'}
$roots=@{menu=$root;apps=(Join-Path $data.windows 'APPS95')}
$sources=@{menu=$data.sourceRoot;apps=$data.programSourceRoot}
if((Test-Path -LiteralPath $roots.apps) -and @(Get-ChildItem -LiteralPath $roots.apps -Force).Count){throw 'Preserve existing APPS95 data; expected an empty/new short-path payload root'}
if(@(Get-Process -Name ntvdm -ErrorAction SilentlyContinue).Count){throw 'Close running VDM sessions before menu installation'}
$backup=[IO.Path]::GetFullPath($BackupRoot)
if(Test-Path -LiteralPath $backup){throw 'Fresh backup required'}
foreach($allowed in $roots.Values){if($backup -ieq $allowed -or $backup.StartsWith($allowed+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Backup outside prepared roots required'}}
$release=Get-Content "$repo/assets/release/addon-manifest.json" -Raw|ConvertFrom-Json
$lnk=@($release.utilities|Where-Object id -eq 'lnk')
if($lnk.Count -ne 1 -or $lnk[0].sha256 -ne $data.lnkSha256){throw 'Plan must use the released LNK.EXE identity'}
$seen=[Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
foreach($row in $data.files){
 if($row.role -notin @('menu','apps') -or [IO.Path]::IsPathRooted($row.relative) -or ($row.relative -split '[\\/]') -contains '..'){throw 'Unsafe relative prepared name'}
 if($row.role -eq 'menu' -and [IO.Path]::GetExtension($row.relative) -notin @('.LNK','.PIF')){throw 'Unexpected menu artifact'}
 $allowed=$roots[$row.role]
 $dest=[IO.Path]::GetFullPath($row.destination)
 if(!$dest.StartsWith($allowed+'\',[StringComparison]::OrdinalIgnoreCase) -or !$seen.Add($dest)){throw 'Unsafe or duplicate prepared destination'}
 if($dest -ine [IO.Path]::GetFullPath((Join-Path $allowed $row.relative)) -or !$row.source.StartsWith($sources[$row.role]+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Prepared source/destination mismatch'}
 $candidate=(Resolve-Path -LiteralPath $row.candidate).Path
 if(!$candidate.StartsWith((Split-Path $planPath)+'\'+$row.role+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Candidate outside owned prepared stage'}
 if((Get-FileHash -LiteralPath $candidate).Hash -ne $row.sha256 -or (Get-FileHash -LiteralPath $row.source).Hash -ne $row.sourceSha256){throw 'Candidate/original identity changed'}
}
New-Item -ItemType Directory $backup|Out-Null
Copy-Item -LiteralPath $planPath "$backup/plan.json"
$saved=@();$dirs=[Collections.Generic.List[string]]::new();$done=$false
try {
 foreach($row in $data.files){
  $allowed=$roots[$row.role];$dest=[IO.Path]::GetFullPath($row.destination);$record=@{path=$dest;exists=[IO.File]::Exists($dest);role=$row.role;relative=$row.relative;installedSha256=$row.sha256}
  if($record.exists){$before=Join-Path "$backup/previous/$($row.role)" $row.relative;New-Item -ItemType Directory (Split-Path $before) -Force|Out-Null;[IO.File]::Copy($dest,$before,$false);$record.backup=$before;$record.sha256=(Get-FileHash -LiteralPath $dest).Hash;$record.attributes=(Get-Item -LiteralPath $dest -Force).Attributes}
  $saved+=,$record
  $original=Join-Path "$backup/originals/$($row.role)" $row.relative;New-Item -ItemType Directory (Split-Path $original) -Force|Out-Null;[IO.File]::Copy($row.source,$original,$false)
  $missing=[Collections.Generic.List[string]]::new();$parent=Split-Path $dest
  while(!(Test-Path -LiteralPath $parent)){if($parent -ine $allowed -and !$parent.StartsWith($allowed+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Directory escaped prepared root'};$missing.Add($parent);$parent=Split-Path $parent}
  for($i=$missing.Count-1;$i -ge 0;$i--){New-Item -ItemType Directory -Path $missing[$i]|Out-Null;$dirs.Add($missing[$i])}
  [IO.File]::Copy($row.candidate,$dest,$true)
  if((Get-FileHash -LiteralPath $dest).Hash -ne $row.sha256){throw 'Installed menu identity mismatch'}
 }
 @{installed=$true;files=@($data.files).Count;menuRoot=$root;lnkSha256=$data.lnkSha256;before=$saved;createdDirectories=@($dirs)}|ConvertTo-Json -Depth 5|Set-Content "$backup/install.json"
 $done=$true
}finally{
 if(!$done){foreach($row in $saved){if($row.exists){[IO.File]::Copy($row.backup,$row.path,$true);[IO.File]::SetAttributes($row.path,$row.attributes)}elseif([IO.File]::Exists($row.path)){Remove-Item -LiteralPath $row.path -Force}};for($i=$dirs.Count-1;$i -ge 0;$i--){if((Test-Path -LiteralPath $dirs[$i]) -and @(Get-ChildItem -LiteralPath $dirs[$i] -Force).Count -eq 0){Remove-Item -LiteralPath $dirs[$i]}}}
}
'PASS installed repaired menu and exact program copies; original trees, launch profile and hives/INIs unchanged'
