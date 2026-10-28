$ErrorActionPreference='Stop'
$repo='O:\repos.hobby\ntvdm64'
$out=Join-Path $repo 'build/t473-s10/retired-host-publication-recovery'
$target=(Resolve-Path -LiteralPath 'O:\winnt\system32').Path
if($target -ine 'O:\winnt\system32'){throw 'Unexpected installation root'}
$baseline=Get-Content (Join-Path $repo 'build/package-uuid/package-c/product-system32-manifest.json') -Raw|ConvertFrom-Json
$candidate=Get-Content (Join-Path $repo 'artifacts/mvdm-audit/sol-creation-debugger/candidate-package-inputs.json') -Raw|ConvertFrom-Json
$names=@('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe','ntmon.exe','WOW32.DLL','VDMREDIR.DLL','nthook32.dll','nthook64.dll')
if(@($baseline).Count -ne 10 -or @($candidate).Count -ne 10){throw 'Incomplete package identity'}
$processes=@(Get-CimInstance Win32_Process|Where-Object {$_.ExecutablePath -and $_.ExecutablePath.StartsWith($target+'\',[StringComparison]::OrdinalIgnoreCase)})
if($processes.Count){throw 'Installed project processes became active'}
$before=@()
foreach($row in $baseline){
    if($row.Name -notin $names -or $row.PackageId -ne 'ad4ac7ac-cffa-4892-8770-e41a5b765e94'){throw 'Unexpected baseline member'}
    $source=Join-Path (Join-Path $repo 'build/package-uuid/package-c/system32') $row.Name
    $destination=Join-Path $target $row.Name
    $expected=@($candidate|Where-Object {(Split-Path $_.RelativePath -Leaf) -ieq $row.Name})
    if($expected.Count -ne 1){throw 'Candidate member missing'}
    if((Get-FileHash -LiteralPath $source).Hash -ne $row.Sha256){throw 'Accepted source drift'}
    if((Get-FileHash -LiteralPath (Join-Path (Join-Path $repo 'assets/release') $row.Name)).Hash -ne $row.Sha256){throw 'Release asset drift'}
    $actual=(Get-FileHash -LiteralPath $destination).Hash
    if($actual -ne $expected[0].Sha256){throw 'Installation changed since observation'}
    $stream=[IO.File]::Open($destination,[IO.FileMode]::Open,[IO.FileAccess]::ReadWrite,[IO.FileShare]::Read)
    $stream.Dispose()
    $before+=@{name=$row.Name;installed_sha256=$actual;accepted_sha256=$row.Sha256}
}
@{observed_utc=[DateTime]::UtcNow.ToString('o');running_project_processes=0;members=$before;cause='Unknown; timestamps do not prove actor or change time'}|ConvertTo-Json -Depth 5|Set-Content "$out/before.json"
$backup=Join-Path $out 'backup'
New-Item -ItemType Directory -Path $backup|Out-Null
foreach($row in $before){Copy-Item -LiteralPath (Join-Path $target $row.name) -Destination (Join-Path $backup $row.name)}
foreach($row in $before){if((Get-FileHash -LiteralPath (Join-Path $backup $row.name)).Hash -ne $row.installed_sha256){throw 'Backup hash mismatch'}}
foreach($row in $baseline){Copy-Item -LiteralPath (Join-Path (Join-Path $repo 'build/package-uuid/package-c/system32') $row.Name) -Destination (Join-Path $target $row.Name) -Force}
$after=@()
foreach($row in $baseline){
    $actual=(Get-FileHash -LiteralPath (Join-Path $target $row.Name)).Hash
    if($actual -ne $row.Sha256){throw 'Installed recovery hash mismatch'}
    $after+=@{name=$row.Name;installed_sha256=$actual;accepted_sha256=$row.Sha256}
}
@{result='pass';completed_utc=[DateTime]::UtcNow.ToString('o');restored_existing_accepted_package=$true;new_candidate_qualified=$false;members=$after;guest_changed=$false}|ConvertTo-Json -Depth 5|Set-Content "$out/result.json"
'PASS ten installed images restored to existing accepted baseline; exact candidate backup retained; no guest changed'
