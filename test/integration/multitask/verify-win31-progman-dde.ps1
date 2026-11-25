param(
 [Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$InstallSource,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$OutputRoot,
 [ValidateSet('std-dosonly0','std-dosonly1','386-dosonly0','386-dosonly1')][string]$Case='386-dosonly0',
 [switch]$ExpectUnwrittenProfile
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../../..").Path
$copy=& "$repo/test/integration/ntvdm64-386/New-Win31ProbeCopy.ps1" `
 -DriverImage "$repo/assets/release/ntvdm64.386" -InstanceDriver "$repo/assets/release/insignia.386" `
 -InstallSource $InstallSource -InstallRoot 'O:/tmp/q31' -OutputRoot $OutputRoot
$dest=$copy.Destination;$task=$copy.Task
try {
 & "$repo/tools/prepare-guest-witness.cmd" $dest $task dde empty-profile *> "$task/profile-preparation.log"
 if($LASTEXITCODE){throw 'Win16 empty profile preparation failed'}
 $mode=if($Case.StartsWith('std-')){1}else{2}
 $dos=[int]$Case.EndsWith('1')
 & "$repo/tools/select-guest-launch.cmd" $dest $task 31 $dos 0 install $mode *> "$task/select.log"
 if($LASTEXITCODE){throw 'Cannot prepare the Win31 launcher profile'}
 & "$repo/test/integration/win31_runtime_matrix_test.ps1" -PackageRoot $PackageRoot -InstallRoot $dest `
  -Observer $Observer -OutputRoot "$task/runtime" -WindowsProbeImage "$task/dde.exe" -Cases $Case -PreparedProfile
 $groups=@('DDEMAIN.GRP','DDEACC.GRP','DDEGAME.GRP','DDESTART.GRP')
 foreach($name in @('PROGMAN.INI','PATCH/DDE.BIN')+@($groups|ForEach-Object {"PATCH/$_"})){
  if(Test-Path -LiteralPath "$dest/$name" -PathType Leaf){Copy-Item -LiteralPath "$dest/$name" -Destination "$task/$([IO.Path]::GetFileName($name))"}
 }
 if(!(Test-Path -LiteralPath "$dest/PATCH/DDE.BIN" -PathType Leaf)){throw 'DDE witness result missing'}
 $result=[IO.File]::ReadAllBytes("$dest/PATCH/DDE.BIN")
 if($result.Length -ne 8 -or [BitConverter]::ToUInt16($result,0) -ne 6 -or
    [BitConverter]::ToUInt16($result,2) -ne 7){throw 'DDE transactions did not complete'}
 $items=@{'DDEMAIN.GRP'='WINFILE.EXE';'DDEACC.GRP'='NOTEPAD.EXE';'DDEGAME.GRP'='WINMINE.EXE'}
 foreach($group in $groups){
  if(!(Test-Path -LiteralPath "$dest/PATCH/$group" -PathType Leaf)){throw "DDE group missing: $group"}
  $shown=@(& "$repo/assets/release/GRP.EXE" show "$dest/PATCH/$group")
  if($LASTEXITCODE){throw "DDE group invalid: $group"}
  if($items.ContainsKey($group) -and !($shown | Where-Object {$_ -match ([regex]::Escape($items[$group])+'\s*$')})){
   throw "Expected program item missing from $group"
  }
 }
 $ini=[IO.File]::ReadAllText("$dest/PROGMAN.INI")
 $found=@($groups|Where-Object {$ini -match ('(?im)^Group\d+\s*=\s*[^\r\n]*'+[regex]::Escape($_)+'\s*$')})
 if($ExpectUnwrittenProfile){
  if($ini.Length -ne 0 -or $found.Count){throw 'Baseline did not reproduce the unwritten empty profile'}
  'PASS baseline DDE creates four groups but leaves original profile empty'
 } else {
  if(!$ini.Length -or $found.Count -ne $groups.Count){throw 'DDE groups are not all referenced by the profile'}
  'PASS original Program Manager DDE creates four groups with initially empty profile'
 }
} finally {
 try {foreach($row in $copy.Original){if((Get-FileHash ($copy.Source+$row.path)).Hash -ne $row.sha256){throw "Source changed: $($row.path)"}}}
 finally {. "$repo/test/observation/isolated_package_cleanup.ps1";Remove-OwnedGuestProbeCopy $dest $task $copy.Source}
}
