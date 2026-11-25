param(
 [Parameter(Mandatory)][string]$BuildRoot,
 [Parameter(Mandatory)][string]$InstallSource,
 [Parameter(Mandatory)][string]$PackageRoot
)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath("$PSScriptRoot/../..")
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned evidence required'}
$null=New-Item -ItemType Directory $root
. "$repo/test/observation/isolated_package_cleanup.ps1"
. "$repo/test/integration/Invoke-Win31ProbeBuild.ps1"
$enc=[Text.Encoding]::GetEncoding(28591)
$dest='O:\tmp\e31'
$copy=$null
try {
 $copy=& "$repo/test/integration/ntvdm64-386/New-Win31ProbeCopy.ps1" -DriverImage "$repo/assets/release/ntvdm64.386" -InstanceDriver "$repo/assets/release/insignia.386" -InstallSource $InstallSource -InstallRoot $dest -OutputRoot "$root/copy"
 $win=[IO.File]::ReadAllText("$dest/WIN.INI",$enc)
 $system=[IO.File]::ReadAllText("$dest/SYSTEM.INI",$enc)
 & "$repo/tools/prepare-win31-editors.cmd" $dest $copy.Task $PackageRoot - 1 0 1 0 *> "$root/editor.log"
 if($LASTEXITCODE){throw 'Editor CMD preparation failed'}
 $expected=[regex]::Replace($win,'(?im)^load=[^\r\n]*',"load=$dest\PATCH\A.PIF $dest\PATCH\B.PIF")
 if([IO.File]::ReadAllText("$dest/WIN.INI",$enc) -cne $expected){throw 'Editor WIN.INI differs from prior contract'}
 $expected=[regex]::Replace($system,'(?im)^\[386Enh\]','[386Enh]'+"`r`ndevice=*vmd")
 if([IO.File]::ReadAllText("$dest/SYSTEM.INI",$enc) -cne $expected){throw 'Editor SYSTEM.INI differs from prior contract'}
 foreach($id in 'A','B'){
  $shown=& "$repo/assets/release/PIF.EXE" show "$dest/PATCH/$id.PIF"
  if($LASTEXITCODE -or "DIRECTORY=$dest\PATCH" -notin $shown -or "PROGRAM=$dest\PATCH\COMMAND.COM" -notin $shown){throw 'Editor PIF target mismatch'}
  if((Get-Item "$dest/PATCH/$id.TXT").Length){throw 'Save witness not empty'}
 }
 if((Get-FileHash "$PackageRoot/system32/COMMAND.COM").Hash -ne (Get-FileHash "$dest/PATCH/COMMAND.COM").Hash){throw 'COMMAND changed'}
 $identityInput="$root/identity-input"
 $null=New-Item -ItemType Directory $identityInput
 & "$repo/tools/prepare-test-inputs.cmd" -GuestPatch $dest $identityInput "$PackageRoot/system32/MEM.EXE" MEM.EXE
 if($LASTEXITCODE -ne 2){throw 'Wrong guest owner accepted'}
 & "$repo/tools/prepare-test-inputs.cmd" -GuestPatch $dest $copy.Task "$PackageRoot/system32/MEM.EXE" MEM.EXE
 if($LASTEXITCODE -or (Get-FileHash "$PackageRoot/system32/MEM.EXE").Hash -ne (Get-FileHash "$dest/PATCH/MEM.EXE").Hash){throw 'Guest patch input changed'}
 $rootWitness="$dest/STAGED.COM"
 if(Test-Path $rootWitness){throw 'Root witness already exists'}
 & "$repo/tools/prepare-test-inputs.cmd" -GuestRoot $dest $identityInput "$PackageRoot/system32/MEM.EXE" STAGED.COM
 if($LASTEXITCODE -ne 2 -or (Test-Path $rootWitness)){throw 'Wrong guest owner accepted for root staging'}
 & "$repo/tools/prepare-test-inputs.cmd" -GuestRoot $dest $copy.Task "$PackageRoot/system32/MEM.EXE" STAGED.COM
 if($LASTEXITCODE -or (Get-FileHash "$PackageRoot/system32/MEM.EXE").Hash -ne (Get-FileHash $rootWitness).Hash -or
    (Test-Path "$dest/PATCH/STAGED.COM")){throw 'Root staging destination or file bytes changed'}
 $matrix="$root/matrix";$null=New-Item -ItemType Directory $matrix
 Invoke-Win31ProbeBuild -OutputRoot $matrix
 $before=@{}
 foreach($file in 'WIN.INI','PATCH/CONFIG.NT','PATCH/AUTOEXEC.NT'){$before[$file]=(Get-FileHash "$dest/$file").Hash}
 & "$repo/tools/prepare-win31-matrix.cmd" prepare $dest $matrix 0 0 *> "$root/matrix.log"
 if($LASTEXITCODE){throw 'Matrix preparation failed'}
 foreach($mode in 0,1,0){
  & "$repo/tools/prepare-win31-matrix.cmd" case $dest $matrix $mode 0
  if($LASTEXITCODE){throw 'Matrix selection failed'}
  $content=[IO.File]::ReadAllText("$dest/PATCH/CONFIG.NT",$enc)
  if(@([regex]::Matches($content,'(?im)^\s*dosonly\s*$')).Count -ne $mode){throw 'DOSONLY count mismatch'}
 }
 & "$repo/tools/prepare-win31-matrix.cmd" restore $dest $matrix 0 0
 if($LASTEXITCODE){throw 'Matrix restore failed'}
 foreach($file in $before.Keys){if((Get-FileHash "$dest/$file").Hash -ne $before[$file]){throw "Restore changed $file"}}
 & "$repo/tools/prepare-win31-matrix.cmd" prepare $dest $matrix 0 0
 if($LASTEXITCODE -ne 2){throw 'Reused preparation accepted'}
 foreach($row in $copy.Original){if((Get-FileHash ($copy.Source+$row.path)).Hash -ne $row.sha256){throw 'Installation source changed'}}
 'PASS editor INI baseline, PIF targets, save witnesses, COMMAND bytes, owned patch/root staging, matrix DOSONLY 0/1/0 and exact restoration'|Set-Content "$root/result.txt"
} finally {
 if($copy){Remove-OwnedGuestProbeCopy $dest $copy.Task $copy.Source}
}
$copy=$null
try {
 $copy=& "$repo/test/integration/ntvdm64-386/New-Win31ProbeCopy.ps1" -DriverImage "$repo/assets/release/ntvdm64.386" -InstallSource $InstallSource -InstallRoot $dest -OutputRoot "$root/empty-load"
 & "$repo/tools/prepare-win31-matrix.cmd" launch $dest $copy.Task 1 0 none 0 *> "$root/empty-load.log"
 if($LASTEXITCODE){throw 'Empty load profile preparation failed'}
 if([IO.File]::ReadAllText("$dest/WIN.INI",$enc) -notmatch '(?im)^load=\r?$' -or
    [IO.File]::ReadAllText("$dest/PATCH/CONFIG.NT",$enc) -notmatch '(?im)^dosonly\r?$'){throw 'Empty load profile or DOSONLY mismatch'}
} finally {
 if($copy){Remove-OwnedGuestProbeCopy $dest $copy.Task $copy.Source}
}
$global:LASTEXITCODE=0
