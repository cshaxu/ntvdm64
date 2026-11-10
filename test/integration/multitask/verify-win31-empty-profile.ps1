param(
 [Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$InstallSource,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$OutputRoot,
 [ValidateSet('386-dosonly0','386-dosonly1')][string]$Case='386-dosonly0'
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../../..").Path
$copy=& "$repo/test/integration/ntvdm64-386/New-Win31ProbeCopy.ps1" `
 -DriverImage "$repo/assets/release/ntvdm64.386" -InstanceDriver "$repo/assets/release/insignia.386" `
 -InstallSource $InstallSource -InstallRoot 'O:/tmp/q31' -OutputRoot $OutputRoot
$dest=$copy.Destination;$task=$copy.Task
try {
 [IO.File]::WriteAllBytes("$dest/PROGMAN.INI",[byte[]]@())
 & nasm -f bin "$repo/test/integration/win31_empty_profile_probe.asm" -o "$task/profile.exe"
 if($LASTEXITCODE){throw 'Win16 profile witness assembly failed'}
 & "$repo/test/integration/win31_runtime_matrix_test.ps1" -PackageRoot $PackageRoot -InstallRoot $dest `
  -Observer $Observer -OutputRoot "$task/runtime" -WindowsProbeImage "$task/profile.exe" -Cases $Case
 $result=[IO.File]::ReadAllBytes("$dest/PATCH/APITEST.BIN")
 if($result.Length -ne 18){throw 'Wrong profile witness size'}
 $r=0..8 | ForEach-Object {[BitConverter]::ToUInt16($result,2*$_)}
 if($r[0] -ne 1 -or $r[2] -ne 1 -or $r[4] -eq 0xffff -or $r[5] -ne 0 -or $r[6] -ne 4 -or $r[7] -ne 0 -or $r[8] -ne 4){throw "Unexpected profile or read/write status: $r"}
 foreach($name in 'PROGMAN.INI','PTEST.INI'){
  $contents=[IO.File]::ReadAllText("$dest/$name")
  if(!$contents.Length -or $contents -notmatch '(?im)^Group1=O:\\tmp\\q31\\MAIN\.GRP\s*$'){throw "Missing written group in $name"}
  Copy-Item "$dest/$name" "$task/$name"
 }
 Copy-Item "$dest/PATCH/APITEST.BIN" "$task/APITEST.BIN"
 if([IO.File]::ReadAllText("$dest/PATCH/DIRECT.BIN") -ne 'PASS'){throw 'Ordinary write/read witness failed'}
 'PASS empty INI profile writes and zero/nonzero Win16 file I/O'
} finally {
 try {foreach($row in $copy.Original){if((Get-FileHash ($copy.Source+$row.path)).Hash -ne $row.sha256){throw "Source changed: $($row.path)"}}}
 finally {. "$repo/tests/observation/isolated_package_cleanup.ps1";Remove-OwnedGuestProbeCopy $dest $task $copy.Source}
}
