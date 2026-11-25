param([Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($OutputRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned output required'}
New-Item -ItemType Directory $root|Out-Null
$names=@('SCHED.COM','FULLKEY.COM','RUNA.COM','RUNB.COM','PROMPT.EXE','PAIR.EXE','SCHEDULE.EXE','INTERACT.EXE')
$index=0;$receipts=@{}
foreach($install in @('O:\tmp\W31TEST','O:\tmp\W95 TEST')){
 & "$repo/tools/build-fixtures.cmd" x86 win31-profile $install *> "$root/build-$index.log"
 if($LASTEXITCODE){throw 'Native profile build failed'}
 $line=Select-String -LiteralPath "$root/build-$index.log" -Pattern '^NTVDM_PROFILE_BIN=(.+)$'
 if(!$line){throw 'Profile build did not report its output directory'}
 $cache=$line.Matches[0].Groups[1].Value
 if($receipts.ContainsKey($cache)){throw 'Different installations used the same fixture cache'}
 $receipts[$cache]=@($names|ForEach-Object {(Get-FileHash "$cache/$_").Hash})
 foreach($name in $names){
  if(!(Test-Path "$cache/$name") -or !(Get-Item "$cache/$name").Length){throw "Missing profile image: $name"}
 }
 $before=@($names|ForEach-Object {(Get-Item "$cache/$_").LastWriteTimeUtc.Ticks})
 & "$repo/tools/build-fixtures.cmd" x86 win31-profile $install *> "$root/noop-$index.log"
 if($LASTEXITCODE){throw 'No-op profile build failed'}
 $after=@($names|ForEach-Object {(Get-Item "$cache/$_").LastWriteTimeUtc.Ticks})
 if(Compare-Object $before $after){throw 'No-op rebuilt a witness'}
 $expected=$install+'\PATCH\'
 foreach($name in $names){
  $image=[Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes("$cache/$name"))
  if(!$image.Contains($expected)){throw "Wrong guest path in $name"}
 }
 $index++
}
& "$repo/tools/build-fixtures.cmd" x86 win31-profile 'invalid-relative' *> "$root/rejected.log"
if(!$LASTEXITCODE){throw 'Invalid install path accepted'}
foreach($cache in $receipts.Keys){
 $after=@($names|ForEach-Object {(Get-FileHash "$cache/$_").Hash})
 if(Compare-Object $receipts[$cache] $after){throw 'Later profile builds changed earlier witnesses'}
}
foreach($file in @('verify-win31-setup-modes.ps1','verify-win95-launch-matrix.ps1')){
 $path="$repo/test/integration/multitask/$file";$tokens=$null;$errors=$null
 $null=[Management.Automation.Language.Parser]::ParseFile($path,[ref]$tokens,[ref]$errors)
 if($errors){throw ($errors|Out-String)}
 if((Get-Content $path -Raw) -match '& nasm'){throw 'Matrix still owns assembler invocation'}
}
'PASS eight matrix witnesses/two install paths, isolated cached no-op, path rejection and both callers'
