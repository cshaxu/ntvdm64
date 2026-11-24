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
# Shared controller mode retains every unrelated byte and validates before save.
Reset-Inputs
$fields=@(@('@W31_BOOT_PATH@',"$directory\BOOT.BIN"),@('@W31_READY_PATH@',"$directory\READY.BIN"))
& $Binder --fields "$root/W.EXE" $fields[0][0] $fields[0][1] $fields[1][0] $fields[1][1]
if($LASTEXITCODE){throw 'Controller binding failed'}
Same-ExceptFields $originalWin ([IO.File]::ReadAllBytes("$root/W.EXE")) $fields
foreach($case in @('missing-later','duplicate-argument','too-long','foreign-marker','odd-arguments')){
 Reset-Inputs
 $before=(Get-FileHash "$root/W.EXE").Hash
 $args=@('--fields',"$root/W.EXE",'@W31_BOOT_PATH@',"$directory\BOOT.BIN")
 switch($case){
  missing-later {$args+=@('@VM_MISSING@','X')}
  duplicate-argument {$args+=@('@W31_BOOT_PATH@','X')}
  too-long {$args+=@('@W31_READY_PATH@',('X'*128))}
  foreign-marker {$args+=@('not-a-witness-field','X')}
  odd-arguments {$args+='@W31_READY_PATH@'}
 }
 & $Binder @args 2> "$root/controller-$case.stderr"
 if(!$LASTEXITCODE -or (Get-FileHash "$root/W.EXE").Hash -ne $before){throw "Controller negative failed: $case"}
}
'PASS controller fields and five rejection cases without partial writes'
# Real identity/editor controllers: compare the complete result with old filling.
$variants=@('', '-DREUSE=1', '-DSHUTDOWN_WITNESS=1', '-DPRODUCTION_GUEST_INPUT=1', '-DREUSE=1;-DSHUTDOWN_WITNESS=1;-DPRODUCTION_GUEST_INPUT=1')
$index=0
foreach($defines in $variants){
 $file="$root/controller-$index.exe"
 & $env:ComSpec /d /s /c ('""'+$repo+'/tools/build-vm-identity.cmd" "'+$file+'" controller "'+$defines+'""') *> "$root/controller-$index-build.log"
 if($LASTEXITCODE){throw 'Identity controller build failed'}
 $index++
}
& $env:ComSpec /d /s /c ('""'+$repo+'/tools/build-win95-editor.cmd" "'+$root+'/editor.exe" "O:\tmp\W31TEST" "" """') *> "$root/editor-build.log"
if($LASTEXITCODE){throw 'Editor controller build failed'}
foreach($file in @((Get-ChildItem "$root/controller-*.exe").FullName)+@("$root/editor.exe")){
 $before=[IO.File]::ReadAllBytes($file);$expected=[byte[]]$before.Clone()
 $matches=[regex]::Matches($ascii.GetString($before),'@(VM|W31)_[A-Z_]+@')
 $binding=@('--fields',$file);$fields=@();$slot=0
 foreach($match in $matches){
  $value="$directory\FIELD$slot.BIN";$slot++
  $binding+=@($match.Value,$value);$fields+=,@($match.Value,$value)
  [Array]::Clear($expected,$match.Index,128)
  $bytes=$ascii.GetBytes($value);[Array]::Copy($bytes,0,$expected,$match.Index,$bytes.Length)
 }
 if(!$slot){throw 'No controller fields'}
 & $Binder @binding
 if($LASTEXITCODE){throw 'Real controller binding failed'}
 $after=[IO.File]::ReadAllBytes($file)
 if([Convert]::ToBase64String($expected) -cne [Convert]::ToBase64String($after)){throw 'Old/new controller bytes differ'}
 Same-ExceptFields $before $after $fields
}
'PASS five actual identity variants and Win95 editor controller byte-equivalent'
