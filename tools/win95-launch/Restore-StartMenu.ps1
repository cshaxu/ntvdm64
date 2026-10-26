[CmdletBinding()]
param([Parameter(Mandatory)][string]$BackupRoot)
$ErrorActionPreference='Stop'
$backup=(Resolve-Path -LiteralPath $BackupRoot).Path
$state=Get-Content "$backup/install.json" -Raw|ConvertFrom-Json
$plan=Get-Content "$backup/plan.json" -Raw|ConvertFrom-Json
if(!$state.installed -or $plan.schema -ne 1){throw 'Expected a completed owned menu installation'}
if(@(Get-CimInstance Win32_Process -Filter "Name='ntvdm.exe'").Count){throw 'Close running VDMs before restoring menu configuration'}
$roots=@{menu=[IO.Path]::GetFullPath($plan.menuRoot);apps=(Join-Path $plan.windows 'APPS95')}
foreach($row in $state.before){
 if($row.role -notin @('menu','apps')){throw 'Unknown installed artifact role'}
 $root=$roots[$row.role];$path=[IO.Path]::GetFullPath($row.path)
 if(!$path.StartsWith($root+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Installed path escaped its owned root'}
 if(![IO.File]::Exists($path) -or (Get-FileHash -LiteralPath $path).Hash -ne $row.installedSha256){throw "Preserve changed/missing installed file: $path"}
 if($row.exists){$original=(Resolve-Path -LiteralPath $row.backup).Path;if(!$original.StartsWith($backup+'\previous\',[StringComparison]::OrdinalIgnoreCase) -or (Get-FileHash -LiteralPath $original).Hash -ne $row.sha256){throw 'Invalid rollback identity'}}
}
foreach($row in $state.before){if($row.exists){[IO.File]::Copy($row.backup,$row.path,$true);[IO.File]::SetAttributes($row.path,$row.attributes)}else{Remove-Item -LiteralPath $row.path -Force}}
for($i=$state.createdDirectories.Count-1;$i -ge 0;$i--){
 $path=[IO.Path]::GetFullPath($state.createdDirectories[$i]);$owned=$false
 foreach($root in $roots.Values){if($path -ieq $root -or $path.StartsWith($root+'\',[StringComparison]::OrdinalIgnoreCase)){$owned=$true}}
 if(!$owned){throw 'Unexpected rollback directory'}
 if((Test-Path -LiteralPath $path) -and @(Get-ChildItem -LiteralPath $path -Force).Count -eq 0){Remove-Item -LiteralPath $path}
}
@{restored=$true;utc=[DateTime]::UtcNow.ToString('o')}|ConvertTo-Json|Set-Content "$backup/restored.json"
'PASS restored prior menu and removed only unchanged owned files; other user files preserved'
