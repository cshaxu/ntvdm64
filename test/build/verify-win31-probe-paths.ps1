param([Parameter(Mandatory)][string]$Binder,
      [Parameter(Mandatory)][string]$WindowsTemplate,
      [Parameter(Mandatory)][string]$DosTemplate,
      [Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($OutputRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned output required'}
$null=New-Item -ItemType Directory $root
$originalWin=[IO.File]::ReadAllBytes((Resolve-Path $WindowsTemplate).Path)
$originalDos=[IO.File]::ReadAllBytes((Resolve-Path $DosTemplate).Path)
$ascii=[Text.Encoding]::ASCII
function Reset-Inputs {
 [IO.File]::WriteAllBytes("$root/W.EXE",$originalWin)
 [IO.File]::WriteAllBytes("$root/D.COM",$originalDos)
}
function Same-ExceptFields($before,$after,$fields) {
 if($before.Length -ne $after.Length){throw 'Image size changed'}
 $allowed=[Collections.Generic.HashSet[int]]::new()
 foreach($field in $fields){
  $at=$ascii.GetString($before).IndexOf($field[0],[StringComparison]::Ordinal)
  if($at -lt 0){throw 'Template marker missing'}
  $value=$ascii.GetBytes($field[1])
  for($i=0;$i -lt 128;$i++){
   $null=$allowed.Add($at+$i)
   $expected=if($i -lt $value.Length){$value[$i]}else{0}
   if($after[$at+$i] -ne $expected){throw 'Wrong bound field or padding'}
  }
 }
 for($i=0;$i -lt $before.Length;$i++){
  if(!$allowed.Contains($i) -and $before[$i] -ne $after[$i]){throw 'Unrelated image byte changed'}
 }
}
Reset-Inputs
$directory='O:\tmp\W31TEST\PATCH'
& $Binder "$root/W.EXE" "$root/D.COM" $directory
if($LASTEXITCODE){throw 'Valid binding failed'}
Same-ExceptFields $originalWin ([IO.File]::ReadAllBytes("$root/W.EXE")) @(@('@W31_BOOT_PATH@',"$directory\W31BOOT.BIN"),@('@W31_READY_PATH@',"$directory\W31READY.BIN"))
Same-ExceptFields $originalDos ([IO.File]::ReadAllBytes("$root/D.COM")) @(,@('@W31_DOS_PATH@',"$directory\W31DOS.BIN"))
foreach($case in @('missing','duplicate','truncated','long-path')){
 Reset-Inputs
 switch($case){
  missing {[IO.File]::WriteAllBytes("$root/D.COM",[byte[]]::new(256))}
  duplicate {[IO.File]::WriteAllBytes("$root/D.COM",[byte[]]($originalDos+$originalDos))}
  truncated {[IO.File]::WriteAllBytes("$root/D.COM",$ascii.GetBytes('@W31_DOS_PATH@'))}
 }
 $before=@(Get-FileHash "$root/W.EXE","$root/D.COM"|ForEach-Object Hash)
 $path=if($case -eq 'long-path'){'O:\'+('X'*128)}else{$directory}
 & $Binder "$root/W.EXE" "$root/D.COM" $path 2> "$root/$case.stderr"
 if(!$LASTEXITCODE){throw "Accepted invalid case: $case"}
 $after=@(Get-FileHash "$root/W.EXE","$root/D.COM"|ForEach-Object Hash)
 if(Compare-Object $before $after){throw "Invalid case modified an image: $case"}
}
'PASS exact authored image bytes/fields and four rejection cases without partial writes'
