param([Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($OutputRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned output required'}
New-Item -ItemType Directory $root|Out-Null
$cache="$repo/build/.ninja/fixtures-x86/guest-probes/win31-profile"
$cases=@(
 @('SCHED.COM','test/integration/multitask/win31-scheduling-child.asm',@()),
 @('FULLKEY.COM','test/observation/win31_fullscreen_key.asm',@()),
 @('RUNA.COM','test/observation/win31_dos_pair.asm',@("-DROLE='A'","-DPEER='B'")),
 @('RUNB.COM','test/observation/win31_dos_pair.asm',@("-DROLE='B'","-DPEER='A'")),
 @('PROMPT.EXE','test/observation/win31_startup_contract.asm',@('-DW31_PROMPT_CHECK')),
 @('PAIR.EXE','test/observation/win31_startup_contract.asm',@('-DW31_PROMPT_CHECK','-DW31_PROMPT_PAIR')),
 @('SCHEDULE.EXE','test/observation/win31_startup_contract.asm',@('-DW31_PROMPT_CHECK','-DW31_SCHEDULING_CHECK')),
 @('INTERACT.EXE','test/observation/win31_startup_contract.asm',@('-DW31_PROMPT_CHECK','-DW31_SCHEDULING_CHECK','-DW31_SCHEDULING_INTERACTIVE')))
$index=0
foreach($install in @('O:\tmp\W31TEST','O:\tmp\W95 TEST')){
 & "$repo/tools/build-fixtures.cmd" x86 win31-profile $install *> "$root/build-$index.log"
 if($LASTEXITCODE){throw 'Native profile build failed'}
 foreach($case in $cases){
  $name,$source,$flags=$case
  # Old assembler invocation is a comparison oracle only, never a product input.
  & nasm -f bin @flags "-DW31_INSTALL='$install'" "$repo/$source" -o "$root/$index-$name"
  if($LASTEXITCODE -or (Get-FileHash "$cache/$name").Hash -ne (Get-FileHash "$root/$index-$name").Hash){throw "Profile bytes differ: $index/$name"}
 }
 $before=@($cases|ForEach-Object {(Get-Item "$cache/$($_[0])").LastWriteTimeUtc.Ticks})
 & "$repo/tools/build-fixtures.cmd" x86 win31-profile $install *> "$root/noop-$index.log"
 if($LASTEXITCODE){throw 'No-op profile build failed'}
 $after=@($cases|ForEach-Object {(Get-Item "$cache/$($_[0])").LastWriteTimeUtc.Ticks})
 if(Compare-Object $before $after){throw 'No-op rebuilt a witness'}
 $index++
}
& "$repo/tools/build-fixtures.cmd" x86 win31-profile 'invalid-relative' *> "$root/rejected.log"
if(!$LASTEXITCODE){throw 'Invalid install path accepted'}
& "$repo/tools/build-fixtures.cmd" x86 win31-profile 'O:\tmp\W95 TEST' *> "$root/restore.log"
if($LASTEXITCODE){throw 'Restore valid fixture cache after negative failed'}
foreach($file in @('verify-win31-setup-modes.ps1','verify-win95-launch-matrix.ps1')){
 $path="$repo/test/integration/multitask/$file";$tokens=$null;$errors=$null
 $null=[Management.Automation.Language.Parser]::ParseFile($path,[ref]$tokens,[ref]$errors)
 if($errors){throw ($errors|Out-String)}
 if((Get-Content $path -Raw) -match '& nasm'){throw 'Matrix still owns assembler invocation'}
}
'PASS eight matrix witnesses/two install paths byte-identical, cached no-op, path rejection and both callers'
