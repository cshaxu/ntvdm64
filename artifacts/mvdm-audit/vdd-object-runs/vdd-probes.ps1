$ErrorActionPreference='Stop'
$repo='O:/repos.hobby/ntvdm64';Set-Location $repo
& tests/observation/verify-softpc-object-contract.ps1 -BuildRoot build/t473-s10/vdd-object-contract-r2 -Contract vdd
if($LASTEXITCODE){throw 'Normal VDD controls failed'}
$root="$repo/build/t473-s10/vdd-object-contract-r2";$exe="$root/vdd-object-contract.exe"
foreach($mode in @('deinstall-contract','dma-boundary')){
 $p=Start-Process $exe -ArgumentList "--$mode" -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/$mode.stdout.txt" -RedirectStandardError "$root/$mode.stderr.txt"
 @{pid=$p.Id;started=$p.StartTime.ToUniversalTime().ToString('o');image=$exe;arguments="--$mode"}|ConvertTo-Json|Set-Content "$root/$mode.process.json"
 $p.WaitForExit();$p.WaitForExit();$c=$p.ExitCode;$p.Dispose();@{exit=$c;expected='strict contract non-pass retained as original-source defect'}|ConvertTo-Json|Set-Content "$root/$mode.completion.json"
 if($c -ne 1){throw "Unexpected strict-contract outcome: $mode exit=$c"}
}
'PASS normal controls; both strict original-source defects reproduced without changing runtime'
