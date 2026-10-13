param([Parameter(Mandatory)][string]$CandidateRoot,
    [Parameter(Mandatory)][string]$System32,
    [Parameter(Mandatory)][string]$EvidenceRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$candidate=(Resolve-Path $CandidateRoot).Path
$target=(Resolve-Path $System32).Path
$evidence=[IO.Path]::GetFullPath($EvidenceRoot)
if(!$candidate.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
   !$evidence.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
   (Test-Path $evidence)){throw 'Use build-owned candidate and fresh evidence roots'}
if((Get-Content "$candidate/exit-code.txt" -Raw).Trim() -ne '0'){throw 'Full candidate verification required'}
foreach($list in @('inputs.json','dll-inputs.json')){
    foreach($item in (Get-Content "$candidate/$list" -Raw|ConvertFrom-Json)){
        if((Get-FileHash $item.Path).Hash -ne $item.Hash){throw "Stale candidate input: $($item.Path)"}
    }
}
$release=Join-Path $repo assets/release
$manifest=Join-Path $release release-manifest.json
$old=@(Get-Content $manifest -Raw|ConvertFrom-Json)
$names=@('ntw16s32.dll','ntw16s64.dll','W16Style.exe')
foreach($item in $old|Where-Object name -NotIn $names){
    foreach($dir in @($release,$target)){
        if((Get-FileHash (Join-Path $dir $item.name)).Hash -ne $item.sha256){throw "Existing package mismatch: $dir/$($item.name)"}
    }
}
$sources=@("$candidate/x86/ntw16s32.dll","$candidate/x64/ntw16s64.dll","$candidate/x64/W16Style.exe")
$machines=@(0x14c,0x8664,0x8664)
for($i=0;$i -lt 3;$i++){
    $bytes=[IO.File]::ReadAllBytes($sources[$i]);$pe=[BitConverter]::ToInt32($bytes,60)
    if([BitConverter]::ToUInt32($bytes,$pe) -ne 0x4550 -or
       [BitConverter]::ToUInt16($bytes,$pe+4) -ne $machines[$i]){throw 'Wrong PE architecture'}
}
$null=New-Item -ItemType Directory $evidence
$backups=@();$started=$false
try{
    $paths=@($manifest)
    foreach($dir in @($release,$target)){foreach($name in $names){$paths+=Join-Path $dir $name}}
    for($i=0;$i -lt $paths.Count;$i++){
        $exists=Test-Path -LiteralPath $paths[$i];$backup="$evidence/backup-$i"
        if($exists){Copy-Item -LiteralPath $paths[$i] -Destination $backup}
        $backups+=@{path=$paths[$i];existed=$exists;backup=$backup}
    }
    $backups|ConvertTo-Json|Set-Content "$evidence/backups.json"
    $started=$true
    foreach($dir in @($release,$target)){
        for($i=0;$i -lt 3;$i++){Copy-Item -LiteralPath $sources[$i] -Destination (Join-Path $dir $names[$i]) -Force}
    }
    $new=@($old|Where-Object name -NotIn $names)
    for($i=0;$i -lt 3;$i++){$new+=[ordered]@{name=$names[$i];sha256=(Get-FileHash $sources[$i]).Hash}}
    $new|ConvertTo-Json|Set-Content $manifest -Encoding utf8
    foreach($item in $new){foreach($dir in @($release,$target)){
        if((Get-FileHash (Join-Path $dir $item.name)).Hash -ne $item.sha256){throw 'Publication hash mismatch'}
    }}
    & "$repo/tests/observation/verify-ntw16s-boundary.ps1" -BuildRoot "$evidence/deployed-test" -CandidateRoot $candidate -System32 $target
    if($LASTEXITCODE -or (Get-Content "$evidence/deployed-test/exit-code.txt" -Raw).Trim() -ne '0'){throw 'Deployed boundary test failed'}
    $new|ConvertTo-Json|Set-Content "$evidence/verified-manifest.json"
    '0'|Set-Content "$evidence/exit-code.txt"
    Write-Output "PASS published $($new.Count) matching images; actual deployed DLL/demo tests pass"
}catch{
    if($started){foreach($entry in $backups){
        if($entry.existed){Copy-Item -LiteralPath $entry.backup -Destination $entry.path -Force}
        elseif(Test-Path -LiteralPath $entry.path){Remove-Item -LiteralPath $entry.path}
    }}
    $_|Out-String|Set-Content "$evidence/failure.txt"
    '1'|Set-Content "$evidence/exit-code.txt"
    throw
}
