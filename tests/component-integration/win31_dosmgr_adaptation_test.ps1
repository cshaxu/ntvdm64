param([Parameter(Mandatory)][string]$OriginalWin386,
      [Parameter(Mandatory)][string]$NtDos,
      [Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)) {
    throw 'Fresh build-owned test output required'
}
$generator=Join-Path $repo 'src/addon/win31-launch/adapt-retail-dosmgr.ps1'
& $generator -OriginalWin386 $OriginalWin386 -NtDos $NtDos -OutputRoot "$root/positive"
$output=Join-Path $root 'positive/WIN386.EXE'
if((Get-FileHash $output).Hash -ne 'C67E667E25E91C65EFB5ACDDEC437DA8B586037DA7DE502228CA8FBD806CF3F8') {
    throw 'Changed candidate identity'
}
$original=[IO.File]::ReadAllBytes((Resolve-Path $OriginalWin386).Path)
$changed=[IO.File]::ReadAllBytes($output)
if($original.Length -ne $changed.Length){throw 'File extent changed'}
$offset=407661;$size=10;$differences=0
for($i=0;$i -lt $original.Length;$i++) {
    if($original[$i] -eq $changed[$i]){continue}
    if($i -lt $offset -or $i -ge $offset+$size){throw "Unexpected changed byte $i"}
    $differences++
}
if(!$differences){throw 'No substitution made'}
$manifest=Get-Content "$root/positive/adaptation.json" -Raw|ConvertFrom-Json
if(!$manifest.discoveryBypassedBeforeResourceAcquisition -or !$manifest.clientStatePairPreserved -or
   !$manifest.existingRelocatedCommitPreserved -or !$manifest.allOtherBytesUnchanged -or
   $manifest.regions.Count -ne 1 -or $manifest.regions[0].offset -ne $offset -or
   $manifest.regions[0].length -ne $size){throw 'Wrong adaptation contract'}
$rejected=$false
try { & $generator -OriginalWin386 $output -NtDos $NtDos -OutputRoot "$root/rejected" }
catch { if($_.Exception.Message -ne 'Unsupported retail WIN386 identity'){throw};$rejected=$true }
if(!$rejected -or (Test-Path "$root/rejected")){throw 'Non-original input not safely rejected'}
'PASS exact candidate, single changed span, unchanged file/other bytes, wrong-input rejection; runtime separately required'
