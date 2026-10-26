[CmdletBinding()]
param([Parameter(Mandatory)][string]$OutputRoot,
 [Parameter(Mandatory)][string]$Inventory,
 [string]$WindowsRoot='O:\WINDOWS',
 [string]$ProgramFilesRoot='O:\Program Files',
 [string]$Lnk='', [string]$Pif='')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
if(!$Lnk){$Lnk=Join-Path $repo 'assets/release/LNK.EXE'}
if(!$Pif){$Pif=Join-Path $repo 'assets/release/PIF.EXE'}
$out=[IO.Path]::GetFullPath($OutputRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh repository build staging required'}
$windows=(Resolve-Path -LiteralPath $WindowsRoot).Path
$programs=(Resolve-Path -LiteralPath $ProgramFilesRoot).Path
$sourceRoot=Join-Path $windows 'STARTM~1';$menuRoot=Join-Path $windows 'STRTMENU'
$appsRoot=Join-Path $windows 'APPS95'
$promptTemplate=Join-Path $windows 'DOSPRMPT.PIF'
if(-not(Test-Path -LiteralPath $promptTemplate -PathType Leaf)){throw "Missing Win95 MS-DOS Prompt PIF template: $promptTemplate"}
# STDPIF::defpath is a 64-byte OEM field and the released writer requires the
# terminating NUL to fit strictly inside it. Refuse before staging any plan
# rather than emitting a partial candidate that PIF.EXE cannot write.
if(([Text.Encoding]::Default.GetByteCount($windows)+1) -ge 64){throw "Win95 MS-DOS Prompt PIF directory exceeds its 62-byte OEM limit: $windows"}
$promptTemplateSha256=(Get-FileHash -LiteralPath $promptTemplate).Hash
$map=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::OrdinalIgnoreCase)
foreach($row in (Get-Content -LiteralPath $Inventory -Raw|ConvertFrom-Json)){$map.Add($row.path.TrimStart('/').Replace('ß','SS'),$row)}
function StableDosName([string]$name){
 $pieces=$name.Split('.');if($pieces.Count -gt 2 -or !$pieces[0] -or $pieces[0].Length -gt 8 -or ($pieces.Count -eq 2 -and $pieces[1].Length -gt 3)){return $false}
 foreach($c in $name.ToCharArray()){if([int]$c -gt 127 -and ($c.ToString() -cne $c.ToString().ToUpperInvariant() -or $c -eq 'ß')){return $false}}
 return $true
}
$folderNames=@{'多媒体'='MEDIA';'系统工具'='SYSTEM'}
function Relocate([string]$path){
 if(!$path){return $path}
 if($path.StartsWith('C:\PROGRA~1\',[StringComparison]::OrdinalIgnoreCase)){return $appsRoot+'\'+$path.Substring(12)}
 if($path.StartsWith('C:\WINDOWS\',[StringComparison]::OrdinalIgnoreCase)){return $windows+'\'+$path.Substring(11)}
 if($path -ieq 'C:\WINDOWS'){return $windows}
 return $path.Replace('C:\','O:\').Replace('c:\','O:\')
}
New-Item -ItemType Directory "$out/menu","$out/apps" -Force|Out-Null
$files=@()
foreach($source in Get-ChildItem -LiteralPath $programs -File -Recurse -Force){
 $relative=$source.FullName.Substring($programs.Length+1);$candidate=Join-Path "$out/apps" $relative
 New-Item -ItemType Directory (Split-Path $candidate) -Force|Out-Null
 Copy-Item -LiteralPath $source.FullName -Destination $candidate
 $files+=@{role='apps';source=$source.FullName;sourceSha256=(Get-FileHash -LiteralPath $source.FullName).Hash;relative=$relative;candidate=$candidate;sha256=(Get-FileHash -LiteralPath $candidate).Hash;destination=(Join-Path $appsRoot $relative);target=$null}
}
function CheckTarget([string]$path){if($path.StartsWith($appsRoot+'\',[StringComparison]::OrdinalIgnoreCase)){$path=Join-Path "$out/apps" $path.Substring($appsRoot.Length+1)};return Test-Path -LiteralPath $path}
foreach($source in Get-ChildItem -LiteralPath $sourceRoot -File -Recurse -Force){
 if($source.Extension -notin @('.LNK','.PIF')){throw "Unexpected menu file: $($source.FullName)"}
 # Inventory paths describe the Win95 tree, not the host drive used for an
 # installed copy.  Keep the fixed archive `/WINDOWS/...` form so an approved
 # alternate WindowsRoot has the same original-name provenance lookup.
 $key=('WINDOWS/'+$source.FullName.Substring($windows.Length+1).Replace('\','/')).Replace('ß','SS')
 if(!$map.ContainsKey($key)){throw "No original long-name provenance: $key"}
 $original=$map[$key].long_path
 $prefix='/WINDOWS/Start Menu/'
 if(!$original.StartsWith($prefix,[StringComparison]::OrdinalIgnoreCase)){throw 'Unexpected menu provenance root'}
 $fields=@{}
 if($source.Extension -ieq '.LNK'){
  foreach($line in (& $Lnk show $source.FullName --codepage 936)){if($line.Contains('=')){$k,$v=$line.Split('=',2);$fields[$k]=$v}}
  if($LASTEXITCODE -or !$fields.TARGET){throw 'Menu link must have a supported local target'}
 }
 # Restore the actual FAT/OEM byte names, not Unicode uppercased extraction
 # artifacts. DBCS names that cannot survive host OEM case conversion use
 # stable ASCII program/folder names instead of unopenable pseudo-Chinese.
 $parts=$map[$key].path.Substring('/WINDOWS/STARTM~1/'.Length).Split('/')
 $longParts=$original.Substring($prefix.Length).Split('/')
 for($i=0;$i -lt $parts.Count;$i++){
  if(StableDosName $parts[$i]){continue}
  if($i -lt $parts.Count-1){if(!$folderNames.ContainsKey($longParts[$i])){throw 'Unknown unstable menu folder'};$parts[$i]=$folderNames[$longParts[$i]]}
  else{$base=[IO.Path]::GetFileNameWithoutExtension($fields.TARGET).ToUpperInvariant();if($base -notmatch '^[A-Z0-9_~-]{1,8}$'){throw 'No stable program short name'};$parts[$i]=$base+'.LNK'}
 }
 $relative=$parts -join '\'
 if([IO.Path]::IsPathRooted($relative) -or ($relative -split '\\') -contains '..'){throw 'Unsafe menu relative path'}
 $candidate=Join-Path "$out/menu" $relative
 New-Item -ItemType Directory (Split-Path $candidate) -Force|Out-Null
 $target=$null;$templateSource=$null;$templateSha256=$null
 if($source.Extension -ieq '.LNK'){
  Copy-Item -LiteralPath $source.FullName -Destination $candidate
  $target=Relocate $fields.TARGET
  if(!(CheckTarget $target)){throw "Missing relocated target: $target"}
  $directory=if($fields.DIRECTORY){Relocate $fields.DIRECTORY}else{Split-Path $target}
  if(!(CheckTarget $directory)){throw "Missing working directory: $directory"}
  $args=@('update',$candidate,'--target',$target,'--directory',$directory,'--codepage','936')
  if($fields.ARGUMENTS){$args+=@('--arguments',(Relocate $fields.ARGUMENTS))}
  if($fields.ICON){$args+=@('--icon',(Relocate $fields.ICON))}
  & $Lnk @args
  if($LASTEXITCODE){throw "Link rewrite failed: $relative"}
 }else{
  $target='O:\WINNT\system32\COMMAND.COM'
  if(!(Test-Path -LiteralPath $target)){throw 'Stock NTDOS command interpreter absent'}
  # The extracted STARTM~1 PIF is not parseable by the OpenNT editor.  The
  # installed root template is: preserve its Win95 386/task-memory records,
  # and change only the dispatchable command and working directory.
  Copy-Item -LiteralPath $promptTemplate -Destination $candidate
  & $Pif update $candidate --title 'MS-DOS Prompt' --program $target --directory $windows
  if($LASTEXITCODE){throw 'Win95 MS-DOS Prompt PIF update failed'}
  $promptView=@(& $Pif show $candidate)
  if($LASTEXITCODE -or $promptView -notcontains 'STANDARD.MAXMEM_KB=640' -or
     $promptView -notcontains 'STANDARD.MINMEM_KB=128' -or
     $promptView -notcontains 'W386.MAXMEM_KB=65535' -or
     $promptView -notcontains 'W386.MINMEM_KB=65535'){
   throw 'Win95 MS-DOS Prompt PIF template lost its conventional-memory profile'
  }
  $templateSource=$promptTemplate;$templateSha256=$promptTemplateSha256
 }
 $record=@{role='menu';source=$source.FullName;sourceSha256=(Get-FileHash -LiteralPath $source.FullName).Hash;originalLongName=$original;relative=$relative;candidate=$candidate;sha256=(Get-FileHash -LiteralPath $candidate).Hash;destination=(Join-Path $menuRoot $relative);target=$target}
 if($templateSource){$record.templateSource=$templateSource;$record.templateSha256=$templateSha256}
 $files+=$record
}
if(@($files|Where-Object {$_.role -eq 'menu' -and $_.relative -match '\.LNK$'}).Count -lt 25){throw 'Expected populated original Win95 menu'}
@{schema=1;windows=$windows;sourceRoot=$sourceRoot;programSourceRoot=$programs;menuRoot=$menuRoot;appsRoot=$appsRoot;inventorySha256=(Get-FileHash -LiteralPath $Inventory).Hash;lnkSha256=(Get-FileHash -LiteralPath $Lnk).Hash;pifSha256=(Get-FileHash -LiteralPath $Pif).Hash;files=$files}|ConvertTo-Json -Depth 5|Set-Content "$out/plan.json" -Encoding UTF8
'PASS prepared original DOS/OEM menu names and relocated shortcut copies; installation untouched'
