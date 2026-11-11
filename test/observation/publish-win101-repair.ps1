param([Parameter(Mandatory)][string]$ReleaseRoot,
      [Parameter(Mandatory)][string]$ProductEvidence,
      [Parameter(Mandatory)][string]$ControlEvidence,
      [Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$prefix=(Join-Path $repo 'build')+'\'
$release=(Resolve-Path $ReleaseRoot).Path
$product=(Resolve-Path $ProductEvidence).Path;$control=(Resolve-Path $ControlEvidence).Path
$out=[IO.Path]::GetFullPath((Join-Path $repo $OutputRoot))
foreach($path in @($release,$product,$control,$out)) {
    if(!$path.StartsWith($prefix,[StringComparison]::OrdinalIgnoreCase)){throw 'Build-owned delivery evidence required'}
}
if(Test-Path $out){throw 'Fresh publication evidence required'}
$candidate=Join-Path $release 'runtime/system32';$destination='O:\winnt\system32'
if((Resolve-Path $destination).Path -ne $destination){throw 'Unexpected publication destination'}
$names=@('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe','ntmon.exe','WOW32.DLL','VDMREDIR.DLL','nthook32.dll','nthook64.dll')
$a=@(Get-Content "$product/runtime-manifest.json" -Raw|ConvertFrom-Json)
$b=@(Get-Content "$control/runtime-manifest.json" -Raw|ConvertFrom-Json)
if($a.Count -ne 10 -or $b.Count -ne 10 -or @($a.Name|Sort-Object -Unique).Count -ne 10){throw 'Incomplete gate manifests'}
foreach($row in $a) {
    $peer=@($b|Where-Object Name -eq $row.Name)
    if($row.Name -notin $names -or $peer.Count -ne 1 -or $peer[0].Sha256 -ne $row.Sha256 -or
       (Get-FileHash "$candidate/$($row.Name)").Hash -ne $row.Sha256){throw 'Gate/candidate input identity mismatch'}
}
$required=@('wow-frontiers','Console-17','Window-17','rpc','native-gui','version-negatives','strict-dir',
 'modern-edit-return','cooked-return','rapid-relaunch','interactive-relaunch','session-isolation','retirement-wiring','nested-window-handoff')
$productGates=(Get-Content "$product/timings.json" -Raw|ConvertFrom-Json).Gates
$controlGates=(Get-Content "$control/timings.json" -Raw|ConvertFrom-Json).Gates
$accepted=@()
foreach($name in $required) {
    $fromProduct=$name -in @('wow-frontiers','Console-17','Window-17')
    $rows=@($(if($fromProduct){$productGates}else{$controlGates})|Where-Object Gate -eq $name)
    if($rows.Count -ne 1 -or !$rows[0].Passed){throw "Missing accepted gate: $name"}
    $accepted+=@{gate=$name;evidence=$(if($fromProduct){$product}else{$control});passed=$true}
}
$baseline=Join-Path $repo 'build/M0-T434/S5/r037-runtime/system32'
foreach($name in $names){if((Get-FileHash "$destination/$name").Hash -ne (Get-FileHash "$baseline/$name").Hash){throw "Published baseline changed: $name"}}
$backup=Join-Path $out 'recovery'
New-Item -ItemType Directory -Path $backup -Force|Out-Null
foreach($name in $names){Copy-Item -LiteralPath "$destination/$name" -Destination "$backup/$name"}
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$paths=@($names|Where-Object {$_ -like '*.exe'}|ForEach-Object {Join-Path $destination $_})
Stop-IdentityCheckedProcesses @(Get-CimInstance Win32_Process|Where-Object {$_.ExecutablePath -in $paths}) $paths
try {
    foreach($name in $names){Copy-Item -LiteralPath "$candidate/$name" -Destination "$destination/$name" -Force}
    foreach($row in $a){if((Get-FileHash "$destination/$($row.Name)").Hash -ne $row.Sha256){throw 'Published identity mismatch'}}
    $a|ConvertTo-Json|Set-Content "$out/published.json"
    $accepted|ConvertTo-Json -Depth 4|Set-Content "$out/accepted-gates.json"
    'PASS coherent ten-image publication and all14 same-input gates; original media/configuration unchanged'
} catch {
    foreach($name in $names){Copy-Item -LiteralPath "$backup/$name" -Destination "$destination/$name" -Force}
    foreach($name in $names){if((Get-FileHash "$backup/$name").Hash -ne (Get-FileHash "$destination/$name").Hash){throw 'Publication rollback mismatch'}}
    throw
}
