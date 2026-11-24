param([Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($OutputRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned output required'}
New-Item -ItemType Directory $root|Out-Null
$cache="$repo/build/.ninja/fixtures-x86/guest-probes/win31-profile"
$names=@('SCHED.COM','FULLKEY.COM','RUNA.COM','RUNB.COM','PROMPT.EXE','PAIR.EXE','SCHEDULE.EXE','INTERACT.EXE')
$index=0
foreach($install in @('O:\tmp\W31TEST','O:\tmp\W95 TEST')){
 & "$repo/tools/build-fixtures.cmd" x86 win31-profile $install *> "$root/build-$index.log"
 if($LASTEXITCODE){throw 'Native profile build failed'}
 foreach($name in $names){
  if(!(Test-Path "$cache/$name") -or !(Get-Item "$cache/$name").Length){throw "Missing profile image: $name"}
 }
 $before=@($names|ForEach-Object {(Get-Item "$cache/$_").LastWriteTimeUtc.Ticks})
 & "$repo/tools/build-fixtures.cmd" x86 win31-profile $install *> "$root/noop-$index.log"
 if($LASTEXITCODE){throw 'No-op profile build failed'}
 $after=@($names|ForEach-Object {(Get-Item "$cache/$_").LastWriteTimeUtc.Ticks})
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
'PASS eight matrix witnesses/two install paths, cached no-op, path rejection and both callers'
