$ErrorActionPreference='Stop'
$repo='O:\repos.hobby\ntvdm64'
$root=Join-Path $repo 'build/t473-s10/ccpu-header-regression'
$hostPath=(Get-Process -Id $PID).Path
$cases=@(
 @{name='control';workload='Control';package='build/package-uuid/package-c';negative=$false;expected=0},
 @{name='control-negative';workload='Control';package='build/package-uuid/package-c';negative=$true;expected=0},
 @{name='sign-flags';workload='SignFlags';package='build/package-uuid/package-c';negative=$false;expected=0},
 @{name='sign-flags-negative';workload='SignFlags';package='build/package-uuid/package-c';negative=$true;expected=0},
 @{name='bit-baseline';workload='BitMemory';package='build/package-uuid/package-c';negative=$false;expected=1},
 @{name='bit-candidate';workload='BitMemory';package='build/t473-s10/candidate-validation/package';negative=$false;expected=1}
)
foreach($case in $cases){
 $name=$case.name
 $line='-NoProfile -ExecutionPolicy Bypass -File "'+$repo+'\tests\observation\verify-ccpu-sign-flags-guest.ps1" -Workload '+$case.workload+' -PackageRoot "'+(Join-Path $repo $case.package)+'" -Observer "'+$repo+'\build\tools\console-startup-observer.exe" -LogonHelper "'+$repo+'\build\t449-s2-message-construction-r3\logon-env.dll" -BuildRoot "'+$root+'\'+$name+'"'
 if($case.negative){$line+=' -NegativeOracle'}
 $p=Start-Process -FilePath $hostPath -ArgumentList $line -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/$name.stdout.txt" -RedirectStandardError "$root/$name.stderr.txt"
 try{
  @{pid=$p.Id;started=$p.StartTime.ToUniversalTime().ToString('o');image=$hostPath;arguments=$line;diagnostic_budget_seconds=180;expected_diagnostic_exit=$case.expected}|ConvertTo-Json|Set-Content "$root/$name.process.json"
  if(!$p.WaitForExit(180000)){& taskkill.exe /PID $p.Id /T /F;throw "Owned $name diagnostic budget exceeded"}
  $p.WaitForExit();$code=$p.ExitCode
  @{exit=$code;finished=$p.ExitTime.ToUniversalTime().ToString('o')}|ConvertTo-Json|Set-Content "$root/$name.completion.json"
  if($code -ne $case.expected){throw "$name unexpected runner exit $code"}
  if($case.expected){
   $report=Get-Content "$root/$name/observer.txt" -Raw
   $screen=Get-Content "$root/$name/observer.txt.console.txt" -Raw
   if($report -notmatch 'result=exited' -or $report -notmatch 'exit=0x00000001' -or $screen -notmatch 'BITMEM POSITIVE PASS' -or $screen -notmatch 'BITMEM near=8000 far=8000 carry=0' -or $screen -notmatch 'BITMEM NEGATIVE FAIL'){throw 'Known bit-index difference did not reproduce precisely'}
  }
 }finally{$p.Dispose()}
}
'REGRESSION PASS four existing positive/negative runs; KNOWN FAILURE reproduced in both bit-memory packages, not repaired'