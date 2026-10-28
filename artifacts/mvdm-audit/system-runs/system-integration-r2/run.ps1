$ErrorActionPreference='Stop'
$repo='O:\repos.hobby\ntvdm64'
$run=Join-Path $repo 'build/t473-s10/system-integration-r2'
$hostPath='C:\Users\neko\.cache\codex-runtimes\codex-primary-runtime\dependencies\native\powershell\pwsh.exe'
function RunOwned($name,$image,$line,$working,$seconds){
 $p=Start-Process -FilePath $image -ArgumentList $line -WindowStyle Hidden -PassThru -WorkingDirectory $working -RedirectStandardOutput "$run/$name.stdout.txt" -RedirectStandardError "$run/$name.stderr.txt"
 try{
  @{pid=$p.Id;started=$p.StartTime.ToUniversalTime().ToString('o');image=$image;arguments=$line;working_directory=$working;diagnostic_budget_seconds=$seconds}|ConvertTo-Json|Set-Content "$run/$name.process.json"
  if(!$p.WaitForExit($seconds*1000)){& taskkill.exe /PID $p.Id /T /F;throw "Owned $name diagnostic budget exceeded"}
  $p.WaitForExit();$code=$p.ExitCode
  @{exit=$code;finished=$p.ExitTime.ToUniversalTime().ToString('o')}|ConvertTo-Json|Set-Content "$run/$name.completion.json"
  if($code){Get-Content "$run/$name.stderr.txt";throw "$name failed: $code"}
 }finally{$p.Dispose()}
}
RunOwned image (Get-Command python).Source ('-B -X utf8 "'+$repo+'\artifacts\mvdm-audit\system-image-observer.py"') $repo 180
$worker=Join-Path $repo 'build/.ninja/msvc-x86-release/worker'
RunOwned incremental (Get-Command ninja).Source '-n pic-reset-eoi-test.exe' $worker 60
if((Get-Content "$run/incremental.stdout.txt" -Raw) -notmatch 'no work to do'){throw 'Fixture cache requires build; do not run a stale native fixture'}
Get-FileHash "$worker/pic-reset-eoi-test.exe","$worker/pic-reset-eoi-test.exe.map","$repo/tests/mvdm-host/pic_reset_eoi_test.c","$worker/build.ninja","$worker/original-softpc-system.lib"|Export-Csv "$run/native-inputs.csv" -NoTypeInformation
RunOwned pic "$worker/pic-reset-eoi-test.exe" '' $run 120
if((Get-Content "$run/pic.stdout.txt" -Raw) -notmatch 'PIC_RESET_EOI assertions=24 failures=0'){throw 'Native PIC/device assertions missing'}
$cases=@(
 @{name='system-baseline';workload='SystemDevices';package='build/package-uuid/package-c';negative=$false},
 @{name='system-baseline-negative';workload='SystemDevices';package='build/package-uuid/package-c';negative=$true},
 @{name='system-candidate';workload='SystemDevices';package='build/t473-s10/candidate-validation/package';negative=$false},
 @{name='system-candidate-negative';workload='SystemDevices';package='build/t473-s10/candidate-validation/package';negative=$true},
 @{name='dummy-baseline';workload='DummyInt';package='build/package-uuid/package-c';negative=$false},
 @{name='dummy-baseline-negative';workload='DummyInt';package='build/package-uuid/package-c';negative=$true},
 @{name='dummy-candidate';workload='DummyInt';package='build/t473-s10/candidate-validation/package';negative=$false},
 @{name='dummy-candidate-negative';workload='DummyInt';package='build/t473-s10/candidate-validation/package';negative=$true},
 @{name='bios-control';workload='BiosDispatch';package='build/package-uuid/package-c';negative=$false},
 @{name='bios-control-negative';workload='BiosDispatch';package='build/package-uuid/package-c';negative=$true}
)
foreach($case in $cases){
 $name=$case.name
 $line='-NoProfile -ExecutionPolicy Bypass -File "'+$repo+'\tests\observation\verify-ccpu-sign-flags-guest.ps1" -Workload '+$case.workload+' -PackageRoot "'+(Join-Path $repo $case.package)+'" -Observer "'+$repo+'\build\tools\console-startup-observer.exe" -LogonHelper "'+$repo+'\build\t449-s2-message-construction-r3\logon-env.dll" -BuildRoot "'+$run+'\'+$name+'"'
 if($case.negative){$line+=' -NegativeOracle'}
 RunOwned $name $hostPath $line $repo 180
}
'PASS original system image/recipe, cached native PIC assertions and ten real guest/oracle contracts'