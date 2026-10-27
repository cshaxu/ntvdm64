$ErrorActionPreference='Stop'
Set-Location 'O:\repos.hobby\ntvdm64'
$root='artifacts/mvdm-audit/ccpu-neg64'
$manifest=Get-Content "$root/observations.json" -Raw|ConvertFrom-Json
foreach($r in $manifest.records){if((Get-FileHash "$root/$($r.file)").Hash -ne $r.sha256){throw 'Durable record drift'}}
foreach($r in (Get-Content "$root/source-identities.json" -Raw|ConvertFrom-Json)){if((Get-FileHash ('src/mvdm/'+$r.relative_path)).Hash -ne $r.opennt_sha256){throw 'Original source drift'}}
$result=Get-Content "$root/result.json" -Raw|ConvertFrom-Json
if($result.result -ne 'pass' -or $result.observed_cases -ne 631410 -or $result.cpu_state_stubs -ne $false){throw 'Missing native witness'}
$image=Get-Content "$root/image-result.json" -Raw|ConvertFrom-Json
foreach($r in $image.images){if((Get-FileHash $r.path).Hash -ne $r.image_sha256){throw 'Tested image drift'}}
if($image.images[0].callers.Count -ne 2 -or ($image.images[0].callers|ForEach-Object {$_.neg64_call_vas.Count}|Measure-Object -Sum).Sum -ne 4){throw 'Missing signed calls'}
& artifacts/mvdm-audit/selected-ccpu-header-role-delivery-verification.ps1
if($LASTEXITCODE){exit $LASTEXITCODE}
& git diff --check
if($LASTEXITCODE){exit $LASTEXITCODE}
if((Get-Item docs/states/CURRENT.md).Length -gt 16384){throw 'CURRENT exceeds 16KiB'}
'PASS NEG64 native evidence, exact inputs, existing ledger and documentation gates'