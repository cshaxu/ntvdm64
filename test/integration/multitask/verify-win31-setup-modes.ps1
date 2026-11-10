[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$InstallSource,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$OutputRoot
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../../..").Path
$output=[IO.Path]::GetFullPath($OutputRoot)
if(!$output.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $output)){throw 'Fresh build-owned output required'}
New-Item -ItemType Directory $output|Out-Null
$savedPath=$env:PATH;$savedPause=$env:NTVDM_SETUP_TEST_NO_PAUSE
foreach($mode in 0,1){
 $task=Join-Path $output "mode$mode"
 $copy=& "$repo/test/integration/ntvdm64-386/New-Win31ProbeCopy.ps1" `
  -DriverImage "$repo/assets/release/ntvdm64.386" -InstanceDriver "$repo/assets/release/insignia.386" `
  -InstallSource $InstallSource -InstallRoot 'O:/tmp/q31' -OutputRoot $task
 $dest=$copy.Destination
 try {
  # Remove only the two exact entries and files added by the copy helper.
  $ini="$dest/SYSTEM.INI";$enc=[Text.Encoding]::GetEncoding(28591)
  $text=$enc.GetString([IO.File]::ReadAllBytes($ini))
  foreach($name in 'INSIGNIA.386','NTVDM64.386'){
   $line='(?im)^device='+[regex]::Escape("$dest\PATCH\$name")+'\r*\n'
   if([regex]::Matches($text,$line).Count -ne 1){throw "Expected one copied driver entry: $name"}
   $text=[regex]::Replace($text,$line,'')
   if((Get-FileHash "$dest/PATCH/$name").Hash -ne (Get-FileHash "$repo/assets/release/$name").Hash){throw "Unexpected copied driver: $name"}
   Remove-Item -LiteralPath "$dest/PATCH/$name"
  }
  [IO.File]::WriteAllBytes($ini,$enc.GetBytes($text))
  foreach($name in 'CONFIG.NT','AUTOEXEC.NT','WINSTD.PIF','WIN386.PIF','WINSTD.CMD','WIN386.CMD'){
   $path=Join-Path "$dest/PATCH" $name;if(Test-Path -LiteralPath $path){Remove-Item -LiteralPath $path}
  }
  $guest=@{};foreach($name in 'WIN.COM','SYSTEM/DOSX.EXE','SYSTEM/KRNL386.EXE','SYSTEM/WIN386.EXE'){$guest[$name]=(Get-FileHash "$dest/$name").Hash}
  $media=Join-Path $task 'media/PATCH';$shim=Join-Path $task 'shim'
  New-Item -ItemType Directory -Path $media,$shim|Out-Null
  Copy-Item "$repo/tools/win31-setup/SETUP.CMD" "$media/SETUP.CMD"
  Copy-Item "$repo/tools/win-multitask/APPLY.CMD" "$media/MULTITASK.CMD"
  foreach($name in 'PIF.EXE','HASH.EXE','INSIGNIA.386','NTVDM64.386'){Copy-Item "$repo/assets/release/$name" "$media/$name"}
  [IO.File]::WriteAllText("$shim/run16.cmd","@echo off`r`nexit /b 0`r`n",[Text.Encoding]::ASCII)
  [IO.File]::WriteAllText("$task/input.txt","$dest`r`n$mode`r`n",[Text.Encoding]::ASCII)
  $env:PATH="$shim;$savedPath";$env:NTVDM_SETUP_TEST_NO_PAUSE='1'
  & cmd.exe /d /c "`"$media/SETUP.CMD`" < `"$task/input.txt`"" *> "$task/setup.log"
  if($LASTEXITCODE){throw "Setup profile generation failed for DOSONLY=$mode"}
  $env:PATH=$savedPath
  $config=Get-Content "$dest/PATCH/CONFIG.NT" -Raw
  if(($config -match '(?im)^dosonly\s*$') -ne ($mode -eq 1)){throw "Wrong DOSONLY=$mode configuration"}
  foreach($name in 'INSIGNIA.386','NTVDM64.386'){
   $installed=Test-Path -LiteralPath "$dest/PATCH/$name"
   if($installed -ne ($mode -eq 0)){throw "Wrong DOSONLY=$mode driver state: $name"}
   if($installed -and (Get-FileHash "$dest/PATCH/$name").Hash -ne (Get-FileHash "$repo/assets/release/$name").Hash){throw "Driver differs: $name"}
  }
  foreach($name in $guest.Keys){if((Get-FileHash "$dest/$name").Hash -ne $guest[$name]){throw "Guest binary changed: $name"}}
  & "$repo/test/integration/win31_program_groups_test.ps1" -InstallRoot $dest
  $cases=if($mode -eq 0){@('std-dosonly0','386-dosonly0')}else{@('std-dosonly1','386-dosonly1')}
  & "$repo/test/integration/win31_runtime_matrix_test.ps1" -PackageRoot $PackageRoot -InstallRoot $dest -Observer $Observer -OutputRoot "$task/runtime" -Cases $cases
 } finally {
  $env:PATH=$savedPath;$env:NTVDM_SETUP_TEST_NO_PAUSE=$savedPause
  try {
   if(@(Get-ChildItem $copy.Source -Force -Recurse -File).Count -ne $copy.Original.Count){throw 'Source installation inventory changed'}
   foreach($row in $copy.Original){if((Get-FileHash ($copy.Source+$row.path)).Hash -ne $row.sha256){throw "Source installation changed: $($row.path)"}}
  } finally {
   . "$repo/tests/observation/isolated_package_cleanup.ps1"
   Remove-OwnedGuestProbeCopy $dest $task $copy.Source
  }
 }
}
'PASS both actual setup selections and four Win3.1 startup/exit cases; Setup itself used a test shim'
