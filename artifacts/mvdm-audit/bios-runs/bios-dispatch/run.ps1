$ErrorActionPreference='Stop'
$repo='O:\repos.hobby\ntvdm64'
$run=Join-Path $repo 'build/t473-s10/bios-dispatch'
$hostPath='C:\Users\neko\.cache\codex-runtimes\codex-primary-runtime\dependencies\native\powershell\pwsh.exe'
$cases=@(
 @{name='bios-baseline';workload='BiosDispatch';package='build/package-uuid/package-c';negative=$false},
 @{name='bios-baseline-negative';workload='BiosDispatch';package='build/package-uuid/package-c';negative=$true},
 @{name='bios-candidate';workload='BiosDispatch';package='build/t473-s10/candidate-validation/package';negative=$false},
 @{name='bios-candidate-negative';workload='BiosDispatch';package='build/t473-s10/candidate-validation/package';negative=$true},
 @{name='control';workload='Control';package='build/package-uuid/package-c';negative=$false},
 @{name='control-negative';workload='Control';package='build/package-uuid/package-c';negative=$true},
 @{name='sign-flags';workload='SignFlags';package='build/package-uuid/package-c';negative=$false},
 @{name='sign-flags-negative';workload='SignFlags';package='build/package-uuid/package-c';negative=$true}
)
foreach($case in $cases){
 $name=$case.name
 $line='-NoProfile -ExecutionPolicy Bypass -File "'+$repo+'\tests\observation\verify-ccpu-sign-flags-guest.ps1" -Workload '+$case.workload+' -PackageRoot "'+(Join-Path $repo $case.package)+'" -Observer "'+$repo+'\build\tools\console-startup-observer.exe" -LogonHelper "'+$repo+'\build\t449-s2-message-construction-r3\logon-env.dll" -BuildRoot "'+$run+'\'+$name+'"'
 if($case.negative){$line+=' -NegativeOracle'}
 $p=Start-Process -FilePath $hostPath -ArgumentList $line -WindowStyle Hidden -PassThru -WorkingDirectory $repo -RedirectStandardOutput "$run/$name.stdout.txt" -RedirectStandardError "$run/$name.stderr.txt"
 try{
  @{pid=$p.Id;started=$p.StartTime.ToUniversalTime().ToString('o');image=$hostPath;arguments=$line;diagnostic_budget_seconds=180;negative_oracle=$case.negative}|ConvertTo-Json|Set-Content "$run/$name.process.json"
  if(!$p.WaitForExit(180000)){& taskkill.exe /PID $p.Id /T /F;throw "Owned $name diagnostic budget exceeded"}
  $p.WaitForExit();$code=$p.ExitCode
  @{exit=$code;finished=$p.ExitTime.ToUniversalTime().ToString('o')}|ConvertTo-Json|Set-Content "$run/$name.completion.json"
  if($code){Get-Content "$run/$name/failure.txt" -ErrorAction SilentlyContinue;throw "$name runner nonpass: $code"}
 }finally{$p.Dispose()}
}
'PASS four BIOS dispatch/oracle runs and four existing CPU control/sign regression runs'