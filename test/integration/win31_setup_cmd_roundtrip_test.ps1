param([Parameter(Mandatory)][string]$OriginalMedia,[Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
if(!$root.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build fixture required'}
New-Item -ItemType Directory -Path $root|Out-Null
foreach($n in @('SETUP.EXE','DOSX.EX_','KRNL386.EX_','WIN386.EX_','MOUSE.DR_')){Copy-Item -LiteralPath (Join-Path $OriginalMedia $n) -Destination $root}
$before=@{};foreach($n in @('KRNL386.EX_','WIN386.EX_','MOUSE.DR_')){$before[$n]=(Get-FileHash (Join-Path $root $n)).Hash}
$apply=Join-Path $repo 'tools/win31-setup/APPLY.CMD';$undo=Join-Path $repo 'tools/win31-setup/UNAPPLY.CMD'
$old=$env:NTVDM_SETUP_TEST_NO_PAUSE
try {
 $env:NTVDM_SETUP_TEST_NO_PAUSE='1'
 $root | & cmd.exe /d /c $apply;if($LASTEXITCODE){throw 'First APPLY failed'}
 foreach($n in @('KRNL386.EX_','WIN386.EX_','MOUSE.DR_')){if(!(Test-Path (Join-Path $root "$n.BAK")) -or (Get-FileHash (Join-Path $root "$n.BAK")).Hash -ne $before[$n]){throw "Original backup missing beside $n"}}
 foreach($n in @('KRNL386.EX_.BAK','WIN386.EX_.BAK')){if(Test-Path (Join-Path $root "PATCH/$n")){throw "Backup incorrectly stored in PATCH: $n"}}
 & expand.exe (Join-Path $root 'MOUSE.DR_') (Join-Path $root 'MOUSE-CHECK.DRV') | Out-Null
 if($LASTEXITCODE -or (Get-FileHash (Join-Path $root 'MOUSE-CHECK.DRV')).Hash -ne (Get-FileHash (Join-Path $repo 'assets/release/MOUSE31.DRV')).Hash){throw 'Setup media mouse driver is not the compatibility driver'}
 $root | & cmd.exe /d /c $apply;if($LASTEXITCODE){throw 'Repeat APPLY failed'}
 $unknown=Join-Path $root 'PATCH/USER.TXT';[IO.File]::WriteAllText($unknown,'keep')
 $root | & cmd.exe /d /c $undo;if($LASTEXITCODE){throw 'UNAPPLY failed'}
 foreach($n in $before.Keys){if((Get-FileHash (Join-Path $root $n)).Hash -ne $before[$n]){throw "Original compressed bytes not restored: $n"}}
 foreach($n in @('KRNL386.EX_.BAK','WIN386.EX_.BAK','MOUSE.DR_.BAK')){if(Test-Path (Join-Path $root $n)){throw "Backup remains after recovery: $n"}}
 foreach($n in @('KRNL386.EX_.BAK','WIN386.EX_.BAK','SETUP.CMD','PIF.EXE','PATCHSET.EXE','MOUSE31.DRV','KRNL386-RETAIL.EXE','WIN386-RETAIL.EXE','KRNL386-ADAPTED.EXE','WIN386-ADAPTED.EXE')){if(Test-Path -LiteralPath (Join-Path $root "PATCH/$n")){throw "Recovered tool file remains: $n"}}
 if(Test-Path $unknown){throw 'PATCH was not removed'}
 New-Item -ItemType Directory -Path (Join-Path $root 'PATCH') | Out-Null
 $unknownPif=Join-Path $root 'PATCH/PIF.EXE';[IO.File]::WriteAllText($unknownPif,'user payload')
 $root | & cmd.exe /d /c $apply;if(!$LASTEXITCODE){throw 'Modified PIF payload was accepted'}
 if([IO.File]::ReadAllText($unknownPif) -ne 'user payload'){throw 'Unknown PIF payload was overwritten'}
 foreach($n in $before.Keys){if((Get-FileHash (Join-Path $root $n)).Hash -ne $before[$n]){throw 'Rejected apply changed media'}}
 Remove-Item -LiteralPath $unknownPif
 $root | & cmd.exe /d /c $apply;if($LASTEXITCODE){throw 'Reapply failed'}
 $setup=Join-Path $root 'PATCH/SETUP.CMD';[IO.File]::AppendAllText($setup,"`r`nrem user modification`r`n")
 $patched=(Get-FileHash (Join-Path $root 'KRNL386.EX_')).Hash
 $root | & cmd.exe /d /c $undo;if($LASTEXITCODE){throw 'UNAPPLY failed'}
 if((Get-FileHash (Join-Path $root 'KRNL386.EX_')).Hash -ne $before['KRNL386.EX_']){throw 'Original media not restored'}
 if(Test-Path (Join-Path $root 'PATCH')){throw 'PATCH was not removed'}
 'PASS CMD apply, repeat, backup restore and PATCH removal'
} finally {$env:NTVDM_SETUP_TEST_NO_PAUSE=$old}
