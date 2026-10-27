param([Parameter(Mandatory)][string]$OriginalMedia,[Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
if(!$root.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build fixture required'}
New-Item -ItemType Directory -Path $root|Out-Null
foreach($n in @('SETUP.EXE','DOSX.EX_','KRNL386.EX_','WIN386.EX_')){Copy-Item -LiteralPath (Join-Path $OriginalMedia $n) -Destination $root}
$before=@{};foreach($n in @('KRNL386.EX_','WIN386.EX_')){$before[$n]=(Get-FileHash (Join-Path $root $n)).Hash}
$apply=Join-Path $repo 'tools/win31-setup/APPLY.CMD';$undo=Join-Path $repo 'tools/win31-setup/UNAPPLY.CMD'
$old=$env:NTVDM_SETUP_TEST_NO_PAUSE
try {
 $env:NTVDM_SETUP_TEST_NO_PAUSE='1'
 & cmd.exe /d /c $apply $root;if($LASTEXITCODE){throw 'First APPLY failed'}
 & cmd.exe /d /c $apply $root;if($LASTEXITCODE){throw 'Repeat APPLY failed'}
 $unknown=Join-Path $root 'PATCH/USER.TXT';[IO.File]::WriteAllText($unknown,'keep')
 & cmd.exe /d /c $undo $root;if($LASTEXITCODE){throw 'UNAPPLY failed'}
 foreach($n in $before.Keys){if((Get-FileHash (Join-Path $root $n)).Hash -ne $before[$n]){throw "Original compressed bytes not restored: $n"}}
 foreach($n in @('KRNL386.EX_.BAK','WIN386.EX_.BAK','SETUP.CMD','PATCHSET.EXE','MOUSE31.DRV','KRNL386-RETAIL.EXE','WIN386-RETAIL.EXE','KRNL386-ADAPTED.EXE','WIN386-ADAPTED.EXE')){if(Test-Path -LiteralPath (Join-Path $root "PATCH/$n")){throw "Recovered tool file remains: $n"}}
 if([IO.File]::ReadAllText($unknown) -ne 'keep'){throw 'Unknown payload was lost'}
 & cmd.exe /d /c $apply $root;if($LASTEXITCODE){throw 'Reapply failed'}
 $setup=Join-Path $root 'PATCH/SETUP.CMD';[IO.File]::AppendAllText($setup,"`r`nrem user modification`r`n")
 $patched=(Get-FileHash (Join-Path $root 'KRNL386.EX_')).Hash
 & cmd.exe /d /c $undo $root;if(!$LASTEXITCODE){throw 'Modified payload was accepted'}
 if((Get-FileHash (Join-Path $root 'KRNL386.EX_')).Hash -ne $patched){throw 'Failed UNAPPLY changed media'}
 'PASS CMD apply, repeat, exact unapply, unknown payload and conflict preservation'
} finally {$env:NTVDM_SETUP_TEST_NO_PAUSE=$old}
