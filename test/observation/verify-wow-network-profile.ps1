param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath("$PSScriptRoot/../..")
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned evidence required'}
$null=New-Item -ItemType Directory $root
& "$repo/tools/build-fixtures.cmd" x64 wow-network-profile *> "$root/build.log"
if($LASTEXITCODE){throw 'Network profile tool build failed'}
$tool="$repo/build/.ninja/fixtures-x64/wow-network-profile.exe"
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
& "$repo/tools/prepare-wow-network.cmd" $root Empty
if($LASTEXITCODE -ne 2){throw 'Unowned stage accepted'}
'PASS both profiles, unrelated bytes, duplicate/missing/NUL/selection rejection and stage boundary'|Set-Content "$root/result.txt"
$global:LASTEXITCODE=0
