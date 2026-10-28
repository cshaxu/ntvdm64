$ErrorActionPreference='Stop'
$repo='O:\repos.hobby\ntvdm64';$out=Join-Path $repo 'build/t473-s10/latest-source-release-r3'
$pwsh='C:\Users\neko\.cache\codex-runtimes\codex-primary-runtime\dependencies\native\powershell\pwsh.exe'
$media='O:\assets\mswindos\ewin31'
$original=@{};foreach($name in @('SETUP.EXE','DOSX.EX_','KRNL386.EX_','WIN386.EX_','MOUSE.DR_')){$original[$name]=(Get-FileHash (Join-Path $media $name)).Hash}
$record=[ordered]@{result='running';started=[DateTime]::UtcNow.ToString('o');runs=@();original_media=$media;original_hashes=$original}
function RunTest($name,$file,$arguments){
    $p=Start-Process $file -ArgumentList $arguments -WindowStyle Hidden -WorkingDirectory $repo -PassThru -RedirectStandardOutput "$out/$name.stdout.txt" -RedirectStandardError "$out/$name.stderr.txt"
    $record.runs+=@{name=$name;pid=$p.Id;started=$p.StartTime.ToUniversalTime().ToString('o');image=$file;arguments=$arguments;diagnostic_budget_ms=120000}
    $record|ConvertTo-Json -Depth 8|Set-Content "$out/tools-result.json"
    try{if(!$p.WaitForExit(120000)){$p.Kill($true);$p.WaitForExit();throw "$name diagnostic timeout"};$p.WaitForExit();$record.runs[-1].exit=$p.ExitCode;if($p.ExitCode){throw "$name failed: $($p.ExitCode)"}}finally{$p.Dispose()}
}
try{
    RunTest media $pwsh '-NoProfile -File tests/component-integration/win31_setup_cmd_roundtrip_test.ps1 -OriginalMedia O:/assets/mswindos/ewin31 -BuildRoot build/t473-s10/w31-media-r3'
    RunTest profiles $pwsh '-NoProfile -File tests/component-integration/win31_setup_profile_generation_test.ps1 -BuildRoot build/w31r3'
    RunTest safety $pwsh '-NoProfile -File tests/component-integration/win31_setup_runner_safety_test.ps1 -BuildRoot build/t473-s10/w31-safety-r3'
    RunTest runner $pwsh '-NoProfile -File tests/component-integration/win31_setup_runner_test.ps1 -PackageRoot build/t473-s10/w31-media-r3 -BuildRoot build/t473-s10/w31-runner-r3'
    # Test the legacy byte-payload preparation boundary with controlled inputs;
    # no fixture executable or guest is launched here.
    $work=Join-Path $out 'payload-work';$package=Join-Path $out 'payload-media'
    New-Item -ItemType Directory -Path $package|Out-Null
    foreach($name in @('SETUP.EXE','DOSX.EX_','KRNL386.EX_','WIN386.EX_')){Copy-Item -LiteralPath (Join-Path $media $name) -Destination (Join-Path $package $name)}
    & tools/build/win31-setup/apply-setup.ps1 -MediaRoot $package
    if((Get-FileHash "$package/PATCH/PIF.EXE").Hash -ne (Get-FileHash assets/release/PIF.EXE).Hash){throw 'Legacy payload lacks matching PIF tool'}
    RunTest payload $pwsh ('-NoProfile -File tests/component-integration/win31_setup_apply_idempotence_test.ps1 -PackageRoot "'+$package+'" -BuildRoot build/t473-s10/w31-payload-r3')
    foreach($name in $original.Keys){if((Get-FileHash (Join-Path $media $name)).Hash -ne $original[$name]){throw 'Original media changed'}}
    $record.result='pass'
}catch{$record.result='fail';$record.error=$_.Exception.Message}
$record.finished=[DateTime]::UtcNow.ToString('o');$record|ConvertTo-Json -Depth 8|Set-Content "$out/tools-result.json"
if($record.result -ne 'pass'){throw $record.error}
'PASS five setup tool contract groups; original media hashes unchanged; no actual Setup/installed guest executed'
