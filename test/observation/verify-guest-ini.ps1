param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath("$PSScriptRoot/../..")
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned evidence required'}
$null=New-Item -ItemType Directory $root
& "$repo/tools/build-fixtures.cmd" x64 guest-ini *> "$root/build.log"
if($LASTEXITCODE){throw 'Network profile tool build failed'}
$tool="$repo/build/.ninja/fixtures-x64/guest-ini.exe"
$encoding=[Text.Encoding]::GetEncoding(28591)
$original="; caf$([char]233)`r`n[other]`r`nnetwork.drv=untouched`r`n[boot]`r`nshell=progman.exe`r`nnetwork.drv=original.drv`r`n[tail]`r`nx=y`r`n"
foreach($mode in 'Empty','WfwNet'){
 $file=Join-Path $root "$mode.ini"
 [IO.File]::WriteAllBytes($file,$encoding.GetBytes($original))
 & $tool $file $mode
 if($LASTEXITCODE){throw "Profile update failed: $mode"}
 $value=if($mode -eq 'WfwNet'){'wfwnet.drv'}else{''}
 if($encoding.GetString([IO.File]::ReadAllBytes($file)) -cne $original.Replace('network.drv=original.drv',"network.drv=$value")){throw 'Unrelated INI bytes changed'}
}
foreach($case in @(@('missing',"[other]`r`nx=y`r`n",'Empty'),@('duplicate',"[boot]`r`nx=y`r`n[BOOT]`r`n",'Empty'),@('nul',"[boot]`r`nx=$([char]0)`r`n",'Empty'),@('selection',$original,'invalid'))){
 $file=Join-Path $root ($case[0]+'.ini')
 [IO.File]::WriteAllBytes($file,$encoding.GetBytes($case[1]))
 $hash=(Get-FileHash $file).Hash
 & $tool $file $case[2]
 if($LASTEXITCODE -ne 2 -or (Get-FileHash $file).Hash -ne $hash){throw "Invalid profile changed: $($case[0])"}
}
$driverInput="; caf$([char]233)`r`n[boot]`r`nmouse.drv=mouse.drv`r`n[386Enh]`r`ndevice=C:\old\NTVDM64.386 ; previous`r`ndevice=other.386`r`n[tail]`r`nx=y`r`n"
foreach($version in '31','95'){
 $file=Join-Path $root "drivers-$version.ini"
 [IO.File]::WriteAllBytes($file,$encoding.GetBytes($driverInput))
 $flags=if($version -eq '31'){'15'}else{'11'}
 $candidate="$file.candidate"
 & $tool $file $candidate 'O:\tmp\drv' $version $flags
 if($LASTEXITCODE){throw 'Driver candidate failed'}
 $entries=if($version -eq '31'){"device=O:\tmp\drv\PATCH\RETIRE.386`r`ndevice=O:\tmp\drv\PATCH\INTS.386`r`ndevice=O:\tmp\drv\PATCH\INSIGNIA.386"}else{"device=O:\tmp\drv\PATCH\INSIGNIA.386`r`ndevice=O:\tmp\drv\PATCH\INTS.386"}
 $expected=$driverInput.Replace("[386Enh]`r`ndevice=C:\old\NTVDM64.386 ; previous","[386Enh]`r`n$entries`r`ndevice=O:\tmp\drv\PATCH\NTVDM64.386")
 if($encoding.GetString([IO.File]::ReadAllBytes($candidate)) -cne $expected){throw 'Driver entry order or unrelated bytes changed'}
 if($encoding.GetString([IO.File]::ReadAllBytes($file)) -cne $driverInput){throw 'Driver source changed'}
 $hash=(Get-FileHash $candidate).Hash
 & $tool $file $candidate 'O:\tmp\drv' $version $flags
 if(!$LASTEXITCODE -or (Get-FileHash $candidate).Hash -ne $hash){throw 'Existing driver candidate overwritten'}
}
foreach($case in @(@('missing-enh',"[boot]`r`nmouse.drv=mouse.drv`r`n"),@('duplicate-enh',"[386Enh]`r`n[386Enh]`r`n"),@('bad-mouse',"[boot]`r`nmouse.drv=other.drv`r`n[386Enh]`r`n"))){
 $file=Join-Path $root ($case[0]+'.ini')
 [IO.File]::WriteAllBytes($file,$encoding.GetBytes($case[1]))
 & $tool $file "$file.candidate" 'O:\tmp\drv' 95 8
 if($LASTEXITCODE -ne 2 -or (Test-Path "$file.candidate")){throw "Invalid driver input accepted: $($case[0])"}
}
& "$repo/tools/prepare-guest-ini.cmd" $root Empty
if($LASTEXITCODE -ne 2){throw 'Unowned stage accepted'}
foreach($newline in "`r`n","`n"){
 $file=Join-Path $root ('literal-'+$newline.Length+'.ini')
 $input="; caf$([char]233)$newline[windows]${newline}load=old${newline}run=${newline}[386Enh]${newline}device=old.386"
 [IO.File]::WriteAllBytes($file,$encoding.GetBytes($input))
 & $tool $file "$file.set" set windows load 'O:\tmp\ed\A.PIF O:\tmp\ed\B.PIF'
 if($LASTEXITCODE -or $encoding.GetString([IO.File]::ReadAllBytes("$file.set")) -cne $input.Replace('load=old','load=O:\tmp\ed\A.PIF O:\tmp\ed\B.PIF')){throw 'Literal set changed unrelated bytes'}
 & $tool $file "$file.append" append 386Enh device '*vmd'
 if($LASTEXITCODE -or $encoding.GetString([IO.File]::ReadAllBytes("$file.append")) -cne $input.Replace("[386Enh]$newline","[386Enh]${newline}device=*vmd$newline")){throw 'Literal append lost repeated keys or line endings'}
 $hash=(Get-FileHash "$file.set").Hash
 & $tool $file "$file.set" set windows load new
 if(!$LASTEXITCODE -or (Get-FileHash "$file.set").Hash -ne $hash){throw 'Existing literal candidate overwritten'}
}
foreach($case in @(@('missing-key',"[windows]`r`nrun=x`r`n"),@('duplicate-key',"[windows]`r`nload=a`r`nLOAD=b`r`n"),@('duplicate-section',"[windows]`r`nload=a`r`n[WINDOWS]`r`nload=b`r`n"))){
 $file=Join-Path $root ($case[0]+'.ini')
 [IO.File]::WriteAllBytes($file,$encoding.GetBytes($case[1]))
 & $tool $file "$file.candidate" set windows load new
 if($LASTEXITCODE -ne 2 -or (Test-Path "$file.candidate")){throw "Ambiguous literal edit accepted: $($case[0])"}
}
'PASS network and driver profiles, entry ordering, existing-output refusal, unrelated bytes, duplicate/missing/NUL/selection rejection and stage boundary'|Set-Content "$root/result.txt"
$global:LASTEXITCODE=0
